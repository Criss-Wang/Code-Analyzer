#ifndef CACHE_H
#define CACHE_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>

#include "../../query_parser/token.h"
#include "../../../PKB/pkb.h"
#include "../../../Utility/CFG/control_flow_graph.h"

//Cache computes the Next* and Affect relationship
//It will stores the computed result to speed up the evaulaution process
namespace pql_cache {
  struct hash_pair_fn {
    std::size_t operator() (const std::pair<int, int>& p) const {
      std::size_t h1 = std::hash<int>{}(p.first);
      std::size_t h2 = std::hash<int>{}(p.second);

      return h1 ^ h2;
    }
  };

  const unordered_set<pql::RelationshipTypes> cache_store_type = {
    pql::kNextT, pql::kAffects, pql::kAffectsT
  };

  class Cache {
    public:
      //pair_cache_ stores the pair relationship such that all <x,y> in vector satisfy the relationship
      //For clauses like Next(syn, syn) 
      std::unordered_map<pql::RelationshipTypes, std::unordered_set<std::pair<int, int>, hash_pair_fn>> pair_cache_;
      std::unordered_map<pql::RelationshipTypes, bool> pair_cache_boolean_;
      //rel_cache_ stores the table mapping the key to vector of values, where <key, value> satisfy the relationship
      //For clauses like Next(ent, syn) or Next(ent, ent)
      std::unordered_map<pql::RelationshipTypes, std::unordered_map<int, std::unordered_set<int>>> rel_cache_;
      std::unordered_map<pql::RelationshipTypes, bool> rel_cache_boolean_;
      //inverse_rel_cache_ stores the table mapping the key to vector of values, where <value, key> satisfy the relationship
      //For clauses like Next(syn, ent)
      std::unordered_map<pql::RelationshipTypes, std::unordered_map<int, std::unordered_set<int>>> inverse_rel_cache_;
      std::unordered_map<pql::RelationshipTypes, bool> inverse_rel_cache_boolean_;

      Pkb* pkb_;

    public:
      Cache(Pkb* pkb) {
        pkb_ = pkb;

        //we havent create any table for these types
        for (auto type : cache_store_type) {
          pair_cache_boolean_[type] = false;
          rel_cache_boolean_[type] = false;
          inverse_rel_cache_boolean_[type] = false;
        }
      }

    public:
    /*-------------------------------------------------------API for domain---------------------------------------------------------*/
      unordered_set<int> GetAllEntity(const EntityIdentifier entity_identifier);

    /*-------------------------------------------------------API for clause----------------------------------------------------------*/
      bool IsComputeRelHolds(pql::RelationshipTypes type, int left, int right);

      std::vector<int> GetComputeRelDomain(pql::RelationshipTypes type, int left);

      std::vector<int> GetComputeInverseRelDomain(pql::RelationshipTypes type, int right);

      std::vector<std::pair<int, int>> GetComputeAllRelPairs(pql::RelationshipTypes type);

      bool IsComputeRelExists(pql::RelationshipTypes type);

      //Overload pkb's function
      bool IsRelationshipHolds(pql::RelationshipTypes rel_types, int key, int value);

      bool IsRelationshipExists(pql::RelationshipTypes rel_types);

      vector<int> GetRelFirstArgument(pql::RelationshipTypes rel_types, int second_arg_idx);

      vector<int> GetRelSecondArgument(const pql::RelationshipTypes rel_types, const int first_arg_idx);

      vector<pair<int, int>> GetRelArgumentPairs(const pql::RelationshipTypes rel_types);

    /*--------------------------------------------------API for attribute------------------------------------------------------------*/
      int GetIndexByString(IndexTableType index_table_type, const string& entity_name);

      std::vector<int> GetStmtNumByStringAttribute(EntityIdentifier entity_identifier, const int string_idx);

      int GetStringAttribute(EntityIdentifier entity_identifier, const int stmt_no);

      string GetStringByIndex(IndexTableType index_table_type, int idx);

    /*--------------------------------------------------API for pattern clause------------------------------------------------------------*/
      unordered_set<int> GetAllStmtsWithPattern(const string& pattern, bool is_exact);

      vector<pair<int, int>> GetContainerStmtVarPair(TableIdentifier table_identifier);

      unordered_set<int> GetAllStmtsWithPatternVariable(int pattern_var_idx, TableIdentifier table_identifier);

    /*---------------------------------------------------Next* and Affects*----------------------------------------------------------*/
      void GenerateNextTOrAffectsTRelDomain(pql::RelationshipTypes type);

      void GenerateNextTOrAffectsTInverseRelDomain(pql::RelationshipTypes type);

      void GenerateNextTOrAffectsTPairDomain(pql::RelationshipTypes type);

      static unordered_map<int, unordered_set<int>> PopulateStarRelationship(unordered_map<int, unordered_set<int>>& rel_table);
    /*---------------------------------------------------------Affect---------------------------------------------------------------*/
      void GenerateAffectsRelDomain(pql::RelationshipTypes type);

      void GenerateAffectsInverseRelDomain(pql::RelationshipTypes type);

      void GenerateAffectsPairDomain(pql::RelationshipTypes type);

      void ComputeAffectsRelationship(cfg::GraphNode& head);

      void ConstructAndAddAssignAffectPair(int assign_stmt, std::unordered_map<int, std::unordered_set<int>>& last_modified_table);
      
      static void MergeTable(unordered_map<int, unordered_set<int>>& dst, unordered_map<int, unordered_set<int>>& src);      

      void ComputeAffectsRelationshipForStmt(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
          stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack);
    
      void ComputeAffectsRelationshipForIf(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
          stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack);

      void ComputeAffectsRelationshipForWhile(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
          stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack);

      void ComputeAffectsRelationshipForThenEnd(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
          stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack);

      void ComputeAffectsRelationshipForIfEnd(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
          stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack);

      void ComputeAffectsRelationshipForWhileEnd(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
          stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack);
  };
}

#endif
