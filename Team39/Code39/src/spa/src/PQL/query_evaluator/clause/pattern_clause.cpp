#include "pattern_clause.h"

#define WILDCARD 0
#define ENTITY 1
#define SYNONYM 2

namespace pql_clause {
  typedef std::vector<int> (Pkb::*GetPatternDomainByVar)(const int) const;
  typedef std::vector<std::pair<int, int>>(Pkb::* GetPatternVarPair)() const;
  typedef void (PatternClause::* EvaluateLeftFn)(Pkb&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);

  const map<pql::RelationshipTypes, GetPatternDomainByVar> GetPatternDomainByVarMap = {
    { pql::kAssignPattern, &Pkb::GetModifiesStmtsByVar }
  };

  const map<pql::RelationshipTypes, GetPatternVarPair> GetPatternVarPairMap = {
    { pql::kAssignPattern, &Pkb::GetAllModifiesStmtVarPairs }
  };

  const map<int, EvaluateLeftFn> EvaluateLeftFnMap = {
    { WILDCARD, &PatternClause::EvaluateLeftWildcard },
    { ENTITY, &PatternClause::EvaluateLeftEnt },
    { SYNONYM, &PatternClause::EvaluateLeftSyn }
  };

  void PatternClause::EvaluateLeftWildcard(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain, 
      std::vector<pql_table::Predicate>& predicates) {
    //do nothing if the first argument is wildcard
    //e.g. pattern a (_, expr), if (_, _, _), while (_, _)
  }
  
  void PatternClause::EvaluateLeftEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int var_index = pkb.GetIndexByVar(left_);
    GetPatternDomainByVar fn = GetPatternDomainByVarMap.at(type_);
    std::vector<int> pattern_domain = {};
    pattern_domain = (pkb.*fn)(var_index);
    UpdateHashmap<int>(domain, pattern_synonym_name_, pattern_domain);
  }

  void PatternClause::EvaluateLeftSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetPatternVarPair fn = GetPatternVarPairMap.at(type_);
    std::vector<std::pair<int, int>> domain_pair_lst = (pkb.*fn)();
    pql_table::Predicate pred(pattern_synonym_name_, left_, domain_pair_lst);

    predicates.push_back(pred);
  }

  int GetVarType(string& name, bool is_syn) {
    if (is_syn) {
      return SYNONYM;
    }

    return name == "_" ? WILDCARD : ENTITY;
  }

  void PatternClause::EvaluateLeft(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int var_type = GetVarType(left_, is_synonymy_left_);
    EvaluateLeftFn fn = EvaluateLeftFnMap.at(var_type);
    (this->*fn)(pkb, domain, predicates);
  }

  void AssignPatternClause::EvaluateExpr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain) {
    if (expression_ == "_") {
      return;
    }

    std::unordered_set<int> domain_set = is_exact_ 
                                            ? pkb.GetStmtsWithExactPattern(expression_)
                                            : pkb.GetAllStmtsWithPattern(expression_);
    std::vector<int> domain_lst(domain_set.begin(), domain_set.end());
    UpdateHashmap<int>(domain, pattern_synonym_name_, domain_lst);
  }

  void AssignPatternClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    PatternClause::EvaluateLeft(pkb, domain, predicates);
    AssignPatternClause::EvaluateExpr(pkb, domain);
  }
}