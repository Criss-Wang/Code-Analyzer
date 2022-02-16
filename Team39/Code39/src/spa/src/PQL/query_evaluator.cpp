#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "clause.h"
#include "solver.h"
#include "predicate.h"
#include "query_evaluator.h"
#include "query_evaluator_exceptions.h"

using namespace std;

namespace pql {
  template <typename T>
  void UpdateHashmap(unordered_map<string, vector<T>>& hmap, string name, const vector<T>& lst) {
      vector<T> oriLst = hmap[name];
      vector<T> inter = Intersect<T>(oriLst, lst);
      hmap[name] = inter;
  }

  template <typename T>
  vector<T> Intersect(const vector<T>& lst1, const vector<T>& lst2) {
      //refer from stack overflow https://stackoverflow.com/questions/38993415/how-to-apply-the-intersection-between-two-lists-in-c
      vector<T> res;
      unordered_set<T> st;
      for_each(lst2.begin(), lst2.end(), [&st](const T& k) { st.insert(k); });
      for_each(lst1.begin(), lst1.end(),
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

  bool IsNumber(const string& str) {
    for (char const& c : str) {
      if (isdigit(c) == 0) {
        return false;
      }
    }
    return true;
  }

  void GetAllDomain(std::vector<pql::Synonym>& synonyms, std::unordered_map<std::string, std::vector<int>>& stmt_hashmap, 
                    std::unordered_map<std::string, std::vector<std::string>>& var_hashmap, Pkb& pkb) {
    //hashmap stores <synonym.name, domain> pair.
    for (pql::Synonym& synonym : synonyms) {
      if (synonym.GetDeclaration() == EntityIdentifier::kVariable) {
        std::unordered_set<std::string> domain_set = pkb.GetAllEntityString(synonym.GetDeclaration());
        std::vector<std::string> domain_list(std::begin(domain_set), std::end(domain_set));
        var_hashmap.insert({ synonym.GetName(), domain_list });
      } else {
        std::unordered_set<int> domain_set = pkb.GetAllEntityInt(synonym.GetDeclaration());
        std::vector<int> domain_list(std::begin(domain_set), std::end(domain_set));
        stmt_hashmap.insert({ synonym.GetName(), domain_list });
      }
          
    }
  }

  Clause GenerateClause(pql::RelationshipToken& token, Pkb& pkb,
                        std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
                        std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
                        std::vector<pql_table::Predicate>& predicates) {
    switch (token.GetRelationship()) {
      case RelationshipTypes::kFollows:
        return FollowsClause(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kFollowsT:
        return FollowsTClause(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kParent:
        return ParentClause(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kParentT:
        return ParentTClause(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kUsesS:
        return UsesSClause(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kModifiesS:
        return ModifiesSClause(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      default: 
        return;
    }


  }

  std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb) {
    std::vector<pql::RelationshipToken> such_that_clauses = query.GetSuchThatClause();
    pql::PatternToken pattern_token = query.GetPattern();
    std::vector<pql::Synonym> synonyms = query.GetAllUsedSynonyms();
    pql::Synonym selected_syn = query.GetResultSynonym();

    std::vector<pql_table::Predicate> predicates;
    std::unordered_map<std::string, std::vector<int>> stmt_hashmap;
    std::unordered_map<std::string, std::vector<std::string>> var_hashmap;
    

    GetAllDomain(synonyms, stmt_hashmap, var_hashmap, pkb);

    if (false) {
      //this is for pattern clauses, haven't implemented yet from parser side
      /*
      Synonym s = patternClause.getSynonym();
      vector<int> matched;

      if (patternClause.getExactMatch()) {
          matched = PKB::getExactMatchAssign(patternClause.getVar(), patternClause.getExpr());
      }
      else {
          matched = PKB::getSubMatchAssign(patternClause.getVar(), patternClause.getExpr());
      }

      UpdateHashmap(hashmap, s.name, matched);

      if (hashmap[s.name].empty()) {
          //there is no assign statement that satisfy the pattern
          //return empty list
          vector<string> res{};
          return res;
      }
      */

    }

    for (RelationshipToken& token : such_that_clauses) {
      pql::Clause clause = GenerateClause(token, pkb, stmt_hashmap, var_hashmap, predicates);
      clause.Evaluate();
    }

    pql_solver::Solver solver(stmt_hashmap, var_hashmap, predicates, synonyms, selected_syn);
    std::vector<std::string> res = solver.ExtractResult();

    return res;
  }
}
