#include "PKB/pkb.h"
#include "PKB/tables/table.h"
#include "PKB/pkb_exceptions.h"

#include "catch.hpp"

TEST_CASE("Read Table") {
  SECTION("Populate read table with key value pairs") {
    ReadTable *read_table = new ReadTable();
    read_table->AddKeyValuePair(1, "y");
    bool add_success = read_table->AddKeyValuePair(2, "x");
    REQUIRE(read_table->GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    ReadTable *read_table = new ReadTable();
    read_table->AddKeyValuePair(1, "y");
    CHECK_THROWS(read_table->AddKeyValuePair(1, "x"));
  }

  SECTION("Get value from read table") {
    ReadTable *read_table = new ReadTable();
    read_table->AddKeyValuePair(1, "y");
    REQUIRE(read_table->GetValueByKey(1) == "y");
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value") {
    ReadTable *read_table = new ReadTable();
    read_table->AddKeyValuePair(1, "y");
    CHECK_THROWS(read_table->GetValueByKey(2));
  }
}

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
    const bool success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 2);
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

TEST_CASE("Populate Table") {
  SECTION("Check all nested tables udpated") {
    auto pkb = Pkb();
    const vector<int> value = { 4,5,6 };
    const bool success = pkb.AddInfoToTable(TableIdentifier::kParent, 3, value);
    const vector<int> value_2 = { 2,3 };
    const bool success_2 = pkb.AddInfoToTable(TableIdentifier::kParent, 1, value_2);
    const int populate_success = pkb.PopulateNestedRelationship();

    REQUIRE(populate_success == 1);
    REQUIRE(pkb.IsTransitiveParent(1, 4) == 1);
    REQUIRE(pkb.IsParent(2, 5) == 0);
    REQUIRE(pkb.IsParent(5, 2) == 0);
    REQUIRE(pkb.GetParent(2)[0] == 1);
    REQUIRE(pkb.GetParent(4)[0] == 3);
    REQUIRE(pkb.GetAllParents(4)[1] == 1);
    REQUIRE(pkb.GetChild(3)[2] == 6);
    REQUIRE(pkb.GetAllChildren(1)[0] == 2);

    const vector<string> value_3 = { "x" };
    const vector<string> value_4 = { "y" };
    const bool success_3 = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 2, value_3);
    const bool success_4 = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 4, value_4);
    const int populate_success_2 = pkb.PopulateNestedRelationship();

    REQUIRE(populate_success_2 == 1);
  }
}

TEST_CASE("Add entity items") {
  
}
