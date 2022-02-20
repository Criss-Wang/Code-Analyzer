#include "test_util.h"
#include "../../spa/src/PQL/query_evaluator/predicate.h"
#include "../../spa/src/PQL/query_evaluator/query_evaluator_exceptions.h"
#include "../../spa/src/PQL/query_evaluator/query_evaluator.h"
#include "../../spa/src/PQL/token.h"

#include "catch.hpp"

TEST_CASE("Check correctness of ConsumePatternWithoutSyn") {
  InitializePkb();

  vector<int> assign_domain({1,2,3,5,6,7,9,10,11,12});
  unordered_map<string, vector<int>> stmt_hashmap;
  unordered_map<string, vector<int>> std_stmt_hashmap;

  SECTION("Both arguments are wildcard") {
    //pattern a (_, _)
    stmt_hashmap.clear();
    stmt_hashmap["a"] = assign_domain;
    vector<int> std_assign_domain({ 1,2,3,5,6,7,9,10,11,12 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["a"] = std_assign_domain;

    pql::PatternToken token("a", "_", "_", false, false);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    pql::ConsumePatternWithoutSyn(token, pkb, stmt_hashmap);
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is wildcard, expression is a string") {
    //pattern a (_, _"count"_)
    stmt_hashmap.clear();
    stmt_hashmap["a"] = assign_domain;
    vector<int> std_assign_domain({ 5,10,11 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["a"] = std_assign_domain;

    pql::PatternToken token("a", "_", "count", false, false);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    pql::ConsumePatternWithoutSyn(token, pkb, stmt_hashmap);
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is a string, expression is a wildcard") {
    //pattern a ("cenX", _)
    stmt_hashmap.clear();
    stmt_hashmap["a"] = assign_domain;
    vector<int> std_assign_domain({ 2,6,10 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["a"] = std_assign_domain;

    pql::PatternToken token("a", "cenX", "_", false, false);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    pql::ConsumePatternWithoutSyn(token, pkb, stmt_hashmap);
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is a string, expression is a string also") {
    //pattern a ("cenX", _"cenX"_)
    stmt_hashmap.clear();
    stmt_hashmap["a"] = assign_domain;
    vector<int> std_assign_domain({ 6,10 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["a"] = std_assign_domain;

    pql::PatternToken token("a", "cenX", "cenX", false, false);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    pql::ConsumePatternWithoutSyn(token, pkb, stmt_hashmap);
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }
}

TEST_CASE("Check correctness of ConsumPatternWithSyn") {

  vector<int> assign_domain({ 1,2,3,5,6,7,9,10,11,12 });
  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag" });
  unordered_map<string, vector<int>> stmt_hashmap;
  unordered_map<string, vector<string>> var_hashmap;
  unordered_map<string, vector<int>> std_stmt_hashmap;
  unordered_map<string, vector<string>> std_var_hashmap;
  vector<pql_table::Predicate> predicates;
  vector<pql_table::Predicate> std_predicates;

  SECTION("First argument is a synonym, expression is a wildcard") {
    //pattern a (v, _)
    stmt_hashmap.clear();
    stmt_hashmap["a"] = assign_domain;
    var_hashmap.clear();
    var_hashmap["v"] = var_domain;
    vector<int> std_assign_domain({ 1,2,3,5,6,7,9,10,11,12 });
    vector<string> std_var_domain({ "count", "cenX", "cenY", "x", "y", "flag" });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["a"] = std_assign_domain;
    std_var_hashmap.clear();
    std_var_hashmap["v"] = std_var_domain;
    vector<pair<int, string>> std_predicates_lst({ make_pair(1,"count"),make_pair(2,"cenX"),make_pair(3,"cenY"),make_pair(5,"count"),
        make_pair(6,"cenX"),make_pair(7,"cenY"),make_pair(9,"flag"),make_pair(10,"cenX"),make_pair(11,"cenY"),make_pair(12,"normSq")});
    string first = "a";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::PatternToken token("a", "v", "_", false, true);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(var_hashmap == std_var_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    pql::ConsumePatternWithSyn(token, pkb, stmt_hashmap, var_hashmap, predicates);
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(var_hashmap == std_var_hashmap);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}