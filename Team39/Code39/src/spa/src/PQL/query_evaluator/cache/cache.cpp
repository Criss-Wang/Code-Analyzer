#include <memory>
#include <unordered_map>

#include "cache.h"

using namespace std;

namespace pql_cache {
  /*-------------------------------------------------------API for domain---------------------------------------------------------*/
  unordered_set<int> Cache::GetAllEntity(const EntityIdentifier entity_identifier) {
    return pkb_->GetAllEntity(entity_identifier);
  }

  /*----------------------------------------------------API for attribute------------------------------------------------------------*/
  int Cache::GetIndexByString(IndexTableType index_table_type, const string& entity_name) {
    return pkb_->GetIndexByString(index_table_type, entity_name);
  }

  vector<int> Cache::GetStmtNumByStringAttribute(EntityIdentifier entity_identifier, const int string_idx) {
    return pkb_->GetStmtNumByStringAttribute(entity_identifier, string_idx);
  }

  int Cache::GetStringAttribute(EntityIdentifier entity_identifier, const int stmt_no) {
    return pkb_->GetStringAttribute(entity_identifier, stmt_no);
  }

  string Cache::GetStringByIndex(IndexTableType index_table_type, int idx) {
    return pkb_->GetStringByIndex(index_table_type, idx);
  }
  /*-------------------------------------------------------API for clause----------------------------------------------------------*/
  typedef void (Cache::* GenerateCache)(pql::RelationshipTypes);

  const unordered_map<pql::RelationshipTypes, GenerateCache> GenerateRelCacheMap = {
    { pql::kNextT, &Cache::GenerateNextTOrAffectsTRelDomain },
    { pql::kAffects, &Cache::GenerateAffectsRelDomain },
    { pql::kAffectsT, &Cache::GenerateNextTOrAffectsTRelDomain }
  };

  const unordered_map<pql::RelationshipTypes, GenerateCache> GenerateInverseRelCacheMap = {
    { pql::kNextT, &Cache::GenerateNextTOrAffectsTInverseRelDomain },
    { pql::kAffects, &Cache::GenerateAffectsInverseRelDomain },
    { pql::kAffectsT, &Cache::GenerateNextTOrAffectsTInverseRelDomain }
  };

  const unordered_map<pql::RelationshipTypes, GenerateCache> GeneratePairCacheMap = {
    { pql::kNextT, &Cache::GenerateNextTOrAffectsTPairDomain },
    { pql::kAffects, &Cache::GenerateAffectsPairDomain },
    { pql::kAffectsT, &Cache::GenerateNextTOrAffectsTPairDomain }
  };

  bool Cache::IsComputeRelHolds(pql::RelationshipTypes type, int left, int right) {
    if (!rel_cache_boolean_[type]) {
      GenerateCache fn = GenerateRelCacheMap.at(type);
      (this->*fn)(type);
    }
    
    if (rel_cache_[type].find(left) == rel_cache_[type].end()
        || find(rel_cache_[type][left].begin(), rel_cache_[type][left].end(), right) == rel_cache_[type][left].end()) {
      return false;
    } 
      
    return true;
  }

  vector<int> Cache::GetComputeRelDomain(pql::RelationshipTypes type, int left) {
    if (!rel_cache_boolean_[type]) {
      GenerateCache fn = GenerateRelCacheMap.at(type);
      (this->*fn)(type);
    }
    
    return rel_cache_[type].find(left) == rel_cache_[type].end() 
        ? vector<int>() : vector<int>(rel_cache_[type][left].begin(), rel_cache_[type][left].end());
  }


  vector<int> Cache::GetComputeInverseRelDomain(pql::RelationshipTypes type, int right) {
    if (!inverse_rel_cache_boolean_[type]) {
      GenerateCache fn = GenerateInverseRelCacheMap.at(type);
      (this->*fn)(type);
    }

    return inverse_rel_cache_[type].find(right) == inverse_rel_cache_[type].end() 
        ? vector<int>() : vector<int>(inverse_rel_cache_[type][right].begin(), inverse_rel_cache_[type][right].end());
  }

  vector<pair<int, int>> Cache::GetComputeAllRelPairs(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateCache fn = GeneratePairCacheMap.at(type);
      (this->*fn)(type);
    }

    return vector<pair<int,int>> (pair_cache_[type].begin(), pair_cache_[type].end());
  }

  bool Cache::IsComputeRelExists(pql::RelationshipTypes type) {
    if (type == pql::kNextT) {
      return pkb_->IsRelationshipExists(pql::kNext);
    } 
      
    if (!pair_cache_boolean_[type]) {
      GenerateCache fn = GeneratePairCacheMap.at(type);
      (this->*fn)(type);
    }

    return !pair_cache_[type].empty();
  }  

  const unordered_set<pql::RelationshipTypes> NonPrecomputeRelationshipTypeMap = { pql::kAffects, pql::kAffectsT, pql::kNextT };

  bool Cache::IsRelationshipHolds(pql::RelationshipTypes rel_types, int key, int value) {
    if (NonPrecomputeRelationshipTypeMap.find(rel_types) != NonPrecomputeRelationshipTypeMap.end()) {
      return IsComputeRelHolds(rel_types, key, value);
    }

    return pkb_->IsRelationshipHolds(rel_types, key, value);
  }

  bool Cache::IsRelationshipExists(pql::RelationshipTypes rel_types) {
    if (NonPrecomputeRelationshipTypeMap.find(rel_types) != NonPrecomputeRelationshipTypeMap.end()) {
      return IsComputeRelExists(rel_types);
    }

    return pkb_->IsRelationshipExists(rel_types);
  }

  vector<int> Cache::GetRelFirstArgument(pql::RelationshipTypes rel_types, int second_arg_idx) {
    if (NonPrecomputeRelationshipTypeMap.find(rel_types) != NonPrecomputeRelationshipTypeMap.end()) {
      return GetComputeInverseRelDomain(rel_types, second_arg_idx);
    }

    return pkb_->GetRelFirstArgument(rel_types, second_arg_idx);
  }

  vector<int> Cache::GetRelSecondArgument(const pql::RelationshipTypes rel_types, const int first_arg_idx) {
    if (NonPrecomputeRelationshipTypeMap.find(rel_types) != NonPrecomputeRelationshipTypeMap.end()) {
      return GetComputeRelDomain(rel_types, first_arg_idx);
    }

    return pkb_->GetRelSecondArgument(rel_types, first_arg_idx);
  }

  vector<pair<int, int>> Cache::GetRelArgumentPairs(const pql::RelationshipTypes rel_types) {
    if (NonPrecomputeRelationshipTypeMap.find(rel_types) != NonPrecomputeRelationshipTypeMap.end()) {
      return GetComputeAllRelPairs(rel_types);
    }

    return pkb_->GetRelArgumentPairs(rel_types);
  }

  /*--------------------------------------------------API for pattern clause------------------------------------------------------------*/
  unordered_set<int> Cache::GetAllStmtsWithPattern(const string& pattern, bool is_exact) {
    return pkb_->GetAllStmtsWithPattern(pattern, is_exact);
  }

  vector<pair<int, int>> Cache::GetContainerStmtVarPair(TableIdentifier table_identifier) {
    return pkb_->GetContainerStmtVarPair(table_identifier);
  }

  unordered_set<int> Cache::GetAllStmtsWithPatternVariable(int pattern_var_idx, TableIdentifier table_identifier) {
    return pkb_->GetAllStmtsWithPatternVariable(pattern_var_idx, table_identifier);
  }

  /*-----------------------------------------------------Next* and Affects*------------------------------------------------------------*/  
  unordered_map<int, unordered_set<int>> ConvertNextTableToTableOfSet(unordered_map<int, vector<int>>& next_table) {
    unordered_map<int, unordered_set<int>> res;

    for (auto it = next_table.begin(); it != next_table.end(); it++) {
      res[it->first] = unordered_set<int>(it->second.begin(), it->second.end());
    }

    return res;
  }

  void Cache::GenerateNextTOrAffectsTRelDomain(pql::RelationshipTypes type) {
    unordered_map<int, unordered_set<int>> rel_domain;
    unordered_map<int, unordered_set<int>> star_rel_domain;
      
    if (type == pql::kNextT) {
      unordered_map<int, vector<int>> next_table = pkb_->GetNextInternalMap();
      rel_domain = move(ConvertNextTableToTableOfSet(next_table));
      star_rel_domain = move(generator_.PopulateStarRelationship(rel_domain));

    } else {
      //the type here will be AffectsT
      if (!rel_cache_boolean_[pql::kAffects]) {
        GenerateAffectsRelDomain(pql::kAffects);
      }

      star_rel_domain = move(generator_.PopulateStarRelationship(rel_cache_[pql::kAffects]));
    }

    rel_cache_[type] = move(star_rel_domain);
    rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateNextTOrAffectsTInverseRelDomain(pql::RelationshipTypes type) {
    if (!rel_cache_boolean_[type]) {
      GenerateNextTOrAffectsTRelDomain(type);
    }

    inverse_rel_cache_[type] = move(generator_.GenerateInverseRelDomainFromRelDomain(rel_cache_[type]));
    inverse_rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateNextTOrAffectsTPairDomain(pql::RelationshipTypes type) {
    if (!rel_cache_boolean_[type]) {
      GenerateNextTOrAffectsTRelDomain(type);
    }

    pair_cache_[type] = move(generator_.GeneratePairDomainFromRelDomain(rel_cache_[type]));
    pair_cache_boolean_[type] = true;
  }

  /*---------------------------------------------------------Affect--------------------------------------------------------------*/
  void Cache::GenerateAffectsRelDomain(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateAffectsPairDomain(type);
    }

    rel_cache_[type] = move(generator_.GenerateRelDomainFromPairDomain(pair_cache_[type]));
    rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateAffectsInverseRelDomain(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateAffectsPairDomain(type);
    }

    inverse_rel_cache_[type] = move(generator_.GenerateInverseRelDomainFromPairDomain(pair_cache_[type]));
    inverse_rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateAffectsPairDomain(pql::RelationshipTypes type) {
    vector<shared_ptr<cfg::CFG>> cfg_lst = pkb_->GetCfgList();

    for (auto& cfg_ptr : cfg_lst) {
      generator_.ComputeAffectsRelationship(*cfg_ptr->GetHead());  
    }

    pair_cache_boolean_[pql::kAffects] = true;
  }
}