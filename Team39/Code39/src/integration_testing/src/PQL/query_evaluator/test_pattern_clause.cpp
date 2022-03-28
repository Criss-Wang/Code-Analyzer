#include "../../../../spa/src/PQL/query_evaluator/clause/pattern_clause.h"
#include "../../../../spa/src/SP/design_extractor.h"
#include "test_util.h"

#include "catch.hpp"

using namespace std;

Pkb InitializePatternPkb() {
  /*
  01  if (1 == 2) then {
  02      while (3 == 4) {
  03          x = 1; } }
      else {
  04      if ((x == 1) && (a > b)) then {
  05          while (y == x) {
  06              z = 1; } }
          else {
  07          y =  x + y * z; } }  
  */

  Pkb pkb;

  //add variable 
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "a");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "b");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "z");

  //line 1 if (1 == 2)
  pkb.AddEntityToSet(EntityIdentifier::kStmt, 1);
  pkb.AddEntityToSet(EntityIdentifier::kIf, 1);
  pkb.AddInfoToTable(TableIdentifier::kIf, 1, vector<string>{});
  pkb.AddInfoToTable(TableIdentifier::kConstant, 1, vector<int>{1, 2});
  pkb.AddInfoToTable(TableIdentifier::kIfPattern, 1, vector<string>{});

  //line 2 while (3 == 4)
  pkb.AddEntityToSet(EntityIdentifier::kStmt, 2);
  pkb.AddEntityToSet(EntityIdentifier::kWhile, 2);
  pkb.AddInfoToTable(TableIdentifier::kWhile, 2, vector<string>{});
  pkb.AddInfoToTable(TableIdentifier::kConstant, 2, vector<int>{3, 4});
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 2, vector<string>{});
  pkb.AddInfoToTable(TableIdentifier::kWhilePattern, 2, vector<string>{});

  //line 3 x = 1
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 3);
  pkb.AddInfoToTable(TableIdentifier::kAssign, 3, "1");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 3, "1");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 3, vector<int>({1}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 3, vector<string>{ "x" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 3, vector<string>{});

  //line 4 if ((x == 1) && (a > b))
  pkb.AddEntityToSet(EntityIdentifier::kStmt, 4);
  pkb.AddEntityToSet(EntityIdentifier::kIf, 4);
  pkb.AddInfoToTable(TableIdentifier::kIf, 4, vector<string>{"a", "b", "x"});
  pkb.AddInfoToTable(TableIdentifier::kConstant, 4, vector<int>{1});
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 4, vector<string>{"a", "b", "x"});
  pkb.AddInfoToTable(TableIdentifier::kIfPattern, 4, vector<string>{"a", "b", "x"});

  //line 5 while (y == x)
  pkb.AddEntityToSet(EntityIdentifier::kStmt, 5);
  pkb.AddEntityToSet(EntityIdentifier::kWhile, 5);
  pkb.AddInfoToTable(TableIdentifier::kWhile, 5, vector<string>{"x", "y"});
  pkb.AddInfoToTable(TableIdentifier::kConstant, 5, vector<int>{});
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>{"x", "y"});
  pkb.AddInfoToTable(TableIdentifier::kWhilePattern, 5, vector<string>{"x", "y"});

  //line 6 z = 2
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 6);
  pkb.AddInfoToTable(TableIdentifier::kAssign, 6, "1");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 6, "1");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 6, vector<int>({ 2 }));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 6, vector<string>{ "z" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>{});

  //line 7 y =  x + y * z
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 7);
  pkb.AddInfoToTable(TableIdentifier::kAssign, 7, "x + y * z");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 7, "x + y * z");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 7, vector<int>({}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 7, vector<string>{ "y" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{"x", "y", "Z"});

  PopulateNestedRelationships(pkb);

  return pkb;
}

TEST_CASE("Test if pattern clause") {
  Pkb pkb_pattern = move(InitializePatternPkb());
  pql_cache::Cache cache(&pkb_pattern);
  vector<int> if_domain{1, 4};
  vector<int> var_domain{ 0,1,2,3,4 };

  unordered_map<string, vector<int>> query_domain;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_query_domain;
  vector<pql_table::Predicate> std_predicates;

  SECTION("Check when left argument is wildcard") {
    //e.g. pattern ifs (_, _, _)
    query_domain.clear();
    query_domain["ifs"] = if_domain;
    vector<int> std_domain({ 4 });
    std_query_domain.clear();
    std_query_domain["ifs"] = std_domain;

    pql_clause::IfPatternClause if_clause("ifs", "_", false);

    REQUIRE(query_domain != std_query_domain);
    if_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }
  
  SECTION("Check when left argument is entity") {
    SECTION("Positive case") {
      //e.g. pattern ifs ("a", _, _)
      query_domain.clear();
      query_domain["ifs"] = if_domain;
      vector<int> std_domain({ 4 });
      std_query_domain.clear();
      std_query_domain["ifs"] = std_domain;

      pql_clause::IfPatternClause if_clause("ifs", "a", false);

      REQUIRE(query_domain != std_query_domain);
      if_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
    
    SECTION("Negative case") {
      query_domain.clear();
      query_domain["ifs"] = if_domain;

      pql_clause::IfPatternClause if_clause("ifs", "z", false);

      CHECK_THROWS_AS(if_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Check when left argument is synonym") {
    //e.g. pattern ifs (v, _, _ )
    query_domain.clear();
    predicates.clear();
    query_domain["ifs"] = if_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["ifs"] = if_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({make_pair(4,0),make_pair(4,1),make_pair(4,2)});
    string first = "ifs";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::IfPatternClause if_clause("ifs", "v", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    if_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}

TEST_CASE("Test while pattern clause") {
  Pkb pkb_pattern = move(InitializePatternPkb());
  pql_cache::Cache cache(&pkb_pattern);
  vector<int> while_domain{2, 5};
  vector<int> var_domain{ 0,1,2,3,4 };

  unordered_map<string, vector<int>> query_domain;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_query_domain;
  vector<pql_table::Predicate> std_predicates;

  SECTION("Check when left argument is wildcard") {
    //e.g. pattern w (_, _)
    query_domain.clear();
    query_domain["w"] = while_domain;
    vector<int> std_domain({ 5 });
    std_query_domain.clear();
    std_query_domain["w"] = std_domain;

    pql_clause::WhilePatternClause while_clause("w", "_", false);

    REQUIRE(query_domain != std_query_domain);
    while_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }
  
  SECTION("Check when left argument is entity") {
    SECTION("Positive case") {
    //e.g. pattern w ("x", _)
      query_domain.clear();
      query_domain["w"] = while_domain;
     vector<int> std_domain({ 5 });
      std_query_domain.clear();
      std_query_domain["w"] = std_domain;

      pql_clause::WhilePatternClause while_clause("w", "x", false);
  
      REQUIRE(query_domain != std_query_domain);
      while_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
    
    SECTION("Negative case") {
      query_domain.clear();
      query_domain["w"] = while_domain;

      pql_clause::WhilePatternClause while_clause("w", "a", false);
      CHECK_THROWS_AS(while_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Check when left argument is synonym") {
    //e.g. pattern w (v, _)
    query_domain.clear();
    predicates.clear();
    query_domain["w"] = while_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["w"] = while_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({make_pair(5,2),make_pair(5,3)});
    string first = "w";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::WhilePatternClause while_clause("w", "v", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    while_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}

//For Evaluate in assign pattern clause pattern a (v, expr)
//We split into (1) Modifies(a, v) and (2) Check RHS of expr
//Hence we can test the arguments separately, no need to test different combinations of left and right arguments
TEST_CASE("Checks the corretness for assign pattern clause by varying the left argument") {
  Pkb pkb_pattern = move(InitializePatternPkb());
  pql_cache::Cache cache(&pkb_pattern);
  vector<int> assign_domain{3, 6, 7};
  vector<int> var_domain{ 0,1,2,3,4 };

  unordered_map<string, vector<int>> query_domain;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_query_domain;
  vector<pql_table::Predicate> std_predicates;

  SECTION("Check when left argument is wildcard") {
    //e.g. pattern a (_, _)
    query_domain.clear();
    query_domain["a"] = assign_domain;
    vector<int> std_domain({ 3, 6, 7 });
    std_query_domain.clear();
    std_query_domain["a"] = std_domain;

    pql_clause::AssignPatternClause assign_clause("a", "_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    assign_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }
  
  SECTION("Check when left argument is entity") {
    SECTION("Positive case") {
    //e.g. pattern a ("x", _)
      query_domain.clear();
      query_domain["a"] = assign_domain;
      vector<int> std_domain({ 3 });
      std_query_domain.clear();
      std_query_domain["a"] = std_domain;

      pql_clause::AssignPatternClause assign_clause("a", "x", false, "_", false);
  
      REQUIRE(query_domain != std_query_domain);
      assign_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
    
    SECTION("Negative case when no assign statement modify the variable entity") {
      query_domain.clear();
      query_domain["a"] = assign_domain;

      pql_clause::AssignPatternClause assign_clause("a", "b", false, "_", false);
      CHECK_THROWS_AS(assign_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }

    SECTION("Negative case when variable entity does not exist in source program") {
      query_domain.clear();
      query_domain["a"] = assign_domain;

      pql_clause::AssignPatternClause assign_clause("a", "gg", false, "_", false);
      CHECK_THROWS_AS(assign_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::VariableDoesNotExistException);
    }
  }

  SECTION("Check when left argument is synonym") {
    //e.g. pattern a (v, _)
    query_domain.clear();
    predicates.clear();
    query_domain["a"] = assign_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["a"] = assign_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({make_pair(3,2), make_pair(6,4), make_pair(7,3)});
    string first = "a";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::AssignPatternClause assign_clause("a", "v", true, "_", false);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    assign_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}

TEST_CASE("Checks the corretness for assign pattern clause by varying the right(expression) argument") {
  Pkb pkb_pattern = move(InitializePatternPkb());
  pql_cache::Cache cache(&pkb_pattern);
  vector<int> assign_domain{3, 6, 7};
  vector<int> var_domain{ 0,1,2,3,4 };

  unordered_map<string, vector<int>> query_domain;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_query_domain;
  vector<pql_table::Predicate> std_predicates;

  SECTION("Check when right argument is wildcard") {
    //e.g. pattern a (_, _)
    query_domain.clear();
    query_domain["a"] = assign_domain;
    vector<int> std_domain({ 3, 6, 7 });
    std_query_domain.clear();
    std_query_domain["a"] = std_domain;

    pql_clause::AssignPatternClause assign_clause("a", "_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    assign_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }
  
  SECTION("Check when right argument is exact match") {
    SECTION("Positive case") {
    //e.g. pattern a (_, "1")
      query_domain.clear();
      query_domain["a"] = assign_domain;
      vector<int> std_domain({ 3, 6 });
      std_query_domain.clear();
      std_query_domain["a"] = std_domain;

      pql_clause::AssignPatternClause assign_clause("a", "_", false, "1", true);
  
      REQUIRE(query_domain != std_query_domain);
      assign_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
    
    SECTION("Negative case") {
      query_domain.clear();
      query_domain["a"] = assign_domain;

      pql_clause::AssignPatternClause assign_clause("a", "_", false, "x + y", true);
      CHECK_THROWS_AS(assign_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Check when right argument is sub match") {
    SECTION("Positive case") {
    //e.g. pattern a (_, "y * z")
      query_domain.clear();
      query_domain["a"] = assign_domain;
      vector<int> std_domain({ 7 });
      std_query_domain.clear();
      std_query_domain["a"] = std_domain;

      pql_clause::AssignPatternClause assign_clause("a", "_", false, "y * z", false);
  
      REQUIRE(query_domain != std_query_domain);
      assign_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
    
    SECTION("Negative case") {
      query_domain.clear();
      query_domain["a"] = assign_domain;

      pql_clause::AssignPatternClause assign_clause("a", "_", false, "2", false);
      CHECK_THROWS_AS(assign_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}
