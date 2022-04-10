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

    HandleClauses();
    Group();
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
      syn_to_clauses_map_.erase(to_be_delete);
    }

    return to_be_insert;
  }

  void Ufds::HandleClauses() {
    syn_groups_.push_back(std::vector<pql::Synonym>());
    clause_groups_.push_back(std::vector<std::shared_ptr<pql_clause::Clause>>());

    for (auto clause : *clauses_) {
      std::vector<std::string> syn_invovled = clause->GetInvovledSynonyms();

      if (syn_invovled.size() == 0) {

        clause_groups_[GROUP_INDEX_WITH_NO_SYN].push_back(clause);

      } else if (syn_invovled.size() == 1) {

        int index = name_to_idx_map_[syn_invovled[0]];
        syn_to_clauses_map_[Find(index)].push_back(clause);

      } else {

        int first_idx = name_to_idx_map_[syn_invovled[0]];
        int second_idx = name_to_idx_map_[syn_invovled[1]];

        int root = Union(first_idx, second_idx);
        syn_to_clauses_map_[root].push_back(clause);

      }
    }
  }
  
  void Ufds::Group() {
    std::unordered_map<int, std::vector<int>> idx_group_map;

    for (int i = 0; i < parent_.size(); i++) {
      if (idx_group_map.find(Find(i)) == idx_group_map.end()) {
        idx_group_map[Find(i)] = vector<int>{ i };
      } else {
        idx_group_map[Find(i)].push_back(i);
      }
    }


    //we already found all connected components at this stage
    for (auto it = syn_to_clauses_map_.begin(); it != syn_to_clauses_map_.end(); it++) {
      int group_idx = it->first;

      std::vector<pql::Synonym> curr_syn_groups;

      for (int idx : idx_group_map[group_idx]) {
        std::string syn_name = idx_to_syn_map_[idx]->GetName();
        curr_syn_groups.push_back(std::move(*idx_to_syn_map_[idx]));
        syn_invovled_in_clause_set_.insert(std::move(syn_name));
      }

      clause_groups_.push_back(std::move(it->second));
      syn_groups_.push_back(std::move(curr_syn_groups));
    }
  }

  std::vector<std::vector<pql::Synonym>> Ufds::GetSynGroups() {
    return syn_groups_;
  }

  std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>> Ufds::GetClauseGroups() {
    return clause_groups_;
  }

  std::unordered_set<std::string> Ufds::GetSynInvovledInClause() {
    return syn_invovled_in_clause_set_;
  }

  Solver::Solver(pql::Query* query, pql_cache::Cache* cache) {
    query_ = query;
    cache_ = cache;
  }

  int Solver::GetTableIndex(std::string& name, std::vector<pql_table::InterTable>& tables) {
    for (int index = 0; index < tables.size(); index++) {
      if (tables[index].FindSynCol(name) >= 0) {
        return index;
      }
    }

    return -1;
  }

  void Solver::Consume(pql_table::Predicate& pred, std::vector<pql_table::InterTable>& tables) {
    int first_index = GetTableIndex(pred.first_syn_, tables);
    int second_index = GetTableIndex(pred.second_syn_, tables);

    if (first_index != second_index) {
      //both synonyms are in different table, can use mergeAndFilter
      //need to remove the second table because it is already merged to the first table
      tables[first_index] = std::move(tables[first_index].MergeAndFilter(tables[second_index], pred));
      tables.erase(tables.begin() + second_index);
    } else {
      //both synonyms are in the same table, can only use filter
      tables[first_index] = std::move(tables[first_index].Filter(pred));
    }
  }

  void Solver::AddConnectedComponentTable(pql_table::InterTable& table) {
    //Remove columns that are not returned and return table with returned synonyms
    std::unordered_set<std::string> added_syns;
    std::vector<int> return_idxs;

    for (auto& attr_ref : query_->GetAttrRef()) {
      std::string syn_name = attr_ref.GetSynName();

      if (added_syns.find(syn_name) == added_syns.end()
          && table.FindSynCol(syn_name) >= 0) {
        return_idxs.push_back(table.FindSynCol(syn_name));
        added_syns.insert(syn_name);
      }
    }

    if (return_idxs.size() > 0) {
      //add the table to the list of tables only if it contains at least one returned synonyms 
      tables_.push_back(std::move(table.GetColsByIndices(return_idxs)));
    }
  }

  pql_table::InterTable MergeComponents(std::vector<pql_table::InterTable>& tables) {
    pql_table::InterTable return_table = tables[0];

    for (int index = 1; index < tables.size(); index++) {
      return_table = std::move(return_table.Merge(tables[index]));
    }

    return return_table;
  }

  void Solver::AddTablesForSynonymNotInClause(std::unordered_set<std::string>& syn_set) {
    //need to initialize table for return synonyms that are not invovled in any clauses
    for (auto& attr_ref : query_->GetAttrRef()) {
      if (syn_set.find(attr_ref.GetSynName()) == syn_set.end()) {
        std::unordered_set<int> curr_domain_set = cache_->GetAllEntity(attr_ref.GetSynDeclaration());
        std::vector<int> curr_domain(curr_domain_set.begin(), curr_domain_set.end());
        
        tables_.push_back(pql_table::InterTable(attr_ref.GetSynName(), curr_domain));
        syn_set.insert(attr_ref.GetSynName());
      }
    }
  }

  void GetAllDomain(std::vector<pql::Synonym>& synonyms, std::unordered_map<std::string, std::vector<int>>& domain, pql_cache::Cache* cache) {
    //domain stores <synonym.name, domain> pair.
    for (pql::Synonym& synonym : synonyms) {
      std::unordered_set<int> domain_set = cache->GetAllEntity(synonym.GetDeclaration());
      std::vector<int> domain_list(std::begin(domain_set), std::end(domain_set));
      domain.insert({ synonym.GetName(), domain_list });
    }
  }

  static bool CompareClause(const std::shared_ptr<pql_clause::Clause>& c1, const std::shared_ptr<pql_clause::Clause>& c2) {
    int c1_size = c1->GetInvovledSynonyms().size();
    int c2_size = c2->GetInvovledSynonyms().size();
    
    if (c1 < c2) {
      return true;
    }

    if (c2 < c1) {
      return false;
    }

    //At this point, both size are the same
    return c1->GetPriority() < c2->GetPriority();
  }

  void CreateTablesForConnectedComponent(std::vector<pql::Synonym>& used_synonyms, 
      std::unordered_map<std::string, std::vector<int>>& domain, std::vector<pql_table::InterTable>& tables) {
    for (auto& synonym : used_synonyms) {
      tables.push_back(pql_table::InterTable(synonym.GetName(), domain[synonym.GetName()]));
    }
  }

  void Solver::SolveConnectedComponent(std::vector<pql::Synonym>& used_synonyms, std::vector<std::shared_ptr<pql_clause::Clause>>& clauses) {
    std::unordered_map<std::string, std::vector<int>> domain;
    std::vector<pql_table::Predicate> predicates;
    std::vector<pql_table::InterTable> tables;

    GetAllDomain(used_synonyms, domain, cache_);

    //Sort all clauses
    std::sort(clauses.begin(), clauses.end(), CompareClause);

    //Evaluate all clause
    for (auto& clause : clauses) {
      clause->Evaluate(*cache_, domain, predicates);
    }

    //Create an InterTable for each synonym used here
    CreateTablesForConnectedComponent(used_synonyms, domain, tables);

    //Consume all predicates
    for (pql_table::Predicate& pred : predicates) {
      Consume(pred, tables);
    }

    if (used_synonyms.empty()) {
      //this is for group index 0 where there is no synonym invovled (which there are no tables)
      return;
    }

    AddConnectedComponentTable(tables[0]);
  }

  pql_table::InterTable Solver::Solve() {
    Ufds ufds(&query_->GetAllUsedSynonyms(), &query_->GetClauses());
    
    std::vector<std::vector<pql::Synonym>> syn_groups = std::move(ufds.GetSynGroups());
    std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>> clause_groups = std::move(ufds.GetClauseGroups());
    std::unordered_set<std::string> syn_invovled_in_clause_set = std::move(ufds.GetSynInvovledInClause());

    AddTablesForSynonymNotInClause(syn_invovled_in_clause_set);

    for (int idx = 0; idx < syn_groups.size(); idx++) {
      SolveConnectedComponent(syn_groups[idx], clause_groups[idx]);
    }

    if (query_->GetBoolean()) {
      throw pql_exceptions::TrueResultException();
    }

    return MergeComponents(tables_);
  }
}