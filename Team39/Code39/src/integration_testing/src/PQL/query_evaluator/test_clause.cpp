#include "test_util.h"
#include "../../spa/src/PQL/query_evaluator/clause/such_that_clause.h"
#include "../../spa/src/PQL/query_evaluator/solver/predicate.h"
#include "../../spa/src/PQL/query_evaluator/query_evaluator_exceptions.h"

#include "catch.hpp"

static unordered_map<string, vector<int>> query_domain;
static vector<pql_table::Predicate> predicates;

//to be compare with (expected result)
static unordered_map<string, vector<int>> std_query_domain;
static vector<pql_table::Predicate> std_predicates;

/*----------------------------------------Follows-----------------------------------------------------------------*/

TEST_CASE("Checks the correctness of Follows clause when both arguments are wildcard") {
    InitializePkb();

    vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

    SECTION("Both of the argument is wildcard") {
        // Follows(_, _)
        query_domain.clear();
        query_domain["s"] = domain;
        vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
        std_query_domain.clear();
        std_query_domain["s"] = std_domain;

        pql_clause::FollowsClause follows_clause("_", false, "_", false);

        REQUIRE(query_domain == std_query_domain);
        follows_clause.Evaluate(pkb, query_domain, predicates);
        REQUIRE(query_domain == std_query_domain); //The result should be the same since there is a Follows relationship
    }
}

TEST_CASE("Checks correctness when at least one of the argument is a entity") {
  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows(_, 3)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("_", false, "3", false);

      REQUIRE(query_domain == std_query_domain);
      follows_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(_, 5)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsClause follows_clause("_", false, "5", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows(1, _)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("1", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      follows_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(14, _)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsClause follows_clause("14", false, "_", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows(10, 11)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("10", false, "11", false);

      REQUIRE(query_domain == std_query_domain);
      follows_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(4, 5)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsClause follows_clause("4", false, "5", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when one synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows(s, _)
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,8,10,12,13 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsClause follows_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    follows_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows(s, 3)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 2 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("s", true, "3", false);

      REQUIRE(query_domain != std_query_domain);
      follows_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(s, 1)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsClause follows_clause("s", true, "1", false);

      CHECK_THROWS_AS(follows_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Follows(_, s)
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 2,3,4,6,7,8,11,12,13,14 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsClause follows_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    follows_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Follows(3, s)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 4 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsClause follows_clause("3", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      follows_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows(14, s)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsClause follows_clause("14", false, "s", true);

      CHECK_THROWS_AS(follows_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows clause when two synonyms are involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Follows(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = domain;
    query_domain["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;
    std_query_domain["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2),make_pair(2,3),make_pair(3,4),make_pair(5,6),
        make_pair(6,7),make_pair(4,8),make_pair(10,11),make_pair(8,12),make_pair(12,13),make_pair(13,14) });
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::FollowsClause follows_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    follows_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //Will only modify the predicates
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------Follows*-----------------------------------------------------------------*/

TEST_CASE("Checks the correctness of Follows* clause when no synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both of the argument is wildcard") {
    // Follows*(_, _)
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsTClause followsT_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    followsT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); //The result should be the same since there is a Follows relationship
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows*(_, 12)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("_", false, "12", false);

      REQUIRE(query_domain == std_query_domain);
      followsT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(_, 10)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsTClause followsT_clause("_", false, "10", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Follows*(4, _)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("4", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      followsT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(11, _)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsTClause followsT_clause("11", false, "_", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Follows*(1, 14)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("1", false, "14", false);

      REQUIRE(query_domain == std_query_domain);
      followsT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(7, 3)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsTClause followsT_clause("7", false, "3", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows* clause when one synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Follows*(s, _)
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,8,10,12,13 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::FollowsTClause followsT_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    followsT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Follows*(s, 8)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("s", true, "8", false);

      REQUIRE(query_domain != std_query_domain);
      followsT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(s, 9)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsTClause followsT_clause("s", true, "9", false);

      CHECK_THROWS_AS(followsT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

    SECTION("First argument is wildcard, second argument is synonym") {
      //e.g Follows*(_, s)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 2,3,4,6,7,8,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("_", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      followsT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Follows*(3, s)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 4,8,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::FollowsTClause followsT_clause("3", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      followsT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Follows*(9, s)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::FollowsTClause followsT_clause("9", false, "s", true);

      CHECK_THROWS_AS(followsT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Follows* clause when two synonyms are involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Follows*(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = domain;
    query_domain["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;
    std_query_domain["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(1,2),make_pair(1,3),make_pair(1,4),make_pair(1,8),
        make_pair(1,12),make_pair(1,13),make_pair(1,14),make_pair(2,3),make_pair(2,4),make_pair(2,8),make_pair(2,12),
        make_pair(2,13),make_pair(2,14),make_pair(3,4),make_pair(3,8),make_pair(3,12),make_pair(3,13),make_pair(3,14),
        make_pair(4,8),make_pair(4,12),make_pair(4,13),make_pair(4,14), make_pair(8,12),make_pair(8,13),make_pair(8,14),
        make_pair(12,13),make_pair(12,14),make_pair(13,14),make_pair(5,6),make_pair(5,7),make_pair(6,7),make_pair(10,11) });
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::FollowsTClause followsT_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    followsT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 
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
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::ParentClause parent_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    parent_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain); 

    //Parent*(_,_)
    query_domain.clear();
    query_domain["s"] = domain;

    pql_clause::ParentTClause parentT_clause("_", false, "_", false);

    REQUIRE(query_domain == std_query_domain);
    parentT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is wildcard, second argument is number") {
    SECTION("Positive case") {
      //e.g Parent(_, 5)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("_", false, "5", false);

      REQUIRE(query_domain == std_query_domain);
      parent_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //Parent*(_, 6)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("_", false, "6", false);

      REQUIRE(query_domain == std_query_domain);
      parentT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(_, 1)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentClause parent_clause("_", false, "1", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(_, 2)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("_", false, "2", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("The first argument is number, second argument is wildcard") {
    SECTION("Positive case") {
      //e.g Parent(4, _)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("4", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      parent_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(4, _)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("4", false, "_", false);

      REQUIRE(query_domain == std_query_domain);
      parentT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(11, _)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentClause parent_clause("11", false, "_", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(12, _)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("12", false, "_", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("Both arguments are number") {
    SECTION("Positive case") {
      //e.g Parent(8, 10)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("8", false, "10", false);

      REQUIRE(query_domain == std_query_domain);
      parent_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(8, 9)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("8", false, "9", false);

      REQUIRE(query_domain == std_query_domain);
      parentT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(5, 4)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentClause parent_clause("5", false, "4", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::FalseRelationException);

      //e.g Parent*(1, 3)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("1", false, "3", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::FalseRelationException);
    }
  }
}

TEST_CASE("Checks the correctness of Parent/Parent* clause when one synonym is involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("First argument is synonym, second argument is wildcard") {
    // Parent(s, _)
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 4,8 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::ParentClause parent_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    parent_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);

    // Parent*(s, _)
    query_domain.clear();
    query_domain["s"] = domain;

    pql_clause::ParentTClause parentT_clause("s", true, "_", false);

    REQUIRE(query_domain != std_query_domain);
    parentT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("The first argument is synonym, second argument is number") {
    SECTION("Positive case") {
      //e.g Parent(s, 5)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 4 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("s", true, "5", false);

      REQUIRE(query_domain != std_query_domain);
      parent_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(s, 5)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("s", true, "5", false);

      REQUIRE(query_domain != std_query_domain);
      parentT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(s, 12)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentClause parent_clause("s", true, "12", false);

      CHECK_THROWS_AS(parent_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(s, 13)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("s", true, "13", false);

      CHECK_THROWS_AS(parentT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }

  SECTION("First argument is wildcard, second argument is synonym") {
    //e.g Parent(_, s)
    query_domain.clear();
    query_domain["s"] = domain;
    vector<int> std_domain({ 5,6,7,9,10,11 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;

    pql_clause::ParentClause parent_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    parent_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);

    //e.g Parent*(_, s)
    query_domain.clear();
    query_domain["s"] = domain;

    pql_clause::ParentClause parentT_clause("_", false, "s", true);

    REQUIRE(query_domain != std_query_domain);
    parentT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
  }

  SECTION("First argument is number, second argument is synonym") {
    SECTION("Positive case") {
      //e.g Parent(8, s)
      query_domain.clear();
      query_domain["s"] = domain;
      vector<int> std_domain({ 9,10,11 });
      std_query_domain.clear();
      std_query_domain["s"] = std_domain;

      pql_clause::ParentClause parent_clause("8", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      parent_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);

      //e.g Parent*(8, s)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("8", false, "s", true);

      REQUIRE(query_domain != std_query_domain);
      parentT_clause.Evaluate(pkb, query_domain, predicates);
      REQUIRE(query_domain == std_query_domain);
    }

    SECTION("Negative case") {
      //e.g Parent(9, s)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentClause parent_clause("9", false, "s", true);

      CHECK_THROWS_AS(parent_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);

      //e.g Parent*(9, s)
      query_domain.clear();
      query_domain["s"] = domain;

      pql_clause::ParentTClause parentT_clause("9", false, "s", true);

      CHECK_THROWS_AS(parentT_clause.Evaluate(pkb, query_domain, predicates), pql_exceptions::EmptyDomainException);
    }
  }
}

TEST_CASE("Checks the correctness of Parent/Parent* clause when two synonyms are involved") {

  vector<int> domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });

  SECTION("Both arguments are synonym") {
    // Parent(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = domain;
    query_domain["s1"] = domain;
    vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
    std_query_domain.clear();
    std_query_domain["s"] = std_domain;
    std_query_domain["s1"] = std_domain;
    vector<pair<int, int>> std_predicates_lst({ make_pair(4,5),make_pair(4,6),make_pair(4,7),make_pair(8,9),
        make_pair(8,10),make_pair(8,11)});
    string first = "s";
    string second = "s1";
    std_predicates.clear();
    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));

    pql_clause::ParentClause parent_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    parent_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));

    // Parent*(s, s1)
    query_domain.clear();
    predicates.clear();
    query_domain["s"] = domain;
    query_domain["s1"] = domain;

    pql_clause::ParentTClause parentT_clause("s", true, "s1", true);

    REQUIRE(query_domain == std_query_domain);
    REQUIRE(!ComparePredicates(predicates, std_predicates));
    parentT_clause.Evaluate(pkb, query_domain, predicates);
    REQUIRE(query_domain == std_query_domain);
    REQUIRE(ComparePredicates(predicates, std_predicates));
  }
}
/*-----------------------------------------------------------------------------------------------------------------*/

///*--------------------------------------------UsesS-----------------------------------------------------------------*/
////In UsesS, the first argument cannot be a wildcard
////stmt s; variable v;
////Select s such that Uses()
//TEST_CASE("Checks the correctness of UsesS clause when no synonym is involved") {
//
//  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//  vector<string> var_domain({"count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  SECTION("The first argument is number, second argument is wildcard") {
//    SECTION("Positive case") {
//      //e.g Uses(5, _)
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//      std_query_domain.clear();
//      std_query_domain["s"] = std_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "5", "_", false, false);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(query_domain == std_query_domain);
//      usesS_clause.Evaluate();
//      REQUIRE(query_domain == std_query_domain);
//    }
//
//    SECTION("Negative case") {
//      //e.g Uses(1, _)
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "1", "_", false, false);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
//    }
//  }
//
//  SECTION("First argument is a number, second argument is a string") {
//    SECTION("Positive case") {
//      //e.g Uses(4, "x")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//      std_query_domain.clear();
//      std_query_domain["s"] = std_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "4", "x", false, false);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(query_domain == std_query_domain);
//      usesS_clause.Evaluate();
//      REQUIRE(query_domain == std_query_domain);
//    }
//
//    SECTION("Negative case") {
//      //e.g Uses(5, "y")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "5", "y", false, false);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::FalseRelationException);
//    }
//  }
//}
//
//TEST_CASE("Checks the correctness of Uses clause when one synonym is involved") {
//
//  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  SECTION("First argument is synonym, second argument is wildcard") {
//    // Uses(s, _)
//    query_domain.clear();
//    query_domain["s"] = stmt_domain;
//    vector<int> std_stmt_domain({ 4,5,6,7,8,10,11,12,14 });
//    std_query_domain.clear();
//    std_query_domain["s"] = std_stmt_domain;
//
//    pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "_", true, false);
//    pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//    REQUIRE(query_domain != std_query_domain);
//    usesS_clause.Evaluate();
//    REQUIRE(query_domain == std_query_domain);
//  }
//
//  SECTION("The first argument is synonym, second argument is string") {
//    SECTION("Positive case") {
//      //e.g Uses(s, "y")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//      vector<int> std_stmt_domain({ 4,7,14 });
//      std_query_domain.clear();
//      std_query_domain["s"] = std_stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "y", true, false);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(query_domain != std_query_domain);
//      usesS_clause.Evaluate();
//      REQUIRE(query_domain == std_query_domain);
//    }
//
//    SECTION("Negative case") {
//      //e.g UsesS(s, "a")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "a", true, false);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
//    }
//  }
//
//    
//  SECTION("First argument is number, second argument is synonym") {
//    SECTION("Positive case") {
//      //e.g Uses(12, v)
//      query_domain.clear();
//      var_hashmap.clear();
//      var_hashmap["v"] = var_domain;
//      vector<string> std_var_domain({ "cenX", "cenY"});
//      std_var_hashmap.clear();
//      std_var_hashmap["v"] = std_var_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "12", "v", false, true);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(var_hashmap != std_var_hashmap);
//      usesS_clause.Evaluate();
//      REQUIRE(var_hashmap == std_var_hashmap);
//    }
//
//    SECTION("Negative case") {
//      //e.g Uses(1, v)
//      query_domain.clear();
//      var_hashmap.clear();
//      var_hashmap["v"] = var_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "1", "v", false, true);
//      pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(usesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
//    }
//  }
//}
//
//TEST_CASE("Checks the correctness of UsesS clause when two synonyms are involved") {
//
//  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  SECTION("Both arguments are synonym") {
//    // UsesS(s, v)
//    query_domain.clear();
//    var_hashmap.clear();
//    predicates.clear();
//    query_domain["s"] = stmt_domain;
//    var_hashmap["v"] = var_domain;
//    std_query_domain.clear();
//    std_var_hashmap.clear();
//    std_query_domain["s"] = stmt_domain;
//    std_var_hashmap["v"] = var_domain;
//    vector<pair<int, string>> std_predicates_lst({ make_pair(4,"x"),make_pair(4,"y"),make_pair(4,"count"),make_pair(4,"cenX"),
//        make_pair(4,"cenY"),make_pair(5,"count"),make_pair(6,"cenX"),make_pair(7,"cenY"),make_pair(6,"x"),make_pair(7,"y"),
//        make_pair(8,"count"),make_pair(8,"cenX"),make_pair(8,"cenY"),make_pair(10,"cenX"),make_pair(10,"count"),make_pair(11,"cenY"),
//        make_pair(11,"count"),make_pair(12,"cenX"),make_pair(12,"cenY"),make_pair(14,"y")});
//    string first = "s";
//    string second = "v";
//    std_predicates.clear();
//    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));
//
//    pql::RelationshipToken token(pql::RelationshipTypes::kUsesS, "s", "v", true, true);
//    pql::UsesSClause usesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//    REQUIRE(query_domain == std_query_domain);
//    REQUIRE(var_hashmap == std_var_hashmap);
//    REQUIRE(!ComparePredicates(predicates, std_predicates));
//    usesS_clause.Evaluate();
//    REQUIRE(query_domain == std_query_domain);
//    REQUIRE(var_hashmap == std_var_hashmap);
//    REQUIRE(ComparePredicates(predicates, std_predicates));
//  }
//}
///*-----------------------------------------------------------------------------------------------------------------*/
//
///*--------------------------------------------ModifiesS-----------------------------------------------------------------*/
////In Modifies, the first argument cannot be a wildcard
////stmt s; variable v;
////Select s such that Modifies()
//TEST_CASE("Checks the correctness of ModifiesS clause when no synonym is involved") {
//
//  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  SECTION("The first argument is number, second argument is wildcard") {
//    SECTION("Positive case") {
//      //e.g Modifies(5, _)
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//      std_query_domain.clear();
//      std_query_domain["s"] = std_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "5", "_", false, false);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(query_domain == std_query_domain);
//      modifiesS_clause.Evaluate();
//      REQUIRE(query_domain == std_query_domain);
//    }
//
//    SECTION("Negative case") {
//      //e.g Modifies(14, _)
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "14", "_", false, false);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
//    }
//  }
//
//  SECTION("First argument is a number, second argument is a string") {
//    SECTION("Positive case") {
//      //e.g Modifies(2, "cenX")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//      vector<int> std_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//      std_query_domain.clear();
//      std_query_domain["s"] = std_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "2", "cenX", false, false);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(query_domain == std_query_domain);
//      modifiesS_clause.Evaluate();
//      REQUIRE(query_domain == std_query_domain);
//    }
//
//    SECTION("Negative case") {
//      //e.g Modifies(2, "flag")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "2", "flag", false, false);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::FalseRelationException);
//    }
//  }
//}
//
//TEST_CASE("Checks the correctness of Modifies clause when one synonym is involved") {
//
//  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  SECTION("First argument is synonym, second argument is wildcard") {
//    // Modifies(s, _)
//    query_domain.clear();
//    query_domain["s"] = stmt_domain;
//    vector<int> std_stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13 });
//    std_query_domain.clear();
//    std_query_domain["s"] = std_stmt_domain;
//
//    pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "_", true, false);
//    pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//    REQUIRE(query_domain != std_query_domain);
//    modifiesS_clause.Evaluate();
//    REQUIRE(query_domain == std_query_domain);
//  }
//
//  SECTION("The first argument is synonym, second argument is string") {
//    SECTION("Positive case") {
//      //e.g Modifies(s, "count")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//      vector<int> std_stmt_domain({ 1,4,5 });
//      std_query_domain.clear();
//      std_query_domain["s"] = std_stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "count", true, false);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(query_domain != std_query_domain);
//      modifiesS_clause.Evaluate();
//      REQUIRE(query_domain == std_query_domain);
//    }
//
//    SECTION("Negative case") {
//      //e.g Modifies(s, "y")
//      query_domain.clear();
//      query_domain["s"] = stmt_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "y", true, false);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
//    }
//}
//
//
//SECTION("First argument is number, second argument is synonym") {
//    SECTION("Positive case") {
//      //e.g Modifies(4, v)
//      query_domain.clear();
//      var_hashmap.clear();
//      var_hashmap["v"] = var_domain;
//      vector<string> std_var_domain({ "count", "cenX", "cenY"});
//      std_var_hashmap.clear();
//      std_var_hashmap["v"] = std_var_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "4", "v", false, true);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      REQUIRE(var_hashmap != std_var_hashmap);
//      modifiesS_clause.Evaluate();
//      REQUIRE(var_hashmap == std_var_hashmap);
//    }
//
//    SECTION("Negative case") {
//      //e.g Uses(14, v)
//      query_domain.clear();
//      var_hashmap.clear();
//      var_hashmap["v"] = var_domain;
//
//      pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "14", "v", false, true);
//      pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//      CHECK_THROWS_AS(modifiesS_clause.Evaluate(), pql_exceptions::EmptyDomainException);
//    }
//  }
//}
//
//TEST_CASE("Checks the correctness of ModifiesS clause when two synonyms are involved") {
//
//  vector<int> stmt_domain({ 1,2,3,4,5,6,7,8,9,10,11,12,13,14 });
//  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  SECTION("Both arguments are synonym") {
//    // Modifies(s, v)
//    query_domain.clear();
//    var_hashmap.clear();
//    predicates.clear();
//    query_domain["s"] = stmt_domain;
//    var_hashmap["v"] = var_domain;
//    std_query_domain.clear();
//    std_var_hashmap.clear();
//    std_query_domain["s"] = stmt_domain;
//    std_var_hashmap["v"] = var_domain;
//    vector<pair<int, string>> std_predicates_lst({ make_pair(1,"count"),make_pair(2,"cenX"),make_pair(3,"cenY"),make_pair(5,"count"),
//        make_pair(6,"cenX"),make_pair(7,"cenY"),make_pair(9,"flag"),make_pair(10,"cenX"),make_pair(11,"cenY"),make_pair(12,"normSq"),
//        make_pair(4,"cenX"),make_pair(4,"cenY"),make_pair(4,"count"), make_pair(8,"cenX"),make_pair(8,"cenY"),make_pair(8,"flag"),
//        make_pair(13, "x")});
//    string first = "s";
//    string second = "v";
//    std_predicates.clear();
//    std_predicates.push_back(pql_table::Predicate(first, second, std_predicates_lst));
//
//    pql::RelationshipToken token(pql::RelationshipTypes::kModifiesS, "s", "v", true, true);
//    pql::ModifiesSClause modifiesS_clause(&token, pkb, &query_domain, &var_hashmap, &predicates);
//
//    REQUIRE(query_domain == std_query_domain);
//    REQUIRE(var_hashmap == std_var_hashmap);
//    REQUIRE(!ComparePredicates(predicates, std_predicates));
//    modifiesS_clause.Evaluate();
//    REQUIRE(query_domain == std_query_domain);
//    REQUIRE(var_hashmap == std_var_hashmap);
//    REQUIRE(ComparePredicates(predicates, std_predicates));
//  }
//}

