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
        ? vector<int>() : rel_cache_[type][left];
  }


  vector<int> Cache::GetComputeInverseRelDomain(pql::RelationshipTypes type, int right) {
    if (!inverse_rel_cache_boolean_[pql::kNextT]) {
      GenerateCache fn = GenerateInverseRelCacheMap.at(type);
      (this->*fn)(type);
    }

    return inverse_rel_cache_[type].find(right) == inverse_rel_cache_[type].end() 
        ? vector<int>() : inverse_rel_cache_[type][right];
  }

  vector<pair<int, int>> Cache::GetComputeAllRelPairs(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateCache fn = GeneratePairCacheMap.at(type);
      (this->*fn)(type);
    }

    return pair_cache_[type];
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

  int Dfs(unordered_map<int, vector<int>>& table_to_refer, unordered_map<int, vector<int>>& table_to_update, int key) {
    if (table_to_update.find(key) != table_to_update.end()
        || table_to_refer.find(key) != table_to_refer.end()) {
      //we immediately return if the key is already populated
      //or the key does not have any relationship
      return key;
    }

    vector<int> children_lst = table_to_refer[key];
    vector<int> ans;

    for (int child_key : children_lst) {
      if (child_key == key) {
        //we skip to prevent infinite loop since it is possible to have Next(2,2)
        continue;
      }

      int end_val = Dfs(table_to_refer, table_to_update, child_key);
      ans.push_back(end_val);

      if (table_to_update.find(end_val) != table_to_update.end()) {
        //children of child key wil satisfy the star relationship
        vector<int> value = table_to_update[end_val];
        ans.insert(ans.end(), value.begin(), value.end());
      }
    }

    if (!ans.empty()) {
      table_to_update[key] = move(ans);
    }

    return key;
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

  void Cache::GenerateNextTOrAffectsTRelDomain(pql::RelationshipTypes type) {
    unordered_map<int, vector<int>> rel_domain;
    unordered_map<int, vector<int>> star_rel_domain;
      
    if (type == pql::kNextT) {
      rel_domain = pkb_->GetNextInternalMap();
    } else {
      //the type here will be AffectsT
      if (!rel_cache_boolean_[pql::kAffects]) {
        GenerateAffectsRelDomain(pql::kAffects);
      }
    }

    for (auto it = rel_domain.begin(); it != rel_domain.end(); it++) {
      Dfs(rel_domain, star_rel_domain, it->first);
    }

    rel_cache_[type] = move(star_rel_domain);
    rel_cache_boolean_[type] = true;
  }

  void Cache::GenerateNextTOrAffectsTInverseRelDomain(pql::RelationshipTypes type) {
    if (!rel_cache_boolean_[type]) {
      GenerateNextTOrAffectsTRelDomain(type);
    }

    unordered_map<int, vector<int>> inverse_table;

    for (auto it = rel_cache_[type].begin(); it != rel_cache_[type].end(); it++) {
      for (int& right : it->second) {
        if (inverse_table.find(right) == inverse_table.end()) {
          inverse_table[right] = vector<int>{ it->first };
        } else {
          inverse_table[right].push_back(it->first);
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

    vector<pair<int, int>> res;

    for (auto it = rel_cache_[type].begin(); it != rel_cache_[type].end(); it++) {
      for (int& right : it->second) {
        res.push_back(make_pair(it->first, right));
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

    unordered_map<int, vector<int>> rel_table;

    for (auto& pair : pair_cache_[pql::kAffects]) {
      if (rel_table.find(pair.first) == rel_table.end()) {
        rel_table[pair.first] = vector<int>{ pair.second };
      } else {
        rel_table[pair.first].push_back(pair.second);
      }
    }

    rel_cache_[pql::kAffects] = move(rel_table);
    rel_cache_boolean_[pql::kAffects] = true;
  }

  void Cache::GenerateAffectsInverseRelDomain(pql::RelationshipTypes type) {
    if (!pair_cache_boolean_[type]) {
      GenerateAffectsPairDomain(type);
    }

    unordered_map<int, vector<int>> inverse_rel_table;

    for (auto& pair : pair_cache_[pql::kAffects]) {
      if (inverse_rel_table.find(pair.second) == inverse_rel_table.end()) {
        inverse_rel_table[pair.second] = vector<int>{ pair.first };
      } else {
        inverse_rel_table[pair.second].push_back(pair.first);
      }
    }

    inverse_rel_cache_[pql::kAffects] = move(inverse_rel_table);
    inverse_rel_cache_boolean_[pql::kAffects] = true;
  }

  void Cache::GenerateAffectsPairDomain(pql::RelationshipTypes type) {
    vector<shared_ptr<CFG>> cfg_lst = pkb_->GetCfgList();
    vector<pair<int, int>> affects_lst;

    for (auto& cfg_ptr : cfg_lst) {
      unordered_set<pair<int, int>, hash_pair_fn> curr_affects_set = move(ComputeAffectsRelationship(*cfg_ptr->GetHead()));
      affects_lst.insert(affects_lst.end(), curr_affects_set.begin(), curr_affects_set.end());
    }

    pair_cache_[pql::kAffects] = move(affects_lst);
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

  void Cache::ConstructAssignAffectPair(int assign_stmt,
      unordered_map<int, unordered_set<int>>& last_modified_table, unordered_set<pair<int, int>, hash_pair_fn>& affect_set ) {
    vector<int> used_vars = pkb_->GetRelSecondArgument(pql::RelationshipTypes::kUsesS, assign_stmt);

    for (int& used_var : used_vars) {
        //check used_var in LMT
      if (last_modified_table.find(used_var) != last_modified_table.end()) {
        for (const int left : last_modified_table[used_var]) {
          affect_set.insert(make_pair(left, assign_stmt));
        }
      }
    }
  }

  unordered_set<pair<int, int>, hash_pair_fn> Cache::ComputeAffectsRelationship(GraphNode& head) {
    //LMT maps the variable to the stmt that modifies it  
    //It is mapped to a vector because we could have multiple assign statements modifying same variable
    //e.g. if (1==1) then {a = a + 1;} else {a = a + 1;}
    unordered_map<int, unordered_set<int>> last_modified_table;
    unordered_set<pair<int, int>, hash_pair_fn> affect_set;
    stack<unordered_map<int, unordered_set<int>>> last_modified_stack;
    stack<shared_ptr<GraphNode>> ptr_stack;
    
    shared_ptr<GraphNode> curr = head.GetNext();

    while (curr->GetNodeType() != NodeType::END) {
      if (curr->GetNodeType() == NodeType::STMT) {
        int start = curr->GetStart();
        int end = curr->GetEnd();

        for (int curr_stmt = start; curr_stmt <= end; curr_stmt++) {
          //only 4 possible types: assign, call, print, read
          EntityIdentifier curr_type = curr->GetStmtType(curr_stmt);

          if (curr_type == EntityIdentifier::kAssign) {
            int modvar = pkb_->GetRelSecondArgument(pql::RelationshipTypes::kModifiesS, curr_stmt)[0];
            ConstructAssignAffectPair(curr_stmt, last_modified_table, affect_set);
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

      } else if (curr->GetNodeType() == NodeType::IF) {

        unordered_map<int, unordered_set<int>> last_modified_table_else = last_modified_table;
        //we push this copy for else branch later
        last_modified_stack.push(last_modified_table_else);
        ptr_stack.push(curr);
        curr = curr->GetNext();

      } else if (curr->GetNodeType() == NodeType::WHILE) {
        //make a copy
        unordered_map<int, unordered_set<int>> before_last_modified_table = last_modified_table;
        last_modified_stack.push(before_last_modified_table);
        ptr_stack.push(curr);
        curr = curr->GetNext();

      } else if (curr->GetNodeType() == NodeType::THENEND) {

        unordered_map<int, unordered_set<int>> last_modified_table_else = move(last_modified_stack.top());
        last_modified_stack.pop();
        //store the current LMT to be merge after else branch is finish
        last_modified_stack.push(last_modified_table);
        //we do a move here since moving is faster than copying and last_modified_table_else will not be reference anymore
        last_modified_table = move(last_modified_table_else);
        
        shared_ptr<GraphNode> if_node = move(ptr_stack.top());
        ptr_stack.pop();
        curr = if_node->GetAlternative();

      } else if (curr->GetNodeType() == NodeType::IFEND) {
        
        unordered_map<int, unordered_set<int >> last_modified_table_then = move(last_modified_stack.top());
        last_modified_stack.pop();
        MergeTable(last_modified_table, last_modified_table_then);
        curr = curr->GetNext();

      } else {
        //It will be WhileEnd until this point

        shared_ptr<GraphNode> while_node = move(ptr_stack.top());
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

    return affect_set;
  }
}