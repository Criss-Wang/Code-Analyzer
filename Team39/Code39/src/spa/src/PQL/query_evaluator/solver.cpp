#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>


#include "solver.h"
#include "../../Utility/entity.h"

namespace pql_solver {

  Solver::Solver(std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
    std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
    std::vector<pql_table::Predicate>* preds,
    std::vector<pql::Synonym>& syn_list, std::vector<pql::Synonym> selected_syns) {
    
    predicates_ = preds;
    return_syns_ = selected_syns;

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
      tables_[first_index] = tables_[first_index].MergeAndFilter(tables_[second_index], pred);
      tables_.erase(tables_.begin() + second_index);
    } else {
      //both synonyms are in the same table, can only use filter
      tables_[first_index] = tables_[first_index].Filter(pred);
    }
  }

  std::vector<pql_table::InterTable> Solver::GetReturnTables() {
    std::vector<pql_table::InterTable> new_tables;

    for (auto& table : tables_) {
      std::vector<int> return_idxs;

      for (auto& syn : return_syns_) {
        int cur_idx = table.FindSynCol(syn.GetName());
        if (cur_idx >= 0) {
          return_idxs.push_back(cur_idx);
        }
      }

      if (!return_idxs.empty()) {
        new_tables.push_back(table.GetColsByIndices(return_idxs));
      }
    }

    return new_tables;
  }

  pql_table::InterTable MergeComponents(std::vector<pql_table::InterTable>& tables) {
    pql_table::InterTable return_table = tables[0];

    for (int index = 1; index < tables.size(); index++) {
      return_table = return_table.Merge(tables[index]);
    }

    return return_table;
  }

  std::vector<std::string> Solver::ExtractResult() {
    std::vector<pql_table::InterTable> tables = GetReturnTables();
    pql_table::InterTable final_table = MergeComponents(tables);

    std::vector<std::string> result_string(final_table.GetRowNum());
    
    //We add the synonym according to their position in return_syns_
    for (auto& syn : return_syns_) {
      int col_num_in_table = final_table.FindSynCol(syn.GetName());

      for (int index = 0; index < final_table.GetRowNum(); index++) {
        std::string cur_string = "";

        if (syn.GetDeclaration() == EntityIdentifier::kVariable
            || syn.GetDeclaration() == EntityIdentifier::kProc) {
          cur_string = final_table.rows_[index][col_num_in_table].name;
        } else {
          cur_string = std::to_string(final_table.rows_[index][col_num_in_table].val);
        }

        result_string[index] += cur_string;
      }
    }

    return result_string;
  }

  std::vector<std::string> Solver::Solve() {
    for (pql_table::Predicate& pred : *predicates_) {
      Consume(pred);
    }

    std::vector<std::string> res = ExtractResult();

    return res;
  }
}