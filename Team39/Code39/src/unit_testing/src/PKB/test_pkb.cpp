#include "PKB/pkb.h"
#include "PKB/tables/table.h"
#include "PKB/pkb_exceptions.h"

#include "catch.hpp"

TEST_CASE("Read Table") {
  SECTION("Populate read table with key value pairs") {
    ReadTable read_table = ReadTable();
    read_table.AddKeyValuePair(1, "y");
    bool add_success = read_table.AddKeyValuePair(2, "x");
    REQUIRE(read_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    ReadTable read_table = ReadTable();
    read_table.AddKeyValuePair(1, "y");
    CHECK_THROWS(read_table.AddKeyValuePair(1, "x"));
  }

  SECTION("Get value from read table") {
    ReadTable read_table = ReadTable();
    read_table.AddKeyValuePair(1, "y");
    REQUIRE(read_table.GetValueByKey(1) == "y");
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value") {
    ReadTable read_table = ReadTable();
    read_table.AddKeyValuePair(1, "y");
    CHECK_THROWS(read_table.GetValueByKey(2));
  }
}
