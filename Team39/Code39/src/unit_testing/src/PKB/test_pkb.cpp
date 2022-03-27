//#include <vector>
//
//#include "PKB/pkb.h"
//#include "PKB/pkb_exceptions.h"
//
//#include "catch.hpp"
//
//// Read and print tables not tested as it is the same
//TEST_CASE("Populating Assign Table") {
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, "x");
//
//  SECTION("Add item into table: int -> string") {
//    REQUIRE(success == true);
//  }
//
//  SECTION("Add invalid item into table") {
//    success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, 2);
//    REQUIRE(success == false);
//
//    string str;
//    success = pkb.AddInfoToTable(TableIdentifier::kAssign, 2, str);
//    REQUIRE(success == false);
//  }
//}
//
//TEST_CASE("Populating Constant Table") {
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, vector<int>{50, 100});
//
//  SECTION("Add item into table: int -> vector<int>") {
//    REQUIRE(success == true);
//  }
//
//  SECTION("Add invalid item into table") {
//    success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, "invalid");
//    REQUIRE(success == false);
//
//    string str;
//    success = pkb.AddInfoToTable(TableIdentifier::kConstant, 2, str);
//    REQUIRE(success == false);
//  }
//}
//
//// While table not tested as it is the same
//TEST_CASE("Populating If Table") {
//  Pkb pkb = Pkb();
//
//  SECTION("Add item into table: int -> vector<string>") {
//    const bool success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, vector<string>{"x != 0"});
//    REQUIRE(success == true);
//  }
//
//  SECTION("Add invalid item into table") {
//    bool success = pkb.AddInfoToTable(TableIdentifier::kIf, 1, 1);
//    REQUIRE(success == false);
//
//    vector<string> vec = {};
//    success = pkb.AddInfoToTable(TableIdentifier::kIf, 2, vec);
//    REQUIRE(success == false);
//  }
//}
//
//TEST_CASE("Populating Follows and FollowsBefore Table") {
//  // FollowsBefore table is populated behind the scenes
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kFollows, 1, 2);
//  success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 3) && success;
//
//  SECTION("Add item into table: int -> int") {
//    REQUIRE(success == true);
//  }
//
//  SECTION("Add invalid item into table") {
//    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 1, vector<int>{2, 3, 4});
//    REQUIRE(success == false);
//
//    string str;
//    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 2, str);
//    REQUIRE(success == false);
//
//    int negative_number = -1;
//    success = pkb.AddInfoToTable(TableIdentifier::kFollows, 3, negative_number);
//    REQUIRE(success == false);
//  }
//
//  SECTION("Test API for PQL side for FollowsTable") {
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kFollows, 1, 2));
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kFollows, 2, 3));
//
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kFollows, 1) == vector<int>{2});
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kFollows, 2) == vector<int>{3});
//    // Key does not exist so the result should be empty
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kFollows, 3000) == vector<int>{});
//
//    vector<pair<int, int>> follows_pairs = pkb.GetRelArgumentPairs(pql::RelationshipTypes::kFollows);
//    vector<pair<int, int>> expected_follows_pairs = vector<pair<int, int>>{make_pair(1, 2), make_pair(2, 3)};
//    vector<pair<int, int>> invalid_first_pair = vector<pair<int, int>>{make_pair(1, 3)};
//    REQUIRE(follows_pairs == expected_follows_pairs);
//    REQUIRE(follows_pairs != invalid_first_pair);
//  }
//
//  SECTION("Test API for PQL side for FollowsBeforeTable") {
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kFollows, 0) == vector<int>{});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kFollows, 2) == vector<int>{1});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kFollows, 3) == vector<int>{2});
//  }
//}
//
//TEST_CASE("Populating Parent and Child Table") {
//  // Child table is populated behind the scenes
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, vector<int>{2, 3, 4});
//  success = pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>{5, 6}) && success;
//
//  SECTION("Add item into table: int -> vector<int>") {
//    REQUIRE(success);
//  }
//
//  SECTION("Add invalid item into table") {
//    success = pkb.AddInfoToTable(TableIdentifier::kParent, 1, 2);
//    REQUIRE(!success);
//
//    string str;
//    success = pkb.AddInfoToTable(TableIdentifier::kParent, 2, str);
//    REQUIRE(!success);
//
//    vector<int> empty_vector = {};
//    success = pkb.AddInfoToTable(TableIdentifier::kParent, 3, empty_vector);
//    REQUIRE(!success);
//  }
//
//  SECTION("Test API for PQL side for ParentTable") {
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kParent, 1, 2));
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kParent, 1, 3));
//    REQUIRE(!pkb.IsRelationshipHolds(pql::RelationshipTypes::kParent, 2, 3));
//    REQUIRE(!pkb.IsRelationshipHolds(pql::RelationshipTypes::kParent, 1, 5));
//
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kParent, 1).empty());
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kParent, 2) == vector<int>{1});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kParent, 5) == vector<int>{4});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kParent, 5) != vector<int>{1});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kParent, 6) != vector<int>{1});
//  }
//
//  SECTION("Test API for PQL side for ChildTable") {
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kParent, 1) == vector<int>{2, 3, 4});
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kParent, 1) != vector<int>{1, 2});
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kParent, 5).empty());
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kParent, 4) == vector<int>{5, 6});
//  }
//}
//
//TEST_CASE("Populating Calls Table") {
//  // Called by reverse relation is populated behind the scenes
//  Pkb pkb = Pkb();
//  bool success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p1");
//  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p2") && success;
//  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p3") && success;
//  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p4") && success;
//  success = pkb.AddEntityToSet(EntityIdentifier::kProc, "p5") && success;
//  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p1", vector<string>{"p4", "p5"}) && success;
//  success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p2", vector<string>{"p3", "p4"}) && success;
//
//  SECTION("Add item into table: string -> vector<string>") {
//    REQUIRE(success);
//  }
//
//  SECTION("Add invalid item into table") {
//    success = pkb.AddInfoToTable(TableIdentifier::kCalls, 1, 2);
//    REQUIRE(!success);
//
//    vector<string> empty_vector = {};
//    success = pkb.AddInfoToTable(TableIdentifier::kCalls, "p100", empty_vector);
//    REQUIRE(!success);
//  }
//
//  int p1_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, "p1");
//  int p2_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, "p2");
//  int p3_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, "p3");
//  int p4_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, "p4");
//  int p5_idx = pkb.GetIndexByString(IndexTableType::kProcIndex, "p5");
//  const int invalid_idx = 100;
//
//  SECTION("Test API for PQL side for Calls Table") {
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kCalls, p1_idx, p4_idx));
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kCalls, p1_idx, p5_idx));
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kCalls, p2_idx, p3_idx));
//    REQUIRE(pkb.IsRelationshipHolds(pql::RelationshipTypes::kCalls, p2_idx, p4_idx));
//    REQUIRE(!pkb.IsRelationshipHolds(pql::RelationshipTypes::kCalls, p1_idx, p2_idx));
//    REQUIRE(!pkb.IsRelationshipHolds(pql::RelationshipTypes::kCalls, p1_idx, invalid_idx));
//
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kCalls, p5_idx) == vector<int>{p1_idx});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kCalls, p3_idx) == vector<int>{p2_idx});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kCalls, p4_idx) == vector<int>{p1_idx, p2_idx});
//    REQUIRE(pkb.GetRelFirstArgument(pql::RelationshipTypes::kCalls, p5_idx) != vector<int>{p1_idx, p2_idx});
//
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kCalls, p1_idx) == vector<int>{p4_idx, p5_idx});
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kCalls, p2_idx) == vector<int>{p3_idx, p4_idx});
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kCalls, invalid_idx).empty());
//    REQUIRE(pkb.GetRelSecondArgument(pql::RelationshipTypes::kCalls, p1_idx) != vector<int>{p4_idx});
//
//    vector<pair<int, int>> expected_calls_pairs = {make_pair(p1_idx, p4_idx), make_pair(p1_idx, p5_idx), make_pair(p2_idx, p3_idx), make_pair(p2_idx, p4_idx)};
//    vector<pair<int, int>> calls_pairs = pkb.GetRelArgumentPairs(pql::RelationshipTypes::kCalls);
//    std::sort(calls_pairs.begin(), calls_pairs.end());
//    std::sort(expected_calls_pairs.begin(), expected_calls_pairs.end());
//    REQUIRE(calls_pairs == expected_calls_pairs);
//  }
//}
//
//TEST_CASE("Populating ModifiesProcToVariables and ModifiesVariableToProcs Table") {
//  // ModifiesVariableToProcs table is populated behind the scenes
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, "p1", vector<string>{"a", "b", "c"});
//  success = pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, "p2", vector<string>{"c", "y", "z"}) && success;
//
//  SECTION("Add item into table: string -> vector<string>") {
//    REQUIRE(success);
//  }
//
//  SECTION("Add invalid item into table") {
//    success = pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, 1, 2);
//    REQUIRE(!success);
//
//    string str;
//    success = pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, 2, str);
//    REQUIRE(!success);
//
//    vector<string> empty_vector = {};
//    success = pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, "p4", empty_vector);
//    REQUIRE(!success);
//  }
//}
//
//TEST_CASE("Populating if and while pattern") {
//  Pkb pkb = Pkb();
//  vector<string> empty_list;
//  bool success = pkb.AddInfoToTable(TableIdentifier::kIfPattern, 2, vector<string>{"A"});
//  success = success && pkb.AddInfoToTable(TableIdentifier::kIfPattern, 3, vector<string>{"A", "D", "AB", "C"});
//  success = success && pkb.AddInfoToTable(TableIdentifier::kIfPattern, 4, vector<string>{"A", "D", "AB", "C2E"});
//  success = success && pkb.AddInfoToTable(TableIdentifier::kIfPattern, 6, vector<string>{"m", "QAQ"});
//  success = success && pkb.AddInfoToTable(TableIdentifier::kWhilePattern, 11, vector<string>{"A2", "EB2E"});
//  SECTION("Adding patterns") {
//    REQUIRE(success);
//  }
//
//
//  SECTION("Test Search by stmt") {
//    unordered_set<string> res = pkb.GetAllPatternVariablesInStmt(2, TableIdentifier::kIfPattern);
//    REQUIRE(res == unordered_set<string>{"A"});
//
//    res = pkb.GetAllPatternVariablesInStmt(3, TableIdentifier::kIfPattern);
//    REQUIRE(res == unordered_set<string>{"A", "D", "AB", "C"});
//
//    res = pkb.GetAllPatternVariablesInStmt(4, TableIdentifier::kIfPattern);
//    REQUIRE(res == unordered_set<string>{"A", "D", "AB", "C2E"});
//
//    res = pkb.GetAllPatternVariablesInStmt(6, TableIdentifier::kIfPattern);
//    REQUIRE(res == unordered_set<string>{"m", "QAQ"});
//
//    res = pkb.GetAllPatternVariablesInStmt(10, TableIdentifier::kWhilePattern);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllPatternVariablesInStmt(11, TableIdentifier::kWhilePattern);
//    REQUIRE(res == unordered_set<string>{"A2", "EB2E"});
//  }
//
//  SECTION("Test Search by variable") {
//    unordered_set<int> res = pkb.GetAllStmtsWithPatternVariable("A", TableIdentifier::kIfPattern);
//    REQUIRE(res == unordered_set<int>{2, 3, 4});
//
//    res = pkb.GetAllStmtsWithPatternVariable("D", TableIdentifier::kIfPattern);
//    REQUIRE(res == unordered_set<int>{3, 4});
//
//    res = pkb.GetAllStmtsWithPatternVariable("A", TableIdentifier::kWhilePattern);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPatternVariable("EB2E", TableIdentifier::kWhilePattern);
//    REQUIRE(res == unordered_set<int>{11});
//  }
//
//  SECTION("Test search entire pair") {
//    vector<pair<int, string>> res = pkb.GetContainerStmtVarPair(TableIdentifier::kWhilePattern);
//    vector<pair<int, string>> expected_res = { make_pair(11, "A2"), make_pair(11, "EB2E") };
//    REQUIRE(res == expected_res);
//  }
//}
//
//TEST_CASE("Populating StmtToPatterns Table") {
//  // PatternToStmtsTable is populated behind the scenes
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 2, "A + (Bbs + C) + 2");
//  success = success && pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 3, "X + (Bbs + C) * (B + C)");
//  success = success && pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 4, "A + Bbs + C + 2");
//  success = success && pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 5, "289 * 444 + (f * cenX)");
//  SECTION("Adding patterns") {
//    REQUIRE(success);
//  }
//
//  SECTION("Search pattern") {
//    unordered_set<int> res = pkb.GetAllStmtsWithPattern("X + B + C", false);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPattern("A + (Bbs + C) + 2", false);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("Bbs ", false);
//    REQUIRE(res == unordered_set<int>{2, 3, 4});
//
//    res = pkb.GetAllStmtsWithPattern("Bbs + C", false);
//    REQUIRE(res == unordered_set<int>{3, 2});
//
//    res = pkb.GetAllStmtsWithPattern("A + (Bbs + C)", false);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("cenX", false);
//    REQUIRE(res == unordered_set<int>{5});
//
//    res = pkb.GetAllStmtsWithPattern("((2))", false);
//    REQUIRE(res == unordered_set<int>{2, 4});
//
//    res = pkb.GetAllStmtsWithPattern("289", false);
//    REQUIRE(res == unordered_set<int>{5});
//
//    res = pkb.GetAllStmtsWithPattern("((289 * (444)))", false);
//    REQUIRE(res == unordered_set<int>{5});
//  }
//
//  SECTION("Search Exact pattern") {
//    unordered_set<int> res = pkb.GetAllStmtsWithPattern("A + (Bbs + C) + 2", true);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("Bbs", true);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPattern("289 * 444 + (f * cenX)", true);
//    REQUIRE(res == unordered_set<int>{5});
//  }
//}
//
//TEST_CASE("Sample Tests for Pattern") {
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 2, "v + x * y + z % t");
//  success = pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 11, "lm + n") && success;
//  success = pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 12, "l + mn") && success;
//  success = pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 13, "(s1k + dks)") && success;
//  success = pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 14, "((a+(b)))*((c))") && success;
//  
//  SECTION("Adding patterns") {
//    REQUIRE(success);
//  }
//
//  SECTION("Nonempty results") {
//    unordered_set<int> res = pkb.GetAllStmtsWithPattern("v + x * y + z % t", true);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("v", false);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("x*y", false);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("v+x*y", false);
//    REQUIRE(res == unordered_set<int>{2});
//
//    res = pkb.GetAllStmtsWithPattern("v + x * y+ z % t", false);
//    REQUIRE(res == unordered_set<int>{2});
//  }
//
//  SECTION("Corner cases") {
//    unordered_set<int> res = pkb.GetAllStmtsWithPattern("(lm+n)", false);
//    REQUIRE(res == unordered_set<int>{11});
//
//    res = pkb.GetAllStmtsWithPattern("l+mn", false);
//    REQUIRE(res == unordered_set<int>{12});
//
//    res = pkb.GetAllStmtsWithPattern("((l+((mn))))", false);
//    REQUIRE(res == unordered_set<int>{12});
//
//    res = pkb.GetAllStmtsWithPattern("l", true);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPattern("((l+((mn))))", true);
//    REQUIRE(res == unordered_set<int>{12});
//
//    res = pkb.GetAllStmtsWithPattern("(a+b)*c", true);
//    REQUIRE(res == unordered_set<int>{14});
//
//    res = pkb.GetAllStmtsWithPattern("s1k + dks", false);
//    REQUIRE(res == unordered_set<int>{13}); 
//  }
//
//  SECTION("Empty result") {
//    unordered_set<int> res = pkb.GetAllStmtsWithPattern("v", true);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPattern("v+x", false);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPattern("y+z%t", false);
//    REQUIRE(res.empty());
//
//    res = pkb.GetAllStmtsWithPattern("x * y + z % t", false);
//    REQUIRE(res.empty());
//  }
//}
//
//TEST_CASE("Add Integer Entity") {
//  Pkb pkb = Pkb();
//
//  SECTION("Adding statement") {
//    bool success = pkb.AddEntityToSet(EntityIdentifier::kStmt, 1);
//    success = success && pkb.AddEntityToSet(EntityIdentifier::kStmt, 2);
//    success = success && pkb.AddEntityToSet(EntityIdentifier::kStmt, 3);
//    REQUIRE(success);
//
//    const unordered_set<int> res = pkb.GetAllEntity(EntityIdentifier::kStmt);
//    REQUIRE(res.size() == 3);
//    REQUIRE(res.find(1) != res.end());
//    REQUIRE(res.find(2) != res.end());
//    REQUIRE(res.find(3) != res.end());
//  }
//}
//
//TEST_CASE("Add String Entity") {
//  Pkb pkb = Pkb();
//
//  SECTION("Adding variable name") {
//    bool success = pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
//    success = success && pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
//    success = success && pkb.AddEntityToSet(EntityIdentifier::kVariable, "z");
//    REQUIRE(success);
//
//    const unordered_set<int> res = pkb.GetAllEntity(EntityIdentifier::kVariable);
//    REQUIRE(res.size() == 3);
//    REQUIRE(res.find(pkb.GetIndexByString(IndexTableType::kVarIndex, "x")) != res.end());
//    REQUIRE(res.find(pkb.GetIndexByString(IndexTableType::kVarIndex, "y")) != res.end());
//    REQUIRE(res.find(pkb.GetIndexByString(IndexTableType::kVarIndex, "z")) != res.end());
//
//    const vector<pair<int, string>> full_res = pkb.GetAllIndexStringPairs(IndexTableType::kVar);
//    REQUIRE(full_res.size() == 3);
//    const vector<pair<string, int>> full_res_2 = pkb.GetAllStringIndexPairs(IndexTableType::kVarIndex);
//    REQUIRE(full_res_2.size() == 3);
//    const string var_res = pkb.GetStringByIndex(IndexTableType::kVar, 0);
//    REQUIRE(var_res == "x");
//    const int idx_res = pkb.GetIndexByString(IndexTableType::kVarIndex, "y");
//    REQUIRE(idx_res == 1);
//  }
//
//  SECTION("Adding procedure name") {
//    bool success = pkb.AddEntityToSet(EntityIdentifier::kProc, "x");
//    success = success && pkb.AddEntityToSet(EntityIdentifier::kProc, "y");
//    success = success && pkb.AddEntityToSet(EntityIdentifier::kProc, "z");
//    REQUIRE(success);
//
//    const unordered_set<int> res = pkb.GetAllEntity(EntityIdentifier::kProc);
//    REQUIRE(res.size() == 3);
//    REQUIRE(res.find(pkb.GetIndexByString(IndexTableType::kProcIndex, "x")) != res.end());
//    REQUIRE(res.find(pkb.GetIndexByString(IndexTableType::kProcIndex, "y")) != res.end());
//    REQUIRE(res.find(pkb.GetIndexByString(IndexTableType::kProcIndex, "z")) != res.end());
//
//    const vector<pair<int, string>> full_res = pkb.GetAllIndexStringPairs(IndexTableType::kProc);
//    REQUIRE(full_res.size() == 3);
//    const vector<pair<string, int>> full_res_2 = pkb.GetAllStringIndexPairs(IndexTableType::kProcIndex);
//    REQUIRE(full_res_2.size() == 3);
//    const string proc_res = pkb.GetStringByIndex(IndexTableType::kProc, 0);
//    REQUIRE(proc_res == "x");
//    const int idx_res = pkb.GetIndexByString(IndexTableType::kProcIndex, "y");
//    REQUIRE(idx_res == 1);
//  }
//}
//
//TEST_CASE("Entity Attribute Operations") {
//  Pkb pkb = Pkb();
//  bool success = pkb.AddInfoToTable(TableIdentifier::kRead, 1, "x");
//  success = success && pkb.AddEntityToSet(EntityIdentifier::kRead, 1);
//  success = success && pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
//  success = success && pkb.AddInfoToTable(TableIdentifier::kRead, 2, "x");
//  success = success && pkb.AddEntityToSet(EntityIdentifier::kRead, 2);
//  success = success && pkb.AddInfoToTable(TableIdentifier::kRead, 3, "y");
//  success = success && pkb.AddEntityToSet(EntityIdentifier::kRead, 3);
//  success = success && pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
//
//  SECTION("Positive Test Cases") {
//    //bool res = pkb.IsRead(1);
//    //REQUIRE(res == true);
//
//    int x_idx = pkb.GetIndexByString(IndexTableType::kVarIndex, "x");
//    int var_idx = pkb.GetStringAttribute(EntityIdentifier::kRead, 1);
//    REQUIRE(var_idx == x_idx);
//
//    vector<int> stmt_res = pkb.GetStmtNumByStringAttribute(EntityIdentifier::kRead, x_idx);
//    REQUIRE(stmt_res == vector<int>{1, 2});
//  }
//
//  SECTION("Negative Test Cases") {
//    //bool res = pkb.IsRead(4);
//    //REQUIRE(res == false);
//
//    int var_idx = pkb.GetStringAttribute(EntityIdentifier::kRead, 4);
//    REQUIRE(var_idx == -1);
//
//    vector<int> stmt_res = pkb.GetStmtNumByStringAttribute(EntityIdentifier::kRead, 20);
//    REQUIRE(stmt_res.empty());
//  }
//}
