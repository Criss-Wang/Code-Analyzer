#include <unordered_map>
#include <vector>

#include "../../query_parser/token.h"
#include "../../../PKB/pkb.h"
#include "../../../Utility/CFG/control_flow_graph.h"

//Cache computes the Next* and Affect relationship
//It will stores the computed result to speed up the evaulaution process
namespace pql_cache {
  class Cache {
    private:
      std::unordered_map<pql::RelationshipTypes, std::vector<std::pair<int, int>>> cache_;
      Pkb pkb_;

    public:
      Cache(Pkb& pkb) {
        pkb_ = pkb;
      }

    public:
      std::vector<std::pair<int, int>> ComputeNextTRelationship(GraphNode& head);

      std::vector<std::pair<int, int>> ComputeAffectsRelationship(GraphNode& head);

      void ConstructAssignAffectPair(int assign_stmt,
         std::unordered_map<int, std::unordered_set<int>>& last_modified_table, vector<pair<int, int>>& affect_lst);
      
      static void MergeTable(unordered_map<int, unordered_set<int>>& dst, unordered_map<int, unordered_set<int>>& src);
  };
}
