#include<stdio.h>
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "clause.h"
#include "solver.h"
#include "query_evaluator.h"
#include "query_evaluator_exceptions.h"

using namespace std;

namespace pql {
   
  void GetAllDomain(std::vector<pql::Synonym>& synonyms, std::unordered_map<std::string, std::vector<int>>& stmt_hashmap, 
                    std::unordered_map<std::string, std::vector<std::string>>& var_hashmap, Pkb& pkb) {
    //hashmap stores <synonym.name, domain> pair.
      for (pql::Synonym& synonym : synonyms) {
      if (synonym.GetDeclaration() == EntityIdentifier::kVariable
          || synonym.GetDeclaration() == EntityIdentifier::kProc) {
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

  std::unique_ptr<Clause> GenerateClause(pql::RelationshipToken& token, Pkb& pkb,
                        std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
                        std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
                        std::vector<pql_table::Predicate>* predicates) {
    switch (token.GetRelationship()) {
      case RelationshipTypes::kFollows:
        return std::make_unique<FollowsClause>(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kFollowsT:
        return std::make_unique<FollowsTClause>(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kParent:
        return std::make_unique<ParentClause>(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kParentT:
        return std::make_unique<ParentTClause>(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kUsesS:
        return std::make_unique<UsesSClause>(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      case RelationshipTypes::kModifiesS:
        return std::make_unique<ModifiesSClause>(&token, pkb, stmt_hashmap, var_hashmap, predicates);
      default: 
        throw pql_exceptions::EmptyDomainException();
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
      UpdateHashmap<int>(stmt_hashmap, pattern_token.GetAssignSynonym(), assign_domain);
    }

    if (!is_expr_wildcard) {
      std::unordered_set<int> domain_set = pkb.GetAllStmtsWithPattern(pattern_token.GetExpression());
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
      std::unordered_set<int> domain_set = pkb.GetAllStmtsWithPattern(pattern_token.GetExpression());
      std::vector<int> domain(domain_set.begin(), domain_set.end());
      UpdateHashmap<int>(stmt_hashmap, pattern_token.GetAssignSynonym(), domain);
    }

    std::vector<std::string> var_domain = var_hashmap[pattern_token.GetLeft()];
    std::vector<std::pair<int, std::string>> pred_lst;
    std::vector<int> assign_domain = stmt_hashmap[pattern_token.GetAssignSynonym()];
    std::unordered_set<int> assign_set(assign_domain.begin(), assign_domain.end());

    for (std::string& str : var_domain) {
      std::vector<int> assign_domain_by_var = pkb.GetModifiesStmtsByVar(str);

      for (int& val : assign_domain_by_var) {
        //need to check whether this val is in assign domain as well
        std::unordered_set<int>::iterator iter = assign_set.find(val);

        if (iter != assign_set.end()) {
          pred_lst.push_back(std::make_pair(val, str));
        }
      }
    }

    std::string assign_syn = pattern_token.GetAssignSynonym();
    std::string var_syn = pattern_token.GetLeft();

    pql_table::Predicate pred(assign_syn, var_syn, pred_lst);
    predicates.push_back(pred);
  }

  void ConsumePattern(pql::PatternToken& pattern_token, Pkb& pkb,
                      std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
                      std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
                      std::vector<pql_table::Predicate>& predicates) {

    bool is_left_syn = pattern_token.IsSynonymLeft();

    if (!is_left_syn) {
      ConsumePatternWithoutSyn(pattern_token, pkb, stmt_hashmap);
    } else {
      ConsumePatternWithSyn(pattern_token, pkb, stmt_hashmap, var_hashmap, predicates);
    }
  }

  std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb) {
    try {
      std::vector<pql::RelationshipToken> such_that_clauses = query.GetSuchThatClause();
      std::optional<pql::PatternToken> pattern_token_opt = query.GetPattern();
      std::vector<pql::Synonym> synonyms = query.GetAllUsedSynonyms();
      //Need to wait for query preprocessor
      std::vector<pql::Synonym> selected_syns = query.GetResultSynonym();

      std::vector<pql_table::Predicate> predicates;
      std::unordered_map<std::string, std::vector<int>> stmt_hashmap;
      std::unordered_map<std::string, std::vector<std::string>> var_hashmap;


      GetAllDomain(synonyms, stmt_hashmap, var_hashmap, pkb);

      if (pattern_token_opt != std::nullopt) {
        pql::PatternToken pattern_token = pattern_token_opt.value();
        ConsumePattern(pattern_token, pkb, stmt_hashmap, var_hashmap, predicates);
      }

      for (pql::RelationshipToken& token : such_that_clauses) {
        std::unique_ptr<pql::Clause> clause = GenerateClause(token, pkb, &stmt_hashmap, &var_hashmap, &predicates);
        clause->Evaluate();
      }

      pql_solver::Solver solver(&stmt_hashmap, &var_hashmap, &predicates, synonyms, selected_syns);
      std::vector<std::string> res = solver.Solve();

      return res;

    } catch (pql_exceptions::EmptyResultException e) {
      std::vector<std::string> empty_res({});
      return empty_res;
    }
  }
}
