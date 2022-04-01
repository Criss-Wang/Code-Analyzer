#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "solver.h"
#include "../query_evaluator_exceptions.h"

#define GROUP_INDEX_WITH_NO_SYN 0

namespace pql_solver {
  Ufds::Ufds(std::vector<pql::Synonym>* synonyms, std::vector<std::shared_ptr<pql_clause::Clause>>* clauses) {
    synonyms_ = synonyms;
    clauses_ = clauses;

    for (int i = 0; i < synonyms->size(); i++) {
      parent_.push_back(i);
      rank_.push_back(0);
      name_to_idx_map_[(*synonyms)[i].GetName()] = i;
      idx_to_syn_map_[i] = &(*synonyms)[i];
    }
  }

  int Ufds::Find(int idx) {
    if (parent_[idx] == idx) {
      return idx;
    } else {
      parent_[idx] = Find(parent_[idx]);
      return parent_[idx];
    }
  }

  int Ufds::Union(int i, int j) {
    int idx_i = Find(i);
    int idx_j = Find(j);
    int to_be_insert = idx_i;
    int to_be_delete = idx_j;

    if (idx_i != idx_j) {
      if (rank_[idx_i] > rank_[idx_j]) {
        parent_[idx_j] = idx_i;
      } else {
        parent_[idx_i] = idx_j;
        to_be_insert = idx_j;
        to_be_delete = idx_i;

        if (rank_[idx_i] == rank_[idx_j]) {
          rank_[idx_j] += 1;
        }
      }

      //add the clauses of the children to the parent's vector
      syn_to_clauses_map_[to_be_insert].insert(syn_to_clauses_map_[to_be_insert].end(),
            syn_to_clauses_map_[to_be_delete].begin(), syn_to_clauses_map_[to_be_delete].end());
      syn_to_clauses_map_[to_be_delete].clear();
    }

    return to_be_insert;
  }

  void Ufds::Group() {
    syn_groups_.push_back(std::vector<pql::Synonym>());
    clause_groups_.push_back(std::vector<std::shared_ptr<pql_clause::Clause>>());

    for (auto clause : *clauses_) {
      std::vector<std::string> syn_invovled = clause->GetInvovledSynonyms();

      if (syn_invovled.size() == 0) {
        clause_groups_[GROUP_INDEX_WITH_NO_SYN].push_back(clause);
      } else if (syn_invovled.size() == 1) {
        std::string first_syn = syn_invovled[0];
        int index = name_to_idx_map_[first_syn];

      } else {
        
      }
    }
  }

  std::pair<std::vector<std::vector<pql::Synonym>>,
      std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>>> Ufds::GetGroupings() {
  
  }

  Solver::Solver(pql::Query* query, pql_cache::Cache* cache) {
    query_ = query;
    cache_ = cache;
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

      for (auto& attr_ref : query_->GetAttrRef()) {
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