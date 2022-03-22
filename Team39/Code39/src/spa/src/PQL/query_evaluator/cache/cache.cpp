#include <memory>
#include <map>

#include "cache.h"

using namespace std;

namespace pql_cache {

  vector<pair<int, int>> Cache::ComputeNextTRelationship(GraphNode& head) {
    return vector<pair<int, int>>();
  }

  /*-----------------------------------------Affect-----------------------------------------------------*/
  vector<pair<int, int>> ConstructAffectPair(vector<int>& lefts, int right) {
    vector<pair<int, int>> res_lst;

    for (int& left : lefts) {
      res_lst.push_back(make_pair(left, right));
    }

    return res_lst;
  }
  
  void Cache::ConstructAssignAffectPair(int assign_stmt,
      unordered_map<int, vector<int>>& last_modified_table, vector<pair<int, int>>& affect_lst ) {
    vector<int> used_vars = pkb_.GetUsesVarByStmt(assign_stmt);

    for (int& used_var : used_vars) {
        //check used_var in LMT
      if (last_modified_table.find(used_var) != last_modified_table.end()) {
        vector<pair<int, int>> curr_res_lst = ConstructAffectPair(last_modified_table[used_var], assign_stmt);
        affect_lst.insert(affect_lst.end(), curr_res_lst.begin(), curr_res_lst.end());
      }
    }
  }

  vector<pair<int, int>> Cache::ComputeAffectsRelationship(GraphNode& head) {
    //LMT maps the variable to the stmt that modifies it  
    //It is mapped to a vector because we could have multiple assign statements modifying same variable
    //e.g. if (1==1) then {a = a + 1;} else {a = a + 1;}
    unordered_map<int, vector<int>> last_modified_table;
    vector<pair<int, int>> affect_lst;

    shared_ptr<GraphNode> curr = head.GetNext();

    while (curr->GetNodeType() != NodeType::END) {
      if (curr->GetNodeType() == NodeType::STMT) {
        int start = curr->GetStart();
        int end = curr->GetEnd();

        for (int curr_stmt = start; curr_stmt <= end; curr_stmt++) {
          //only 4 possible types: assign, call, print, read
          EntityIdentifier curr_type = curr->GetStmtType(curr_stmt);

          if (curr_type == EntityIdentifier::kAssign) {
            int modvar = pkb_.GetModifiesVarByStmt(curr_stmt)[0];
            ConstructAssignAffectPair(curr_stmt, last_modified_table, affect_lst);
            //Add LastModified(modvar, curr_stmt)
            last_modified_table[modvar] = vector<int>{ curr_stmt };
          }

          if (curr_type == EntityIdentifier::kRead) {
            int read_var = pkb_.GetVarFromRead(curr_stmt);
            last_modified_table.erase(read_var);
          }

          if (curr_type == EntityIdentifier::kCall) {
            vector<int> call_modvars = pkb_.GetModifiesVarByStmt(curr_stmt);
            for (int& call_modvar : call_modvars) {
              last_modified_table.erase(call_modvar);
            }
          }
        }

        curr = curr->GetNext();
      } else if () {

      }
    }

    return affect_lst;
  }
}