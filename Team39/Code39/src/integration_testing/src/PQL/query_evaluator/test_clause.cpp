#include "test_util.h"
#include "../../spa/src/PQL/query_evaluator/clause.h"
#include "../../spa/src/PQL/query_evaluator/predicate.h"
#include "../../spa/src/PQL/Token.h"

#include "catch.hpp"


TEST_CASE("Checks the correctness of Follows clause when no synonym is involved") {
  InitializePkb();

  //The query for test cases below are in this format
  //stmt s, s1; Select s such that Follows()
  unordered_map<string, vector<int>> stmt_hashmap;
  unordered_map<string, vector<string>> var_hashmap;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_stmt_hashmap;
  unordered_map<string, vector<string>> std_var_hashmap;
  vector<pql_table::Predicate> std_predicates;

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both of the argument is wildcard") {
    // Follows(_, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "_", "_", false, false);
    pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    follows_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap); //The result should be the same since there is a Follows relationship
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows(_, 3)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "_", "3", false, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      follows_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows(_, 5)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "_", "5", false, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE_THROWS(follows_clause.Evaluate());
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows(1, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "1", "_", false, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      follows_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows(14, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "14", "_", false, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE_THROWS(follows_clause.Evaluate());
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows(10, 11)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "10", "11", false, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      follows_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows(4, 5)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "4", "5", false, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE_THROWS(follows_clause.Evaluate());
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when one synonym is involved") {

  //The query for test cases below are in this format
  //stmt s, s1; Select s such that Follows()
  unordered_map<string, vector<int>> stmt_hashmap;
  unordered_map<string, vector<string>> var_hashmap;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_stmt_hashmap;
  unordered_map<string, vector<string>> std_var_hashmap;
  vector<pql_table::Predicate> std_predicates;

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,8,10,12,13 });
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "s", "_", true, false);
    pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    follows_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap); 
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows(s, 3)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 2 });
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "s", "3", true, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      follows_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows(s, 1)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "s", "1", true, false);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE_THROWS(follows_clause.Evaluate());
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Follows(_, s)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 2,3,4,6,7,8,11,12,13,14 });
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "_", "s", false, true);
    pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    follows_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows(3, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 4 });
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "3", "s", false, true);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      follows_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows(14, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "14", "s", false, true);
      pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE_THROWS(follows_clause.Evaluate());
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when two synonyms are involved") {

  //The query for test cases below are in this format
  //stmt s, s1; Select s such that Follows()
  unordered_map<string, vector<int>> stmt_hashmap;
  unordered_map<string, vector<string>> var_hashmap;
  vector<pql_table::Predicate> predicates;

  //to be compare with (expected result)
  unordered_map<string, vector<int>> std_stmt_hashmap;
  unordered_map<string, vector<string>> std_var_hashmap;
  vector<pql_table::Predicate> std_predicates;

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Follows(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    stmt_hashmap["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap["s"] = std_domain;
    std_stmt_hashmap["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2),make_pair(2,3),make_pair(3,4),make_pair(5,6),
        make_pair(6,7),make_pair(4,8),make_pair(10,11),make_pair(8,12),make_pair(12,13),make_pair(13,14) });
    string first = "s";
    string second = "s1";
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "s", "s1", true, true);
    pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    follows_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}

