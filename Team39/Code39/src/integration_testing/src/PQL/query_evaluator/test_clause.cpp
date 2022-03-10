#include "test_util.h"
#include "../../spa/src/PQL/query_evaluator/clause.h"
#include "../../spa/src/PQL/query_evaluator/predicate.h"
#include "../../spa/src/PQL/query_evaluator/query_evaluator_exceptions.h"
#include "PQL/query_parser/token.h"

#include "catch.hpp"

static unordered_map<string, vector<int>> stmt_hashmap;
static unordered_map<string, vector<string>> var_hashmap;
static vector<pql_table::Predicate> predicates;

//to be compare with (expected result)
static unordered_map<string, vector<int>> std_stmt_hashmap;
static unordered_map<string, vector<string>> std_var_hashmap;
static vector<pql_table::Predicate> std_predicates;

/*----------------------------------------Follows-----------------------------------------------------------------*/

TEST_CASE("Checks the correctness of Follows clause when no synonym is involved") {
  InitializePkb();

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both of the argument is wildcard") {
    // Follows(_, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap.clear();
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
      std_stmt_hashmap.clear();
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

      CHECK_THROWS_AS(follows_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows(1, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
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

      CHECK_THROWS_AS(follows_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows(10, 11)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
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

      CHECK_THROWS_AS(follows_clause.Evaluate(), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when one synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,8,10,12,13 });
    std_stmt_hashmap.clear();
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
      std_stmt_hashmap.clear();
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

      CHECK_THROWS_AS(follows_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Follows(_, s)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 2,3,4,6,7,8,11,12,13,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "_", "s", false, true);
    pql::FollowsClause follows_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    follows_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Follows(3, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 4 });
      std_stmt_hashmap.clear();
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

      CHECK_THROWS_AS(follows_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when two synonyms are involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Follows(s, s1)
    stmt_hashmap.clear();
    predicates.clear();
    stmt_hashmap["s"] = domain;
    stmt_hashmap["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;
    std_stmt_hashmap["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2),make_pair(2,3),make_pair(3,4),make_pair(5,6),
        make_pair(6,7),make_pair(4,8),make_pair(10,11),make_pair(8,12),make_pair(12,13),make_pair(13,14) });
    string first = "s";
    string second = "s1";
    std_predicates.clear();
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
/*-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Follows*-----------------------------------------------------------------*/

TEST_CASE("Checks the correctness of Follows* clause when no synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both of the argument is wildcard") {
    // Follows*(_, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "_", "_", false, false);
    pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    followsT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap); //The result should be the same since there is a Follows relationship
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows*(_, 12)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "_", "12", false, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      followsT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows*(_, 10)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "_", "10", false, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(followsT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows*(4, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "4", "_", false, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      followsT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows*(11, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "11", "_", false, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(followsT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows*(1, 14)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "1", "14", false, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      followsT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows*(7, 3)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "7", "3", false, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(followsT_clause.Evaluate(), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows* clause when one synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows*(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,8,10,12,13 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "s", "_", true, false);
    pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    followsT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows*(s, 8)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "s", "8", true, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      followsT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows*(s, 9)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "s", "9", true, false);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(followsT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

    SECTION("First argument is wildcard, second argument is synonym") {
      //e.g Follows*(_, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 2,3,4,6,7,8,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "_", "s", false, true);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      followsT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Follows*(3, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 4,8,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "3", "s", false, true);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      followsT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Follows*(9, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kFollowsT, "9", "s", false, true);
      pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(followsT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows* clause when two synonyms are involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Follows*(s, s1)
    stmt_hashmap.clear();
    predicates.clear();
    stmt_hashmap["s"] = domain;
    stmt_hashmap["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;
    std_stmt_hashmap["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2),make_pair(1,3),make_pair(1,4),make_pair(1,8),
        make_pair(1,12),make_pair(1,13),make_pair(1,14),make_pair(2,3),make_pair(2,4),make_pair(2,8),make_pair(2,12),
        make_pair(2,13),make_pair(2,14),make_pair(3,4),make_pair(3,8),make_pair(3,12),make_pair(3,13),make_pair(3,14),
        make_pair(4,8),make_pair(4,12),make_pair(4,13),make_pair(4,14), make_pair(8,12),make_pair(8,13),make_pair(8,14),
        make_pair(12,13),make_pair(12,14),make_pair(13,14),make_pair(5,6),make_pair(5,7),make_pair(6,7),make_pair(10,11) });
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::RelationshipToken token(pql::RelationshipTypes::kFollows, "s", "s1", true, true);
    pql::FollowsTClause followsT_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    followsT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap); 
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Parent/Parent*-----------------------------------------------------------------*/
//Here we test parent and parent* together because there are no nested parent relationship in the sample SIMPLE program
TEST_CASE("Checks the correctness of Parent/Parent* clause when no synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both of the argument is wildcard") {
    // Parent(_, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "_", "_", false, false);
    pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    parent_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap); 

    //Parent*(_,_)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;

    pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "_", "_", false, false);
    pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    parentT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Parent(_, 5)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "_", "5", false, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      parent_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);

      //Parent*(_, 6)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "_", "6", false, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      parentT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Parent(_, 1)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "_", "1", false, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parent_clause.Evaluate(), pql_exceptions::EmptyDomainException);

      //e.g Parent*(_, 2)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "_", "2", false, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parentT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Parent(4, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "4", "_", false, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      parent_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);

      //e.g Parent*(4, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "4", "_", false, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      parentT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Parent(11, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "11", "_", false, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parent_clause.Evaluate(), pql_exceptions::EmptyDomainException);

      //e.g Parent*(12, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "12", "_", false, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parentT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Parent(8, 10)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "8", "10", false, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      parent_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);

      //e.g Parent*(8, 9)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "8", "9", false, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      parentT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Parent(5, 4)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "5", "4", false, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parent_clause.Evaluate(), pql_exceptions::FalseRelationException);

      //e.g Parent*(1, 3)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "1", "3", false, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parentT_clause.Evaluate(), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Parent/Parent* clause when one synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Parent(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 4,8 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "s", "_", true, false);
    pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    parent_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);

    // Parent*(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;

    pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "s", "_", true, false);
    pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    parentT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Parent(s, 5)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 4 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "s", "5", true, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      parent_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);

      //e.g Parent*(s, 5)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "s", "5", true, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      parentT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Parent(s, 12)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "s", "12", true, false);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parent_clause.Evaluate(), pql_exceptions::EmptyDomainException);

      //e.g Parent*(s, 13)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "s", "13", true, false);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parentT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Parent(_, s)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;
    vector<int> std_domain({ 5,6,7,9,10,11 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;

    pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "_", "s", false, true);
    pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    parent_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);

    //e.g Parent*(_, s)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = domain;

    pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "_", "s", false, true);
    pql::ParentClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    parentT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Parent(8, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;
      vector<int> std_domain({ 9,10,11 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "8", "s", false, true);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      parent_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);

      //e.g Parent*(8, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "8", "s", false, true);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      parentT_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Parent(9, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "9", "s", false, true);
      pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parent_clause.Evaluate(), pql_exceptions::EmptyDomainException);

      //e.g Parent*(9, s)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = domain;

      pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "9", "s", false, true);
      pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(parentT_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Parent/Parent* clause when two synonyms are involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Parent(s, s1)
    stmt_hashmap.clear();
    predicates.clear();
    stmt_hashmap["s"] = domain;
    stmt_hashmap["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_domain;
    std_stmt_hashmap["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(4,5),make_pair(4,6),make_pair(4,7),make_pair(8,9),
        make_pair(8,10),make_pair(8,11)});
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::RelationshipToken parent_token(pql::RelationshipTypes::kParent, "s", "s1", true, true);
    pql::ParentClause parent_clause(&parent_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    parent_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(ComparePredicates(predicates, std_predicates));

    // Parent*(s, s1)
    stmt_hashmap.clear();
    predicates.clear();
    stmt_hashmap["s"] = domain;
    stmt_hashmap["s1"] = domain;

    pql::RelationshipToken parentT_token(pql::RelationshipTypes::kParentT, "s", "s1", true, true);
    pql::ParentTClause parentT_clause(&parentT_token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    parentT_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------UsesS-----------------------------------------------------------------*/
//In UsesS, the first argument cannot be a wildcard
//stmt s; variable v;
//Select s such that Uses()
TEST_CASE("Checks the correctness of UsesS clause when no synonym is involved") {

  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
  vector<string> var_domain({"count", "cenX", "cenY", "x", "y", "flag", "normSq" });

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Uses(5, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "5", "_", false, false);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      usesS_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Uses(1, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "1", "_", false, false);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is a number, second argument is a string") {
    SECTION("Positive case") {
      //e.g Uses(4, "x")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "4", "x", false, false);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      usesS_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Uses(5, "y")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "5", "y", false, false);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Uses clause when one synonym is involved") {

  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Uses(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = stmt_domain;
    vector<int> std_stmt_domain({ 4,5,6,7,8,10,11,12,14 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_stmt_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "_", true, false);
    pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    usesS_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("The first argument is synonym, second argument is string") {
    SECTION("Positive case") {
      //e.g Uses(s, "y")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;
      vector<int> std_stmt_domain({ 4,7,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "y", true, false);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      usesS_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g UsesS(s, "a")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "a", true, false);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

    
  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Uses(12, v)
      stmt_hashmap.clear();
      var_hashmap.clear();
      var_hashmap["v"] = var_domain;
      vector<string> std_var_domain({ "cenX", "cenY"});
      std_var_hashmap.clear();
      std_var_hashmap["v"] = std_var_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "12", "v", false, true);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(var_hashmap != std_var_hashmap);
      usesS_clause.Evaluate();
      REQUIRE(var_hashmap == std_var_hashmap);
    }

    SECTION("Negative case") {
      //e.g Uses(1, v)
      stmt_hashmap.clear();
      var_hashmap.clear();
      var_hashmap["v"] = var_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "1", "v", false, true);
      pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of UsesS clause when two synonyms are involved") {

  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });

  SECTION("Both arguments are synonym") {
    // UsesS(s, v)
    stmt_hashmap.clear();
    var_hashmap.clear();
    predicates.clear();
    stmt_hashmap["s"] = stmt_domain;
    var_hashmap["v"] = var_domain;
    std_stmt_hashmap.clear();
    std_var_hashmap.clear();
    std_stmt_hashmap["s"] = stmt_domain;
    std_var_hashmap["v"] = var_domain;
    vector<pair<int, string>> std_predicates_lst({ make_pair(4,"x"),make_pair(4,"y"),make_pair(4,"count"),make_pair(4,"cenX"),
        make_pair(4,"cenY"),make_pair(5,"count"),make_pair(6,"cenX"),make_pair(7,"cenY"),make_pair(6,"x"),make_pair(7,"y"),
        make_pair(8,"count"),make_pair(8,"cenX"),make_pair(8,"cenY"),make_pair(10,"cenX"),make_pair(10,"count"),make_pair(11,"cenY"),
        make_pair(11,"count"),make_pair(12,"cenX"),make_pair(12,"cenY"),make_pair(14,"y")});
    string first = "s";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "v", true, true);
    pql::UsesSClause usesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(var_hashmap == std_var_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    usesS_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(var_hashmap == std_var_hashmap);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------ModifiesS-----------------------------------------------------------------*/
//In Modifies, the first argument cannot be a wildcard
//stmt s; variable v;
//Select s such that Modifies()
TEST_CASE("Checks the correctness of ModifiesS clause when no synonym is involved") {

  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Modifies(5, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "5", "_", false, false);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      modifiesS_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Modifies(14, _)
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "14", "_", false, false);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is a number, second argument is a string") {
    SECTION("Positive case") {
      //e.g Modifies(2, "cenX")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "2", "cenX", false, false);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap == std_stmt_hashmap);
      modifiesS_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Modifies(2, "flag")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "2", "flag", false, false);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Modifies clause when one synonym is involved") {

  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Modifies(s, _)
    stmt_hashmap.clear();
    stmt_hashmap["s"] = stmt_domain;
    vector<int> std_stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13 });
    std_stmt_hashmap.clear();
    std_stmt_hashmap["s"] = std_stmt_domain;

    pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "_", true, false);
    pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap != std_stmt_hashmap);
    modifiesS_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
  }

  SECTION("The first argument is synonym, second argument is string") {
    SECTION("Positive case") {
      //e.g Modifies(s, "count")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;
      vector<int> std_stmt_domain({ 1,4,5 });
      std_stmt_hashmap.clear();
      std_stmt_hashmap["s"] = std_stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "count", true, false);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(stmt_hashmap != std_stmt_hashmap);
      modifiesS_clause.Evaluate();
      REQUIRE(stmt_hashmap == std_stmt_hashmap);
    }

    SECTION("Negative case") {
      //e.g Modifies(s, "y")
      stmt_hashmap.clear();
      stmt_hashmap["s"] = stmt_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "y", true, false);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
}


SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Modifies(4, v)
      stmt_hashmap.clear();
      var_hashmap.clear();
      var_hashmap["v"] = var_domain;
      vector<string> std_var_domain({ "count", "cenX", "cenY"});
      std_var_hashmap.clear();
      std_var_hashmap["v"] = std_var_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "4", "v", false, true);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      REQUIRE(var_hashmap != std_var_hashmap);
      modifiesS_clause.Evaluate();
      REQUIRE(var_hashmap == std_var_hashmap);
    }

    SECTION("Negative case") {
      //e.g Uses(14, v)
      stmt_hashmap.clear();
      var_hashmap.clear();
      var_hashmap["v"] = var_domain;

      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "14", "v", false, true);
      pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of ModifiesS clause when two synonyms are involved") {

  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });

  SECTION("Both arguments are synonym") {
    // Modifies(s, v)
    stmt_hashmap.clear();
    var_hashmap.clear();
    predicates.clear();
    stmt_hashmap["s"] = stmt_domain;
    var_hashmap["v"] = var_domain;
    std_stmt_hashmap.clear();
    std_var_hashmap.clear();
    std_stmt_hashmap["s"] = stmt_domain;
    std_var_hashmap["v"] = var_domain;
    vector<pair<int, string>> std_predicates_lst({ make_pair(1,"count"),make_pair(2,"cenX"),make_pair(3,"cenY"),make_pair(5,"count"),
        make_pair(6,"cenX"),make_pair(7,"cenY"),make_pair(9,"flag"),make_pair(10,"cenX"),make_pair(11,"cenY"),make_pair(12,"normSq"),
        make_pair(4,"cenX"),make_pair(4,"cenY"),make_pair(4,"count"), make_pair(8,"cenX"),make_pair(8,"cenY"),make_pair(8,"flag"),
        make_pair(13, "x")});
    string first = "s";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "v", true, true);
    pql::ModifiesSClause modifiesS_clause(&token, pkb, &stmt_hashmap, &var_hashmap, &predicates);

    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(var_hashmap == std_var_hashmap);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    modifiesS_clause.Evaluate();
    REQUIRE(stmt_hashmap == std_stmt_hashmap);
    REQUIRE(var_hashmap == std_var_hashmap);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}