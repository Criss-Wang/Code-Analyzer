#include "such_that_clause.h"

namespace pql_clause {
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

  void EvaluateRelExist(pql::RelationshipToken& token, Pkb& pkb,
    bool(Pkb::* IsRelHolds)(const int, const int) const, std::vector<int>(Pkb::* GetRelDomain)(const int) const,
    std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, bool(Pkb::* DoesRelExist)() const) {
    //The parameter can be "_", e.g. Follows(_,2)
    bool is_left_entity = token.GetLeft() != "_";
    bool is_right_entity = token.GetRight() != "_";

    if (is_left_entity && is_right_entity) {
      bool rel_exist = (pkb.*IsRelHolds)(stoi(token.GetLeft()), stoi(token.GetRight()));

      if (!rel_exist) {
        throw pql_exceptions::FalseRelationException();
      }
    } else if (is_left_entity || is_right_entity) {
      std::vector<int> domain = is_left_entity ? (pkb.*GetRelDomain)(stoi(token.GetLeft()))
                                               : (pkb.*GetInverseRelDomain)(stoi(token.GetRight()));

      if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
      }
    } else {
      bool rel_exist = (pkb.*DoesRelExist)();

      if (!rel_exist) {
        throw pql_exceptions::EmptyDomainException();
      }
    }
  }

  void EvaluateRelDomain(pql::RelationshipToken& token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<int>(Pkb::* GetRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    //Rel(_, s1) or Rel(1, s1)
    bool is_left_entity = token.GetLeft() != "_";
    std::vector<int> domain_lst;

    if (is_left_entity) {
      domain_lst = (pkb.*GetRelDomain)(stoi(token.GetLeft()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
      std::vector<int> domain_with_duplicates = ExtractSecond<int, int>(domain_pair);
      domain_lst = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(domain, token.GetRight(), domain_lst);
  }

  void EvaluateInverseRelDomain(pql::RelationshipToken& token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    //Follows(s1, _) or Follows(s1, 2)
    bool is_right_entity = token.GetRight() != "_";
    std::vector<int> domain_lst;

    if (is_right_entity) {
      domain_lst = (pkb.*GetInverseRelDomain)(stoi(token.GetRight()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
      std::vector<int> domain_with_duplicates = ExtractFirst<int, int>(domain_pair);
      domain_lst = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(domain, token.GetLeft(), domain_lst);
  }

  void EvaluateRelPair(pql::RelationshipToken& token, Pkb& pkb,
    std::vector<pql_table::Predicate>& predicates, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    std::string left = token.GetLeft();
    std::string right = token.GetRight();
    std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates.push_back(pred);
  }

  void GenericEvaluate(pql::RelationshipToken& token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& domain, std::vector<pql_table::Predicate>* predicates,
    bool(Pkb::* IsRelHolds)(const int, const int) const, std::vector<int>(Pkb::* GetRelDomain)(const int) const,
    std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const,
    bool(Pkb::* DoesRelExist)() const) {
    bool is_left_syn = token.IsSynonymLeft();
    bool is_right_syn = token.IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExist(token, pkb, IsRelHolds, GetRelDomain, GetInverseRelDomain, DoesRelExist);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomain(token, pkb, domain, GetRelDomain, GetRelPairs);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomain(token, pkb, domain, GetInverseRelDomain, GetRelPairs);
    } else {
      EvaluateRelPair(token, pkb, predicates, GetRelPairs);
    }
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

  //This is used when the first argument cannot be wildcard
  void EvaluateRelExistVar(pql::RelationshipToken& token, Pkb& pkb, bool(Pkb::* IsRelHolds)(const int, const int) const,
    std::vector<int>(Pkb::* GetRelDomain)(const int) const) {
    //Rel(1,"x") or Rel(1, _)
    //first parameter must be a string of digits, second argument can be wildcard
    bool is_right_wildcard = token.GetRight() == "_";

    if (!is_right_wildcard) {
      bool rel_exist = (pkb.*IsRelHolds)(stoi(token.GetLeft()), stoi(token.GetRight()));

      if (!rel_exist) {
        throw pql_exceptions::FalseRelationException();
      }
    } else {
      std::vector<int> domain;
      domain = (pkb.*GetRelDomain)(stoi(token.GetLeft()));

      if (domain.empty()) {
        throw pql_exceptions::EmptyDomainException();
      }
    }
  }

  void EvaluateRelDomainVar(pql::RelationshipToken& token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<int>(Pkb::* GetRelDomain)(const int) const) {
    //Rel(1, v) only since the first parameter cannot be "_"
    std::vector<int> domain_lst;
    domain_lst = (pkb.*GetRelDomain)(stoi(token.GetLeft()));
    UpdateHashmap<int>(domain, token.GetRight(), domain_lst);
  }

  void GenericEvaluateVar(pql::RelationshipToken& token, Pkb& pkb,
      std::unordered_map<std::string, std::vector<int>>& domain, std::vector<pql_table::Predicate>* predicates,
      bool(Pkb::* IsRelHolds)(const int, const int) const, std::vector<int>(Pkb::* GetRelDomain)(const int) const,
      std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    bool is_left_syn = token.IsSynonymLeft();
    bool is_right_syn = token.IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExistVar(token, pkb, IsRelHolds, GetRelDomain);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomainVar(token, pkb, domain, GetRelDomain);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomain(token, pkb, domain, GetInverseRelDomain, GetRelPairs);
    } else {
      EvaluateRelPair(token, pkb, predicates, GetRelPairs);
    }
  }

  void UsesSClause::Evaluate() {
    GenericEvaluate(*token_, pkb_, *domain_, predicates_,
        &Pkb::IsUsesStmt, &Pkb::GetUsesVarByStmt, &Pkb::GetUsesStmtsByVar, &Pkb::GetAllUsesStmtVarPairs);
  }

  void ModifiesSClause::Evaluate() {
    GenericEvaluate(*token_, pkb_, *domain_, predicates_,
        &Pkb::IsModifiesStmt, &Pkb::GetModifiesVarByStmt, &Pkb::GetModifiesStmtsByVar, &Pkb::GetAllModifiesStmtVarPairs);
  }

  void UsesPClause::Evaluate() {
    /*GenericEvaluate(*token_, pkb_, *domain_, predicates_,
        &Pkb::IsUsesStmt, &Pkb::GetUsesVarByStmt, &Pkb::GetUsesStmtsByVar, &Pkb::GetAllUsesStmtVarPairs);*/
  }

  void ModifiesPClause::Evaluate() {
    /*GenericEvaluate(*token_, pkb_, *domain_, predicates_,
        &Pkb::IsModifiesStmt, &Pkb::GetModifiesVarByStmt, &Pkb::GetModifiesStmtsByVar, &Pkb::GetAllModifiesStmtVarPairs);*/
  }
}