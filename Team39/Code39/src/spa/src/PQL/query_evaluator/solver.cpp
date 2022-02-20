#include <string>
#include <vector>
#include <unordered_map>


#include "solver.h"
#include "../../Utility/entity.h"

namespace pql_solver {

  Solver::Solver(std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
    std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
    std::vector<pql_table::Predicate>* preds,
    std::vector<pql::Synonym>& syn_list, pql::Synonym* selected_syn) {
      predicates_ = preds;
      return_syn_ = selected_syn;

      for (pql::Synonym& syn : syn_list) {
        if (syn.GetDeclaration() == EntityIdentifier::kVariable
            || syn.GetDeclaration() == EntityIdentifier::kProc) {
          pql_table::InterTable table(syn, (*var_hashmap)[syn.GetName()]);
          tables_.push_back(table);
        } else {
          pql_table::InterTable table(syn, (*stmt_hashmap)[syn.GetName()]);
          tables_.push_back(table);
        }
      }
  }

  int Solver::GetTableIndex(std::string& name) {
    for (int index = 0; index < tables_.size(); index++) {
      if (tables_[index].FindSynCol(name) >= 0) {
        return index;
      }
    }

    return -1;
  }

  void Solver::Consume(pql_table::Predicate& pred) {
    int first_index = GetTableIndex(pred.first_syn_);
    int second_index = GetTableIndex(pred.second_syn_);

    if (first_index != second_index) {
      //both synonyms are in different table, can use mergeAndFilter
      //need to remove the second table because it is already merged to the first table
      tables_[first_index].MergeAndFilter(tables_[second_index], pred);
      tables_.erase(tables_.begin() + second_index);
    } else {
      //both synonyms are in the same table, can only use filter
      tables_[first_index].Filter(pred);
    }
  }

  std::vector<std::string> Solver::ExtractResult() {
    std::string name = return_syn_->GetName();
    int index = GetTableIndex(name);
    pql_table::InterTable curr_table = tables_[index];
    std::vector<std::string> lst;

    std::vector<pql_table::element> col = curr_table.GetColByName(name);

    for (auto& ele : col) {
      if (return_syn_->GetDeclaration() == EntityIdentifier::kVariable
          || return_syn_->GetDeclaration() == EntityIdentifier::kProc) {
        lst.push_back(ele.name);
      } else {
        lst.push_back(std::to_string(ele.val));
      }
    }
    
    return lst;
  }

  std::vector<std::string> Solver::Solve() {
    for (pql_table::Predicate& pred : *predicates_) {
      Consume(pred);
    }

    std::vector<std::string> res = ExtractResult();

    return res;
  }
}