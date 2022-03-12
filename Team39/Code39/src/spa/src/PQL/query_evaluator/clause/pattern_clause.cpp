#include "pattern_clause.h"

namespace pql_clause {
  void ConsumePatternWithoutSyn(pql::PatternToken& pattern_token, Pkb& pkb,
    std::unordered_map<std::string, std::vector<int>>& domain) {
    //pattern a(_, _"x"_) or pattern a("x", _"x"_)
    //pattern a(_, _) or pattern a("x", _)
    // We split the pattern a("x", _"x"_) query into: Modifies(a, "x") and GetAllStmtsWithPattern(expr)
    bool is_left_wildcard = pattern_token.GetLeft() == "_";
    bool is_expr_wildcard = pattern_token.GetExpression() == "_";

    if (!is_left_wildcard) {
        std::vector<int> assign_domain = pkb.GetModifiesStmtsByVar(pattern_token.GetLeft());
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
        //std::vector<int> assign_domain_by_var = pkb.GetModifiesStmtsByVar(var);

        //for (int& val : assign_domain_by_var) {
        //  //need to check whether this val is in assign domain as well
        //  std::unordered_set<int>::iterator iter = assign_set.find(val);

        //  if (iter != assign_set.end()) {
        //    pred_lst.push_back(std::make_pair(val, var));
        //  }
        //}
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
    }
    else {
        ConsumePatternWithSyn(pattern_token, pkb, domain, predicates);
    }
  }
}