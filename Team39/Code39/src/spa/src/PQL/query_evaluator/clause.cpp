#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "clause.h"
#include "query_evaluator_exceptions.h"

namespace pql {

  template <typename T>
  std::vector<int> ExtractFirst(std::vector<std::pair<int, T>>& lst) {
    std::vector<int> res;

    for (std::pair<int, T>& ele : lst) {
      res.push_back(ele.first);
    }

    return res;
  }

  template <typename T>
  std::vector<T> ExtractSecond(std::vector<std::pair<int, T>>& lst) {
    std::vector<T> res;

    for (std::pair<int, T>& ele : lst) {
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

  bool IsNumber(const std::string& str) {
    for (char const& c : str) {
      if (std::isdigit(c) == 0) {
        return false;
      }
    }
    return true;
  }
 
  Clause::Clause(pql::RelationshipToken* token, Pkb& pkb,
                 std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
                 std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
                 std::vector<pql_table::Predicate>* predicates) {
    token_ = token;
    pkb_ = pkb;
    stmt_hashmap_ = stmt_hashmap;
    var_hashmap_ = var_hashmap;
    predicates_ = predicates;
  }

  void EvaluateRelExistStmt(pql::RelationshipToken& token, Pkb& pkb,
    bool(Pkb::* IsRelHolds)(const int, const int) const, std::vector<int>(Pkb::* GetRelDomain)(const int) const,
    std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    //The parameter can be "_", e.g. Follows(_,2)
    bool is_left_num = IsNumber(token.GetLeft());
    bool is_right_num = IsNumber(token.GetRight());

    if (is_left_num && is_right_num) {
      bool rel_exist = (pkb.*IsRelHolds)(stoi(token.GetLeft()), stoi(token.GetRight()));

      if (!rel_exist) {
        throw pql_exceptions::FalseRelationException();
      }
    } else if (is_left_num || is_right_num) {
      std::vector<int> domain = is_left_num ? (pkb.*GetRelDomain)(stoi(token.GetLeft()))
                                            : (pkb.*GetInverseRelDomain)(stoi(token.GetRight()));
      if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
      }
    } else {
      std::vector<std::pair<int, int>> domain = (pkb.*GetRelPairs)();

      if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
      }
    }
  }

  void EvaluateRelDomainStmt(pql::RelationshipToken& token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
    std::vector<int>(Pkb::* GetRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    //Rel(_, s1) or Rel(1, s1)
    bool is_left_num = IsNumber(token.GetLeft());
    std::vector<int> domain;

    if (is_left_num) {
      domain = (pkb.*GetRelDomain)(stoi(token.GetLeft()));
    }
    else {
      std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
      std::vector<int> domain_with_duplicates = ExtractSecond<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap, token.GetRight(), domain);
  }

  void EvaluateInverseRelDomainStmt(pql::RelationshipToken& token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
    std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    //Follows(s1, _) or Follows(s1, 2)
    bool is_right_num = IsNumber(token.GetRight());
    std::vector<int> domain;

    if (is_right_num) {
      domain = (pkb.*GetInverseRelDomain)(stoi(token.GetRight()));
    }
    else {
      std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
      std::vector<int> domain_with_duplicates = ExtractFirst<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap, token.GetLeft(), domain);
  }

  void EvaluateRelPairStmt(pql::RelationshipToken& token, Pkb& pkb, 
    std::vector<pql_table::Predicate>* predicates, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    std::string left = token.GetLeft();
    std::string right = token.GetRight();
    std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
    pql_table::Predicate pred(left, right, domain_pair);

    (*predicates).push_back(pred);
  }

  void GenericEvaluate(pql::RelationshipToken& token, Pkb& pkb,
      std::unordered_map<std::string, std::vector<int>>& stmt_hashmap, std::vector<pql_table::Predicate>* predicates,
      bool(Pkb::* IsRelHolds)(const int, const int) const, std::vector<int>(Pkb::* GetRelDomain)(const int) const,
      std::vector<int>(Pkb::* GetInverseRelDomain)(const int) const, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    bool is_left_syn = token.IsSynonymLeft();
    bool is_right_syn = token.IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExistStmt(token, pkb, IsRelHolds, GetRelDomain, GetInverseRelDomain, GetRelPairs);
    }
    else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomainStmt(token, pkb, stmt_hashmap, GetRelDomain, GetRelPairs);
    }
    else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomainStmt(token, pkb, stmt_hashmap, GetInverseRelDomain, GetRelPairs);
    }
    else {
      EvaluateRelPairStmt(token, pkb, predicates, GetRelPairs);
    }
  }

  void FollowsClause::Evaluate() {
    GenericEvaluate(*token_, pkb_, *stmt_hashmap_, predicates_,
        &Pkb::IsFollows, &Pkb::GetStmtRightAfter, &Pkb::GetStmtRightBefore, &Pkb::GetAllFollowsPairs);
  }

  void FollowsTClause::Evaluate() {
    GenericEvaluate(*token_, pkb_, *stmt_hashmap_, predicates_,
        &Pkb::IsTransitiveFollows, &Pkb::GetStmtsAfter, &Pkb::GetStmtsBefore, &Pkb::GetAllTransitiveFollowsPairs);
  }

  void ParentClause::Evaluate() {
    GenericEvaluate(*token_, pkb_, *stmt_hashmap_, predicates_,
        &Pkb::IsParent, &Pkb::GetChild, &Pkb::GetParent, &Pkb::GetAllParentPairs);
  }

  void ParentTClause::Evaluate() {
    GenericEvaluate(*token_, pkb_, *stmt_hashmap_, predicates_,
        &Pkb::IsTransitiveParent, &Pkb::GetAllChildren, &Pkb::GetAllParents, &Pkb::GetAllTransitiveParentPairs);
  }
   
  void EvaluateRelExistVar(pql::RelationshipToken& token, Pkb& pkb, bool(Pkb::* IsRelHolds)(const int, const string&) const,
      std::vector<std::string>(Pkb::* GetRelDomain)(const int) const) {
    //Rel(1,"x") or Rel(1, _)
    //first parameter must be a string of digits, second argument can be wildcard
    bool is_right_wildcard = token.GetRight() == "_";

    if (!is_right_wildcard) {
      bool rel_exist = (pkb.*IsRelHolds)(stoi(token.GetLeft()), token.GetRight());

      if (!rel_exist) {
        throw pql_exceptions::FalseRelationException();
      }
    } else {
        std::vector<std::string> domain;
        domain = (pkb.*GetRelDomain)(stoi(token.GetLeft()));

      if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
      }
    }
  }

  void EvaluateRelDomainVar(pql::RelationshipToken& token, Pkb& pkb, std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
      std::vector<std::string>(Pkb::* GetRelDomain)(const int) const) {
    //Rel(1, v) only since the first parameter cannot be "_"
    std::vector<std::string> domain;
    domain = (pkb.*GetRelDomain)(stoi(token.GetLeft()));
    UpdateHashmap<std::string>(var_hashmap, token.GetRight(), domain);
  }
 
  void EvaluateInverseRelDomainVar(pql::RelationshipToken& token, Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& stmt_hashmap, 
      std::vector<int>(Pkb::* GetInverseRelDomain)(const string&) const, vector<pair<int, string>>(Pkb::*GetRelPairs)() const) {
    //Rel(s1, _) or Rel(s1, "x")
    bool is_right_wildcard = token.GetRight() == "_";
    std::vector<int> domain;

    if (!is_right_wildcard) {
      domain = (pkb.*GetInverseRelDomain)(token.GetRight());
    } else {
      std::vector<std::pair<int, std::string>> domain_pair = (pkb.*GetRelPairs)();
      std::vector<int> domain_with_duplicates = ExtractFirst<std::string>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap, token.GetLeft(), domain);
  }

  void EvaluateRelPairVar(pql::RelationshipToken& token, Pkb& pkb, std::vector<pql_table::Predicate>* predicates,
    vector<pair<int, string>>(Pkb::* GetRelPairs)() const) {
    std::string left = token.GetLeft();
    std::string right = token.GetRight();
    std::vector<std::pair<int, std::string>> domain_pair = (pkb.*GetRelPairs)();
    pql_table::Predicate pred(left, right, domain_pair);

    (*predicates).push_back(pred);
  }

  void UsesSClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExistVar(*token_, pkb_, &Pkb::IsUsesStmt, &Pkb::GetUsesVarByStmt);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomainVar(*token_, pkb_, *var_hashmap_, &Pkb::GetUsesVarByStmt);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomainVar(*token_, pkb_, *stmt_hashmap_, &Pkb::GetUsesStmtsByVar, &Pkb::GetAllUsesStmtVarPairs);
    } else {
      EvaluateRelPairVar(*token_, pkb_, predicates_, &Pkb::GetAllUsesStmtVarPairs);
    }
  }

  void ModifiesSClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExistVar(*token_, pkb_, &Pkb::IsModifiesStmt, &Pkb::GetModifiesVarByStmt);
    }
    else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomainVar(*token_, pkb_, *var_hashmap_, &Pkb::GetModifiesVarByStmt);
    }
    else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomainVar(*token_, pkb_, *stmt_hashmap_, &Pkb::GetModifiesStmtsByVar, &Pkb::GetAllModifiesStmtVarPairs);
    }
    else {
      EvaluateRelPairVar(*token_, pkb_, predicates_, &Pkb::GetAllModifiesStmtVarPairs);
    }
  }
}