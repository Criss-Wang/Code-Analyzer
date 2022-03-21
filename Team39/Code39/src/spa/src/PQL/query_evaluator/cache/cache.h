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

    private:
      std::vector<std::pair<int, int>> ComputeNextTRelationship(GraphNode& head);

      std::vector<std::pair<int, int>> ComputeAffectsRelationship(GraphNode& head);

    public:
      pair<shared_ptr<GraphNode>, vector<pair<int, int>>> 
          ComputeStmtAffects(GraphNode& node, unordered_map<int, int>& last_modified_table);
  };
}
