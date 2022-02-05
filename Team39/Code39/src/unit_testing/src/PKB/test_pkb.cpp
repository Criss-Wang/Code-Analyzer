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
    const bool success = pkb.AddInfoToTable(Pkb::TableIdentifier::kConstant, 2, value);
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: int -> string list") {
    auto pkb = Pkb();
    const vector<string> value = { "a", "b", "c" };
    const bool success = pkb.AddInfoToTable(Pkb::TableIdentifier::kIf, 2, value);
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: int -> int") {
    auto pkb = Pkb();
    const bool success = pkb.AddInfoToTable(Pkb::TableIdentifier::kFollowsBy, 2, 2);
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: int -> string") {
    auto pkb = Pkb();
    const bool success = pkb.AddInfoToTable(Pkb::TableIdentifier::kAssign, 2, "x");
    REQUIRE(success == 1);
    //REQUIRE(pkb.assign_table->GetValueByKey(2) == "()");
  }

  SECTION("Add item into table: invalid identifier") {
    auto pkb = Pkb();
    bool success = pkb.AddInfoToTable(Pkb::TableIdentifier::kConstant, 2, "xvar");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kIf, 2, "xvar");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kUsesVarToStmt, 2, "xvar");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kFollowsBy, 2, "x");
    REQUIRE(success == 0);

    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kAssign, 2, 2);
    REQUIRE(success == 0);
  }

  SECTION("Add item into table: invalid value") {
    auto pkb = Pkb();
    vector<int> value_1 = {};
    bool success = pkb.AddInfoToTable(Pkb::TableIdentifier::kConstant, 2, value_1);
    REQUIRE(success == 0);

    vector<string> value_2 = {};
    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kIf, 2, value_2);
    REQUIRE(success == 0);

    int value_3 = -1;
    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kFollowsBy, 2, value_3);
    REQUIRE(success == 0);

    string value_4;
    success = pkb.AddInfoToTable(Pkb::TableIdentifier::kAssign, 2, value_4);
    REQUIRE(success == 0);
  }
}
