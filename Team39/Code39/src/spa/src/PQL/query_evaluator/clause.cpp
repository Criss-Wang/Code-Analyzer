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

  void EvaluateRelExist(pql::RelationshipToken& token, Pkb& pkb,
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

  void EvaluateRelDomain(pql::RelationshipToken& token, Pkb& pkb,
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

  void EvaluateInverseRelDomain(pql::RelationshipToken& token, Pkb& pkb,
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

  void EvaluateRelPair(pql::RelationshipToken& token, Pkb& pkb, 
    std::vector<pql_table::Predicate>* predicates, std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)() const) {
    std::string left = token.GetLeft();
    std::string right = token.GetRight();
    std::vector<std::pair<int, int>> domain_pair = (pkb.*GetRelPairs)();
    pql_table::Predicate pred(left, right, domain_pair);

    (*predicates).push_back(pred);
  }

  void FollowsClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExist(*token_, pkb_, &Pkb::IsFollows, &Pkb::GetStmtRightAfter, 
          &Pkb::GetStmtRightBefore, &Pkb::GetAllFollowsPairs);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetStmtRightAfter, &Pkb::GetAllFollowsPairs);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetStmtRightBefore, &Pkb::GetAllFollowsPairs);
    } else {
      EvaluateRelPair(*token_, pkb_, predicates_, &Pkb::GetAllFollowsPairs);
    }
  }

  void FollowsTClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExist(*token_, pkb_, &Pkb::IsTransitiveFollows, &Pkb::GetStmtsAfter,
        &Pkb::GetStmtsBefore, &Pkb::GetAllTransitiveFollowsPairs);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetStmtsAfter, &Pkb::GetAllTransitiveFollowsPairs);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetStmtsBefore, &Pkb::GetAllTransitiveFollowsPairs);
    } else {
      EvaluateRelPair(*token_, pkb_, predicates_, &Pkb::GetAllTransitiveFollowsPairs);
    }
  }

  void ParentClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExist(*token_, pkb_, &Pkb::IsParent, &Pkb::GetChild,
        &Pkb::GetParent, &Pkb::GetAllParentPairs);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetChild, &Pkb::GetAllParentPairs);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetParent, &Pkb::GetAllParentPairs);
    } else {
      EvaluateRelPair(*token_, pkb_, predicates_, &Pkb::GetAllParentPairs);
    }
  }

  void ParentTClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      EvaluateRelExist(*token_, pkb_, &Pkb::IsTransitiveParent, &Pkb::GetAllChildren,
        &Pkb::GetAllParents, &Pkb::GetAllTransitiveParentPairs);
    } else if (!is_left_syn && is_right_syn) {
      EvaluateRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetAllChildren, &Pkb::GetAllTransitiveParentPairs);
    } else if (is_left_syn && !is_right_syn) {
      EvaluateInverseRelDomain(*token_, pkb_, *stmt_hashmap_, &Pkb::GetAllParents, &Pkb::GetAllTransitiveParentPairs);
    } else {
      EvaluateRelPair(*token_, pkb_, predicates_, &Pkb::GetAllTransitiveParentPairs);
    }
  }

 
  void UsesSClause::ExtractRelExist() {
    //Uses(1, "x") or Uses(1, _)
    //first parameter must be a string of digits, second argument can be wildcard
    bool is_right_wildcard = token_->GetRight() == "_";

    if (!is_right_wildcard) {
      bool rel_exist = pkb_.IsUsesStmt(stoi(token_->GetLeft()), token_->GetRight());

      if (!rel_exist) {
        throw pql_exceptions::EmptyDomainException();
      }
    } else {
      std::vector<std::string> domain = pkb_.GetUsesVarByStmt(stoi(token_->GetLeft()));
                                              
      if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
      }
    } 
  }

  void UsesSClause::ExtractRelDomain() {
    //Uses(1, v) only since the first parameter cannot be "_"
    std::vector<std::string> domain = pkb_.GetUsesVarByStmt(stoi(token_->GetLeft()));
    UpdateHashmap<std::string>(*var_hashmap_, token_->GetRight(), domain);
  }

  void UsesSClause::ExtractInverseRelDomain() {
    //Uses(s1, _) or Uses(s1, "x")
    bool is_right_wildcard = token_->GetRight() == "_";
    std::vector<int> domain;

    if (!is_right_wildcard) {
      domain = pkb_.GetUsesStmtsByVar(token_->GetRight());
    } else {
      std::vector<std::pair<int, std::string>> domain_pair = pkb_.GetAllUsesStmtVarPairs();
      std::vector<int> domain_with_duplicates = ExtractFirst<std::string>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(*stmt_hashmap_, token_->GetLeft(), domain);
  }

  void UsesSClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, std::string>> domain_pair = pkb_.GetAllUsesStmtVarPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    (*predicates_).push_back(pred);
  }

  void UsesSClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      UsesSClause::ExtractRelExist();
    } else if (!is_left_syn && is_right_syn) {
      UsesSClause::ExtractRelDomain();
    } else if (is_left_syn && !is_right_syn) {
      UsesSClause::ExtractInverseRelDomain();
    } else {
      UsesSClause::ExtractRelPair();
    }
  }

  void ModifiesSClause::ExtractRelExist() {
    //similar to Uses, can only be Modifies(1, _) or Modifies(1, "x")
    bool is_right_wildcard = token_->GetRight() == "_";

    if (!is_right_wildcard) {
      bool rel_exist = pkb_.IsModifiesStmt(stoi(token_->GetLeft()), token_->GetRight());

      if (!rel_exist) {
        throw pql_exceptions::EmptyDomainException();
      }
    } else {
      std::vector<std::string> domain = pkb_.GetModifiesVarByStmt(stoi(token_->GetLeft()));

      if (domain.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
      }
    } 
  }

  void ModifiesSClause::ExtractRelDomain() {
    // can only be Modifies(1, v)
    std::vector<std::string> domain = pkb_.GetModifiesVarByStmt(stoi(token_->GetLeft()));
    UpdateHashmap<std::string>(*var_hashmap_, token_->GetRight(), domain);
  }

  void ModifiesSClause::ExtractInverseRelDomain() {
    //Modifies(s1, _) or Modifies(s1, "x")
    bool is_right_wildcard = token_->GetRight() == "_";
    std::vector<int> domain;

    if (!is_right_wildcard) {
      domain = pkb_.GetModifiesStmtsByVar(token_->GetRight());
    } else {
      std::vector<std::pair<int, std::string>> domain_pair = pkb_.GetAllModifiesStmtVarPairs();
      std::vector<int> domain_with_duplicates = ExtractFirst<std::string>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(*stmt_hashmap_, token_->GetLeft(), domain);
  }

  void ModifiesSClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, std::string>> domain_pair = pkb_.GetAllModifiesStmtVarPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    (*predicates_).push_back(pred);
  }

  void ModifiesSClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      ModifiesSClause::ExtractRelExist();
    } else if (!is_left_syn && is_right_syn) {
      ModifiesSClause::ExtractRelDomain();
    } else if (is_left_syn && !is_right_syn) {
      ModifiesSClause::ExtractInverseRelDomain();
    } else {
      ModifiesSClause::ExtractRelPair();
    }
  }
}