#include <string> 
#include <vector>
#include <unordered_map>
#include <unordered_set>


#include "solver.h"
#include "../query_evaluator_exceptions.h"


namespace pql_solver {

  Solver::Solver(std::unordered_map<std::string, std::vector<int>>* domain,
    std::vector<pql_table::Predicate>* preds,
    std::vector<pql::Synonym>& syn_list, std::vector<pql::AttrRef>& selected_syns,
    bool is_return_boolean) {
    
    predicates_ = preds;
    return_syns_ = selected_syns;
    is_return_boolean_ = is_return_boolean;

    for (pql::Synonym& syn : syn_list) {
      pql_table::InterTable table(syn, (*domain)[syn.GetName()]);
      tables_.push_back(table);
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
    //Remove synonyms that are not returned and return a list of tables with only returned synonyms involved
    std::vector<pql_table::InterTable> new_tables;
    std::unordered_set<std::string> added_syns;
    for (auto& table : tables_) {
      std::vector<int> return_idxs;

      for (auto& attr_ref : return_syns_) {
        std::string syn_name = attr_ref.GetSynName();

        if (added_syns.find(syn_name) == added_syns.end()
            && table.FindSynCol(syn_name) >= 0) {
          return_idxs.push_back(table.FindSynCol(syn_name));
          added_syns.insert(syn_name);
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

  pql_table::InterTable Solver::ExtractResult() {
    //At this stage all the tabls will not be empty
    //Or there will be no tables which means there are no constraints
    if (is_return_boolean_) {
      throw pql_exceptions::TrueResultException();
    }

    std::vector<pql_table::InterTable> tables = GetReturnTables();
    pql_table::InterTable final_table = MergeComponents(tables);
    
    return final_table;
  }

  pql_table::InterTable Solver::Solve() {
 
    for (pql_table::Predicate& pred : *predicates_) {
    Consume(pred);
    }

    return ExtractResult();
  }
}