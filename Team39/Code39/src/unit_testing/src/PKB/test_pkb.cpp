#include <vector>

#include "PKB/pkb.h"
#include "PKB/pkb_exceptions.h"

#include "catch.hpp"

// Read and print tables not tested as it is the same
TEST_CASE("Populating Assign Table") {
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, "x");

  SECTION("Add item into table: int -> string") {
    REQUIRE(success == true);
  }

  SECTION("Add invalid item into table") {
    success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, 2);
    REQUIRE(success == false);

    string str;
    success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, str);
    REQUIRE(success == false);
  }
}

TEST_CASE("Populating Constant Table") {
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, vector<int>{50, 100});

  SECTION("Add item into table: int -> vector<int>") {
    REQUIRE(success == true);
  }

  SECTION("Add invalid item into table") {
    success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, "invalid");
    REQUIRE(success == false);

    string str;
    success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, str);
    REQUIRE(success == false);
  }
}

// While table not tested as it is the same
TEST_CASE("Populating If Table") {
  Pkb pkb = Pkb();

  SECTION("Add item into table: int -> vector<string>") {
    const bool success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, vector<string>{"x != 0"});
    REQUIRE(success == true);
  }

  SECTION("Add invalid item into table") {
    bool success = pkb.AddInfoToTable(TableIdentifier::kIf, 1, 1);
    REQUIRE(success == false);

    vector<string> vec = {};
    success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, vec);
    REQUIRE(success == false);
  }
}

TEST_CASE("Populating Follows and FollowsBefore Table") {
  // FollowsBefore table is populated behind the scenes
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kFollows, 1, 2);
  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 3) && success;

  SECTION("Add item into table: int -> int") {
    REQUIRE(success == true);
  }

  SECTION("Add invalid item into table") {
    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 1, vector<int>{2, 3, 4});
    REQUIRE(success == false);

    string str;
    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, str);
    REQUIRE(success == false);

    int negative_number = -1;
    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 3, negative_number);
    REQUIRE(success == false);
  }

  SECTION("Test API for PQL side for FollowsTable") {
    REQUIRE(pkb.IsFollows(1, 2));
    REQUIRE(pkb.IsFollows(2, 3));

    REQUIRE(pkb.GetStmtRightAfter(1) == vector<int>{2});
    REQUIRE(pkb.GetStmtRightAfter(2) == vector<int>{3});
    // Key does not exist so the result should be empty
    REQUIRE(pkb.GetStmtRightAfter(3000) == vector<int>{});

    vector<pair<int, int>> follows_pairs = pkb.GetAllFollowsPairs();
    vector<pair<int, int>> expected_follows_pairs = vector<pair<int, int>>{make_pair(1, 2), make_pair(2, 3)};
    vector<pair<int, int>> invalid_first_pair = vector<pair<int, int>>{make_pair(1, 3)};
    REQUIRE(follows_pairs == expected_follows_pairs);
    REQUIRE(follows_pairs != invalid_first_pair);
  }

  SECTION("Test API for PQL side for FollowsBeforeTable") {
    REQUIRE(pkb.GetStmtRightBefore(0) == vector<int>{});
    REQUIRE(pkb.GetStmtRightBefore(2) == vector<int>{1});
    REQUIRE(pkb.GetStmtRightBefore(3) == vector<int>{2});
  }
}

TEST_CASE("Populating Parent and Child Table") {
  // Child table is populated behind the scenes
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, vector<int>{2, 3, 4});
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>{5, 6}) && success;

  SECTION("Add item into table: int -> vector<int>") {
    REQUIRE(success);
  }

  SECTION("Add invalid item into table") {
    success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, 2);
    REQUIRE(!success);

    string str;
    success = pkb.AddInfoToTable(TableIdentifier::kParent, 2, str);
    REQUIRE(!success);

    vector<int> empty_vector = {};
    success = pkb.AddInfoToTable(TableIdentifier::kParent, 3, empty_vector);
    REQUIRE(!success);
  }

  SECTION("Test API for PQL side for ParentTable") {
    REQUIRE(pkb.IsParent(1, 2));
    REQUIRE(pkb.IsParent(1, 3));
    REQUIRE(!pkb.IsParent(2, 3));
    REQUIRE(!pkb.IsParent(1, 5));

    REQUIRE(pkb.GetParent(1) == vector<int>{});
    REQUIRE(pkb.GetParent(2) == vector<int>{1});
    REQUIRE(pkb.GetParent(5) == vector<int>{4});
    REQUIRE(pkb.GetParent(5) != vector<int>{1});
    REQUIRE(pkb.GetParent(6) != vector<int>{1});
  }

  SECTION("Test API for PQL side for ChildTable") {
    REQUIRE(pkb.GetChild(1) == vector<int>{2, 3, 4});
    REQUIRE(pkb.GetChild(1) != vector<int>{1, 2});
    REQUIRE(pkb.GetChild(5) == vector<int>{});
    REQUIRE(pkb.GetChild(4) == vector<int>{5, 6});
  }
}

TEST_CASE("Populating StmtToPatterns Table") {
  // PatternToStmtsTable is populated behind the scenes
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kPattern, 2, "A + (Bbs + C) + 2");
  success = success && pkb.AddInfoToTable(TableIdentifier::kPattern, 3, "X + (Bbs + C) * (B + C)");
  success = success && pkb.AddInfoToTable(TableIdentifier::kPattern, 4, "A + Bbs + C + 2");
  success = success && pkb.AddInfoToTable(TableIdentifier::kPattern, 5, "289 * 444 + (f * cenX)");
  SECTION("Adding patterns") {
    REQUIRE(success);
  }

  SECTION("Search pattern") {
    unordered_set<int> res = pkb.GetAllStmtsWithPattern("X + B + C");
    REQUIRE(res.empty());

    res = pkb.GetAllStmtsWithPattern("A + (Bbs + C) + 2");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetAllStmtsWithPattern("Bbs ");
    REQUIRE(res == unordered_set<int>{2, 3, 4});

    res = pkb.GetAllStmtsWithPattern("Bbs + C");
    REQUIRE(res == unordered_set<int>{3, 2});

    res = pkb.GetAllStmtsWithPattern("A + (Bbs + C)");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetAllStmtsWithPattern("cenX");
    REQUIRE(res == unordered_set<int>{5});
  }

  SECTION("Search Exact pattern") {
    unordered_set<int> res = pkb.GetStmtsWithExactPattern("A + (Bbs + C) + 2");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetStmtsWithExactPattern("Bbs");
    REQUIRE(res.empty());

    res = pkb.GetStmtsWithExactPattern("289 * 444 + (f * cenX)");
    REQUIRE(res == unordered_set<int>{5});
  }
}

TEST_CASE("Sample Tests for Pattern") {
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kPattern, 2, "v + x * y + z % t");
  SECTION("Adding patterns") {
    REQUIRE(success);
  }

  SECTION("Nonempty results") {
    unordered_set<int> res = pkb.GetStmtsWithExactPattern("v + x * y + z % t");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetAllStmtsWithPattern("v");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetAllStmtsWithPattern("x*y");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetAllStmtsWithPattern("v+x*y");
    REQUIRE(res == unordered_set<int>{2});

    res = pkb.GetAllStmtsWithPattern("v + x * y + z % t");
    REQUIRE(res == unordered_set<int>{2});
  }

  SECTION("Empty result") {
    unordered_set<int> res = pkb.GetStmtsWithExactPattern("v");
    REQUIRE(res.empty());

    res = pkb.GetAllStmtsWithPattern("v+x");
    REQUIRE(res.empty());

    res = pkb.GetAllStmtsWithPattern("y+z%t");
    REQUIRE(res.empty());

    res = pkb.GetAllStmtsWithPattern("x * y + z % t");
    REQUIRE(res.empty());
  }
}

TEST_CASE("Add Integer Entity") {
  Pkb pkb = Pkb();

  SECTION("Adding statement") {
    bool success = pkb.AddEntityToSet(EntityIdentifier::kStmt, 1);
    success = success && pkb.AddEntityToSet(EntityIdentifier::kStmt, 2);
    success = success && pkb.AddEntityToSet(EntityIdentifier::kStmt, 3);
    REQUIRE(success);

    const unordered_set<int> res = pkb.GetAllEntityInt(EntityIdentifier::kStmt);
    REQUIRE(res.size() == 3);
    REQUIRE(res.find(1) != res.end());
    REQUIRE(res.find(2) != res.end());
    REQUIRE(res.find(3) != res.end());
  }
}

TEST_CASE("Add String Entity") {
  Pkb pkb = Pkb();

  SECTION("Adding variable name") {
    bool success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
    success = success && pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
    success = success && pkb.AddEntityToSet(EntityIdentifier::kVariable, "z");
    REQUIRE(success);

    const unordered_set<string> res = pkb.GetAllEntityString(EntityIdentifier::kVariable);
    REQUIRE(res.size() == 3);
    REQUIRE(res.find("x") != res.end());
    REQUIRE(res.find("y") != res.end());
    REQUIRE(res.find("z") != res.end());
  }
}
