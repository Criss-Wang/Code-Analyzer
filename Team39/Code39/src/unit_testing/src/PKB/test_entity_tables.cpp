#include "PKB/tables/entity_tables.h"
#include "PKB/pkb_exceptions.h"

#include "catch.hpp"

// Assign table and print table will not be tested since it is the same type as read table
TEST_CASE("Read Table") {
  ReadTable read_table = ReadTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = read_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = read_table.AddKeyValuePair(1, "y");
  add_success = read_table.AddKeyValuePair(2, "x") && add_success;

  SECTION("Check population success") {
    REQUIRE(read_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(read_table.AddKeyValuePair(1, "x"), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    REQUIRE(read_table.GetValueByKey(1) == "y");
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(read_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = read_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, string>> key_value_lst = read_table.GetKeyValueLst();
    vector<pair<int, string>> expected_key_value_lst = {make_pair(1, "y"), make_pair(2, "x")};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    read_table.UpdateKeyWithNewValue(2, "z");
    REQUIRE(read_table.GetValueByKey(2) == "z");
  }
}

TEST_CASE("Constant Table") {
  ConstantTable constant_table = ConstantTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = constant_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = constant_table.AddKeyValuePair(1, vector<int>{10, 20});
  add_success = constant_table.AddKeyValuePair(2, vector<int>{100}) && add_success;

  SECTION("Check population success") {
    REQUIRE(constant_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(constant_table.AddKeyValuePair(1, vector<int>{10, 20}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    vector<int> value = constant_table.GetValueByKey(1);
    vector<int> expected_value = {10, 20};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(constant_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = constant_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, vector<int>>> key_value_lst = constant_table.GetKeyValueLst();
    vector<pair<int, vector<int>>> expected_key_value_lst = {make_pair(1, vector<int>{10, 20}), make_pair(2, vector<int>{100})};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    constant_table.UpdateKeyWithNewValue(2, vector<int>{123, 321});
    REQUIRE(constant_table.GetValueByKey(2) == vector<int>{123, 321});
  }
}

// While table will not be tested since it is the same type as if table
TEST_CASE("If Table") {
  IfTable if_table = IfTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = if_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = if_table.AddKeyValuePair(1, vector<string>{"x != 0", "y != 0"});
  add_success = if_table.AddKeyValuePair(2, vector<string>{"x > 0"}) && add_success;

  SECTION("Check population success") {
    REQUIRE(if_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(if_table.AddKeyValuePair(1, vector<string>{"z != 0"}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    vector<string> value = if_table.GetValueByKey(1);
    vector<string> expected_value = {"x != 0", "y != 0"};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(if_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = if_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, vector<string>>> key_value_lst = if_table.GetKeyValueLst();
    vector<pair<int, vector<string>>> expected_key_value_lst = {make_pair(1, vector<string>{"x != 0", "y != 0"}),
      make_pair(2, vector<string>{"x > 0"})};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    if_table.UpdateKeyWithNewValue(2, vector<string>{"x > 1000"});
    REQUIRE(if_table.GetValueByKey(2) == vector<string>{"x > 1000"});
  }
}
