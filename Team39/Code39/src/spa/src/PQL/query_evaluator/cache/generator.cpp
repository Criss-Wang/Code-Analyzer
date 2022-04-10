#include "generator.h"

using namespace std;

namespace pql_cache {
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

  unordered_map<int, unordered_set<int>> Generator::PopulateStarRelationship(unordered_map<int, unordered_set<int>>& rel_table) {
    unordered_map<int, unordered_set<int>> curr_table = rel_table;
    bool is_same = true;

    do {
      is_same = true;
      for (auto it = curr_table.begin(); it != curr_table.end(); it++) {
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

  unordered_map<int, unordered_set<int>> Generator::GenerateRelDomainFromPairDomain(
      unordered_set<pair<int, int>, hash_pair_fn>& pair_domain) {
    unordered_map<int, unordered_set<int>> rel_table;

    for (auto& pair : pair_domain) {
      if (rel_table.find(pair.first) == rel_table.end()) {
        rel_table[pair.first] = unordered_set<int>{ pair.second };
      } else {
        rel_table[pair.first].insert(pair.second);
      }
    }

    return rel_table;
  }

  unordered_map<int, unordered_set<int>> Generator::GenerateInverseRelDomainFromPairDomain(
      unordered_set<pair<int, int>, hash_pair_fn>& pair_domain) {
    unordered_map<int, unordered_set<int>> inverse_rel_table;

    for (auto& pair : pair_domain) {
      if (inverse_rel_table.find(pair.second) == inverse_rel_table.end()) {
        inverse_rel_table[pair.second] = unordered_set<int>{ pair.first };
      } else {
        inverse_rel_table[pair.second].insert(pair.first);
      }
    }

    return inverse_rel_table;
  }

  unordered_map<int, unordered_set<int>> Generator::GenerateInverseRelDomainFromRelDomain(
      unordered_map<int, unordered_set<int>>& rel_domain) {
     unordered_map<int, unordered_set<int>> inverse_table;

    for (auto it = rel_domain.begin(); it != rel_domain.end(); it++) {
      for (const int right : it->second) {
        if (inverse_table.find(right) == inverse_table.end()) {
          inverse_table[right] = unordered_set<int>{ it->first };
        } else {
          inverse_table[right].insert(it->first);
        }
      }
    }

    return inverse_table;
  }

  unordered_set<pair<int, int>, hash_pair_fn> Generator::GeneratePairDomainFromRelDomain(
      std::unordered_map<int, std::unordered_set<int>>& rel_domain) {
    unordered_set<pair<int, int>, hash_pair_fn> res;

    for (auto it = rel_domain.begin(); it != rel_domain.end(); it++) {
      for (const int right : it->second) {
        res.insert(make_pair(it->first, right));
      }
    }

    return res;
  }

  /*---------------------------------------------------------Affect--------------------------------------------------------------*/
  void Generator::MergeTable(unordered_map<int, unordered_set<int>>& dst, unordered_map<int, unordered_set<int>>& src) {
    //merge from src into dst, will merge the vectors if the keys overlap
    dst.merge(src);

    for (unordered_map<int, unordered_set<int>>::iterator it = src.begin(); it != src.end(); it++) {
      int key = it->first;
      dst[key].merge(src[key]);
    }
  }

  void Generator::ConstructAndAddAssignAffectPair(int assign_stmt, unordered_map<int, unordered_set<int>>& last_modified_table) {
    vector<int> used_vars = pkb_->GetRelSecondArgument(pql::RelationshipTypes::kUsesS, assign_stmt);

    for (int& used_var : used_vars) {
      //check used_var in LMT
      if (last_modified_table.find(used_var) != last_modified_table.end()) {
        for (const int left : last_modified_table[used_var]) {
          affects_pair_domain_->insert(make_pair(left, assign_stmt));
        }
      }
    }
  }

  bool CheckSubsetBetweenTables(unordered_map<int, unordered_set<int>>& x, unordered_map<int, unordered_set<int>>& y) {
    //A table x is considered a subset of table y 
    //if there does not exist a pair <key, val> that exist in  x but not in y
    for (auto it = x.begin(); it != x.end(); it++) {
      //check the existence of the key 
      if (y.find(it->first) == y.end()) {
        return false;
      }

      //check the existence of the value
      for (const int elem_x : it->second) {
        if (y[it->first].find(elem_x) == y[it->first].end()) {
          return false;
        }
      }
    }

    return true;
  }

  typedef void (Generator::*ComputeAffectsRelationshipForToken)(shared_ptr<cfg::GraphNode>&, unordered_map<int, unordered_set<int>>&,
      stack<unordered_map<int, unordered_set<int>>>&, stack<shared_ptr<cfg::GraphNode>>&);

  void Generator::ComputeAffectsRelationshipForStmt(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
      stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack) {
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
  }

  void Generator::ComputeAffectsRelationshipForIf(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
      stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack) {
    unordered_map<int, unordered_set<int>> last_modified_table_else = last_modified_table;
    //we push this copy for else branch later
    last_modified_stack.push(move(last_modified_table_else));
    ptr_stack.push(curr);
    curr = curr->GetNext();
  }

  void Generator::ComputeAffectsRelationshipForWhile(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
      stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack) {
    unordered_map<int, unordered_set<int>> before_last_modified_table = last_modified_table;
    last_modified_stack.push(move(before_last_modified_table));
    ptr_stack.push(curr);
    curr = curr->GetNext();
  }

  void Generator::ComputeAffectsRelationshipForThenEnd(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
      stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack) {
    unordered_map<int, unordered_set<int>> last_modified_table_else = move(last_modified_stack.top());
    last_modified_stack.pop();
    //store the current LMT to be merge after else branch is finish
    last_modified_stack.push(move(last_modified_table));
    //we do a move here since moving is faster than copying and last_modified_table_else will not be reference anymore
    last_modified_table = move(last_modified_table_else);

    shared_ptr<cfg::GraphNode> if_node = move(ptr_stack.top());
    ptr_stack.pop();
    curr = if_node->GetAlternative();
  }

  void Generator::ComputeAffectsRelationshipForIfEnd(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
      stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack) {
    unordered_map<int, unordered_set<int >> last_modified_table_then = move(last_modified_stack.top());
    last_modified_stack.pop();
    Generator::MergeTable(last_modified_table, last_modified_table_then);
    curr = curr->GetNext();
  }

  void Generator::ComputeAffectsRelationshipForWhileEnd(shared_ptr<cfg::GraphNode>& curr, unordered_map<int, unordered_set<int>>& last_modified_table,
      stack<unordered_map<int, unordered_set<int>>>& last_modified_stack, stack<shared_ptr<cfg::GraphNode>>& ptr_stack) {
    shared_ptr<cfg::GraphNode> while_node = move(ptr_stack.top());
    ptr_stack.pop();
    unordered_map<int, unordered_set<int >> before_last_modified_table = move(last_modified_stack.top());
    last_modified_stack.pop();

    if (CheckSubsetBetweenTables(last_modified_table, before_last_modified_table)) {
      last_modified_table = move(before_last_modified_table); //beforeLMT is a superset of LMT
      curr = while_node->GetAlternative();
    } else {
      Generator::MergeTable(last_modified_table, before_last_modified_table);
      curr = while_node;
    }
  }

  const unordered_map<cfg::NodeType, ComputeAffectsRelationshipForToken> ComputeAffectsRelForTokenMap = {
    { cfg::NodeType::STMT, &Generator::ComputeAffectsRelationshipForStmt },
    { cfg::NodeType::IF, &Generator::ComputeAffectsRelationshipForIf },
    { cfg::NodeType::WHILE, &Generator::ComputeAffectsRelationshipForWhile },
    { cfg::NodeType::THENEND, &Generator::ComputeAffectsRelationshipForThenEnd },
    { cfg::NodeType::WHILEEND, &Generator::ComputeAffectsRelationshipForWhileEnd },
    { cfg::NodeType::IFEND, &Generator::ComputeAffectsRelationshipForIfEnd },
  };

  void Generator::ComputeAffectsRelationship(cfg::GraphNode& head) {
    //LMT maps the variable to the stmt that modifies it  
    //It is mapped to an unordered_set because we could have multiple assign statements modifying same variable
    //e.g. if (1==1) then {a = a + 1;} else {a = a + 1;}
    unordered_map<int, unordered_set<int>> last_modified_table;
    stack<unordered_map<int, unordered_set<int>>> last_modified_stack;
    stack<shared_ptr<cfg::GraphNode>> ptr_stack;
    
    shared_ptr<cfg::GraphNode> curr = head.GetNext();

    while (curr->GetNodeType() != cfg::NodeType::END) {
      ComputeAffectsRelationshipForToken fn = ComputeAffectsRelForTokenMap.at(curr->GetNodeType());
      (this->*fn)(curr, last_modified_table, last_modified_stack, ptr_stack);
    }
  }
}