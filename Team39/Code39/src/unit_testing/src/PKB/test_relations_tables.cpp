#include <unordered_set>

#include "PKB/tables/relation_tables.h"
#include "PKB/pkb_exceptions.h"

#include "catch.hpp"

// FollowsBefore table will not be tested since it is the same type
TEST_CASE("Follows Table") {
  FollowsTable follows_table = FollowsTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = follows_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = follows_table.AddKeyValuePair(1, 2);
  add_success = follows_table.AddKeyValuePair(2, 3) && add_success;

  SECTION("Check population success") {
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(follows_table.AddKeyValuePair(1, 4), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    REQUIRE(follows_table.GetValueByKey(1) == 2);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(follows_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = follows_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, int>> key_value_lst = follows_table.GetKeyValueLst();
    vector<pair<int, int>> expected_key_value_lst = {make_pair(1, 2), make_pair(2, 3)};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    follows_table.UpdateKeyWithNewValue(2, 100);
    REQUIRE(follows_table.GetValueByKey(2) == 100);
  }
}

// FollowsBeforeStar, Parent, Child, ParentStar, ChildStar tables will not be tested since it is the same type
TEST_CASE("Follows Star Table") {
  FollowsStarTable follows_star_table = FollowsStarTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = follows_star_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = follows_star_table.AddKeyValuePair(1, vector<int>{2, 3, 4});
  add_success = follows_star_table.AddKeyValuePair(2, vector<int>{3, 4}) && add_success;

  SECTION("Check population success") {
    REQUIRE(follows_star_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(follows_star_table.AddKeyValuePair(1, vector<int>{4, 5, 6}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    vector<int> value = follows_star_table.GetValueByKey(1);
    vector<int> expected_value = {2, 3, 4};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(follows_star_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = follows_star_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, vector<int>>> key_value_lst = follows_star_table.GetKeyValueLst();
    vector<pair<int, vector<int>>> expected_key_value_lst = {make_pair(1, vector<int>{2, 3, 4}), make_pair(2, vector<int>{3, 4})};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    follows_star_table.UpdateKeyWithNewValue(2, vector<int>{123, 321});
    REQUIRE(follows_star_table.GetValueByKey(2) == vector<int>{123, 321});
  }
}

TEST_CASE("Statement to Patterns Table") {
  StmtToPatternsTable stmt_to_patterns_table = StmtToPatternsTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = stmt_to_patterns_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = stmt_to_patterns_table.AddKeyValuePair(1, unordered_set<string>{"x + y"});
  add_success = stmt_to_patterns_table.AddKeyValuePair(2, unordered_set<string>{"z * 2"}) && add_success;

  SECTION("Check population success") {
    REQUIRE(stmt_to_patterns_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(stmt_to_patterns_table.AddKeyValuePair(1, unordered_set<string>{"z * 2"}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    unordered_set<string> value = stmt_to_patterns_table.GetValueByKey(1);
    unordered_set<string> expected_value = {"x + y"};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(stmt_to_patterns_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = stmt_to_patterns_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, unordered_set<string>>> key_value_lst = stmt_to_patterns_table.GetKeyValueLst();
    vector<pair<int, unordered_set<string>>> expected_key_value_lst = { make_pair(1, unordered_set<string>{"x + y"}),
      make_pair(2, unordered_set<string>{"z * 2"})};

    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    stmt_to_patterns_table.UpdateKeyWithNewValue(2, unordered_set<string>{"x + y + z"});
    REQUIRE(stmt_to_patterns_table.GetValueByKey(2) == unordered_set<string>{"x + y + z"});
  }
}

TEST_CASE("Pattern to Statements Table") {
  PatternToStmtsTable pattern_to_stmts_table = PatternToStmtsTable();

  SECTION("Get key list when table is empty") {
    vector<string> key_lst = pattern_to_stmts_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = pattern_to_stmts_table.AddKeyValuePair("x + y", unordered_set<int>{1});
  add_success = pattern_to_stmts_table.AddKeyValuePair("z * 2", unordered_set<int>{2}) && add_success;

  SECTION("Check population success") {
    REQUIRE(pattern_to_stmts_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(pattern_to_stmts_table.AddKeyValuePair("z * 2", unordered_set<int>{100}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    unordered_set<int> value = pattern_to_stmts_table.GetValueByKey("x + y");
    unordered_set<int> expected_value = {1};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(pattern_to_stmts_table.GetValueByKey("z * 3 + 1000"), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<string> key_lst = pattern_to_stmts_table.GetKeyLst();
    vector<string> expected_key_lst = {"x + y", "z * 2"};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<string, unordered_set<int>>> key_value_lst = pattern_to_stmts_table.GetKeyValueLst();
    vector<pair<string, unordered_set<int>>> expected_key_value_lst = {make_pair("z * 2", unordered_set<int>{2}),
      make_pair("x + y", unordered_set<int>{1})};

    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    pattern_to_stmts_table.UpdateKeyWithNewValue("x + y", unordered_set<int>{1, 2, 3, 4});
    REQUIRE(pattern_to_stmts_table.GetValueByKey("x + y") == unordered_set<int>{1, 2, 3, 4});
  }
}

// ModifiesStmtToVariables table will not be tested since it is the same type
TEST_CASE("Uses Statement to Variables Table") {
  UsesStmtToVariablesTable uses_stmt_to_variables_table = UsesStmtToVariablesTable();

  SECTION("Get key list when table is empty") {
    vector<int> key_lst = uses_stmt_to_variables_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = uses_stmt_to_variables_table.AddKeyValuePair(1, vector<string>{"x", "y"});
  add_success = uses_stmt_to_variables_table.AddKeyValuePair(2, vector<string>{"z"}) && add_success;

  SECTION("Check population success") {
    REQUIRE(uses_stmt_to_variables_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(uses_stmt_to_variables_table.AddKeyValuePair(1, vector<string>{"z"}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    vector<string> value = uses_stmt_to_variables_table.GetValueByKey(1);
    vector<string> expected_value = {"x", "y"};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(uses_stmt_to_variables_table.GetValueByKey(999), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<int> key_lst = uses_stmt_to_variables_table.GetKeyLst();
    vector<int> expected_key_lst = {1, 2};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<int, vector<string>>> key_value_lst = uses_stmt_to_variables_table.GetKeyValueLst();
    vector<pair<int, vector<string>>> expected_key_value_lst = {make_pair(1, vector<string>{"x", "y"}), make_pair(2, vector<string>{"z"})};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    uses_stmt_to_variables_table.UpdateKeyWithNewValue(2, vector<string>{"z", "update"});
    REQUIRE(uses_stmt_to_variables_table.GetValueByKey(2) == vector<string>{"z", "update"});
  }
}

// ModifiesVariableToStmts table will not be tested since it is the same type
TEST_CASE("Uses Variable to Statements Table") {
  UsesVariableToStmtsTable uses_variable_to_stmts_table = UsesVariableToStmtsTable();

  SECTION("Get key list when table is empty") {
    vector<string> key_lst = uses_variable_to_stmts_table.GetKeyLst();
    REQUIRE(key_lst.size() == 0);
    REQUIRE(key_lst.empty());
  }

  bool add_success = uses_variable_to_stmts_table.AddKeyValuePair("x", vector<int>{1, 2});
  add_success = uses_variable_to_stmts_table.AddKeyValuePair("y", vector<int>{3}) && add_success;

  SECTION("Check population success") {
    REQUIRE(uses_variable_to_stmts_table.GetTableSize() == 2);
    REQUIRE(add_success == true);
  }

  SECTION("Throw custom exception if key already exists in the table") {
    CHECK_THROWS_AS(uses_variable_to_stmts_table.AddKeyValuePair("x", vector<int>{3, 4, 5}), KeyInUseException);
  }

  SECTION("Get value from table by key") {
    vector<int> value = uses_variable_to_stmts_table.GetValueByKey("x");
    vector<int> expected_value = {1, 2};
    REQUIRE(value == expected_value);
  }

  SECTION("Throw custom exception if key does not exist in the table when retrieving value by key") {
    CHECK_THROWS_AS(uses_variable_to_stmts_table.GetValueByKey("invalid"), InvalidKeyException);
  }

  SECTION("Get key list from table") {
    vector<string> key_lst = uses_variable_to_stmts_table.GetKeyLst();
    vector<string> expected_key_lst = {"x", "y"};

    std::sort(key_lst.begin(), key_lst.end());
    std::sort(expected_key_lst.begin(), expected_key_lst.end());
    REQUIRE(key_lst == expected_key_lst);
  }

  SECTION("Get key-value list from table") {
    vector<pair<string, vector<int>>> key_value_lst = uses_variable_to_stmts_table.GetKeyValueLst();
    vector<pair<string, vector<int>>> expected_key_value_lst = {make_pair("x", vector<int>{1, 2}), make_pair("y", vector<int>{3})};

    std::sort(key_value_lst.begin(), key_value_lst.end());
    std::sort(expected_key_value_lst.begin(), expected_key_value_lst.end());
    REQUIRE(key_value_lst == expected_key_value_lst);
  }

  SECTION("Update value in table") {
    uses_variable_to_stmts_table.UpdateKeyWithNewValue("x", vector<int>{100, 200});
    REQUIRE(uses_variable_to_stmts_table.GetValueByKey("x") == vector<int>{100, 200});
  }
}
