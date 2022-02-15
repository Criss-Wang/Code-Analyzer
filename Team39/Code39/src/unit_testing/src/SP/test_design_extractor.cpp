#include "SP/design_extractor.h"
#include "PKB/pkb.h"

#include "catch.hpp"

using namespace std;

TEST_CASE("Test Nested Population for Follows") {
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kFollows, 1, 2);
  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 3) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 3, 4) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 4, 5) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 7, 8) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 8, 9) && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check PQL queries for nested follows") {
    REQUIRE(pkb.IsTransitiveFollows(1, 2));
    REQUIRE(pkb.IsTransitiveFollows(1, 3));
    REQUIRE(pkb.IsTransitiveFollows(1, 4));
    REQUIRE(pkb.IsTransitiveFollows(1, 5));
    REQUIRE(pkb.IsTransitiveFollows(7, 9));
    // 6 does not exist
    REQUIRE(!pkb.IsTransitiveFollows(1, 6));
    REQUIRE(!pkb.IsTransitiveFollows(1, 7));
    REQUIRE(!pkb.IsTransitiveFollows(1, 1));
    REQUIRE(!pkb.IsTransitiveFollows(8, 7));
    REQUIRE(!pkb.IsTransitiveFollows(4, 1));

    REQUIRE(pkb.GetStmtsBefore(1) == vector<int>{});
    REQUIRE(pkb.GetStmtsBefore(2) == vector<int>{1});
    REQUIRE(pkb.GetStmtsBefore(3) == vector<int>{2, 1});
    REQUIRE(pkb.GetStmtsBefore(4) == vector<int>{3, 2, 1});
    REQUIRE(pkb.GetStmtsBefore(7) == vector<int>{});
    REQUIRE(pkb.GetStmtsBefore(8) == vector<int>{7});
    REQUIRE(pkb.GetStmtsBefore(9) == vector<int>{8, 7});
    // 6 does not exist
    REQUIRE(pkb.GetStmtsBefore(6) == vector<int>{});

    REQUIRE(pkb.GetStmtsBefore(7) != vector<int>{4, 3, 2, 1});
    REQUIRE(pkb.GetStmtsBefore(4) != vector<int>{2, 1});

    REQUIRE(pkb.GetStmtsAfter(1) == vector<int>{2, 3, 4, 5});
    REQUIRE(pkb.GetStmtsAfter(2) == vector<int>{3, 4, 5});
    REQUIRE(pkb.GetStmtsAfter(3) == vector<int>{4, 5});
    REQUIRE(pkb.GetStmtsAfter(7) == vector<int>{8, 9});
    REQUIRE(pkb.GetStmtsAfter(8) == vector<int>{9});
    REQUIRE(pkb.GetStmtsAfter(1000) == vector<int>{});

    REQUIRE(pkb.GetStmtsAfter(1) != vector<int>{3, 4});

    vector<pair<int, int>> transitive_follows_pairs = pkb.GetAllTransitiveFollowsPairs();
    vector<pair<int, int>> expected_transitive_follows_pairs = vector<pair<int, int>>{make_pair(1, 2), make_pair(1, 3), make_pair(1, 4), make_pair(1, 5),
      make_pair(2, 3), make_pair(2, 4), make_pair(2, 5), make_pair(3, 4), make_pair(3, 5), make_pair(4, 5), make_pair(7, 8), make_pair(7, 9), make_pair(8, 9)};
    vector<pair<int, int>> invalid_first_pair = vector<pair<int, int>>{make_pair(1, 2)};
    // Sort to ensure that ordering of pairs in vector does not matter
    std::sort(expected_transitive_follows_pairs.begin(), expected_transitive_follows_pairs.end());
    std::sort(transitive_follows_pairs.begin(), transitive_follows_pairs.end());
    REQUIRE(transitive_follows_pairs == expected_transitive_follows_pairs);
    REQUIRE(transitive_follows_pairs != invalid_first_pair);
  }
}

TEST_CASE("Test Nested Population for Parent") {
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, vector<int>{2, 3, 6, 7, 8});
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 3, vector<int>{4, 5}) && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check PQL queries for nested parent") {
    REQUIRE(pkb.IsTransitiveParent(1, 2));
    REQUIRE(pkb.IsTransitiveParent(1, 3));
    REQUIRE(pkb.IsTransitiveParent(1, 6));
    REQUIRE(pkb.IsTransitiveParent(1, 7));
    REQUIRE(pkb.IsTransitiveParent(1, 8));
    REQUIRE(pkb.IsTransitiveParent(1, 4));
    REQUIRE(pkb.IsTransitiveParent(1, 5));
    REQUIRE(pkb.IsTransitiveParent(3, 4));
    REQUIRE(pkb.IsTransitiveParent(3, 5));

    REQUIRE(!pkb.IsTransitiveParent(3, 2));
    // 10 does not exist
    REQUIRE(!pkb.IsTransitiveParent(10, 2));
    REQUIRE(!pkb.IsTransitiveParent(10, 12));

    vector<int> parents_of_4 = pkb.GetAllParents(4);
    vector<int> expected_parents = {3, 1};
    vector<int> invalid_parents = {};
    REQUIRE(parents_of_4 == expected_parents);
    REQUIRE(parents_of_4 != invalid_parents);

    vector<int> parents_of_7 = pkb.GetAllParents(7);
    expected_parents = {1};
    REQUIRE(parents_of_7 == expected_parents);
    REQUIRE(parents_of_7 != invalid_parents);

    // 100 does not exist
    vector<int> parents_of_100 = pkb.GetAllParents(100);
    expected_parents = {};
    REQUIRE(parents_of_100 == expected_parents);

    vector<int> children_of_1 = pkb.GetAllChildren(1);
    vector<int> expected_children = {2, 3, 4, 5, 6, 7, 8};
    vector<int> invalid_children = {};
    std::sort(children_of_1.begin(), children_of_1.end());
    std::sort(expected_children.begin(), expected_children.end());
    REQUIRE(children_of_1 == expected_children);
    REQUIRE(children_of_1 != invalid_children);

    vector<int> children_of_3 = pkb.GetAllChildren(3);
    expected_children = {4, 5};
    invalid_children = {};
    std::sort(children_of_3.begin(), children_of_3.end());
    std::sort(expected_children.begin(), expected_children.end());
    REQUIRE(children_of_3 == expected_children);
    REQUIRE(children_of_3 != invalid_children);

    vector<int> children_of_1000 = pkb.GetAllChildren(1000);
    vector<int> children_of_2 = pkb.GetAllChildren(2);
    expected_children = {};
    REQUIRE(children_of_1000 == expected_children);
    REQUIRE(children_of_2 == expected_children);

    vector<pair<int, int>> parent_pairs = pkb.GetAllParentPairs();
    vector<pair<int, int>> expected_parent_pairs = vector<pair<int, int>>{make_pair(1, 2), make_pair(1, 3), make_pair(1, 6), make_pair(1, 7),
      make_pair(1, 8), make_pair(3, 4), make_pair(3, 5)};
    vector<pair<int, int>> invalid_pairs = vector<pair<int, int>>{make_pair(1, 2)};
    // Sort to ensure that ordering of pairs in vector does not matter
    std::sort(parent_pairs.begin(), parent_pairs.end());
    std::sort(expected_parent_pairs.begin(), expected_parent_pairs.end());
    REQUIRE(parent_pairs == expected_parent_pairs);
    REQUIRE(parent_pairs != invalid_pairs);

    vector<pair<int, int>> transitive_parent_pairs = pkb.GetAllTransitiveParentPairs();
    vector<pair<int, int>> expected_transitive_parent_pairs = vector<pair<int, int>>{make_pair(1, 2), make_pair(1, 3), make_pair(1, 6), make_pair(1, 7),
      make_pair(1, 8), make_pair(1, 4), make_pair(1, 5), make_pair(3, 4), make_pair(3, 5)};
    // Sort to ensure that ordering of pairs in vector does not matter
    std::sort(transitive_parent_pairs.begin(), transitive_parent_pairs.end());
    std::sort(expected_transitive_parent_pairs.begin(), expected_transitive_parent_pairs.end());
    REQUIRE(transitive_parent_pairs == expected_transitive_parent_pairs);
    REQUIRE(transitive_parent_pairs != invalid_pairs);
  }
}
