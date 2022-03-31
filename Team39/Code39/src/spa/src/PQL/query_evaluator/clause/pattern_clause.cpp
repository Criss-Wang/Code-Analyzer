#include "pattern_clause.h"

#define INVALID_INDEX -1
#define WILDCARD 0
#define ENTITY 1
#define SYNONYM 2

namespace pql_clause {
  typedef void (PatternClause::* EvaluateLeftFn)(pql_cache::Cache&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);

  const unordered_map<pql::RelationshipTypes, TableIdentifier> RelTypeToTableIdMap = {
    { pql::kWhilePattern, TableIdentifier::kWhilePattern },
    { pql::kIfPattern, TableIdentifier::kIfPattern }
  };

  const map<int, EvaluateLeftFn> EvaluateLeftFnMap = {
    { WILDCARD, &PatternClause::EvaluateLeftWildcard },
    { ENTITY, &PatternClause::EvaluateLeftEnt },
    { SYNONYM, &PatternClause::EvaluateLeftSyn }
  };

  std::vector<std::string> PatternClause::GetInvovledSynonyms() {
    std::vector<std::string> res({pattern_synonym_name_});

    if (is_synonym_left_) {
      res.push_back(left_);
    }

    return res;
  }

  void PatternClause::EvaluateLeftWildcard(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    //do nothing only if it is of type pattern clause,e.g. pattern a (_, _)  
    if (type_ != pql::kAssignPattern) {
      TableIdentifier table_id = RelTypeToTableIdMap.at(type_);
      std::vector<std::pair<int,int>> pair_domain = cache.GetContainerStmtVarPair(table_id);
      std::vector<int> domain_with_duplicates = std::move(ExtractFirst<int, int>(pair_domain));
      std::vector<int> domain_lst = std::move(RemoveDuplicate<int>(domain_with_duplicates));

      UpdateHashmap<int>(domain, pattern_synonym_name_, domain_lst);
    }
  }
  
  void PatternClause::EvaluateLeftEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int var_index = cache.GetIndexByString(IndexTableType::kVarIndex, left_);
    
    if (var_index == INVALID_INDEX) {
      throw pql_exceptions::VariableDoesNotExistException();
    }
    
    std::vector<int> pattern_domain;

    if (type_ == pql::kAssignPattern) {
      pattern_domain = cache.GetRelFirstArgument(pql::kModifiesS, var_index);
    } else {
      TableIdentifier table_id = RelTypeToTableIdMap.at(type_);
      std::unordered_set<int> domain_set = cache.GetAllStmtsWithPatternVariable(var_index, table_id);
      pattern_domain.assign(domain_set.begin(), domain_set.end());
    }

    UpdateHashmap<int>(domain, pattern_synonym_name_, pattern_domain);
  }

  void PatternClause::EvaluateLeftSyn(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    std::vector<std::pair<int, int>> domain_pair_lst; 

    if (type_ == pql::kAssignPattern) {
      domain_pair_lst = cache.GetRelArgumentPairs(pql::kModifiesS);
    } else {
      TableIdentifier table_id = RelTypeToTableIdMap.at(type_);
      domain_pair_lst = cache.GetContainerStmtVarPair(table_id);
    }

    pql_table::Predicate pred(pattern_synonym_name_, left_, domain_pair_lst);
    predicates.push_back(pred);
  }

  int GetVarType(string& name, bool is_syn) {
    if (is_syn) {
      return SYNONYM;
    }

    return name == "_" ? WILDCARD : ENTITY;
  }

  void PatternClause::EvaluateLeft(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int var_type = GetVarType(left_, is_synonymy_left_);
    EvaluateLeftFn fn = EvaluateLeftFnMap.at(var_type);
    (this->*fn)(cache, domain, predicates);
  }

  void AssignPatternClause::EvaluateExpr(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain) {
    if (expression_ == "_") {
      return;
    }

    std::unordered_set<int> domain_set = cache.GetAllStmtsWithPattern(expression_, is_exact_);
    std::vector<int> domain_lst(domain_set.begin(), domain_set.end());
    UpdateHashmap<int>(domain, pattern_synonym_name_, domain_lst);
  }

  void AssignPatternClause::Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    PatternClause::EvaluateLeft(cache, domain, predicates);
    AssignPatternClause::EvaluateExpr(cache, domain);
  }

  void IfPatternClause::Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    PatternClause::EvaluateLeft(cache, domain, predicates);
  }
  
  void WhilePatternClause::Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    PatternClause::EvaluateLeft(cache, domain, predicates);
  }
}