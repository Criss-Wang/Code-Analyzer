#include <memory>
#include <map>
#include<stack>

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
    stack<unordered_map<int, vector<int>>> last_modified_stack;
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

      } else if (curr->GetNodeType() == NodeType::IF) {

        unordered_map<int, vector<int>> last_modified_table_else = last_modified_table;
        //we push this copy for else branch later
        last_modified_stack.push(last_modified_table_else);
        ptr_stack.push(curr);
        curr = curr->GetNext();

      } else if (curr->GetNodeType() == NodeType::WHILE) {
        //make a copy
        unordered_map<int, vector<int>> before_last_modified_table = last_modified_table;
        last_modified_stack.push(before_last_modified_table);
        ptr_stack.push(curr);
        curr = curr->GetNext();

      } else if (curr->GetNodeType() == NodeType::THENEND) {

        unordered_map<int, vector<int>> last_modified_table_else = move(last_modified_stack.top());
        last_modified_stack.pop();
        //store the current LMT to be merge after else branch is finish
        last_modified_stack.push(last_modified_table);
        //we do a move here since moving is faster than copying and last_modified_table_else will not be reference anymore
        last_modified_table = move(last_modified_table_else);
        
        shared_ptr<GraphNode> if_node = move(ptr_stack.top());
        ptr_stack.pop();
        curr = if_node->GetAlternative();

      } else if (curr->GetNodeType() == NodeType::IFEND) {
        
        unordered_map<int, vector<int >> last_modified_table_then = move(last_modified_stack.top());
        last_modified_stack.pop();
        //merge the two tables

        curr = curr->GetNext();

      } else {
        //It will be WhileEnd until this point

        shared_ptr<GraphNode> while_node = move(ptr_stack.top());
        ptr_stack.pop();
        unordered_map<int, vector<int >> before_last_modified_table = move(last_modified_stack.top());
        last_modified_stack.pop();

        if (last_modified_table == before_last_modified_table) {
          curr = while_node->GetAlternative();
        } else {
          //merge the two tables
          curr = while_node;
        }
      }
    }

    return affect_lst;
  }
}