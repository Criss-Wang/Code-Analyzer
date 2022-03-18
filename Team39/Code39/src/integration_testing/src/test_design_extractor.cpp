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

TEST_CASE("Test Nested Population for Calls") {
  Pkb pkb = Pkb();
  bool success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p1", vector<string>{"p4", "p5"});
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p2", vector<string>{"p4"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p4", vector<string>{"p10"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p10", vector<string>{"p12", "p14", "p18"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p12", vector<string>{"p20"}) && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check PQL queries for nested calls") {
    int p1_idx = pkb.GetIndexByProc("p1");
    int p2_idx = pkb.GetIndexByProc("p2");
    int p4_idx = pkb.GetIndexByProc("p4");
    int p5_idx = pkb.GetIndexByProc("p5");
    int p10_idx = pkb.GetIndexByProc("p10");
    int p12_idx = pkb.GetIndexByProc("p12");
    int p14_idx = pkb.GetIndexByProc("p14");
    int p18_idx = pkb.GetIndexByProc("p18");
    int p20_idx = pkb.GetIndexByProc("p20");
    REQUIRE(pkb.IsTransitiveCalls(p1_idx, p4_idx));
    REQUIRE(pkb.IsTransitiveCalls(p1_idx, p10_idx));
    REQUIRE(pkb.IsTransitiveCalls(p1_idx, p12_idx));
    REQUIRE(pkb.IsTransitiveCalls(p1_idx, p20_idx));
    REQUIRE(pkb.IsTransitiveCalls(p1_idx, p5_idx));
    REQUIRE(pkb.IsTransitiveCalls(p1_idx, p18_idx));
    REQUIRE(!pkb.IsTransitiveCalls(p1_idx, 10000));

    REQUIRE(pkb.IsTransitiveCalls(p12_idx, p20_idx));
    REQUIRE(!pkb.IsTransitiveCalls(p12_idx, p10_idx));

    REQUIRE(pkb.IsTransitiveCalls(p4_idx, p12_idx));
    REQUIRE(pkb.IsTransitiveCalls(p4_idx, p10_idx));
    REQUIRE(pkb.IsTransitiveCalls(p4_idx, p14_idx));
    REQUIRE(!pkb.IsTransitiveCalls(p4_idx, 2000));

    vector<int> expected_all_callers_of_p20 = {p12_idx, p10_idx, p4_idx, p2_idx, p1_idx};
    vector<int> all_callers_of_p20 = pkb.GetAllCallers(p20_idx);
    std::sort(expected_all_callers_of_p20.begin(), expected_all_callers_of_p20.end());
    std::sort(all_callers_of_p20.begin(), all_callers_of_p20.end());
    REQUIRE(expected_all_callers_of_p20 == all_callers_of_p20);

    vector<int> expected_all_callers_of_p18 = {p10_idx, p4_idx, p2_idx, p1_idx};
    vector<int> all_callers_of_p18 = pkb.GetAllCallers(p18_idx);
    std::sort(expected_all_callers_of_p18.begin(), expected_all_callers_of_p18.end());
    std::sort(all_callers_of_p18.begin(), all_callers_of_p18.end());
    REQUIRE(expected_all_callers_of_p18 == all_callers_of_p18);

    vector<int> expected_all_callers_of_p5 = {p1_idx};
    vector<int> all_callers_of_p5 = pkb.GetAllCallers(p5_idx);
    std::sort(expected_all_callers_of_p5.begin(), expected_all_callers_of_p5.end());
    std::sort(all_callers_of_p5.begin(), all_callers_of_p5.end());
    REQUIRE(expected_all_callers_of_p5 == all_callers_of_p5);

    vector<int> expected_all_callers_of_p1 = {};
    vector<int> all_callers_of_p1 = pkb.GetAllCallers(p1_idx);
    REQUIRE(expected_all_callers_of_p1 == all_callers_of_p1);

    vector<int> expected_all_callees_of_p10 = { p12_idx, p20_idx, p14_idx, p18_idx};
    vector<int> all_callees_of_p10 = pkb.GetAllCallees(p10_idx);
    std::sort(expected_all_callees_of_p10.begin(), expected_all_callees_of_p10.end());
    std::sort(all_callees_of_p10.begin(), all_callees_of_p10.end());
    REQUIRE(expected_all_callees_of_p10 == all_callees_of_p10);

    vector<int> expected_all_callees_of_p1 = { p4_idx, p10_idx, p5_idx, p12_idx, p20_idx, p14_idx, p18_idx};
    vector<int> all_callees_of_p1 = pkb.GetAllCallees(p1_idx);
    std::sort(expected_all_callees_of_p1.begin(), expected_all_callees_of_p1.end());
    std::sort(all_callees_of_p1.begin(), all_callees_of_p1.end());
    REQUIRE(expected_all_callees_of_p1 == all_callees_of_p1);

    vector<int> expected_all_callees_of_p2 = { p4_idx, p10_idx, p12_idx, p20_idx, p14_idx, p18_idx};
    vector<int> all_callees_of_p2 = pkb.GetAllCallees(p2_idx);
    std::sort(expected_all_callees_of_p2.begin(), expected_all_callees_of_p2.end());
    std::sort(all_callees_of_p2.begin(), all_callees_of_p2.end());
    REQUIRE(expected_all_callees_of_p2 == all_callees_of_p2);

    vector<pair<int, int>> transitive_calls_pairs = pkb.GetAllTransitiveCallsPairs();
    vector<pair<int, int>> invalid_pairs = {};
    vector<pair<int, int>> expected_transitive_calls_pairs = vector<pair<int, int>>{make_pair(p1_idx, p4_idx), make_pair(p1_idx, p10_idx), make_pair(p1_idx, p12_idx),
      make_pair(p1_idx, p20_idx), make_pair(p1_idx, p14_idx), make_pair(p1_idx, p18_idx), make_pair(p1_idx, p5_idx), make_pair(p2_idx, p4_idx), make_pair(p2_idx, p10_idx),
      make_pair(p2_idx, p12_idx), make_pair(p2_idx, p20_idx), make_pair(p2_idx, p14_idx), make_pair(p2_idx, p18_idx), make_pair(p4_idx, p10_idx), make_pair(p4_idx, p12_idx),
      make_pair(p4_idx, p20_idx), make_pair(p4_idx, p14_idx), make_pair(p4_idx, p18_idx), make_pair(p12_idx, p20_idx), make_pair(p10_idx, p12_idx), make_pair(p10_idx, p20_idx),
      make_pair(p10_idx, p14_idx), make_pair(p10_idx, p18_idx)};
    std::sort(transitive_calls_pairs.begin(), transitive_calls_pairs.end());
    std::sort(expected_transitive_calls_pairs.begin(), expected_transitive_calls_pairs.end());
    REQUIRE(transitive_calls_pairs == expected_transitive_calls_pairs);
    REQUIRE(transitive_calls_pairs != invalid_pairs);
  }
}

TEST_CASE("Test Nested Population for Modifies") {
  Pkb pkb = Pkb();
  // Nested modifies will be populated behind the scenes
  /*
   * 1. while (condition) {
   * 2.   x = modify * 2
   * 3.   y = modify * 2
   * 4.   if (condition) then {
   * 5.     z = modify * 2
   *      } else {
   * 6.     a = modifies * 2
   *      }
   * 7.   b = modifies * 2
   *    }
   */

  bool success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 2, vector<string>{"x"});
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 3, vector<string>{"y"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>{"z"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 6, vector<string>{"a"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 7, vector<string>{"b"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, vector<int>{2, 3, 4, 7}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>{5, 6}) && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  SECTION("Check population success") {
    REQUIRE(success);
  }
}

TEST_CASE("Test Nested Population for Uses") {
  Pkb pkb = Pkb();
  // Nested uses will be populated behind the scenes
  /*
   * 1. while (c1 != 0) {
   * 2.   x = first * 2
   * 3.   y = second * 2
   * 4.   if (c2 == 1000) then {
   * 5.     z = third * 2
   *      } else {
   * 6.     a = fourth * 2
   *      }
   * 7.   b = fifth * sixth
   *    }
   */

  bool success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "c1");
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "c2") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "first") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "second") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "third") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "fourth") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "sixth") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "fifth") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 1, vector<string>{"c1"});
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 4, vector<string>{"c2"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 2, vector<string>{"first"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 3, vector<string>{"second"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>{"third"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>{"fourth"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{"fifth", "sixth"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, vector<int>{2, 3, 4, 7}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>{5, 6}) && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);
  int c1_idx = pkb.GetIndexByVar("c1");
  int c2_idx = pkb.GetIndexByVar("c2");
  int first_idx = pkb.GetIndexByVar("first");
  int second_idx = pkb.GetIndexByVar("second");
  int third_idx = pkb.GetIndexByVar("third");
  int fourth_idx = pkb.GetIndexByVar("fourth");
  int fifth_idx = pkb.GetIndexByVar("fifth");
  int sixth_idx = pkb.GetIndexByVar("sixth");

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check PQL queries for nested uses") {
    REQUIRE(pkb.IsUsesStmt(1, c1_idx));
    REQUIRE(pkb.IsUsesStmt(4, c2_idx));
    REQUIRE(pkb.IsUsesStmt(2, first_idx));
    REQUIRE(pkb.IsUsesStmt(3, second_idx));
    REQUIRE(pkb.IsUsesStmt(7, fifth_idx));
    REQUIRE(pkb.IsUsesStmt(7, sixth_idx));

    REQUIRE(pkb.IsUsesStmt(1, first_idx));
    REQUIRE(pkb.IsUsesStmt(1, fifth_idx));
    REQUIRE(pkb.IsUsesStmt(1, c2_idx));
    REQUIRE(pkb.IsUsesStmt(1, third_idx));
    REQUIRE(pkb.IsUsesStmt(1, fourth_idx));
    REQUIRE(pkb.IsUsesStmt(4, third_idx));
    REQUIRE(pkb.IsUsesStmt(4, fourth_idx));

    REQUIRE(!pkb.IsUsesStmt(4, c1_idx));
    REQUIRE(!pkb.IsUsesStmt(4, fifth_idx));
    REQUIRE(!pkb.IsUsesStmt(4, sixth_idx));
    REQUIRE(!pkb.IsUsesStmt(1000, first_idx));
    REQUIRE(!pkb.IsUsesStmt(1000, -1));

    vector<int> stmts = pkb.GetUsesStmtsByVar(c2_idx);
    vector<int> expected_stmts = {1, 4};
    REQUIRE(stmts == expected_stmts);

    stmts = pkb.GetUsesStmtsByVar(third_idx);
    expected_stmts = {1, 4, 5};
    REQUIRE(stmts == expected_stmts);

    stmts = pkb.GetUsesStmtsByVar(c1_idx);
    expected_stmts = {1};
    REQUIRE(stmts == expected_stmts);

    stmts = pkb.GetUsesStmtsByVar(second_idx);
    expected_stmts = {1, 3};
    REQUIRE(stmts == expected_stmts);

    stmts = pkb.GetUsesStmtsByVar(fourth_idx);
    expected_stmts = {1, 4, 6};
    REQUIRE(stmts == expected_stmts);

    stmts = pkb.GetUsesStmtsByVar(fifth_idx);
    expected_stmts = {1, 7};
    REQUIRE(stmts == expected_stmts);

    stmts = pkb.GetUsesStmtsByVar(-1);
    expected_stmts = {};
    REQUIRE(stmts == expected_stmts);

    vector<int> variables = pkb.GetUsesVarByStmt(1);
    vector<int> expected_variables = { c1_idx, c2_idx, first_idx, second_idx, third_idx, fourth_idx, fifth_idx, sixth_idx };
    vector<int> invalid_vector = {};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);
    REQUIRE(variables != invalid_vector);

    variables = pkb.GetUsesVarByStmt(2);
    expected_variables = { first_idx };
    invalid_vector = {};
    REQUIRE(variables == expected_variables);
    REQUIRE(variables != invalid_vector);

    variables = pkb.GetUsesVarByStmt(4);
    expected_variables = { c2_idx, third_idx, fourth_idx };
    invalid_vector = {};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);
    REQUIRE(variables != invalid_vector);

    variables = pkb.GetUsesVarByStmt(7);
    expected_variables = { fifth_idx, sixth_idx };
    invalid_vector = {};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);
    REQUIRE(variables != invalid_vector);

    variables = pkb.GetUsesVarByStmt(1000);
    expected_variables = {};
    invalid_vector = {-1};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);
    REQUIRE(variables != invalid_vector);

    vector<pair<int, int>> stmt_var_pairs = pkb.GetAllUsesStmtVarPairs();
    vector<pair<int, int>> expected_stmt_var_pairs = vector<pair<int, int>>{make_pair(1, c1_idx), make_pair(1, first_idx), make_pair(1, second_idx),
      make_pair(1, third_idx), make_pair(1, fourth_idx), make_pair(1, fifth_idx), make_pair(1, sixth_idx), make_pair(1, c2_idx), make_pair(2, first_idx),
      make_pair(3, second_idx), make_pair(4, c2_idx), make_pair(4, third_idx), make_pair(4, fourth_idx), make_pair(5, third_idx), make_pair(6, fourth_idx),
      make_pair(7, fifth_idx), make_pair(7, sixth_idx)};
    vector<pair<int, int>> invalid_pairs = vector<pair<int, int>>{make_pair(1, -1)};
    // Sort to ensure that ordering of pairs in vector does not matter
    std::sort(stmt_var_pairs.begin(), stmt_var_pairs.end());
    std::sort(expected_stmt_var_pairs.begin(), expected_stmt_var_pairs.end());
    REQUIRE(stmt_var_pairs == expected_stmt_var_pairs);
    REQUIRE(stmt_var_pairs != invalid_pairs);
  }
}

TEST_CASE("Test Nested Population for ModifiesP") {
  Pkb pkb = Pkb();
  bool success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "z") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "a") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "b") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "c") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "d") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "e") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "f") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "g") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p1") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p4") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p6") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p7") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p8") && success;

  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p1", vector<string>{"y", "z"});
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p4", vector<string>{"a", "b"});
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p6", vector<string>{"c", "d"});
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p7", vector<string>{"g"});
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p8", vector<string>{"e", "f"});

  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p1", vector<string>{"p4"});
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p4", vector<string>{"p6", "p7"});
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p6", vector<string>{"p8"});

  int p1_idx = pkb.GetIndexByProc("p1");
  int p4_idx = pkb.GetIndexByProc("p4");
  int p6_idx = pkb.GetIndexByProc("p6");
  int p7_idx = pkb.GetIndexByProc("p7");
  int p8_idx = pkb.GetIndexByProc("p8");

  int y_idx = pkb.GetIndexByVar("y");
  int z_idx = pkb.GetIndexByVar("z");
  int a_idx = pkb.GetIndexByVar("a");
  int b_idx = pkb.GetIndexByVar("b");
  int c_idx = pkb.GetIndexByVar("c");
  int d_idx = pkb.GetIndexByVar("d");
  int e_idx = pkb.GetIndexByVar("e");
  int f_idx = pkb.GetIndexByVar("f");
  int g_idx = pkb.GetIndexByVar("g");

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check PQL queries for nested modifiesP") {
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, y_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, z_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, a_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, b_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, c_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, d_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, e_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, f_idx));
    REQUIRE(pkb.IsProcModifiesVar(p1_idx, g_idx));
    REQUIRE(!pkb.IsProcModifiesVar(p4_idx, y_idx));
    REQUIRE(!pkb.IsProcModifiesVar(p4_idx, z_idx));
    REQUIRE(!pkb.IsProcModifiesVar(p6_idx, a_idx));
    REQUIRE(!pkb.IsProcModifiesVar(p6_idx, b_idx));
    REQUIRE(!pkb.IsProcModifiesVar(p7_idx, y_idx));
    REQUIRE(!pkb.IsProcModifiesVar(p8_idx, y_idx));

    vector<int> variables = pkb.GetModifiesVarsByProc(p1_idx);
    vector<int> expected_variables = {y_idx, z_idx, a_idx, b_idx, c_idx, d_idx, e_idx, f_idx, g_idx};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesVarsByProc(p4_idx);
    expected_variables = {a_idx, b_idx, c_idx, d_idx, e_idx, f_idx, g_idx};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);
    REQUIRE(!variables.empty());

    variables = pkb.GetModifiesVarsByProc(p7_idx);
    expected_variables = {g_idx};
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesVarsByProc(p8_idx);
    expected_variables = {e_idx, f_idx};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesProcsByVar(y_idx);
    expected_variables = {p1_idx};
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesProcsByVar(z_idx);
    expected_variables = {p1_idx};
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesProcsByVar(a_idx);
    expected_variables = {p4_idx, p1_idx};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesProcsByVar(c_idx);
    expected_variables = {p6_idx, p4_idx, p1_idx};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);

    variables = pkb.GetModifiesProcsByVar(f_idx);
    expected_variables = {p8_idx, p6_idx, p4_idx, p1_idx};
    std::sort(variables.begin(), variables.end());
    std::sort(expected_variables.begin(), expected_variables.end());
    REQUIRE(variables == expected_variables);

    vector<pair<int, int>> proc_var_pairs = pkb.GetAllModifiesProcVarPairs();
    vector<pair<int, int>> expected_proc_var_pairs = vector<pair<int, int>>{make_pair(p1_idx, y_idx), make_pair(p1_idx, z_idx), make_pair(p1_idx, a_idx),
      make_pair(p1_idx, b_idx), make_pair(p1_idx, c_idx), make_pair(p1_idx, d_idx), make_pair(p1_idx, e_idx), make_pair(p1_idx, f_idx), make_pair(p1_idx, g_idx),
      make_pair(p4_idx, a_idx), make_pair(p4_idx, b_idx), make_pair(p4_idx, c_idx), make_pair(p4_idx, d_idx), make_pair(p4_idx, e_idx), make_pair(p4_idx, f_idx),
      make_pair(p4_idx, g_idx), make_pair(p6_idx, c_idx), make_pair(p6_idx, d_idx), make_pair(p6_idx, e_idx), make_pair(p6_idx, f_idx),
      make_pair(p7_idx, g_idx), make_pair(p8_idx, e_idx), make_pair(p8_idx, f_idx)};
    vector<pair<int, int>> invalid_pairs = vector<pair<int, int>>{make_pair(1, -1)};
    std::sort(proc_var_pairs.begin(), proc_var_pairs.end());
    std::sort(expected_proc_var_pairs.begin(), expected_proc_var_pairs.end());
    REQUIRE(proc_var_pairs == expected_proc_var_pairs);
    REQUIRE(proc_var_pairs != invalid_pairs);
   }
}

TEST_CASE("Test Nested Population for ModifiesS with Calls") {
  Pkb pkb = Pkb();
  /*
   *    p1 {
   * 1.   a = a * 2;
   * 2.   b = a * 2;
   * 3.   call p4;
   * 4.   while (a < 2) {
   * 5.     c = a * 2;
   * 6.     call p6;
   * 7.     while (a == 10) {
   * 8.       d = a * 2;
   * 9.       call p5;
   *        }
   *      }
   *
   *    p5 {
   * 10.  e = a * 2;
   * 11.  f = a * 2;
   *    }
   *
   *    p4 {
   * 12.  x = a * 2;
   * 13.  y = a * 2;
   * 14.  z = a * 2;
   *    }
   *
   *    p6 {
   * 15.  w = a * 2;
   * 16.  call p8;
   *    }
   *
   *    p8 {
   * 17.  v = a * 2;
   *    }
   */

  /* Calls
   * p1 -> p4, p5, p6
   * p6 -> p8
   *
   * Calls*
   * p1 -> p4, p5, p6, p8
   * p6 -> p8
   *
   * Parent
   * 4 -> 5, 6, 7
   * 7 -> 8, 9
   *
   * Parent*
   * 4 -> 5, 6, 7, 8, 9
   * 7 -> 8, 9
   *
   * ModifiesStmtToVars
   * 1 -> a
   * 2 -> b
   * 5 -> c
   * 8 -> d
   * 10 -> e
   * 11 -> f
   * 12 -> x
   * 13 -> y
   * 14 -> z
   * 15 -> w
   * 17 -> v
   *
   * ModifiesStmtToVars including nested but without calls
   * 1 -> a
   * 2 -> b
   * 4 -> c, d
   * 5 -> c
   * 7 -> d
   * 8 -> d
   * 10 -> e
   * 11 -> f
   * 12 -> x
   * 13 -> y
   * 14 -> z
   * 15 -> w
   * 17 -> v
   *
   *
   * ModifiesStmtToVars including nested and calls
   * 1 -> a
   * 2 -> b
   * 3 -> x, y, z
   * 4 -> c, d, e, f w, v
   * 5 -> c
   * 6 -> w, v
   * 7 -> d, e, f
   * 8 -> d
   * 9 -> e, f
   * 10 -> e
   * 11 -> f
   * 12 -> x
   * 13 -> y
   * 14 -> z
   * 15 -> w
   * 16 -> v
   * 17 -> v
   *
   * ModifiesProcToVariables
   * p1 -> a, b, c, d
   * p5 -> e, f
   * p4 -> x, y, z
   * p6 -> w
   * p8 -> v
   *
   * ModifiesProcToVariables with nested
   * p1 -> a, b, c, d, x, y, z, e, f, w, v
   * p5 -> e, f
   * p4 -> x, y, z
   * p6 -> w, v
   * p8 -> v
   */

  /*
   * ModifiesVarToStmts
   * a -> 1
   * b -> 2
   * c -> 5
   * d -> 8
   * e -> 10
   * f -> 11
   * x -> 12
   * y -> 13
   * z -> 14
   * w -> 15
   * v -> 17
   *
   * ModifiesVarToStmts with nested but without call
   * a -> 1
   * b -> 2
   * c -> 5, 4
   * d -> 8, 7, 4
   * e -> 10
   * f -> 11
   * x -> 12
   * y -> 13
   * z -> 14
   * w -> 15
   * v -> 17
   *
   * ModifiesVarToStmts with nested and call
   * a -> 1
   * b -> 2
   * c -> 5, 4
   * d -> 8, 7, 4
   * e -> 10, 9, 7, 4
   * f -> 11, 9, 7, 4
   * x -> 12, 3
   * y -> 13, 3
   * z -> 14, 3
   * w -> 15, 6, 4
   * v -> 17, 16, 6, 4
   */

  bool success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "a");
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "b") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "c") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "d") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "e") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "f") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "v") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "w") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "x") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "y") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "z") && success;

  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p1") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p4") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p5") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p6") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p8") && success;

  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 3) && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 6) && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 9) && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 16) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>{5, 6, 7}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 7, vector<int>{8, 9}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 1, vector<string>{"a"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 2, vector<string>{"b"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>{"c"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 8, vector<string>{"d"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 10, vector<string>{"e"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 11, vector<string>{"f"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 12, vector<string>{"x"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 13, vector<string>{"y"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 14, vector<string>{"z"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 15, vector<string>{"w"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 17, vector<string>{"v"}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p1", vector<string>{"a", "b", "c", "d"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p5", vector<string>{"e", "f"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p4", vector<string>{"x", "y", "z"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p6", vector<string>{"w"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, "p8", vector<string>{"v"}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p1", vector<string>{"p4", "p5", "p6"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p6", vector<string>{"p8"}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 3, "p4") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 6, "p6") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 9, "p5") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 16, "p8") && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  int a_idx = pkb.GetIndexByVar("a");
  int b_idx = pkb.GetIndexByVar("b");
  int c_idx = pkb.GetIndexByVar("c");
  int d_idx = pkb.GetIndexByVar("d");
  int e_idx = pkb.GetIndexByVar("e");
  int f_idx = pkb.GetIndexByVar("f");
  int v_idx = pkb.GetIndexByVar("v");
  int w_idx = pkb.GetIndexByVar("w");
  int x_idx = pkb.GetIndexByVar("x");
  int y_idx = pkb.GetIndexByVar("y");
  int z_idx = pkb.GetIndexByVar("z");

  int p1_idx = pkb.GetIndexByProc("p1");
  int p4_idx = pkb.GetIndexByProc("p4");
  int p5_idx = pkb.GetIndexByProc("p5");
  int p6_idx = pkb.GetIndexByProc("p6");
  int p8_idx = pkb.GetIndexByProc("p8");

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check calls* population before testing ModifiesS calls") {
    vector<pair<int, int>> expected_transitive_calls_pairs = {make_pair(p1_idx, p4_idx), make_pair(p1_idx, p5_idx), make_pair(p1_idx, p6_idx),
      make_pair(p1_idx, p8_idx), make_pair(p6_idx, p8_idx)};
    vector<pair<int, int>> transitive_calls_pairs = pkb.GetAllTransitiveCallsPairs();
    std::sort(expected_transitive_calls_pairs.begin(), expected_transitive_calls_pairs.end());
    std::sort(transitive_calls_pairs.begin(), transitive_calls_pairs.end());
    REQUIRE(transitive_calls_pairs == expected_transitive_calls_pairs);
  }

  SECTION("Check Parent* population before testing ModifiesS calls") {
    vector<pair<int, int>> expected_transitive_parent_pairs = {make_pair(4, 5), make_pair(4, 6), make_pair(4, 7),
      make_pair(4, 8), make_pair(4, 9), make_pair(7, 8), make_pair(7, 9)};
    vector<pair<int, int>> transitive_parent_pairs = pkb.GetAllTransitiveParentPairs();
    std::sort(expected_transitive_parent_pairs.begin(), expected_transitive_parent_pairs.end());
    std::sort(transitive_parent_pairs.begin(), transitive_parent_pairs.end());
    REQUIRE(transitive_parent_pairs == expected_transitive_parent_pairs);
  }

  SECTION("Check ModifiesP population before testing ModifiesS calls") {
    vector<pair<int, int>> expected_proc_var_pairs = {make_pair(p1_idx, a_idx), make_pair(p1_idx, b_idx), make_pair(p1_idx, c_idx), make_pair(p1_idx, d_idx),
      make_pair(p1_idx, e_idx), make_pair(p1_idx, f_idx), make_pair(p1_idx, v_idx), make_pair(p1_idx, w_idx), make_pair(p1_idx, x_idx), make_pair(p1_idx, y_idx),
      make_pair(p1_idx, z_idx), make_pair(p5_idx, e_idx), make_pair(p5_idx, f_idx), make_pair(p4_idx, x_idx), make_pair(p4_idx, y_idx),
      make_pair(p4_idx, z_idx), make_pair(p6_idx, v_idx), make_pair(p6_idx, w_idx), make_pair(p8_idx, v_idx)};
    vector<pair<int, int>> proc_var_pairs = pkb.GetAllModifiesProcVarPairs();
    std::sort(expected_proc_var_pairs.begin(), expected_proc_var_pairs.end());
    std::sort(proc_var_pairs.begin(), proc_var_pairs.end());
    REQUIRE(proc_var_pairs == expected_proc_var_pairs);
  }

  SECTION("Check ModifiesS for calls population") {
    vector<pair<int, int>> expected_stmt_var_pairs = {make_pair(1, a_idx), make_pair(2, b_idx), make_pair(3, x_idx), make_pair(3, y_idx), make_pair(3, z_idx),
      make_pair(4, c_idx), make_pair(4, d_idx), make_pair(4, e_idx), make_pair(4, f_idx), make_pair(4, w_idx), make_pair(4, v_idx), make_pair(5, c_idx),
      make_pair(6, w_idx), make_pair(6, v_idx), make_pair(7, d_idx), make_pair(7, e_idx), make_pair(7, f_idx), make_pair(8, d_idx),
      make_pair(9, e_idx), make_pair(9, f_idx), make_pair(10, e_idx), make_pair(11, f_idx), make_pair(12, x_idx), make_pair(13, y_idx), make_pair(14, z_idx),
      make_pair(15, w_idx), make_pair(16, v_idx), make_pair(17, v_idx)
    };
    vector<pair<int, int>> stmt_var_pairs = pkb.GetAllModifiesStmtVarPairs();
    std::sort(expected_stmt_var_pairs.begin(), expected_stmt_var_pairs.end());
    std::sort(stmt_var_pairs.begin(), stmt_var_pairs.end());
    REQUIRE(stmt_var_pairs == expected_stmt_var_pairs);
  }

  SECTION("Check reverse ModifiesS for calls population") {
    vector<int> expected_stmts = {1};
    vector<int> stmts = pkb.GetModifiesStmtsByVar(a_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {2};
    stmts = pkb.GetModifiesStmtsByVar(b_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {5, 4};
    stmts = pkb.GetModifiesStmtsByVar(c_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {8, 7, 4};
    stmts = pkb.GetModifiesStmtsByVar(d_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {10, 9, 7, 4};
    stmts = pkb.GetModifiesStmtsByVar(e_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {11, 9, 7, 4};
    stmts = pkb.GetModifiesStmtsByVar(f_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {12, 3};
    stmts = pkb.GetModifiesStmtsByVar(x_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {13, 3};
    stmts = pkb.GetModifiesStmtsByVar(y_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {14, 3};
    stmts = pkb.GetModifiesStmtsByVar(z_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {15, 6, 4};
    stmts = pkb.GetModifiesStmtsByVar(w_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);

    expected_stmts = {17, 16, 6, 4};
    stmts = pkb.GetModifiesStmtsByVar(v_idx);
    std::sort(stmts.begin(), stmts.end());
    std::sort(expected_stmts.begin(), expected_stmts.end());
    REQUIRE(expected_stmts == stmts);
  }
}

TEST_CASE("Test Nested Population for UsesS with Calls") {
  Pkb pkb = Pkb();
  /*
   *    p1 {
   * 1.   a = a * 2;
   * 2.   a = b * 2;
   * 3.   call p4;
   * 4.   while (first < 2) {
   * 5.     a = c * 2;
   * 6.     call p6;
   * 7.     while (second == 10) {
   * 8.       a = d * 2;
   * 9.       call p5;
   *        }
   *      }
   *
   *    p5 {
   * 10.  a = e * 2;
   * 11.  a = f * 2;
   *    }
   *
   *    p4 {
   * 12.  a = x * 2;
   * 13.  a = y * 2;
   * 14.  a = z * 2;
   *    }
   *
   *    p6 {
   * 15.  a = w * 2;
   * 16.  call p8;
   *    }
   *
   *    p8 {
   * 17.  a = v * 2;
   * 18.  if (third == 3) then {
   * 19.    a = awesome * hmm * great;
   *      } else {
   * 20.    a = hello + there;
   *      }
   *    }
   */

  /* Calls
   * p1 -> p4, p6, p5
   * p6 -> p8
   *
   * Calls*
   * p1 -> p4, p6, p5, p8
   * p6 -> p8
   *
   * Parent
   * 4 -> 5, 6, 7
   * 7 -> 8, 9
   * 18 -> 19, 20
   *
   * Parent*
   * 4 -> 5, 6, 7, 8, 9
   * 7 -> 8, 9
   * 18 -> 19, 20
   *
   * UsesProcToVariables
   * p1 -> a, b, first, c, second, d
   * p5 -> e, f
   * p4 -> x, y, z
   * p6 -> w
   * p8 -> v, third, awesome, hmm, great, hello, there
   *
   * UsesProcToVariables with nested
   * p1 -> a, b, first, c, second, d, x, y, z, w, v, third, awesome,
   *       hmm, great, hello, there, e, f
   * p5 -> e, f
   * p4 -> x, y, z
   * p6 -> w, v, third, awesome, hmm, great, hello, there
   * p8 -> v, third, awesome, hmm, great, hello, there
   *
   * UsesStmtToVars
   * 1 -> a
   * 2 -> b
   * 4 -> first
   * 5 -> c
   * 7 -> second
   * 8 -> d
   * 10 -> e
   * 11 -> f
   * 12 -> x
   * 13 -> y
   * 14 -> z
   * 15 -> w
   * 17 -> v
   * 18 -> third
   * 19 -> awesome, hmm, great
   * 20 -> hello, there
   *
   * UsesStmtToVars including nested but without calls
   * 1 -> a
   * 2 -> b
   * 4 -> first, c, second, d
   * 5 -> c
   * 7 -> second, d
   * 8 -> d
   * 10 -> e
   * 11 -> f
   * 12 -> x
   * 13 -> y
   * 14 -> z
   * 15 -> w
   * 17 -> v
   * 18 -> third, awesome, hmm, great, hello, there
   * 19 -> awesome, hmm, great
   * 20 -> hello, there
   *
   * UsesStmtToVars including nested and calls
   * 1 -> a
   * 2 -> b
   * 3 -> x, y, z
   * 4 -> first, c, second, d, e, f,
   *      w, v, third, awesome, hmm, great, hello, there
   * 5 -> c
   * 6 -> w, v, third, awesome, hmm, great, hello, there
   * 7 -> second, d, e, f
   * 8 -> d
   * 9 -> e, f
   * 10 -> e
   * 11 -> f
   * 12 -> x
   * 13 -> y
   * 14 -> z
   * 15 -> w
   * 16 -> v, third, awesome, hmm, great, hello, there
   * 17 -> v
   * 18 -> third, awesome, hmm, great, hello, there
   * 19 -> awesome, hmm, great
   * 20 -> hello, there
   */

  bool success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "a");
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "b") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "c") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "d") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "e") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "f") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "v") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "w") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "x") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "y") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "z") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "first") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "second") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "third") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "awesome") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "hmm") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "great") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "hello") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "there") && success;

  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p1") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p4") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p5") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p6") && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p8") && success;

  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 3) && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 6) && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 9) && success;
  success = pkb.AddEntityToSet(EntityIdentifier::kCall, 16) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>{5, 6, 7}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 7, vector<int>{8, 9}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kParent, 18, vector<int>{19, 20}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 1, vector<string>{"a"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 2, vector<string>{"b"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 4, vector<string>{"first"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>{"c"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{"second"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 8, vector<string>{"d"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 10, vector<string>{"e"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 11, vector<string>{"f"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 12, vector<string>{"x"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 13, vector<string>{"y"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 14, vector<string>{"z"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 15, vector<string>{"w"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 17, vector<string>{"v"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 18, vector<string>{"third"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 19, vector<string>{"awesome", "hmm", "great"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 20, vector<string>{"hello", "there"}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "p1", vector<string>{"a", "b", "first", "c", "second", "d"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "p5", vector<string>{"e", "f"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "p4", vector<string>{"x", "y", "z"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "p6", vector<string>{"w"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "p8", vector<string>{"v", "third", "awesome", "hmm", "great", "hello", "there"}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p1", vector<string>{"p4", "p5", "p6"}) && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p6", vector<string>{"p8"}) && success;

  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 3, "p4") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 6, "p6") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 9, "p5") && success;
  success = pkb.AddInfoToTable(TableIdentifier::kCaller, 16, "p8") && success;

  // Populate nested
  success = success && PopulateNestedRelationships(pkb);

  int a_idx = pkb.GetIndexByVar("a");
  int b_idx = pkb.GetIndexByVar("b");
  int c_idx = pkb.GetIndexByVar("c");
  int d_idx = pkb.GetIndexByVar("d");
  int e_idx = pkb.GetIndexByVar("e");
  int f_idx = pkb.GetIndexByVar("f");
  int v_idx = pkb.GetIndexByVar("v");
  int w_idx = pkb.GetIndexByVar("w");
  int x_idx = pkb.GetIndexByVar("x");
  int y_idx = pkb.GetIndexByVar("y");
  int z_idx = pkb.GetIndexByVar("z");
  int first_idx = pkb.GetIndexByVar("first");
  int second_idx = pkb.GetIndexByVar("second");
  int third_idx = pkb.GetIndexByVar("third");
  int awesome_idx = pkb.GetIndexByVar("awesome");
  int hmm_idx = pkb.GetIndexByVar("hmm");
  int great_idx = pkb.GetIndexByVar("great");
  int hello_idx = pkb.GetIndexByVar("hello");
  int there_idx = pkb.GetIndexByVar("there");

  int p1_idx = pkb.GetIndexByProc("p1");
  int p4_idx = pkb.GetIndexByProc("p4");
  int p5_idx = pkb.GetIndexByProc("p5");
  int p6_idx = pkb.GetIndexByProc("p6");
  int p8_idx = pkb.GetIndexByProc("p8");

  SECTION("Check population success") {
    REQUIRE(success);
  }

  SECTION("Check calls* population before testing UsesS calls") {
    vector<pair<int, int>> expected_transitive_calls_pairs = {make_pair(p1_idx, p4_idx), make_pair(p1_idx, p5_idx), make_pair(p1_idx, p6_idx),
      make_pair(p1_idx, p8_idx), make_pair(p6_idx, p8_idx)};
    vector<pair<int, int>> transitive_calls_pairs = pkb.GetAllTransitiveCallsPairs();
    std::sort(expected_transitive_calls_pairs.begin(), expected_transitive_calls_pairs.end());
    std::sort(transitive_calls_pairs.begin(), transitive_calls_pairs.end());
    REQUIRE(transitive_calls_pairs == expected_transitive_calls_pairs);
  }

  SECTION("Check Parent* population before testing UsesS calls") {
    vector<pair<int, int>> expected_transitive_parent_pairs = {make_pair(4, 5), make_pair(4, 6), make_pair(4, 7),
      make_pair(4, 8), make_pair(4, 9), make_pair(7, 8), make_pair(7, 9), make_pair(18, 19), make_pair(18, 20)};
    vector<pair<int, int>> transitive_parent_pairs = pkb.GetAllTransitiveParentPairs();
    std::sort(expected_transitive_parent_pairs.begin(), expected_transitive_parent_pairs.end());
    std::sort(transitive_parent_pairs.begin(), transitive_parent_pairs.end());
    REQUIRE(transitive_parent_pairs == expected_transitive_parent_pairs);
  }

  SECTION("Check UsesP population before testing UsesS calls") {
    vector<pair<int, int>> expected_proc_var_pairs = {make_pair(p1_idx, a_idx), make_pair(p1_idx, b_idx), make_pair(p1_idx, first_idx),
      make_pair(p1_idx, c_idx), make_pair(p1_idx, second_idx), make_pair(p1_idx, d_idx), make_pair(p1_idx, e_idx), make_pair(p1_idx, f_idx),
      make_pair(p1_idx, v_idx), make_pair(p1_idx, w_idx),
      make_pair(p1_idx, x_idx), make_pair(p1_idx, y_idx), make_pair(p1_idx, z_idx), make_pair(p1_idx, third_idx), make_pair(p1_idx, awesome_idx),
      make_pair(p1_idx, hmm_idx), make_pair(p1_idx, great_idx), make_pair(p1_idx, hello_idx), make_pair(p1_idx, there_idx),
      make_pair(p5_idx, e_idx), make_pair(p5_idx, f_idx), make_pair(p4_idx, x_idx), make_pair(p4_idx, y_idx),
      make_pair(p4_idx, z_idx), make_pair(p6_idx, v_idx), make_pair(p6_idx, w_idx),
      make_pair(p6_idx, third_idx), make_pair(p6_idx, awesome_idx), make_pair(p6_idx, hmm_idx), make_pair(p6_idx, great_idx),
      make_pair(p6_idx, hello_idx), make_pair(p6_idx, there_idx),
      make_pair(p8_idx, v_idx), make_pair(p8_idx, third_idx), make_pair(p8_idx, awesome_idx), make_pair(p8_idx, hmm_idx), make_pair(p8_idx, great_idx),
      make_pair(p8_idx, hello_idx), make_pair(p8_idx, there_idx)
    };
    vector<pair<int, int>> proc_var_pairs = pkb.GetAllUsesProcVarPairs();
    std::sort(expected_proc_var_pairs.begin(), expected_proc_var_pairs.end());
    std::sort(proc_var_pairs.begin(), proc_var_pairs.end());
    REQUIRE(proc_var_pairs == expected_proc_var_pairs);
  }

  SECTION("Check UsesS with calls population") {
    vector<pair<int, int>> expected_stmt_var_pairs = {make_pair(1, a_idx), make_pair(2, b_idx), make_pair(3, x_idx), make_pair(3, y_idx), make_pair(3, z_idx),
      make_pair(4, first_idx), make_pair(4, c_idx), make_pair(4, second_idx), make_pair(4, d_idx), make_pair(4, e_idx), make_pair(4, f_idx),
      make_pair(4, w_idx), make_pair(4, v_idx), make_pair(4, third_idx), make_pair(4, awesome_idx), make_pair(4, hmm_idx), make_pair(4, great_idx),
      make_pair(4, hello_idx), make_pair(4, there_idx),
      make_pair(5, c_idx),
      make_pair(6, w_idx), make_pair(6, v_idx), make_pair(6, third_idx), make_pair(6, awesome_idx), make_pair(6, hmm_idx), make_pair(6, great_idx),
      make_pair(6, hello_idx), make_pair(6, there_idx),
      make_pair(7, second_idx), make_pair(7, d_idx), make_pair(7, e_idx), make_pair(7, f_idx),
      make_pair(8, d_idx),
      make_pair(9, e_idx), make_pair(9, f_idx), make_pair(10, e_idx), make_pair(11, f_idx), make_pair(12, x_idx), make_pair(13, y_idx), make_pair(14, z_idx),
      make_pair(15, w_idx),
      make_pair(16, v_idx), make_pair(16, third_idx), make_pair(16, awesome_idx), make_pair(16, hmm_idx), make_pair(16, great_idx),
      make_pair(16, hello_idx), make_pair(16, there_idx),
      make_pair(17, v_idx), make_pair(18, third_idx), make_pair(18, awesome_idx), make_pair(18, hmm_idx), make_pair(18, great_idx),
      make_pair(18, hello_idx), make_pair(18, there_idx),
      make_pair(19, awesome_idx), make_pair(19, hmm_idx), make_pair(19, great_idx),
      make_pair(20, hello_idx), make_pair(20, there_idx)
    };
    vector<pair<int, int>> stmt_var_pairs = pkb.GetAllUsesStmtVarPairs();
    std::sort(expected_stmt_var_pairs.begin(), expected_stmt_var_pairs.end());
    std::sort(stmt_var_pairs.begin(), stmt_var_pairs.end());
    REQUIRE(stmt_var_pairs == expected_stmt_var_pairs);
  }
}
