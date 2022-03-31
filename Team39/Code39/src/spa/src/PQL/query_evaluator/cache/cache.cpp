#include <map>
#include <memory>
#include <unordered_map>
#include <stack>

#include "cache.h"
#include "../../../PKB/tables/table.h"

using namespace std;

namespace pql_cache {

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

  const map<pql::RelationshipTypes, GenerateCache> GenerateRelCacheMap = {
    { pql::kNextT, &Cache::GenerateNextTOrAffectsTRelDomain },
    { pql::kAffects, &Cache::GenerateAffectsRelDomain },
    { pql::kAffectsT, &Cache::GenerateNextTOrAffectsTRelDomain }
  };

  const map<pql::RelationshipTypes, GenerateCache> GenerateInverseRelCacheMap = {
    { pql::kNextT, &Cache::GenerateNextTOrAffectsTInverseRelDomain },
    { pql::kAffects, &Cache::GenerateAffectsInverseRelDomain },
    { pql::kAffectsT, &Cache::GenerateNextTOrAffectsTInverseRelDomain }
  };

  const map<pql::RelationshipTypes, GenerateCache> GeneratePairCacheMap = {
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
  bool AddGrandchildrenToNeigh(unordered_map<int, unordered_set<int>>& rel_table, unordered_set<int>& neighs, int child) {
    if (rel_table.find(child) == rel_table.end()) {
       //the neighs is not modified
       return false;
    }

    unordered_set<int> copy_of_child_neighs = rel_table[child];
    neighs.merge(copy_of_child_neighs);

    //if the size of the child neighs copy is the same as original, means we never added any new child to neighs
    return copy_of_child_neighs.size() != rel_table[child].size();
  }


  unordered_map<int, unordered_set<int>> Cache::PopulateStarRelationship(unordered_map<int, unordered_set<int>>& rel_table) {
    unordered_map<int, unordered_set<int>> curr_table = rel_table;
    bool is_same = true;

    do {
      is_same = true;
      for (auto& it = curr_table.begin(); it != curr_table.end(); it++) {
        unordered_set<int> curr_neigh = it->second;
        for (const int child : curr_neigh) {
          if (child == it->first) {
            continue;
          }
          //merge the neighbours of the child to the current neighbour list
          bool is_modified = AddGrandchildrenToNeigh(curr_table, it->second, child);

          is_same &= !is_modified;
          
        }
      }

    } while (!is_same);

    return curr_table;
  }
  
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
      star_rel_domain = move(PopulateStarRelationship(rel_domain));

    } else {
      //the type here will be AffectsT
      if (!rel_cache_boolean_[pql::kAffects]) {
        GenerateAffectsRelDomain(pql::kAffects);
      }

      star_rel_domain = move(PopulateStarRelationship(rel_cache_[pql::kAffects]));
    }

    rel_cache_[type] = move(star_rel_domain);
    rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateNextTOrAffectsTInverseRelDomain(pql::RelationshipTypes type) {
    if (!rel_cache_boolean_[type]) {
      GenerateNextTOrAffectsTRelDomain(type);
    }

    unordered_map<int, unordered_set<int>> inverse_table;

    for (auto it = rel_cache_[type].begin(); it != rel_cache_[type].end(); it++) {
      for (const int right : it->second) {
        if (inverse_table.find(right) == inverse_table.end()) {
          inverse_table[right] = unordered_set<int>{ it->first };
        } else {
          inverse_table[right].insert(it->first);
        }
      }
    }

    inverse_rel_cache_[type] = move(inverse_table);
    inverse_rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateNextTOrAffectsTPairDomain(pql::RelationshipTypes type) {
    if (!rel_cache_boolean_[type]) {
      GenerateNextTOrAffectsTRelDomain(type);
    }

    unordered_set<pair<int, int>, hash_pair_fn> res;

    for (auto it = rel_cache_[type].begin(); it != rel_cache_[type].end(); it++) {
      for (const int right : it->second) {
        res.insert(make_pair(it->first, right));
      }
    }

    pair_cache_[type] = move(res);
    pair_cache_boolean_[type] = true;
  }

  /*---------------------------------------------------------Affect--------------------------------------------------------------*/
  void Cache::GenerateAffectsRelDomain(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateAffectsPairDomain(type);
    }

    unordered_map<int, unordered_set<int>> rel_table;

    for (auto& pair : pair_cache_[pql::kAffects]) {
      if (rel_table.find(pair.first) == rel_table.end()) {
        rel_table[pair.first] = unordered_set<int>{ pair.second };
      } else {
        rel_table[pair.first].insert(pair.second);
      }
    }

    rel_cache_[pql::kAffects] = move(rel_table);
    rel_cache_boolean_[pql::kAffects] = true;
  }

  void Cache::GenerateAffectsInverseRelDomain(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateAffectsPairDomain(type);
    }

    unordered_map<int, unordered_set<int>> inverse_rel_table;

    for (auto& pair : pair_cache_[pql::kAffects]) {
      if (inverse_rel_table.find(pair.second) == inverse_rel_table.end()) {
        inverse_rel_table[pair.second] = unordered_set<int>{ pair.first };
      } else {
        inverse_rel_table[pair.second].insert(pair.first);
      }
    }

    inverse_rel_cache_[pql::kAffects] = move(inverse_rel_table);
    inverse_rel_cache_boolean_[pql::kAffects] = true;
  }

  void Cache::GenerateAffectsPairDomain(pql::RelationshipTypes type) {
    vector<shared_ptr<cfg::CFG>> cfg_lst = pkb_->GetCfgList();

    for (auto& cfg_ptr : cfg_lst) {
      ComputeAffectsRelationship(*cfg_ptr->GetHead());  
    }

    pair_cache_boolean_[pql::kAffects] = true;
  }

  void Cache::MergeTable(unordered_map<int, unordered_set<int>>& dst, unordered_map<int, unordered_set<int>>& src) {
    //merge from src into dst, will merge the vectors if the keys overlap
    dst.merge(src);

    for (unordered_map<int, unordered_set<int>>::iterator it = src.begin(); it != src.end(); it++) {
      int key = it->first;
      dst[key].merge(src[key]);
    }
  }

  void Cache::ConstructAndAddAssignAffectPair(int assign_stmt, unordered_map<int, unordered_set<int>>& last_modified_table) {
    vector<int> used_vars = pkb_->GetRelSecondArgument(pql::RelationshipTypes::kUsesS, assign_stmt);

    for (int& used_var : used_vars) {
        //check used_var in LMT
      if (last_modified_table.find(used_var) != last_modified_table.end()) {
        for (const int left : last_modified_table[used_var]) {
          pair_cache_[pql::kAffects].insert(make_pair(left, assign_stmt));
        }
      }
    }
  }

  void Cache::ComputeAffectsRelationship(cfg::GraphNode& head) {
    //LMT maps the variable to the stmt that modifies it  
    //It is mapped to a vector because we could have multiple assign statements modifying same variable
    //e.g. if (1==1) then {a = a + 1;} else {a = a + 1;}
    unordered_map<int, unordered_set<int>> last_modified_table;
    stack<unordered_map<int, unordered_set<int>>> last_modified_stack;
    stack<shared_ptr<cfg::GraphNode>> ptr_stack;
    
    shared_ptr<cfg::GraphNode> curr = head.GetNext();

    while (curr->GetNodeType() != cfg::NodeType::END) {
      if (curr->GetNodeType() == cfg::NodeType::STMT) {
        int start = curr->GetStart();
        int end = curr->GetEnd();

        for (int curr_stmt = start; curr_stmt <= end; curr_stmt++) {
          //only 4 possible types: assign, call, print, read
          EntityIdentifier curr_type = curr->GetStmtType(curr_stmt);

          if (curr_type == EntityIdentifier::kAssign) {
            int modvar = pkb_->GetRelSecondArgument(pql::RelationshipTypes::kModifiesS, curr_stmt)[0];
            ConstructAndAddAssignAffectPair(curr_stmt, last_modified_table);
            //Add LastModified(modvar, curr_stmt)
            last_modified_table[modvar] = unordered_set<int>{ curr_stmt };
          }

          if (curr_type == EntityIdentifier::kRead) {
            int read_var = pkb_->GetStringAttribute(curr_type, curr_stmt);
            last_modified_table.erase(read_var);
          }

          if (curr_type == EntityIdentifier::kCall) {
            vector<int> call_modvars = pkb_->GetRelSecondArgument(pql::RelationshipTypes::kModifiesS, curr_stmt);
            for (int& call_modvar : call_modvars) {
              last_modified_table.erase(call_modvar);
            }
          }
        }

        curr = curr->GetNext();

      } else if (curr->GetNodeType() == cfg::NodeType::IF) {

        unordered_map<int, unordered_set<int>> last_modified_table_else = last_modified_table;
        //we push this copy for else branch later
        last_modified_stack.push(last_modified_table_else);
        ptr_stack.push(curr);
        curr = curr->GetNext();

      } else if (curr->GetNodeType() == cfg::NodeType::WHILE) {
        //make a copy
        unordered_map<int, unordered_set<int>> before_last_modified_table = last_modified_table;
        last_modified_stack.push(before_last_modified_table);
        ptr_stack.push(curr);
        curr = curr->GetNext();

      } else if (curr->GetNodeType() == cfg::NodeType::THENEND) {

        unordered_map<int, unordered_set<int>> last_modified_table_else = move(last_modified_stack.top());
        last_modified_stack.pop();
        //store the current LMT to be merge after else branch is finish
        last_modified_stack.push(last_modified_table);
        //we do a move here since moving is faster than copying and last_modified_table_else will not be reference anymore
        last_modified_table = move(last_modified_table_else);
        
        shared_ptr<cfg::GraphNode> if_node = move(ptr_stack.top());
        ptr_stack.pop();
        curr = if_node->GetAlternative();

      } else if (curr->GetNodeType() == cfg::NodeType::IFEND) {
        
        unordered_map<int, unordered_set<int >> last_modified_table_then = move(last_modified_stack.top());
        last_modified_stack.pop();
        MergeTable(last_modified_table, last_modified_table_then);
        curr = curr->GetNext();

      } else {
        //It will be WhileEnd until this point

        shared_ptr<cfg::GraphNode> while_node = move(ptr_stack.top());
        ptr_stack.pop();
        unordered_map<int, unordered_set<int >> before_last_modified_table = move(last_modified_stack.top());
        last_modified_stack.pop();

        if (last_modified_table == before_last_modified_table) {
          curr = while_node->GetAlternative();
        } else {
          MergeTable(last_modified_table, before_last_modified_table);
          curr = while_node;
        }
      }
    }
  }
}