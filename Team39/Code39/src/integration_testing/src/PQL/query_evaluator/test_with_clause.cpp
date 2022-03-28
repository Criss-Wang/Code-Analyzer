#include "../../../../spa/src/PQL/query_evaluator/clause/with_clause.h"
#include "../../../../spa/src/PQL/query_evaluator/cache/cache.h"
#include "../../../../spa/src/SP/design_extractor.h"
#include "test_util.h"

#include "catch.hpp"

using namespace std;

Pkb pkb_with = std::move(InitializePkb());
pql_cache::Cache cache(&pkb_with);

unordered_map<string, vector<int>> query_domain;
vector<pql_table::Predicate> predicates;

//to be compare with (expected result)
unordered_map<string, vector<int>> std_query_domain;
vector<pql_table::Predicate> std_predicates;

TEST_CASE("Checks when LHS is entity and RHS is entity") {
  vector<int> stmt_domain = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

  SECTION("The argument is of type number") {
    SECTION("Positive case") {
      //with 10 == 10
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::WithClause with_clause(nullptr, "10", false, nullptr, "10", false);

      REQUIRE(query_domain == std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::WithClause with_clause(nullptr, "10", false, nullptr, "5", false);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::UnequalWithClauseException);
    }
  }

  SECTION("The argument is of type string") {
    SECTION("Positive case") {
      //with abc == abc
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::WithClause with_clause(nullptr, "abc", false, nullptr, "abc", false);

      REQUIRE(query_domain == std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::WithClause with_clause(nullptr, "abc", false, nullptr, "Abc", false);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::UnequalWithClauseException);
    }
  }
}

/*
AttrRef can be grouped into 3 groups: (all AttrRef in same group behaves the same)
1) stmt.stmt#, read.stmt#, print.stmt#, call.stmt#, while.stmt#, if.stmt#, assign.stmt#, constant.value
2) procedure.procName, variable.varName,
3) call.procName, read.varName, print.varName
*/
TEST_CASE("Check when LHS is AttrRef and RHS is entity") {
  vector<int> print_domain = { 6, 7, 8, 9, 24, 25 };
  vector<int> read_domain = { 4, 5 };
  vector<int> proc_domain = { 0, 1, 2, 3 };
  vector<int> var_domain = { 0, 1, 2, 3, 4, 5, 6 };
  vector<int> call_domain = { 2, 3, 13, 18 };

  SECTION("Group 1") {
    SECTION("Positive case") {
      //with print.stmt# = 7
      query_domain.clear();
      query_domain["pn"] = print_domain;
      vector<int> std_domain({ 8 });
      std_query_domain.clear();
      std_query_domain["pn"] = std_domain;

      pql::Synonym syn("pn", EntityIdentifier::kPrint);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kStmtNum);

      pql_clause::WithClause with_clause(attr_ref, "", true, nullptr, "8", false);

      REQUIRE(query_domain != std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //with print.stmt# = 1
      query_domain.clear();
      query_domain["r"] = print_domain;

      pql::Synonym syn("r", EntityIdentifier::kRead);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kStmtNum);

      pql_clause::WithClause with_clause(attr_ref, "", true, nullptr, "1", false);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Group 2") {
    SECTION("Positive case") {
      //with procedure.procName = "main"
      query_domain.clear();
      query_domain["proc"] = proc_domain;
      vector<int> std_domain({ 0 });
      std_query_domain.clear();
      std_query_domain["proc"] = std_domain;

      pql::Synonym syn("proc", EntityIdentifier::kProc);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kProcName);

      pql_clause::WithClause with_clause(attr_ref, "", true, nullptr, "main", false);

      REQUIRE(query_domain != std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case when procedure does not exits") {
      query_domain.clear();
      query_domain["proc"] = proc_domain;

      pql::Synonym syn("proc", EntityIdentifier::kProc);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kProcName);

      pql_clause::WithClause with_clause(attr_ref, "", true, nullptr, "function", false);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::ProcedureDoesNotExistException);
    }

    SECTION("Negative case when variable does not exits") {
      query_domain.clear();
      query_domain["v"] = var_domain;

      pql::Synonym syn("v", EntityIdentifier::kVariable);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kVarName);

      pql_clause::WithClause with_clause(attr_ref, "", true, nullptr, "abc", false);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::VariableDoesNotExistException);
    }
  }

  SECTION("Group 3") {
    //We can focus on positive case only as the negative cases are similar in those in group 2
    SECTION("Positive case") {
      //with call.procName = "readPoint"
      query_domain.clear();
      query_domain["c"] = call_domain;
      vector<int> std_domain({ 13, 18 });
      std_query_domain.clear();
      std_query_domain["c"] = std_domain;

      pql::Synonym syn("c", EntityIdentifier::kCall);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kProcName);

      pql_clause::WithClause with_clause(attr_ref, "", true, nullptr, "readPoint", false);

      REQUIRE(query_domain != std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
  }
}

TEST_CASE("Check when LHS is entity and RHS is AttrRef") {
  vector<int> print_domain = { 6, 7, 8, 9, 24, 25 };
  vector<int> read_domain = { 4, 5 };
  vector<int> proc_domain = { 0, 1, 2, 3 };
  vector<int> var_domain = { 0, 1, 2, 3, 4, 5, 6 };
  vector<int> call_domain = { 2, 3, 13, 18 };
  vector<int> stmt_domain = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
  vector<int> const_domain = { 0, 1, 5, 7, 20, 25 };

  SECTION("Group 1") {
    SECTION("Positive case") {
      //with 10 = s.stmt#
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 10 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql::Synonym syn("s", EntityIdentifier::kStmt);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kStmtNum);

      pql_clause::WithClause with_clause(nullptr, "10", false, attr_ref, "", true);

      REQUIRE(query_domain != std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //with 27 = constant.value
      query_domain.clear();
      query_domain["const"] = const_domain;

      pql::Synonym syn("const", EntityIdentifier::kConstant);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kValue);

      pql_clause::WithClause with_clause(nullptr, "27", false, attr_ref, "", true);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Group 2") {
    SECTION("Positive case") {
      //with "cenX" = var.varName
      query_domain.clear();
      query_domain["var"] = var_domain;
      vector<int> std_domain({ 4 });
      std_query_domain.clear();
      std_query_domain["var"] = std_domain;

      pql::Synonym syn("var", EntityIdentifier::kVariable);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kVarName);

      pql_clause::WithClause with_clause(nullptr, "cenX", false, attr_ref, "", true);

      REQUIRE(query_domain != std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case when procedure does not exits") {
      query_domain.clear();
      query_domain["proc"] = proc_domain;

      pql::Synonym syn("proc", EntityIdentifier::kProc);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kProcName);

      pql_clause::WithClause with_clause(nullptr, "Hello", false, attr_ref, "", true);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::ProcedureDoesNotExistException);
    }

    SECTION("Negative case when variable does not exits") {
      query_domain.clear();
      query_domain["v"] = var_domain;

      pql::Synonym syn("v", EntityIdentifier::kVariable);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kVarName);

      pql_clause::WithClause with_clause(nullptr, "World", false, attr_ref, "", true);

      CHECK_THROWS_AS(with_clause.Evaluate(cache, query_domain, predicates), pql_exceptions::VariableDoesNotExistException);
    }
  }

  SECTION("Group 3") {
    //We can focus on positive case only as the negative cases are similar in those in group 2
    SECTION("Positive case") {
      //with flag = print.varName
      query_domain.clear();
      query_domain["pn"] = print_domain;
      vector<int> std_domain({ 6, 8 });
      std_query_domain.clear();
      std_query_domain["pn"] = std_domain;

      pql::Synonym syn("pn", EntityIdentifier::kPrint);
      shared_ptr<pql::AttrRef> attr_ref = make_shared<pql::AttrRef>(syn, AttrIdentifier::kVarName);

      pql_clause::WithClause with_clause(nullptr, "flag", false, attr_ref, "", true);

      REQUIRE(query_domain != std_query_domain);
      with_clause.Evaluate(cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
  }
}

TEST_CASE("Check when LHS and RHS are both AttrRef") {
  vector<int> print_domain = { 6, 7, 8, 9, 24, 25 };
  vector<int> read_domain = { 4, 5 };
  vector<int> proc_domain = { 0, 1, 2, 3 };
  vector<int> var_domain = { 0, 1, 2, 3, 4, 5, 6 };
  vector<int> call_domain = { 2, 3, 13, 18 };
  vector<int> stmt_domain = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
  vector<int> const_domain = { 0, 1, 5, 7, 20, 25 };

  SECTION("Group 1") {
    // s.stmt# = constant.value
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["const"] = const_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;
    std_query_domain["const"] = const_domain;
    vector<pair<int, int>> std_predicates_lst({make_pair(1, 1), make_pair(5, 5), make_pair(7, 7), make_pair(20, 20), make_pair(25, 25) });
    string first = "s";
    string second = "const";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::Synonym left_syn("s", EntityIdentifier::kStmt);
    shared_ptr<pql::AttrRef> left_attr_ref = make_shared<pql::AttrRef>(left_syn, AttrIdentifier::kStmtNum);
    pql::Synonym right_syn("const", EntityIdentifier::kConstant);
    shared_ptr<pql::AttrRef> right_attr_ref = make_shared<pql::AttrRef>(right_syn, AttrIdentifier::kValue);

    pql_clause::WithClause with_clause(left_attr_ref, "", true, right_attr_ref, "", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    with_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }

  SECTION("Group 2") {
    //proc.procName = var.varName
    query_domain.clear();
    predicates.clear();
    query_domain["proc"] = proc_domain;
    query_domain["var"] = var_domain;
    std_query_domain.clear();
    std_query_domain["proc"] = proc_domain;
    std_query_domain["var"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({make_pair(2, 2)});
    string first = "proc";
    string second = "var";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::Synonym left_syn("proc", EntityIdentifier::kProc);
    shared_ptr<pql::AttrRef> left_attr_ref = make_shared<pql::AttrRef>(left_syn, AttrIdentifier::kProcName);
    pql::Synonym right_syn("var", EntityIdentifier::kVariable);
    shared_ptr<pql::AttrRef> right_attr_ref = make_shared<pql::AttrRef>(right_syn, AttrIdentifier::kVarName);

    pql_clause::WithClause with_clause(left_attr_ref, "", true, right_attr_ref, "", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    with_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }

  SECTION("Group 3") {
    //read.varName = print.varName
    query_domain.clear();
    predicates.clear();
    query_domain["r"] = read_domain;
    query_domain["pn"] = print_domain;
    std_query_domain.clear();
    std_query_domain["r"] = read_domain;
    std_query_domain["pn"] = print_domain;
    vector<pair<int, int>> std_predicates_lst({make_pair(4, 24), make_pair(5, 25)});
    string first = "r";
    string second = "pn";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::Synonym left_syn("r", EntityIdentifier::kRead);
    shared_ptr<pql::AttrRef> left_attr_ref = make_shared<pql::AttrRef>(left_syn, AttrIdentifier::kVarName);
    pql::Synonym right_syn("pn", EntityIdentifier::kPrint);
    shared_ptr<pql::AttrRef> right_attr_ref = make_shared<pql::AttrRef>(right_syn, AttrIdentifier::kVarName);

    pql_clause::WithClause with_clause(left_attr_ref, "", true, right_attr_ref, "", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    with_clause.Evaluate(cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
