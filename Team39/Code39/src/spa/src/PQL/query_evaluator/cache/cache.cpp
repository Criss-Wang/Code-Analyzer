#include <memory>
#include <map>

#include "cache.h"

using namespace std;

namespace pql_cache {

  vector<pair<int, int>> Cache::ComputeNextTRelationship(GraphNode& head) {
    return vector<pair<int, int>>();
  }

  /*-----------------------------------------Affect-----------------------------------------------------*/
  typedef pair<shared_ptr<GraphNode>, vector<pair<int, int>>>(Cache::* ComputeAffectsFn)(GraphNode&,
      unordered_map<int, int>&);

  const map<NodeType, ComputeAffectsFn> ComputeAffectsFnMap = {
    { NodeType::STMT, &Cache::ComputeStmtAffects }
  };

  pair<shared_ptr<GraphNode>, vector<pair<int, int>>> Cache::ComputeStmtAffects(GraphNode& node,
      unordered_map<int, int>& last_modified_table) {
    int start = node.GetStart();
    int end = node.GetEnd();

    for (int curr_stmt = start; curr_stmt <= end; curr_stmt++) {
      //only 4 possible types: assign, call, print, read
      EntityIdentifier curr_type = node.GetStmtType(curr_stmt);

      if (curr_type == EntityIdentifier::kAssign) {
        int modvar = pkb_.GetModifiesVarByStmt(curr_stmt)[0];
        vector<int> used_vars = pkb_.GetUsesVarByStmt(curr_stmt);

        for (int& used_var : used_vars) {
          
        }
      }
    }

    return make_pair(nullptr, vector<pair<int, int>>());
  }

  vector<pair<int, int>> Cache::ComputeAffectsRelationship(GraphNode& head) {
    //LMT maps the variable to the stmt that modifies it  
    unordered_map<int, int> last_modified_table;
    vector<pair<int, int>> affect_lst;

    shared_ptr<GraphNode> curr = head.GetNext();

    while (curr->GetNodeType() != NodeType::END) {
      ComputeAffectsFn fn = ComputeAffectsFnMap.at(curr->GetNodeType());
      pair<shared_ptr<GraphNode>, vector<pair<int, int>>> p = (this->*fn)(*curr, last_modified_table);
      shared_ptr<GraphNode> next_node = p.first;
      vector<pair<int, int>> res_lst = p.second;
      
      affect_lst.insert(affect_lst.end(), res_lst.begin(), res_lst.end());
      curr = next_node;
    }

    return affect_lst;
  }
}