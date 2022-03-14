#include<stdio.h>
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "query_evaluator.h"
#include "solver/solver.h"
#include "clause/such_that_clause.h"
#include "query_evaluator_exceptions.h"
#include "formatter/formatter.h"

namespace pql {
  void GetAllDomain(std::vector<pql::Synonym>& synonyms, std::unordered_map<std::string, std::vector<int>>& domain, Pkb& pkb) {
    //domain stores <synonym.name, domain> pair.
    for (pql::Synonym& synonym : synonyms) {
      std::unordered_set<int> domain_set = pkb.GetAllEntity(synonym.GetDeclaration());
      std::vector<int> domain_list(std::begin(domain_set), std::end(domain_set));
      domain.insert({ synonym.GetName(), domain_list });
    }
  }

  std::unique_ptr<pql_clause::Clause> GenerateClause(pql::RelationshipToken& token, Pkb& pkb,
                        std::unordered_map<std::string, std::vector<int>>* domain,
                        std::vector<pql_table::Predicate>* predicates) {
    switch (token.GetRelationship()) {
      case RelationshipTypes::kFollows:
        return std::make_unique<pql_clause::FollowsClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kFollowsT:
        return std::make_unique<pql_clause::FollowsTClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kParent:
        return std::make_unique<pql_clause::ParentClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kParentT:
        return std::make_unique<pql_clause::ParentTClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kUsesS:
        return std::make_unique<pql_clause::UsesSClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kModifiesS:
        return std::make_unique<pql_clause::ModifiesSClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kCalls:
        return std::make_unique<pql_clause::CallsClause>(&token, pkb, domain, predicates);
      case RelationshipTypes::kCallsT:
        return std::make_unique<pql_clause::CallsTClause>(&token, pkb, domain, predicates);
      default: 
        throw pql_exceptions::EmptyDomainException();
    }
  }

  void ConsumePatternWithoutSyn(pql::PatternToken& pattern_token, Pkb& pkb, 
                                std::unordered_map<std::string, std::vector<int>>& domain) {
    //pattern a(_, _"x"_) or pattern a("x", _"x"_)
    //pattern a(_, _) or pattern a("x", _)
    // We split the pattern a("x", _"x"_) query into: Modifies(a, "x") and GetAllStmtsWithPattern(expr)
    bool is_left_wildcard = pattern_token.GetLeft() == "_";
    bool is_expr_wildcard = pattern_token.GetExpression() == "_";

    if (!is_left_wildcard) {
      std::string var_name = pattern_token.GetLeft();
      int var_index = pkb.GetIndexByVar(var_name);
      std::vector<int> assign_domain = pkb.GetModifiesStmtsByVar(var_index);
      pql_clause::UpdateHashmap<int>(domain, pattern_token.GetAssignSynonym(), assign_domain);
    }

    if (!is_expr_wildcard) {
      std::unordered_set<int> domain_set = pkb.GetAllStmtsWithPattern(pattern_token.GetExpression());
      std::vector<int> domain_lst(domain_set.begin(), domain_set.end());
      pql_clause::UpdateHashmap<int>(domain, pattern_token.GetAssignSynonym(), domain_lst);
    }
  }

  void ConsumePatternWithSyn(pql::PatternToken& pattern_token, Pkb& pkb,
                             std::unordered_map<std::string, std::vector<int>>& domain,
                             std::vector<pql_table::Predicate>& predicates) {
    //pattern a(v, _"x"_) or pattern a(v, _)
    //we evaluate pattern a (var, expr) into two parts: 1) Modifies(a, var) and 2) GetAllStmtsWithPattern(expr)
    bool is_expr_wildcard = pattern_token.GetExpression() == "_";

    if (!is_expr_wildcard) {
      std::unordered_set<int> domain_set = pkb.GetAllStmtsWithPattern(pattern_token.GetExpression());
      std::vector<int> domain_lst(domain_set.begin(), domain_set.end());
      pql_clause::UpdateHashmap<int>(domain, pattern_token.GetAssignSynonym(), domain_lst);
    }

    std::vector<int> var_domain = domain[pattern_token.GetLeft()];
    std::vector<std::pair<int, int>> pred_lst;
    std::vector<int> assign_domain = domain[pattern_token.GetAssignSynonym()];
    std::unordered_set<int> assign_set(assign_domain.begin(), assign_domain.end());

    for (int& var : var_domain) {
      std::vector<int> assign_domain_by_var = pkb.GetModifiesStmtsByVar(var);

      for (int& val : assign_domain_by_var) {
        //need to check whether this val is in assign domain as well
        std::unordered_set<int>::iterator iter = assign_set.find(val);

        if (iter != assign_set.end()) {
          pred_lst.push_back(std::make_pair(val, var));
        }
      }
    }

    std::string assign_syn = pattern_token.GetAssignSynonym();
    std::string var_syn = pattern_token.GetLeft();

    pql_table::Predicate pred(assign_syn, var_syn, pred_lst);
    predicates.push_back(pred);
  }

  void ConsumePattern(pql::PatternToken& pattern_token, Pkb& pkb,
                      std::unordered_map<std::string, std::vector<int>>& domain,
                      std::vector<pql_table::Predicate>& predicates) {

    bool is_left_syn = pattern_token.IsSynonymLeft();

    if (!is_left_syn) {
      ConsumePatternWithoutSyn(pattern_token, pkb, domain);
    } else {
      ConsumePatternWithSyn(pattern_token, pkb, domain, predicates);
    }
  }

  std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb) {
    try {
      bool is_return_boolean = query.GetBoolean();
      std::vector<pql::RelationshipToken> such_that_clauses = query.GetSuchThatClause();
      std::vector<pql::PatternToken> pattern_clauses = query.GetPattern();
      std::vector<pql::Synonym> synonyms = query.GetAllUsedSynonyms();
      std::vector<pql::Synonym> selected_syns = query.GetResultSynonym();
      std::vector<pql_table::Predicate> predicates;
      std::unordered_map<std::string, std::vector<int>> domain;

      GetAllDomain(synonyms, domain, pkb);

      for (pql::PatternToken& pattern_token : pattern_clauses) {
        ConsumePattern(pattern_token, pkb, domain, predicates);
      }

      for (pql::RelationshipToken& such_that_token : such_that_clauses) {
        std::unique_ptr<pql_clause::Clause> clause = GenerateClause(such_that_token, pkb, &domain, &predicates);
        clause->Evaluate();
      }
      
      pql_solver::Solver solver(&domain, &predicates, synonyms, selected_syns, is_return_boolean);
      pql_table::InterTable table = solver.Solve();
      Formatter formatter = Formatter(pkb);
      
      return formatter.FormatRawInput(table, selected_syns);

    } catch (pql_exceptions::TrueResultException e) {
      return BOOLEAN_TRUE_LIST;
    } catch (pql_exceptions::EmptyResultException e) {      
      if (query.GetBoolean()) {
        return BOOLEAN_FALSE_LIST;
      }
      return EMPTY_LIST;
    }
  }
}
