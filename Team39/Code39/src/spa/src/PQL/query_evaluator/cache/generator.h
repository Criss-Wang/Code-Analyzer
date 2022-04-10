#ifndef GENERATOR_H
#define GENERATOR_H

#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <memory>


#include "../../../PKB/pkb.h"
#include "../../query_parser/token.h"
#include "../../../Utility/CFG/control_flow_graph.h"

namespace pql_cache {
  class Generator {
    public:
      Pkb* pkb_;
      std::unordered_set<std::pair<int, int>, hash_pair_fn>* affects_pair_domain_;
  
    public:
      static std::unordered_map<int, std::unordered_set<int>> PopulateStarRelationship(std::unordered_map<int, std::unordered_set<int>>& rel_table);

      std::unordered_map<int, std::unordered_set<int>> GenerateRelDomainFromPairDomain(std::unordered_set<std::pair<int, int>, hash_pair_fn>& pair_domain);

      std::unordered_map<int, std::unordered_set<int>> GenerateInverseRelDomainFromPairDomain(std::unordered_set<std::pair<int, int>, hash_pair_fn>& pair_domain);

      std::unordered_map<int, std::unordered_set<int>> GenerateInverseRelDomainFromRelDomain(std::unordered_map<int, std::unordered_set<int>>& rel_domain);

      std::unordered_set<std::pair<int, int>, hash_pair_fn> GeneratePairDomainFromRelDomain(std::unordered_map<int, std::unordered_set<int>>& rel_domain);

      void ComputeAffectsRelationship(cfg::GraphNode& head);

      void ConstructAndAddAssignAffectPair(int assign_stmt, unordered_map<int, unordered_set<int>>& last_modified_table);
      
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
