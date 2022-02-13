#include "PKB/pkb.h"
#include "PKB/tables/table.h"
#include "PKB/pkb_exceptions.h"

#include "catch.hpp"

TEST_CASE("Add Key-value Pair into Tables") {
  SECTION("Add item into table: int -> int list") {
    auto pkb = Pkb();
    const vector<int> value = { 1,2,3 };
    const bool success = pkb.AddInfoToTable(TableIdentifier::kParent, 2, value);
    REQUIRE(success == 1);
    const bool check_inserted = pkb.IsParent(2, 1) && pkb.IsParent(2,2) && pkb.IsParent(2,3);
    REQUIRE(check_inserted == 1);
  }

  SECTION("Add item into table: int -> string list") {
    auto pkb = Pkb();
    const vector<string> value = { "a", "b", "c" };
    const bool success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, value);
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: int -> int") {
    auto pkb = Pkb();
    const bool success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 3);
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: int -> string") {
    auto pkb = Pkb();
    const bool success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, "x");
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: invalid identifier") {
    auto pkb = Pkb();
    bool success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, "xvar");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, "xvar");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 2, "xvar");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, "x");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, 2);
    REQUIRE(success == 0);
  }

  SECTION("Add item into table: invalid value") {
    auto pkb = Pkb();
    vector<int> value_1 = {};
    bool success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, value_1);
    REQUIRE(success == 0);

    vector<string> value_2 = {};
    success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, value_2);
    REQUIRE(success == 0);

    int value_3 = -1;
    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, value_3);
    REQUIRE(success == 0);

    string value_4;
    success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, value_4);
    REQUIRE(success == 0);
  }
}

TEST_CASE("Add entity items") {
  SECTION("Adding statement") {
    Pkb pkb = Pkb();
    bool success = pkb.AddEntityToSet(EntityIdentifier::kStmt, 1);
    success = success && pkb.AddEntityToSet(EntityIdentifier::kStmt, 2);
    success = success && pkb.AddEntityToSet(EntityIdentifier::kStmt, 3);

    REQUIRE(success == 1);
    const unordered_set<int> res = pkb.GetAllEntityInt(EntityIdentifier::kStmt);
    REQUIRE(res.size() == 3);
    REQUIRE(res.find(1) != res.end());
    REQUIRE(res.find(2) != res.end());
    REQUIRE(res.find(3) != res.end());
  }
}

TEST_CASE("Add Pattern") {
  SECTION("Adding patterns") {
    Pkb pkb = Pkb();
    bool success = pkb.AddInfoToTable(TableIdentifier::kPattern, 2, "A + (B+C) + 2");
    success = success && pkb.AddInfoToTable(TableIdentifier::kPattern, 3, "X + (B+C) * (B + C)");
    REQUIRE(success == 1);
  }

  SECTION("Search pattern") {
    Pkb pkb = Pkb();
    bool success = pkb.AddInfoToTable(TableIdentifier::kPattern, 2, "A + (B+C) + 2");
    success = success && pkb.AddInfoToTable(TableIdentifier::kPattern, 3, "X + (B+C) * (B + C)");

    unordered_set<int> res = pkb.GetAllStmtWithPattern("X + B + C");
    REQUIRE(res.empty());
  }
}
