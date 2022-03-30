#include "test_util.h"
#include "../../../../spa/src/PQL/query_evaluator/clause/such_that_clause.h"
#include "../../../../spa/src/PQL/query_evaluator/solver/predicate.h"
#include "../../../../spa/src/PQL/query_evaluator/query_evaluator_exceptions.h"
#include "../../../../spa/src/PQL/query_evaluator/cache/cache.h"

#include "catch.hpp"

using namespace std;

static unordered_map<string, vector<int>> query_domain;
static vector<pql_table::Predicate> predicates;

//to be compare with (expected result)
static unordered_map<string, vector<int>> std_query_domain;
static vector<pql_table::Predicate> std_predicates;

Pkb pkb_such_that_clause = move(InitializePkb());
pql_cache::Cache st_cache(&pkb_such_that_clause);

vector<int> print_domain = { 6, 7, 8, 9, 24, 25 };
vector<int> read_domain = { 4, 5 };
vector<int> proc_domain = { 0, 1, 2, 3 };
vector<int> var_domain = { 0, 1, 2, 3, 4, 5, 6 };
vector<int> call_domain = { 2, 3, 13, 18 };
vector<int> stmt_domain = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
vector<int> const_domain = { 0, 1, 5, 7, 20, 25 };

/*----------------------------------------Follows------------------------------------------------------------*/

TEST_CASE("Checks the correctness of Follows clause when both arguments are wildcard") {
  SECTION("Both of the argument is wildcard") {
    // Follows(_, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;

    pql_clause::FollowsClause follows_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    follows_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //The result should be the same since there is a Follows relationship
  }
}

TEST_CASE("Checks correctness when at least one of the argument is a entity") {
  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows(_, 3)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("_", false, "3", false);

      REQUIRE(query_domain == std_query_domain);
      follows_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(_, 4)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("_", false, "4", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows(1, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("1", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      follows_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(25, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("25", false, "_", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows(10, 11)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("10", false, "11", false);

      REQUIRE(query_domain == std_query_domain);
      follows_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(5, 6)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("5", false, "6", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 1,2,4,6,7,8,10,11,12,13,14,15,16,17,19,21,23,24 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsClause follows_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    follows_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows(s, 3)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 2 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("s", true, "3", false);

      REQUIRE(query_domain != std_query_domain);
      follows_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(s, 1)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("s", true, "1", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Follows(_, s)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 2,3,5,7,8,9,11,12,13,14,16,17,18,19,22,23,24,25 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsClause follows_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    follows_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Follows(14, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 19 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("14", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      follows_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(9, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsClause follows_clause("9", false, "s", true);

      CHECK_THROWS_AS(follows_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Follows(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["s1"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;
    std_query_domain["s1"] = stmt_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2), make_pair(2,3), make_pair(4,5), 
        make_pair(6,7), make_pair(7,8), make_pair(8,9), make_pair(10,11), make_pair(11,12), make_pair(12,13),
        make_pair(13,14), make_pair(14,19), make_pair(15,16), make_pair(16,17), make_pair(17,18), make_pair(19,23),
        make_pair(21,22), make_pair(23,24), make_pair(24,25) });
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::FollowsClause follows_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    follows_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Follows*-----------------------------------------------------------------*/
TEST_CASE("Checks the correctness of Follows* clause when no synonym is involved") {
  SECTION("Both of the argument is wildcard") {
    // Follows*(_, _)
    query_domain.clear();
    query_domain["s"] =stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;

    pql_clause::FollowsTClause followsT_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    followsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows*(_, 12)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("_", false, "12", false);

      REQUIRE(query_domain == std_query_domain);
      followsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(_, 10)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("_", false, "10", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows*(4, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("4", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      followsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(5, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("5", false, "_", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows*(10, 25)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("10", false, "25", false);

      REQUIRE(query_domain == std_query_domain);
      followsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(7, 3)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("7", false, "3", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows* clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows*(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 1,2,4,6,7,8,10,11,12,13,14,15,16,17,19,21,23,24 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsTClause followsT_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    followsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows*(s, 19)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 10, 11, 12, 13, 14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("s", true, "19", false);

      REQUIRE(query_domain != std_query_domain);
      followsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(s, 15)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("s", true, "15", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

    SECTION("First argument is wildcard, second argument is synonym") {
      //e.g Follows*(_, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 2,3,5,7,8,9,11,12,13,14,16,17,18,19,22,23,24,25 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("_", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      followsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Follows*(1, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 2, 3 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("1", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      followsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(9, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::FollowsTClause followsT_clause("9", false, "s", true);

      CHECK_THROWS_AS(followsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows* clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Follows*(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["s1"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;
    std_query_domain["s1"] = stmt_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2), make_pair(1,3), make_pair(2,3),
        make_pair(4,5), make_pair(6,7), make_pair(6,8), make_pair(6,9), make_pair(7,8), make_pair(7,9), make_pair(8,9),
        make_pair(10,11), make_pair(10,12), make_pair(10,13), make_pair(10,14), make_pair(10,19), make_pair(10,23),
        make_pair(10,24), make_pair(10,25), make_pair(11,12), make_pair(11,13), make_pair(11,14), make_pair(11,19), 
        make_pair(11,23), make_pair(11,24), make_pair(11,25), make_pair(12,13), make_pair(12,14), make_pair(12,19), 
        make_pair(12,23), make_pair(12,24), make_pair(12,25), make_pair(13,14), make_pair(13,19), make_pair(13,23),
        make_pair(13,24), make_pair(13,25), make_pair(14,19), make_pair(14,23), make_pair(14,24), make_pair(14,25),
        make_pair(19,23), make_pair(19,24), make_pair(19,25), make_pair(23,24), make_pair(23,25), make_pair(24,25),
        make_pair(15,16), make_pair(15,17), make_pair(15,18), make_pair(16,17), make_pair(16,18), make_pair(17,18),
        make_pair(21,22) });
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::FollowsTClause followsT_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    followsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Parent/Parent*-----------------------------------------------------------------*/
//Here we test parent and parent* together because there are no nested parent relationship in the sample SIMPLE program
TEST_CASE("Checks the correctness of Parent/Parent* clause when no synonym is involved") {
  SECTION("Both of the argument is wildcard") {
    // Parent(_, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;

    pql_clause::ParentClause parent_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    parent_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 

    //Parent*(_,_)
    query_domain.clear();
    query_domain["s"] = stmt_domain;

    pql_clause::ParentTClause parentT_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    parentT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Parent(_, 15)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("_", false, "15", false);

      REQUIRE(query_domain == std_query_domain);
      parent_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //Parent*(_, 16)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("_", false, "16", false);

      REQUIRE(query_domain == std_query_domain);
      parentT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(_, 1)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("_", false, "1", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(_, 2)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("_", false, "2", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Parent(14, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("14", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      parent_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(14, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("14", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      parentT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(11, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("11", false, "_", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(12, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("12", false, "_", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Parent(19, 20)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("19", false, "20", false);

      REQUIRE(query_domain == std_query_domain);
      parent_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(19, 21)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("19", false, "21", false);

      REQUIRE(query_domain == std_query_domain);
      parentT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(5, 4)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("5", false, "4", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);

      //e.g Parent*(1, 3)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("1", false, "3", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Parent/Parent* clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Parent(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 14,19 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::ParentClause parent_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    parent_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);

    // Parent*(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;

    pql_clause::ParentTClause parentT_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    parentT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Parent(s, 15)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("s", true, "15", false);

      REQUIRE(query_domain != std_query_domain);
      parent_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(s, 5)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("s", true, "15", false);

      REQUIRE(query_domain != std_query_domain);
      parentT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(s, 12)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("s", true, "12", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(s, 13)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("s", true, "13", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Parent(_, s)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 15, 16, 17, 18, 20, 21, 22 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::ParentClause parent_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    parent_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);

    //e.g Parent*(_, s)
    query_domain.clear();
    query_domain["s"] = stmt_domain;

    pql_clause::ParentClause parentT_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    parentT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Parent(19, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 20,21,22 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("19", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      parent_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(19, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("19", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      parentT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(9, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentClause parent_clause("9", false, "s", true);

      CHECK_THROWS_AS(parent_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(9, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ParentTClause parentT_clause("9", false, "s", true);

      CHECK_THROWS_AS(parentT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Parent/Parent* clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Parent(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["s1"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;
    std_query_domain["s1"] = stmt_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(14,15), make_pair(14,16), make_pair(14,17), make_pair(14,18),
        make_pair(19,20), make_pair(19,21), make_pair(19,22)});
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::ParentClause parent_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    parent_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));

    // Parent*(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["s1"] = stmt_domain;

    pql_clause::ParentTClause parentT_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    parentT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------UsesS-----------------------------------------------------------------*/
//In UsesS, the first argument cannot be a wildcard
//stmt s; variable v;
//Select s such that Uses()
TEST_CASE("Checks the correctness of UsesS clause when no synonym is involved") {
  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Uses(6, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::UsesSClause usesS_clause("6", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      usesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses(1, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::UsesSClause usesS_clause("1", false, "_", false);

      CHECK_THROWS_AS(usesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is a number, second argument is a string") {
    SECTION("Positive case") {
      //e.g Uses(6, "flag") 
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::UsesSClause usesS_clause("6", false, "flag", false);

      REQUIRE(query_domain == std_query_domain);
      usesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses(24, "y")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::UsesSClause usesS_clause("24", false, "y", false);

      CHECK_THROWS_AS(usesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Uses clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Uses(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_stmt_domain({ 2, 3, 6, 7, 8, 9, 14, 15, 16, 17, 19, 21, 22, 23, 24, 25 });
    std_query_domain.clear();
    std_query_domain["s"] = std_stmt_domain;

    pql_clause::UsesSClause usesS_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    usesS_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is string") {
    SECTION("Positive case") {
      //e.g Uses(s, "y")
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_stmt_domain({ 2, 14, 17, 25 });
      std_query_domain.clear();
      std_query_domain["s"] = std_stmt_domain;

      pql_clause::UsesSClause usesS_clause("s", true, "y", false);

      REQUIRE(query_domain != std_query_domain);
      usesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case when the variable does not exist") {
      //e.g UsesS(s, "a")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::UsesSClause usesS_clause("s", true, "a", false);

      CHECK_THROWS_AS(usesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::VariableDoesNotExistException);
    }

    SECTION("Negative case when the variable does exist") {
      //e.g UsesS(s, "normSq")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::UsesSClause usesS_clause("s", true, "normSq", false);

      CHECK_THROWS_AS(usesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

    
  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Uses(14, v)
      query_domain.clear();
      query_domain["v"] = var_domain;
      vector<int> std_var_domain({ 0, 2, 3, 4, 5 });
      std_query_domain.clear();
      std_query_domain["v"] = std_var_domain;

      pql_clause::UsesSClause usesS_clause("14", false, "v",true);

      REQUIRE(query_domain != std_query_domain);
      usesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses(1, v)
      query_domain.clear();
      query_domain["v"] = var_domain;

      pql_clause::UsesSClause usesS_clause("1", false, "v", true);

      CHECK_THROWS_AS(usesS_clause.Evaluate(st_cache, query_domain, predicates) , pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of UsesS clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // UsesS(s, v)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(2, 0), make_pair(2, 2), make_pair(2, 3), make_pair(2, 4), 
        make_pair(2, 5), make_pair(3, 1), make_pair(3, 4), make_pair(6, 1), make_pair(7, 4), make_pair(8, 1), 
        make_pair(9, 4), make_pair(14, 0), make_pair(14, 2), make_pair(14, 3), make_pair(14, 4), make_pair(14, 5),
        make_pair(15, 0), make_pair(16, 2), make_pair(16, 4), make_pair(17, 3), make_pair(17, 5), make_pair(19, 0),
        make_pair(19, 4), make_pair(19, 5), make_pair(21, 0), make_pair(21, 4), make_pair(22, 0), make_pair(22, 5),
        make_pair(23, 4), make_pair(23, 5), make_pair(24, 2), make_pair(25, 3) });
    string first = "s";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::UsesSClause usesS_clause("s", true, "v", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    usesS_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*----------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------ModifiesS-----------------------------------------------------------------*/
//In Modifies, the first argument cannot be a wildcard
//stmt s; variable v;
//Select s such that Modifies()
TEST_CASE("Checks the correctness of ModifiesS clause when no synonym is involved") {
  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Modifies(5, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::ModifiesSClause modifiesS_clause("5", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      modifiesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Modifies(6, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ModifiesSClause modifiesS_clause("6", false, "_", false);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is a number, second argument is a string") {
    SECTION("Positive case") {
      //e.g Modifies(4, "x")
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::ModifiesSClause modifiesS_clause("4", false, "x", false);

      REQUIRE(query_domain == std_query_domain);
      modifiesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Modifies(3, "flag")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ModifiesSClause modifiesS_clause("3", false, "flag", false);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Modifies clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Modifies(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    std_query_domain.clear();
    vector<int> std_domain = {1, 2, 4, 5, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
    std_query_domain["s"] = std_domain;

    pql_clause::ModifiesSClause modifiesS_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    modifiesS_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is string") {
    SECTION("Positive case") {
      //e.g Modifies(s, "count")
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_stmt_domain({ 2, 10, 14, 15 });
      std_query_domain.clear();
      std_query_domain["s"] = std_stmt_domain;

      pql_clause::ModifiesSClause modifiesS_clause("s", true, "count", false);

      REQUIRE(query_domain != std_query_domain);
      modifiesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Modifies(4, v)
      query_domain.clear();
      query_domain["v"] = var_domain;
      vector<int> std_domain({ 2 });
      std_query_domain.clear();
      std_query_domain["v"] = std_domain;

      pql_clause::ModifiesSClause modifiesS_clause("4", false, "v", true);

      REQUIRE(query_domain != std_query_domain);
      modifiesS_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses(8, v)
      query_domain.clear();
      query_domain["v"] = var_domain;

      pql_clause::ModifiesSClause modifiesS_clause("8", false, "v", true);

      CHECK_THROWS_AS(modifiesS_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of ModifiesS clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Modifies(s, v)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = stmt_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1, 1), make_pair(2, 0), make_pair(2, 1), make_pair(2, 2),
        make_pair(2, 3), make_pair(2, 4), make_pair(2, 5), make_pair(2, 6), make_pair(4, 2), make_pair(5, 3), 
        make_pair(10, 0), make_pair(11, 4), make_pair(12, 5), make_pair(13, 2), make_pair(13, 3), make_pair(14, 0),
        make_pair(14, 4), make_pair(14, 5), make_pair(14, 2), make_pair(14, 3), make_pair(15, 0), make_pair(16, 4),
        make_pair(17, 5), make_pair(18, 2), make_pair(18, 3), make_pair(19, 1), make_pair(19, 4), make_pair(19, 5),
        make_pair(20, 1), make_pair(21, 4), make_pair(22, 5), make_pair(23, 6), });
    string first = "s";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::ModifiesSClause modifiesS_clause("s", true, "v", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    modifiesS_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}

/*-----------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------UsesP-----------------------------------------------------------------*/
TEST_CASE("Checks the correctness of UsesP clause when no synonym is involved") {
  SECTION("The first argument is procedure name, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Uses("x", _)
      query_domain.clear();
      query_domain["p"] = proc_domain;
      std_query_domain.clear();
      std_query_domain["p"] = proc_domain;

      pql_clause::UsesPClause usesP_clause("x", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      usesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case when procedure name does exist") {
      //e.g Uses("readPoint", _)
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::UsesPClause usesP_clause("readPoint", false, "_", false);

      CHECK_THROWS_AS(usesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }

    SECTION("Negative case when procedure name does not exist") {
      //e.g Uses("foo", _)
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::UsesPClause usesP_clause("foo", false, "_", false);

      CHECK_THROWS_AS(usesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::ProcedureDoesNotExistException);
    }
  }

  SECTION("First argument is a string, second argument is a string") {
    SECTION("Positive case") {
      //e.g Uses("x", "flag") 
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::UsesPClause usesP_clause("x", false, "flag", false);

      REQUIRE(query_domain == std_query_domain);
      usesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses("x", "y")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::UsesPClause usesP_clause("x", false, "y", false);

      CHECK_THROWS_AS(usesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of UsesP clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Uses(p, _)
    query_domain.clear();
    query_domain["p"] = proc_domain;
    vector<int> std_domain({ 0, 2, 3 });
    std_query_domain.clear();
    std_query_domain["p"] = std_domain;

    pql_clause::UsesPClause usesP_clause("p", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    usesP_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is string") {
    SECTION("Positive case") {
      //e.g Uses(p, "flag")
      query_domain.clear();
      query_domain["p"] = proc_domain;
      vector<int> std_stmt_domain({ 0, 2 });
      std_query_domain.clear();
      std_query_domain["p"] = std_stmt_domain;

      pql_clause::UsesPClause usesP_clause("p", true, "flag", false);

      REQUIRE(query_domain != std_query_domain);
      usesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case when the variable does exist") {
      //e.g UsesS(p, "normSq")
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::UsesPClause usesP_clause("p", true, "normSq", false);

      CHECK_THROWS_AS(usesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

    
  SECTION("First argument is procedure name, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Uses("computeCentroid", v)
      query_domain.clear();
      query_domain["v"] = var_domain;
      vector<int> std_var_domain({ 0, 2, 3, 4, 5 });
      std_query_domain.clear();
      std_query_domain["v"] = std_var_domain;

      pql_clause::UsesPClause usesP_clause("computeCentroid", false, "v",true);

      REQUIRE(query_domain != std_query_domain);
      usesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses("readPoint", v)
      query_domain.clear();
      query_domain["v"] = var_domain;

      pql_clause::UsesPClause usesP_clause("readPoint", false, "v", true);

      CHECK_THROWS_AS(usesP_clause.Evaluate(st_cache, query_domain, predicates) , pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of UsesP clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // UsesS(p, v)
    query_domain.clear();
    predicates.clear();
    query_domain["p"] = proc_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["p"] = proc_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(0, 0), make_pair(0, 1), make_pair(0, 2), make_pair(0, 3), 
        make_pair(0, 4), make_pair(0, 5), make_pair(2, 1), make_pair(2, 4), make_pair(3, 0),  
        make_pair(3, 2), make_pair(3, 3), make_pair(3, 4), make_pair(3, 5) });
    string first = "p";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::UsesPClause usesP_clause("p", true, "v", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    usesP_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*----------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------ModifiesP-----------------------------------------------------------------*/
TEST_CASE("Checks the correctness of ModifiesP clause when no synonym is involved") {
  SECTION("The first argument is string, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Modifies(main, _)
      query_domain.clear();
      query_domain["p"] = proc_domain;
      std_query_domain.clear();
      std_query_domain["p"] = proc_domain;

      pql_clause::ModifiesPClause modifiesP_clause("main", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      modifiesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Modifies("x", _)
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::ModifiesPClause modifiesP_clause("x", false, "_", false);

      CHECK_THROWS_AS(modifiesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is a string, second argument is a string") {
    SECTION("Positive case") {
      //e.g Modifies("readPoint", "x")
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::ModifiesPClause modifiesP_clause("readPoint", false, "x", false);

      REQUIRE(query_domain == std_query_domain);
      modifiesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Modifies("readPoint", "flag")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::ModifiesPClause modifiesP_clause("readPoint", false, "flag", false);

      CHECK_THROWS_AS(modifiesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of ModifiesP clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Modifies(p, _)
    query_domain.clear();
    query_domain["p"] = proc_domain;
    std_query_domain.clear();
    vector<int> std_domain = { 0, 1, 3 };
    std_query_domain["p"] = std_domain;

    pql_clause::ModifiesPClause modifiesP_clause("p", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    modifiesP_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is string") {
    SECTION("Positive case") {
      //e.g Modifies(s, "count")
      query_domain.clear();
      query_domain["p"] = proc_domain;
      vector<int> std_domain({ 0, 3 });
      std_query_domain.clear();
      std_query_domain["p"] = std_domain;

      pql_clause::ModifiesPClause modifiesP_clause("p", true, "count", false);

      REQUIRE(query_domain != std_query_domain);
      modifiesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Modifies("readPoint", v)
      query_domain.clear();
      query_domain["v"] = var_domain;
      vector<int> std_domain({ 2, 3 });
      std_query_domain.clear();
      std_query_domain["v"] = std_domain;

      pql_clause::ModifiesPClause modifiesP_clause("readPoint", false, "v", true);

      REQUIRE(query_domain != std_query_domain);
      modifiesP_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Uses("x", v)
      query_domain.clear();
      query_domain["v"] = var_domain;

      pql_clause::ModifiesPClause modifiesP_clause("x", false, "v", true);

      CHECK_THROWS_AS(modifiesP_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of ModifiesP clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Modifies(s, v)
    query_domain.clear();
    predicates.clear();
    query_domain["p"] = proc_domain;
    query_domain["v"] = var_domain;
    std_query_domain.clear();
    std_query_domain["p"] = proc_domain;
    std_query_domain["v"] = var_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(0, 0), make_pair(0, 1), make_pair(0, 2), make_pair(0, 3),
        make_pair(0, 4), make_pair(0, 5), make_pair(0, 6), make_pair(1, 2), make_pair(1, 3), 
        make_pair(3, 0), make_pair(3, 1), make_pair(3, 2), make_pair(3, 3), make_pair(3, 4), make_pair(3, 5), make_pair(3, 6) });
    string first = "p";
    string second = "v";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::ModifiesPClause modifiesP_clause("p", true, "v", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    modifiesP_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------Calls-----------------------------------------------------------------*/
TEST_CASE("Checks the correctness of Calls clause when both arguments are wildcard") {
  SECTION("Both of the argument is wildcard") {
    // Calls(_, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;

    pql_clause::CallsClause calls_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    calls_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }
}

TEST_CASE("Checks correctness for Calls when at least one of the argument is a entity") {
  SECTION("The first argument is wildcard, second argument is procedure") {
    SECTION("Positive case") {
      //e.g Calls(_, "x")
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::CallsClause calls_clause("_", false, "x", false);

      REQUIRE(query_domain == std_query_domain);
      calls_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls(_, "main")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::CallsClause calls_clause("_", false, "main", false);

      CHECK_THROWS_AS(calls_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is procedure, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Calls("computeCentroid", _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::CallsClause calls_clause("computeCentroid", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      calls_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls("readPoint", _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::CallsClause calls_clause("readPoint", false, "_", false);

      CHECK_THROWS_AS(calls_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are strings") {
    SECTION("Positive case") {
      //e.g Calls("main", "computeCentroid")
      query_domain.clear();
      query_domain["p"] = proc_domain;
      std_query_domain.clear();
      std_query_domain["p"] = proc_domain;

      pql_clause::CallsClause calls_clause("main", false, "computeCentroid", false);

      REQUIRE(query_domain == std_query_domain);
      calls_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls("main", "readPoint")
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::CallsClause calls_clause("main", false, "readPoint", false);

      CHECK_THROWS_AS(calls_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Calls clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Calls(p, _)
    query_domain.clear();
    query_domain["p"] = proc_domain;
    vector<int> std_domain({ 0, 3 });
    std_query_domain.clear();
    std_query_domain["p"] = std_domain;

    pql_clause::CallsClause calls_clause("p", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    calls_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }

  SECTION("The first argument is synonym, second argument is procedure") {
    SECTION("Positive case") {
      //e.g Calls(p, "readPoint")
      query_domain.clear();
      query_domain["p"] = proc_domain;
      vector<int> std_domain({ 3 });
      std_query_domain.clear();
      std_query_domain["p"] = std_domain;

      pql_clause::CallsClause calls_clause("p", true, "readPoint", false);

      REQUIRE(query_domain != std_query_domain);
      calls_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls(p, "main")
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::CallsClause calls_clause("p", true, "main", false);

      CHECK_THROWS_AS(calls_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Calls(_, q)
    query_domain.clear();
    query_domain["q"] = proc_domain;
    vector<int> std_domain({ 1, 2, 3 });
    std_query_domain.clear();
    std_query_domain["q"] = std_domain;

    pql_clause::CallsClause calls_clause("_", false, "q", true);

    REQUIRE(query_domain != std_query_domain);
    calls_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is procedure, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Calls("main", q)
      query_domain.clear();
      query_domain["q"] = proc_domain;
      vector<int> std_domain({ 2, 3 });
      std_query_domain.clear();
      std_query_domain["q"] = std_domain;

      pql_clause::CallsClause calls_clause("main", false, "q", true);

      REQUIRE(query_domain != std_query_domain);
      calls_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls("readPoint", q)
      query_domain.clear();
      query_domain["q"] = proc_domain;

      pql_clause::CallsClause calls_clause("readPoint", false, "q", true);

      CHECK_THROWS_AS(calls_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Calls clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Calls(p, q)
    query_domain.clear();
    predicates.clear();
    query_domain["p"] = proc_domain;
    query_domain["q"] = proc_domain;
    std_query_domain.clear();
    std_query_domain["p"] = proc_domain;
    std_query_domain["q"] = proc_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(0,2), make_pair(0,3), make_pair(3,1) });
    string first = "p";
    string second = "q";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::CallsClause calls_clause("p", true, "q", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    calls_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------Calls*-----------------------------------------------------------------*/
TEST_CASE("Checks the correctness of Calls* clause when both arguments are wildcard") {
  SECTION("Both of the argument is wildcard") {
    // Calls*(_, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;

    pql_clause::CallsTClause callsT_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    callsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }
}

TEST_CASE("Checks correctness for Calls* when at least one of the argument is a entity") {
  SECTION("The first argument is wildcard, second argument is procedure") {
    SECTION("Positive case") {
      //e.g Calls*(_, "x")
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::CallsTClause callsT_clause("_", false, "x", false);

      REQUIRE(query_domain == std_query_domain);
      callsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls*(_, "main")
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::CallsTClause callsT_clause("_", false, "main", false);

      CHECK_THROWS_AS(callsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is procedure, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Calls*("computeCentroid", _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::CallsTClause callsT_clause("computeCentroid", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      callsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls*("readPoint", _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::CallsTClause callsT_clause("readPoint", false, "_", false);

      CHECK_THROWS_AS(callsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are strings") {
    SECTION("Positive case") {
      //e.g Calls*("main", "readPoint")
      query_domain.clear();
      query_domain["p"] = proc_domain;
      std_query_domain.clear();
      std_query_domain["p"] = proc_domain;

      pql_clause::CallsTClause callsT_clause("main", false, "readPoint", false);

      REQUIRE(query_domain == std_query_domain);
      callsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls*("readPoint", "x")
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::CallsTClause callsT_clause("readPoint", false, "x", false);

      CHECK_THROWS_AS(callsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Calls* clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Calls*(p, _)
    query_domain.clear();
    query_domain["p"] = proc_domain;
    vector<int> std_domain({ 0, 3 });
    std_query_domain.clear();
    std_query_domain["p"] = std_domain;

    pql_clause::CallsTClause callsT_clause("p", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    callsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }

  SECTION("The first argument is synonym, second argument is procedure") {
    SECTION("Positive case") {
      //e.g Calls*(p, "readPoint")
      query_domain.clear();
      query_domain["p"] = proc_domain;
      vector<int> std_domain({ 0, 3 });
      std_query_domain.clear();
      std_query_domain["p"] = std_domain;

      pql_clause::CallsTClause callsT_clause("p", true, "readPoint", false);

      REQUIRE(query_domain != std_query_domain);
      callsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls(p, "main")
      query_domain.clear();
      query_domain["p"] = proc_domain;

      pql_clause::CallsTClause callsT_clause("p", true, "main", false);

      CHECK_THROWS_AS(callsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Calls*(_, q)
    query_domain.clear();
    query_domain["q"] = proc_domain;
    vector<int> std_domain({ 1, 2, 3 });
    std_query_domain.clear();
    std_query_domain["q"] = std_domain;

    pql_clause::CallsTClause callsT_clause("_", false, "q", true);

    REQUIRE(query_domain != std_query_domain);
    callsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is procedure, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Calls*("main", q)
      query_domain.clear();
      query_domain["q"] = proc_domain;
      vector<int> std_domain({ 1, 2, 3 });
      std_query_domain.clear();
      std_query_domain["q"] = std_domain;

      pql_clause::CallsTClause callsT_clause("main", false, "q", true);

      REQUIRE(query_domain != std_query_domain);
      callsT_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Calls("readPoint", q)
      query_domain.clear();
      query_domain["q"] = proc_domain;

      pql_clause::CallsTClause callsT_clause("readPoint", false, "q", true);

      CHECK_THROWS_AS(callsT_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Calls* clause when two synonyms are involved") {
  SECTION("Both arguments are synonym") {
    // Calls(p, q)
    query_domain.clear();
    predicates.clear();
    query_domain["p"] = proc_domain;
    query_domain["q"] = proc_domain;
    std_query_domain.clear();
    std_query_domain["p"] = proc_domain;
    std_query_domain["q"] = proc_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(0,1), make_pair(0,2), make_pair(0,3), make_pair(3,1) });
    string first = "p";
    string second = "q";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::CallsTClause callsT_clause("p", true, "q", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    callsT_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------Next------------------------------------------------------------*/
TEST_CASE("Checks the correctness of Next clause when both arguments are wildcard") {
  SECTION("Both of the argument is wildcard") {
    // Next(_, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    std_query_domain.clear();
    std_query_domain["s"] = stmt_domain;

    pql_clause::NextClause next_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    next_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //The result should be the same since there is a Follows relationship
  }
}

TEST_CASE("Checks correctness of Next when at least one of the argument is a entity") {
  //SECTION("The first argument is wildcard, second argument is number") {
  //  SECTION("Positive case") {
  //    //e.g Next(_, 3)
  //    query_domain.clear();
  //    query_domain["s"] = stmt_domain;
  //    std_query_domain.clear();
  //    std_query_domain["s"] = stmt_domain;

  //    pql_clause::NextClause next_clause("_", false, "3", false);

  //    REQUIRE(query_domain == std_query_domain);
  //    next_clause.Evaluate(st_cache, query_domain, predicates);
  //    REQUIRE(query_domain == std_query_domain);
  //  }

  //  SECTION("Negative case") {
  //    //e.g Next(_, 4)
  //    query_domain.clear();
  //    query_domain["s"] = stmt_domain;

  //    pql_clause::NextClause next_clause("_", false, "4", false);

  //    CHECK_THROWS_AS(next_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
  //  }
  //}

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Next(1, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::NextClause next_clause("1", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      next_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Next(3, _)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::NextClause next_clause("3", false, "_", false);

      CHECK_THROWS_AS(next_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Next(10, 11)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      std_query_domain.clear();
      std_query_domain["s"] = stmt_domain;

      pql_clause::NextClause next_clause("10", false, "11", false);

      REQUIRE(query_domain == std_query_domain);
      next_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Next(3, 4)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::NextClause next_clause("3", false, "4", false);

      CHECK_THROWS_AS(next_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Next clause when one synonym is involved") {
  SECTION("First argument is synonym, second argument is wildcard") {
    // Next(s, _)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 1, 2, 4, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::NextClause next_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    next_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Next(s, 14)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 13, 18 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::NextClause next_clause("s", true, "14", false);

      REQUIRE(query_domain != std_query_domain);
      next_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Next(s, 1)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::NextClause next_clause("s", true, "1", false);

      CHECK_THROWS_AS(next_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Next(_, s)
    query_domain.clear();
    query_domain["s"] = stmt_domain;
    vector<int> std_domain({ 2,3,5,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::NextClause next_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    next_clause.Evaluate(st_cache, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Next(14, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;
      vector<int> std_domain({ 15, 19 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::NextClause next_clause("14", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      next_clause.Evaluate(st_cache, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Next(9, s)
      query_domain.clear();
      query_domain["s"] = stmt_domain;

      pql_clause::NextClause next_clause("9", false, "s", true);

      CHECK_THROWS_AS(next_clause.Evaluate(st_cache, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

//TEST_CASE("Checks the correctness of Follows clause when two synonyms are involved") {
//  SECTION("Both arguments are synonym") {
//    // Follows(s, s1)
//    query_domain.clear();
//    predicates.clear();
//    query_domain["s"] = stmt_domain;
//    query_domain["s1"] = stmt_domain;
//    std_query_domain.clear();
//    std_query_domain["s"] = stmt_domain;
//    std_query_domain["s1"] = stmt_domain;
//    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2), make_pair(2,3), make_pair(4,5), 
//        make_pair(6,7), make_pair(7,8), make_pair(8,9), make_pair(10,11), make_pair(11,12), make_pair(12,13),
//        make_pair(13,14), make_pair(14,19), make_pair(15,16), make_pair(16,17), make_pair(17,18), make_pair(19,23),
//        make_pair(21,22), make_pair(23,24), make_pair(24,25) });
//    string first = "s";
//    string second = "s1";
//    std_predicates.clear();
//    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));
//
//    pql_clause::FollowsClause follows_clause("s", true, "s1", true);
//
//    REQUIRE(query_domain == std_query_domain);
//    REQUIRE(!ComparePredicates(predicates, std_predicates));
//    follows_clause.Evaluate(st_cache, query_domain, predicates);
//    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
//    REQUIRE(ComparePredicates(predicates, std_predicates));
//  }
//}
