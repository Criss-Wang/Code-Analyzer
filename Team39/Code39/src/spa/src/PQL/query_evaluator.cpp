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

  void ConsumePatternWithoutSyn(pql::PatternToken& pattern_token, Pkb& pkb, 
                                std::unordered_map<std::string, std::vector<int>>& stmt_hashmap) {
    //pattern a(_, _"x"_) or pattern a("x", _"x"_)
    //pattern a(_, _) or pattern a("x", _)
    // We split the pattern a("x", _"x"_) query into: Modifies(a, "x") and GetAllStmtsWithPattern(expr)
    bool is_left_wildcard = pattern_token.GetLeft() == "_";
    bool is_expr_wildcard = pattern_token.GetExpression() == "_";

    if (!is_left_wildcard) {
      std::vector<int> assign_domain = pkb.GetModifiesStmtsByVar(pattern_token.GetLeft());
      UpdateHashmap(stmt_hashmap, pattern_token.GetAssignSynonym(), assign_domain);
    }

    if (!is_expr_wildcard) {
      std::unordered_set<int> domain_set = pkb_.GetAllStmtsWithPattern(pattern_token.GetExpression());
      std::vector<int> domain(domain_set.begin(), domain_set.end());
      UpdateHashmap<int>(stmt_hashmap, pattern_token.GetAssignSynonym(), domain);
    }
  }

  void ConsumePatternWithSyn(pql::PatternToken& pattern_token, Pkb& pkb,
                             std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
                             std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
                             std::vector<pql_table::Predicate>& predicates) {
    //pattern a(v, _"x"_) or pattern a(v, _)
    bool is_expr_wildcard = pattern_token.GetExpression() == "_";

    if (!is_expr_wildcard) {
      std::unordered_set<int> domain_set = pkb_.GetAllStmtsWithPattern(pattern_token.GetExpression());
      std::vector<int> domain(domain_set.begin(), domain_set.end());
      UpdateHashmap<int>(stmt_hashmap, pattern_token.GetAssignSynonym(), domain);
    }

    std::vector<std::string> var_domain = var_hashmap[pattern_token.GetLeft()];
    std::vector<std::pair<int, std::string>> pred_lst;

    for (std::string& str : var_domain) {
      std::vector<int> assign_domain = pkb.GetModifiesStmtsByVar(pattern_token.GetLeft());

      for (int& val : assign_domain) {
        pred_lst.push_back(std::make_pair(val, str));
      }
    }

    pql_table::Predicate pred(pattern_token.GetAssignSynonym(), pattern_token.GetLeft(), pred_lst);
    predicates.push_back(pred);
  }

  void ConsumePattern(pql::PatternToken& pattern_token, Pkb& pkb,
                      std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
                      std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
                      std::vector<pql_table::Predicate>& predicates) {

    bool is_left_syn = pattern_token.IsLeftSynonym();

    if (!is_left_syn) {
      ConsumePatternWithoutSyn(pattern_token, pkb, stmt_hashmap);
    } else {
      ConsumePatternWithSyn()
    }
  }

  std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb) {
    try {
      std::vector<pql::RelationshipToken> such_that_clauses = query.GetSuchThatClause();
      pql::PatternToken pattern_token = query.GetPattern();
      std::vector<pql::Synonym> synonyms = query.GetAllUsedSynonyms();
      pql::Synonym selected_syn = query.GetResultSynonym();

      std::vector<pql_table::Predicate> predicates;
      std::unordered_map<std::string, std::vector<int>> stmt_hashmap;
      std::unordered_map<std::string, std::vector<std::string>> var_hashmap;


      GetAllDomain(synonyms, stmt_hashmap, var_hashmap, pkb);

      if (pattern_token != std::nullopt) {
        ConsumePattern(pattern_token, pkb, stmt_hashmap, var_hashmap, predicates);
      }

      for (pql::RelationshipToken& token : such_that_clauses) {
        pql::Clause clause = GenerateClause(token, pkb, stmt_hashmap, var_hashmap, predicates);
        clause.Evaluate();
      }

      pql_solver::Solver solver(stmt_hashmap, var_hashmap, predicates, synonyms, selected_syn);
      std::vector<std::string> res = solver.ExtractResult();

      return res;

    } catch (EmptyDomainException e) {
      std::vector<std::string> empty_res({});
      return empty_res;
    }
  }
}
