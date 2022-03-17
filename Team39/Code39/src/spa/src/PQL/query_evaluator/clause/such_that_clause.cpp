#include <map>

#include "such_that_clause.h"

namespace pql_clause {
  typedef bool (Pkb::* IsRelHolds)(const int, const int) const;
  typedef std::vector<int>(Pkb::* GetRelDomain)(const int) const;
  typedef std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const;
  typedef std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const;
  typedef bool (Pkb::* DoesRelHolds)() const;

  const map<pql::RelationshipTypes, IsRelHolds> IsRelHoldsMap = {
    { pql::kFollows, &Pkb::IsFollows },
    { pql::kFollowsT, &Pkb::IsTransitiveFollows },
    { pql::kParent, &Pkb::IsParent },
    { pql::kParentT, &Pkb::IsTransitiveParent },
    { pql::kModifiesS, &Pkb::IsModifiesStmt },
    { pql::kUsesS, &Pkb::IsUsesStmt },
    { pql::kCalls, &Pkb::IsCalls },
    { pql::kCallsT, &Pkb::IsTransitiveCalls }
  };

  const map<pql::RelationshipTypes, GetRelDomain> GetRelDomainMap = {
    { pql::kFollows, &Pkb::GetStmtRightAfter },
    { pql::kFollowsT, &Pkb::GetStmtsAfter },
    { pql::kParent, &Pkb::GetChild },
    { pql::kParentT, &Pkb::GetAllChildren },
    { pql::kModifiesS, &Pkb::GetModifiesVarByStmt },
    { pql::kUsesS, &Pkb::GetUsesVarByStmt },
    { pql::kCalls, &Pkb::GetCallees },
    { pql::kCallsT, &Pkb::GetAllCallees }
  };

  const map<pql::RelationshipTypes, GetInverseRelDomain> GetInverseRelDomainMap = {
    { pql::kFollows, &Pkb::GetStmtRightBefore },
    { pql::kFollowsT, &Pkb::GetStmtsBefore },
    { pql::kParent, &Pkb::GetParent },
    { pql::kParentT, &Pkb::GetAllParents },
    { pql::kModifiesS, &Pkb::GetModifiesStmtsByVar },
    { pql::kUsesS, &Pkb::GetUsesStmtsByVar },
    { pql::kCalls, &Pkb::GetCallers },
    { pql::kCallsT, &Pkb::GetAllCallers }
  };

  const map<pql::RelationshipTypes, GetRelPairs> GetRelPairsMap = {
    { pql::kFollows, &Pkb::GetAllFollowsPairs },
    { pql::kFollowsT, &Pkb::GetAllTransitiveFollowsPairs },
    { pql::kParent, &Pkb::GetAllParentPairs },
    { pql::kParentT, &Pkb::GetAllTransitiveParentPairs },
    { pql::kModifiesS, &Pkb::GetAllModifiesStmtVarPairs },
    { pql::kUsesS, &Pkb::GetAllUsesStmtVarPairs },
    { pql::kCalls, &Pkb::GetAllCallsPairs },
    { pql::kCallsT, &Pkb::GetAllTransitiveCallsPairs }
  };

  const map<pql::RelationshipTypes, DoesRelHolds> DoesRelHoldsMap = {
    { pql::kFollows, &Pkb::IsFollowsExists },
    { pql::kFollowsT, &Pkb::IsFollowsExists },
    { pql::kParent, &Pkb::IsParentExists },
    { pql::kParentT, &Pkb::IsParentExists },
    { pql::kCalls, &Pkb::IsCallsExists },
    { pql::kCallsT, &Pkb::IsCallsExists }
  };

  template <typename T, typename R>
  std::vector<T> ExtractFirst(std::vector<std::pair<T, R>>& lst) {
    std::vector<T> res;

    for (std::pair<T, R>& ele : lst) {
      res.push_back(ele.first);
    }

    return res;
  }

  template <typename T, typename R>
  std::vector<R> ExtractSecond(std::vector<std::pair<T, R>>& lst) {
    std::vector<R> res;

    for (std::pair<T, R>& ele : lst) {
      res.push_back(ele.second);
    }

    return res;
  }

  template <typename T>
  std::vector<T> RemoveDuplicate(std::vector<T>& lst) {
    std::unordered_set<T> st;
    std::for_each(lst.begin(), lst.end(), [&st](const T& k) { st.insert(k); });
    std::vector<T> res(st.begin(), st.end());

    return res;
  }

  void SuchThatClause::EvaluateWildWild(Pkb& pkb) {
    DoesRelHolds fn = DoesRelHoldsMap.at(Clause::GetType());
    bool rel_exist = (pkb.*fn)();

    if (!rel_exist) {
      throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateWildEnt(Pkb& pkb) {
    GetInverseRelDomain fn = GetInverseRelDomainMap.at(Clause::GetType());
    std::vector<int> domain = (pkb.*fn)(stoi(Clause::GetToken().GetRight()));
    
    if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateWildSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain) {
    GetRelPairs fn = GetRelPairsMap.at(Clause::GetType());
    std::vector<std::pair<int, int>> domain_pair = (pkb.*fn)();
    std::vector<int> domain_with_duplicates = ExtractSecond<int, int>(domain_pair);
    std::vector<int> domain_lst = RemoveDuplicate<int>(domain_with_duplicates);

    UpdateHashmap<int>(domain, token.GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateEntWild(Pkb& pkb) {
    GetRelDomain fn = GetRelDomainMap.at(Clause::GetType());
    std::vector<int> domain = (pkb.*fn)(stoi(Clause::GetToken().GetLeft()));

    if (domain.size() == 0) {
      throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateEntEnt(Pkb& pkb) {
    IsRelHolds fn = IsRelHoldsMap.at(Clause::GetType());
    bool rel_exist = (pkb.*fn)(stoi(Clause::GetToken().GetLeft()), stoi(Clause::GetToken().GetRight()));

    if (!rel_exist) {
      throw pql_exceptions::FalseRelationException();
    }
  }

  void SuchThatClause::EvaluateEntSyn(Pkb& pkb) {
    GetRelDomain fn = GetRelDomainMap.at(Clause::GetType());
    std::vector<int> domain_lst = (pkb.*fn)(stoi(Clause::GetToken().GetLeft()));

    UpdateHashmap<int>(domain, token.GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateSynWild() {
    GetRelPairs fn = GetRelPairsMap.at(Clause::GetType());
    std::vector<std::pair<int, int>> domain_pair = (pkb.*fn)();
    std::vector<int> domain_with_duplicates = ExtractFirst<int, int>(domain_pair);
    std::vector<int> domain_lst = RemoveDuplicate<int>(domain_with_duplicates);

    UpdateHashmap<int>(domain, token.GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateSynEnt() {
    GetInverseRelDomain fn = GetInverseRelDomainMap.at(Clause::GetType());
    std::vector<int> domain_lst = (pkb.*fn)(stoi(Clause::GetToken().GetRight()));

    UpdateHashmap<int>(domain, token.GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateSynSyn(Pkb& pkb, std::vector<pql_table::Predicate>& predicate) {
    GetRelPairs fn = GetRelPairsMap.at(Clause::GetType());
    std::string left = Clause::GetToken().GetLeft();
    std::string right = Clause::GetToken().GetRight();
    std::vector<std::pair<int, int>> domain_pair = (pkb.*fn)();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates.push_back(pred);
  }

  void FollowsClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsFollows, &Pkb::GetStmtRightAfter, &Pkb::GetStmtRightBefore, &Pkb::GetAllFollowsPairs, &Pkb::IsFollowsExists);
  }

  void FollowsTClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsTransitiveFollows, &Pkb::GetStmtsAfter, &Pkb::GetStmtsBefore, &Pkb::GetAllTransitiveFollowsPairs, &Pkb::IsFollowsExists);
  }

  void ParentClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsParent, &Pkb::GetChild, &Pkb::GetParent, &Pkb::GetAllParentPairs, &Pkb::IsParentExists);
  }

  void ParentTClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsTransitiveParent, &Pkb::GetAllChildren, &Pkb::GetAllParents, &Pkb::GetAllTransitiveParentPairs, &Pkb::IsParentExists);
  }

  void CallsClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    /*GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsCalls, &Pkb::GetCallees, &Pkb::GetCallers, &Pkb::GetAllCallsPairs, &Pkb::IsCalls);*/
  }

  void CallsTClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    /*GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsTransitiveCalls, &Pkb::GetAllCallees, &Pkb::GetAllCallers, &Pkb::GetAllTransitiveCallsPairs, &Pkb::IsCalls);*/
  }

  void UsesSClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GenericEvaluateVar(*token_, pkb, domain, predicates,
        &Pkb::IsUsesStmt, &Pkb::GetUsesVarByStmt, &Pkb::GetUsesStmtsByVar, &Pkb::GetAllUsesStmtVarPairs);
  }

  void ModifiesSClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GenericEvaluateVar(*token_, pkb, domain, predicates,
        &Pkb::IsModifiesStmt, &Pkb::GetModifiesVarByStmt, &Pkb::GetModifiesStmtsByVar, &Pkb::GetAllModifiesStmtVarPairs);
  }

  void UsesPClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    /*GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsUsesStmt, &Pkb::GetUsesVarByStmt, &Pkb::GetUsesStmtsByVar, &Pkb::GetAllUsesStmtVarPairs);*/
  }

  void ModifiesPClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    /*GenericEvaluate(*token_, pkb, domain, predicates,
        &Pkb::IsModifiesStmt, &Pkb::GetModifiesVarByStmt, &Pkb::GetModifiesStmtsByVar, &Pkb::GetAllModifiesStmtVarPairs);*/
  }
}