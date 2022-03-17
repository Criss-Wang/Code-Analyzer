#include <map>

#include "such_that_clause.h"

#define WILDCARD 0
#define ENTITY 1
#define SYNONYM 2

namespace pql_clause {
  typedef bool (Pkb::* IsRelHolds)(const int, const int) const;
  typedef std::vector<int>(Pkb::* GetRelDomain)(const int) const;
  typedef std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const;
  typedef std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const;
  typedef bool (Pkb::* DoesRelHolds)() const;
  typedef void (SuchThatClause::*EvaluateFn)(Pkb&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);

  const map<int, EvaluateFn> WildcardEvaluateFnMap = {
    { WILDCARD, &SuchThatClause::EvaluateWildWild },
    { ENTITY  , &SuchThatClause::EvaluateWildEnt  },
    { SYNONYM , &SuchThatClause::EvaluateWildSyn  }
  };

  const map<int, EvaluateFn> EntEvaluateFnMap = {
    { WILDCARD, &SuchThatClause::EvaluateEntWild  },
    { ENTITY  , &SuchThatClause::EvaluateEntEnt   },
    { SYNONYM , &SuchThatClause::EvaluateEntSyn   }
  };

  const map<int, EvaluateFn> SynEvaluateFnMap = {
    { WILDCARD, &SuchThatClause::EvaluateSynWild  },
    { ENTITY  , &SuchThatClause::EvaluateSynEnt   },
    { SYNONYM , &SuchThatClause::EvaluateSynSyn   }
  };

  const map<int, map<int, EvaluateFn>> EvaluateFnMap = {
    { WILDCARD, WildcardEvaluateFnMap },
    { ENTITY, EntEvaluateFnMap },
    { SYNONYM,  SynEvaluateFnMap }
  };

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

  void SuchThatClause::EvaluateWildWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    DoesRelHolds fn = DoesRelHoldsMap.at(type_);
    bool rel_exist = (pkb.*fn)();

    if (!rel_exist) {
      throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateWildEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetInverseRelDomain fn = GetInverseRelDomainMap.at(type_);
    std::vector<int> domain_lst = (pkb.*fn)(stoi(token_->GetRight()));
    
    if (domain_lst.empty()) {
        throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateWildSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetRelPairs fn = GetRelPairsMap.at(type_);
    std::vector<std::pair<int, int>> domain_pair = (pkb.*fn)();
    std::vector<int> domain_with_duplicates = ExtractSecond<int, int>(domain_pair);
    std::vector<int> domain_lst = RemoveDuplicate<int>(domain_with_duplicates);

    UpdateHashmap<int>(domain, token_->GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateEntWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetRelDomain fn = GetRelDomainMap.at(type_);
    std::vector<int> domain_lst = (pkb.*fn)(stoi(token_->GetLeft()));

    if (domain_lst.empty()) {
      throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateEntEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    IsRelHolds fn = IsRelHoldsMap.at(type_);
    bool rel_exist = (pkb.*fn)(stoi(token_->GetLeft()), stoi(token_->GetRight()));

    if (!rel_exist) {
      throw pql_exceptions::FalseRelationException();
    }
  }

  void SuchThatClause::EvaluateEntSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetRelDomain fn = GetRelDomainMap.at(type_);
    std::vector<int> domain_lst = (pkb.*fn)(stoi(token_->GetLeft()));

    UpdateHashmap<int>(domain, token_->GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateSynWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetRelPairs fn = GetRelPairsMap.at(type_);
    std::vector<std::pair<int, int>> domain_pair = (pkb.*fn)();
    std::vector<int> domain_with_duplicates = ExtractFirst<int, int>(domain_pair);
    std::vector<int> domain_lst = RemoveDuplicate<int>(domain_with_duplicates);

    UpdateHashmap<int>(domain, token_->GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateSynEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetInverseRelDomain fn = GetInverseRelDomainMap.at(type_);
    std::vector<int> domain_lst = (pkb.*fn)(stoi(token_->GetRight()));

    UpdateHashmap<int>(domain, token_->GetRight(), domain_lst);
  }

  void SuchThatClause::EvaluateSynSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    GetRelPairs fn = GetRelPairsMap.at(type_);
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, int>> domain_pair = (pkb.*fn)();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates.push_back(pred);
  }

  int GetArgumentType(string& name, bool is_syn) {
    if (is_syn) {
      return SYNONYM;
    } 

    return name == "_" ? WILDCARD : ENTITY;
  }

  void SuchThatClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int left_type = GetArgumentType(token_->GetLeft(), token_->IsSynonymLeft());
    int right_type = GetArgumentType(token_->GetRight(), token_->IsSynonymRight());
    EvaluateFn fn = EvaluateFnMap.at(left_type).at(right_type);
    (this->*fn)(pkb, domain, predicates);
  }

  void FollowsClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void FollowsTClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void ParentClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void ParentTClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void CallsClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void CallsTClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void UsesSClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void ModifiesSClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void UsesPClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    /*SuchThatClause::Evaluate(pkb, domain, predicates);*/
  }

  void ModifiesPClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    /*SuchThatClause::Evaluate(pkb, domain, predicates);*/
  }
}