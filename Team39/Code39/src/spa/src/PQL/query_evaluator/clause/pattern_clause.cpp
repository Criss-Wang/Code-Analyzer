#include "pattern_clause.h"

#define WILDCARD 0
#define ENTITY 1
#define SYNONYM 2

namespace pql_clause {
  typedef std::vector<int> (pql_cache::Cache::*GetPatternDomainByVar)(pql::RelationshipTypes, const int);
  typedef std::vector<std::pair<int, int>>(pql_cache::Cache::* GetPatternVarPair)(pql::RelationshipTypes);
  typedef void (PatternClause::* EvaluateLeftFn)(pql_cache::Cache&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);

  const map<pql::RelationshipTypes, GetPatternDomainByVar> GetPatternDomainByVarMap = {
    { pql::kAssignPattern, &pql_cache::Cache::GetRelFirstArgument }
  };

  const map<pql::RelationshipTypes, GetPatternVarPair> GetPatternVarPairMap = {
    { pql::kAssignPattern, &pql_cache::Cache::GetRelArgumentPairs }
  };

  const map<int, EvaluateLeftFn> EvaluateLeftFnMap = {
    { WILDCARD, &PatternClause::EvaluateLeftWildcard },
    { ENTITY, &PatternClause::EvaluateLeftEnt },
    { SYNONYM, &PatternClause::EvaluateLeftSyn }
  };

  void PatternClause::EvaluateLeftWildcard(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    //do nothing only if it is of type pattern clause,e.g. pattern a (_, _)  
    if (type_ != pql::kAssignPattern) {
      //waiting for pkb to implement the API
    }
  }
  
  void PatternClause::EvaluateLeftEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int var_index = cache.GetIndexByString(IndexTableType::kVarIndex, left_);
    GetPatternDomainByVar fn = GetPatternDomainByVarMap.at(type_);
    std::vector<int> pattern_domain = {};
    pattern_domain = (cache.*fn)(pql::kModifiesS, var_index);
    UpdateHashmap<int>(domain, pattern_synonym_name_, pattern_domain);
  }

  void PatternClause::EvaluateLeftSyn(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetPatternVarPair fn = GetPatternVarPairMap.at(type_);
    std::vector<std::pair<int, int>> domain_pair_lst = (cache.*fn)(pql::RelationshipTypes::kCalls);
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