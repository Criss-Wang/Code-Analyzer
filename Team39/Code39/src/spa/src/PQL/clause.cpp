#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "clause.h"
#include "query_evaluator_exceptions.h"

namespace pql {

  template <typename T>
  std::vector<T> Intersect(const std::vector<T>& lst1, const std::vector<T>& lst2) {
    //refer from stack overflow https://stackoverflow.com/questions/38993415/how-to-apply-the-intersection-between-two-lists-in-c
    std::vector<T> res;
    std::unordered_set<T> st;
    std::for_each(lst2.begin(), lst2.end(), [&st](const T& k) { st.insert(k); });
    std::for_each(lst1.begin(), lst1.end(),
        [&st, &res](const T& k) {
          auto iter = st.find(k);
          if (iter != st.end()) {
            res.push_back(k);
            st.erase(iter);
          }
        }
      );
    return res;
  }

  template <typename T>
  void UpdateHashmap(std::unordered_map<std::string, std::vector<T>>& hmap, std::string name, const std::vector<T>& lst) {
    std::vector<T> oriLst = hmap[name];
    std::vector<T> inter = Intersect<T>(oriLst, lst);

    //the domain is empty after intersection
    if (inter.size() == 0) {
        throw EmptyDomainException();
    }

    hmap[name] = inter;
  }

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
                 std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
                 std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
                 std::vector<pql_table::Predicate>& predicates) {
    token_ = token;
    pkb_ = pkb;
    stmt_hashmap_ = stmt_hashmap;
    var_hashmap_ = var_hashmap;
    predicates_ = predicates;
  }

  void FollowsClause::ExtractRelExist() {
    //The parameter can be "_", e.g. Follows(_,2)
    bool is_left_num = IsNumber(token_->GetLeft());
    bool is_right_num = IsNumber(token_->GetRight());

    if (is_left_num && is_right_num) {
      bool rel_exist = pkb_.IsFollows(stoi(token_->GetLeft()), stoi(token_->GetRight()));

      if (!rel_exist) {
        throw EmptyDomainException();
      }
    } else if (is_left_num || is_right_num) {
      std::vector<int> domain = is_left_num ? pkb_.GetStmtRightAfter(stoi(token_->GetLeft()))
                                            : pkb_.GetStmtRightBefore(stoi(token_->GetRight()));
      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    } else {
      std::vector<std::pair<int, int>> domain = pkb_.GetAllFollowsPairs();

      if (domain.size() == 0) {
          throw EmptyDomainException();
      }
    }

  }

  void FollowsClause::ExtractRelDomain() {
    //Follows(_, s1) or Follows(1, s1)
    bool is_left_num = IsNumber(token_->GetLeft());
    std::vector<int> domain;

    if (is_left_num) {
      domain = pkb_.GetStmtRightAfter(stoi(token_->GetLeft()));    
    } else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllFollowsPairs();
      std::vector<int> domain_with_duplicates = ExtractSecond<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetRight(), domain);
  }

  void FollowsClause::ExtractInverseRelDomain() {
    //Follows(s1, _) or Follows(s1, 2)
    bool is_right_num = IsNumber(token_->GetRight());
    std::vector<int> domain;

    if (is_right_num) {
      domain = pkb_.GetStmtRightBefore(stoi(token_->GetRight()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllFollowsPairs();
      std::vector<int> domain_with_duplicates = ExtractFirst<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetLeft(), domain);
  }

  void FollowsClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllFollowsPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates_.push_back(pred);
  }

  void FollowsClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      FollowsClause::ExtractRelExist();
    } else if (!is_left_syn && is_right_syn) {
      FollowsClause::ExtractRelDomain();
    } else if (is_left_syn && !is_right_syn) {
      FollowsClause::ExtractInverseRelDomain();
    } else {
      FollowsClause::ExtractRelPair();
    }
  }

  void FollowsTClause::ExtractRelExist() {
    bool is_left_num = IsNumber(token_->GetLeft());
    bool is_right_num = IsNumber(token_->GetRight());

    if (is_left_num && is_right_num) {
      bool rel_exist = pkb_.IsTransitiveFollows(stoi(token_->GetLeft()), stoi(token_->GetRight()));

      if (!rel_exist) {
        throw EmptyDomainException();
      }
    } else if (is_left_num || is_right_num) {
      std::vector<int> domain = is_left_num ? pkb_.GetStmtsAfter(stoi(token_->GetLeft()))
                                            : pkb_.GetStmtsBefore(stoi(token_->GetRight()));
      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    } else {
      std::vector<std::pair<int, int>> domain = pkb_.GetAllTransitiveFollowsPairs();

      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    }

  }

  void FollowsTClause::ExtractRelDomain() {
    bool is_left_num = IsNumber(token_->GetLeft());
    std::vector<int> domain;

    if (is_left_num) {
      domain = pkb_.GetStmtsAfter(stoi(token_->GetLeft()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllTransitiveFollowsPairs();
      std::vector<int> domain_with_duplicates = ExtractSecond<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetRight(), domain);
  }

  void FollowsTClause::ExtractInverseRelDomain() {
    bool is_right_num = IsNumber(token_->GetRight());
    std::vector<int> domain;

    if (is_right_num) {
      domain = pkb_.GetStmtsBefore(stoi(token_->GetRight()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllTransitiveFollowsPairs();
      std::vector<int> domain_with_duplicates = ExtractFirst<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetLeft(), domain);
  }

  void FollowsTClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllTransitiveFollowsPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates_.push_back(pred);
  }

  void FollowsTClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      FollowsTClause::ExtractRelExist();
    } else if (!is_left_syn && is_right_syn) {
      FollowsTClause::ExtractRelDomain();
    } else if (is_left_syn && !is_right_syn) {
      FollowsTClause::ExtractInverseRelDomain();
    } else {
      FollowsTClause::ExtractRelPair();
    }
  }

  void ParentClause::ExtractRelExist() {
    bool is_left_num = IsNumber(token_->GetLeft());
    bool is_right_num = IsNumber(token_->GetRight());

    if (is_left_num && is_right_num) {
      bool rel_exist = pkb_.IsParent(stoi(token_->GetLeft()), stoi(token_->GetRight()));

      if (!rel_exist) {
        throw EmptyDomainException();
      }
    } else if (is_left_num || is_right_num) {
      std::vector<int> domain = is_left_num ? pkb_.GetChild(stoi(token_->GetLeft()))
                                            : pkb_.GetParent(stoi(token_->GetRight()));
      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    } else {
      std::vector<std::pair<int, int>> domain = pkb_.GetAllParentPairs();

      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    }
  }

  void ParentClause::ExtractRelDomain() {
    bool is_left_num = IsNumber(token_->GetLeft());
    std::vector<int> domain;

    if (is_left_num) {
      domain = pkb_.GetChild(stoi(token_->GetLeft()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllParentPairs();
      std::vector<int> domain_with_duplicates = ExtractSecond<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetRight(), domain);
  }

  void ParentClause::ExtractInverseRelDomain() {
    bool is_right_num = IsNumber(token_->GetRight());
    std::vector<int> domain;

    if (is_right_num) {
      domain = pkb_.GetParent(stoi(token_->GetRight()));
    } else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllParentPairs();
      std::vector<int> domain_with_duplicates = ExtractFirst<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetLeft(), domain);
  }

  void ParentClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllParentPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates_.push_back(pred);
  }

  void ParentClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      ParentClause::ExtractRelExist();
    } else if (!is_left_syn && is_right_syn) {
      ParentClause::ExtractRelDomain();
    } else if (is_left_syn && !is_right_syn) {
      ParentClause::ExtractInverseRelDomain();
    } else {
      ParentClause::ExtractRelPair();
    }
  }

  void ParentTClause::ExtractRelExist() {
    bool is_left_num = IsNumber(token_->GetLeft());
    bool is_right_num = IsNumber(token_->GetRight());

    if (is_left_num && is_right_num) {
      bool rel_exist = pkb_.IsTransitiveParent(stoi(token_->GetLeft()), stoi(token_->GetRight()));

      if (!rel_exist) {
        throw EmptyDomainException();
      }
    } else if (is_left_num || is_right_num) {
      std::vector<int> domain = is_left_num ? pkb_.GetAllChildren(stoi(token_->GetLeft()))
                                            : pkb_.GetAllParents(stoi(token_->GetRight()));
      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    } else {
      std::vector<std::pair<int, int>> domain = pkb_.GetAllTransitiveParentPairs();

      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    }
  }

  void ParentTClause::ExtractRelDomain() {
    bool is_left_num = IsNumber(token_->GetLeft());
    std::vector<int> domain;

    if (is_left_num) {
        domain = pkb_.GetAllChildren(stoi(token_->GetLeft()));
    }
    else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllTransitiveParentPairs();
      std::vector<int> domain_with_duplicates = ExtractSecond<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetRight(), domain);
  }

  void ParentTClause::ExtractInverseRelDomain() {
    bool is_right_num = IsNumber(token_->GetRight());
    std::vector<int> domain;

    if (is_right_num) {
      domain = pkb_.GetAllParents(stoi(token_->GetRight()));
    }
    else {
      std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllTransitiveParentPairs();
      std::vector<int> domain_with_duplicates = ExtractFirst<int>(domain_pair);
      domain = RemoveDuplicate<int>(domain_with_duplicates);
    }

    UpdateHashmap<int>(stmt_hashmap_, token_->GetLeft(), domain);
  }

  void ParentTClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, int>> domain_pair = pkb_.GetAllTransitiveParentPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates_.push_back(pred);
  }

  void ParentTClause::Evaluate() {
    bool is_left_syn = token_->IsSynonymLeft();
    bool is_right_syn = token_->IsSynonymRight();

    if (!is_left_syn && !is_right_syn) {
      ParentTClause::ExtractRelExist();
    } else if (!is_left_syn && is_right_syn) {
      ParentTClause::ExtractRelDomain();
    } else if (is_left_syn && !is_right_syn) {
      ParentTClause::ExtractInverseRelDomain();
    } else {
      ParentTClause::ExtractRelPair();
    }
  }

  void UsesSClause::ExtractRelExist() {
    //Uses(1, "x") or Uses(1, _)
    //first parameter must be a string of digits, second argument can be wildcard
    bool is_right_wildcard = token_->GetRight() == "_";

    if (!is_right_wildcard) {
      bool rel_exist = pkb_.IsUsesStmt(stoi(token_->GetLeft()), token_->GetRight());

      if (!rel_exist) {
        throw EmptyDomainException();
      }
    } else {
      std::vector<std::string> domain = pkb_.GetUsesVarByStmt(stoi(token_->GetLeft()));
                                              
      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    } 
  }

  void UsesSClause::ExtractRelDomain() {
    //Uses(1, v) only since the first parameter cannot be "_"
    std::vector<std::string> domain = pkb_.GetUsesVarByStmt(stoi(token_->GetLeft()));
    UpdateHashmap<std::string>(var_hashmap_, token_->GetRight(), domain);
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

    UpdateHashmap<int>(stmt_hashmap_, token_->GetLeft(), domain);
  }

  void UsesSClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, std::string>> domain_pair = pkb_.GetAllUsesStmtVarPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates_.push_back(pred);
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
        throw EmptyDomainException();
      }
    } else {
      std::vector<std::string> domain = pkb_.GetModifiesVarByStmt(stoi(token_->GetLeft()));

      if (domain.size() == 0) {
        throw EmptyDomainException();
      }
    } 
  }

  void ModifiesSClause::ExtractRelDomain() {
    // can only be Modifies(1, v)
    std::vector<std::string> domain = pkb_.GetModifiesVarByStmt(stoi(token_->GetLeft()));
    UpdateHashmap<std::string>(var_hashmap_, token_->GetRight(), domain);
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

    UpdateHashmap<int>(stmt_hashmap_, token_->GetLeft(), domain);
  }

  void ModifiesSClause::ExtractRelPair() {
    std::string left = token_->GetLeft();
    std::string right = token_->GetRight();
    std::vector<std::pair<int, std::string>> domain_pair = pkb_.GetAllModifiesStmtVarPairs();
    pql_table::Predicate pred(left, right, domain_pair);

    predicates_.push_back(pred);
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