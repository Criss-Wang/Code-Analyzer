#include <map>
#include <set>

#include "such_that_clause.h"

#define INVALID_INDEX -1
#define WILDCARD 0
#define ENTITY 1
#define SYNONYM 2

namespace pql_clause {
  typedef bool (Pkb::* IsRelHolds)(const pql::RelationshipTypes, const int, const int);
  typedef std::vector<int> (Pkb::* GetRelDomain)(const pql::RelationshipTypes, const int);
  typedef std::vector<int> (Pkb::* GetInverseRelDomain)(const pql::RelationshipTypes, const int);
  typedef std::vector<std::pair<int, int>>(Pkb::* GetRelPairs)(const pql::RelationshipTypes);
  typedef bool (Pkb::* DoesRelHolds)(const pql::RelationshipTypes);
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

  const std::set<pql::RelationshipTypes> LeftProcedureTypeSet { pql::kCalls, pql::kCallsT, pql::kModifiesP, pql::kUsesP };
  const std::set<pql::RelationshipTypes> RightProcedureTypeSet{ pql::kCalls, pql::kCallsT };
  const std::set<pql::RelationshipTypes> RightVariableTypeSet{ pql::kModifiesS, pql::kModifiesP, pql::kUsesS, pql::kUsesP };

  int GetIntArgumentRepresentation(Pkb& pkb, pql::RelationshipTypes type, std::string& name, bool is_left) {
    //variable argument : ModifiesS/ModifiesP/UsesS/UsesP right argument
    //procedure argument : Calls/CallsT both argument and ModifiesP/UsesP left argument

    if ((is_left && LeftProcedureTypeSet.find(type) != LeftProcedureTypeSet.end())
        || (!is_left && RightProcedureTypeSet.find(type) != RightProcedureTypeSet.end())) {
      int proc_index = pkb.GetIndexByString(IndexTableType::kProcIndex, name);

      if (proc_index == INVALID_INDEX) {
        throw pql_exceptions::ProcedureDoesNotExistException();
      } else {
        return proc_index;
      }
    }

    if (!is_left && RightVariableTypeSet.find(type) != RightVariableTypeSet.end()) {
      int var_index = pkb.GetIndexByString(IndexTableType::kVarIndex, name);

      if (var_index == INVALID_INDEX) {
        throw pql_exceptions::VariableDoesNotExistException();
      } else {
        return var_index;
      }
    }

    //at this point, the name would be an integer string
    return stoi(name); 
  }

  void SuchThatClause::EvaluateWildWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    bool rel_exist = pkb.IsRelationshipExists(type_);

    if (!rel_exist) {
      throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateWildEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int right = GetIntArgumentRepresentation(pkb, type_, right_, false);
    std::vector<int> domain_lst = pkb.GetRelFirstArgument(type_, right);
    
    if (domain_lst.empty()) {
        throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateWildSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    std::vector<std::pair<int, int>> domain_pair = pkb.GetRelArgumentPairs(type_);
    std::vector<int> domain_with_duplicates = ExtractSecond<int, int>(domain_pair);
    std::vector<int> domain_lst = RemoveDuplicate<int>(domain_with_duplicates);

    UpdateHashmap<int>(domain, right_, domain_lst);
  }

  void SuchThatClause::EvaluateEntWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int left = GetIntArgumentRepresentation(pkb, type_, left_, true);
    std::vector<int> domain_lst = pkb.GetRelSecondArgument(type_, left);

    if (domain_lst.empty()) {
      throw pql_exceptions::EmptyDomainException();
    }
  }

  void SuchThatClause::EvaluateEntEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int left = GetIntArgumentRepresentation(pkb, type_, left_, true);
    int right = GetIntArgumentRepresentation(pkb, type_, right_, false);
    bool rel_exist = pkb.IsRelationshipHolds(type_, left, right);

    if (!rel_exist) {
      throw pql_exceptions::FalseRelationException();
    }
  }

  void SuchThatClause::EvaluateEntSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int left = GetIntArgumentRepresentation(pkb, type_, left_, true);
    std::vector<int> domain_lst = pkb.GetRelSecondArgument(type_, left);

    UpdateHashmap<int>(domain, right_, domain_lst);
  }

  void SuchThatClause::EvaluateSynWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    std::vector<std::pair<int, int>> domain_pair = pkb.GetRelArgumentPairs(type_);
    std::vector<int> domain_with_duplicates = ExtractFirst<int, int>(domain_pair);
    std::vector<int> domain_lst = RemoveDuplicate<int>(domain_with_duplicates);

    UpdateHashmap<int>(domain, left_, domain_lst);
  }

  void SuchThatClause::EvaluateSynEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int right = GetIntArgumentRepresentation(pkb, type_, right_, false);
    std::vector<int> domain_lst = pkb.GetRelFirstArgument(type_, right);

    UpdateHashmap<int>(domain, left_, domain_lst);
  }

  void SuchThatClause::EvaluateSynSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    std::vector<std::pair<int, int>> domain_pair = pkb.GetRelArgumentPairs(type_);
    pql_table::Predicate pred(left_, right_, domain_pair);

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
    int left_type = GetArgumentType(left_, is_synonym_left_);
    int right_type = GetArgumentType(right_, is_synonym_right_);
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
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }

  void ModifiesPClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    SuchThatClause::Evaluate(pkb, domain, predicates);
  }
}
