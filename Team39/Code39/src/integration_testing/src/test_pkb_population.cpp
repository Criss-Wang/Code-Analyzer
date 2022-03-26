#include "SP/parser.h"
#include "PKB/pkb.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

Pkb parse(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
    return {};
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    Pkb pkb;
    Parser parser(input, pkb);
    parser.Populate(pkb);
    return pkb;
  }
}

string populate_dir = "../../../../../../Tests39/sp/valid_programs/";

TEST_CASE("Read/print statements for Population") {

  SECTION("1_test1") {
    Pkb pkb = parse(populate_dir + "1_test1.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    // check relation tables
    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 1);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});

  }

  SECTION("1_test2") {
    Pkb pkb = parse(populate_dir + "1_test2.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_print = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    // check relation tables
    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 1);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }

  SECTION("1_test3") {
    Pkb pkb = parse(populate_dir + "1_test3.txt");

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1, 2, 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 2);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 1);
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }

  SECTION("1_test4") {
    Pkb pkb = parse(populate_dir + "1_test4.txt");

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "yhhs"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x09"), pkb.GetIndexByString(IndexTableType::kVarIndex, "read"), pkb.GetIndexByString(IndexTableType::kVarIndex, "print"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 2, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3, 4, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 3);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "yhhs")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "print")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x09")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "read")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
  }
}

TEST_CASE("Read/print/assign statments for Population") {

  SECTION("2_test1") {
    Pkb pkb = parse(populate_dir + "2_test1.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_assign = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    // check relation tables
    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 1);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 1);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
  }

  SECTION("2_test2") {
    Pkb pkb = parse(populate_dir + "2_test2.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);
    
    unordered_set<int> expected_constant = { 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 2);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 2);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }

  SECTION("2_test3") {
    Pkb pkb = parse(populate_dir + "2_test3.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 3, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 2);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 2);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
  }

  SECTION("2_test4") {
    Pkb pkb = parse(populate_dir + "2_test4.txt");

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 1, 3, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2, 5, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 5);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "z"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});

  }
}


TEST_CASE("Read/print/assign/if/while statments (1 level nesting) for Population") {

  SECTION("3_test1") {
    Pkb pkb = parse(populate_dir + "3_test1.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 4, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 1, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_if = { 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kIf) == expected_if);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(3) == 4);

    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 1);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3, 4, 5 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }

  SECTION("3_test2") {
    Pkb pkb = parse(populate_dir + "3_test2.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 4, 1, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_while = { 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kWhile) == expected_while);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 4);

    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 1);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 3);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }

  SECTION("3_test3") {
    Pkb pkb = parse(populate_dir + "3_test3.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 9, 10, 0, 3, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6, 7 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 2, 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 5, 7 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_if = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kIf) == expected_if);
    
    unordered_set<int> expected_while = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kWhile) == expected_while);
    
    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 3);
    REQUIRE(follows_table.GetValueByKey(1) == 4);
    REQUIRE(follows_table.GetValueByKey(4) == 6);
    REQUIRE(follows_table.GetValueByKey(6) == 7);

    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 2);
    REQUIRE(parent_table.GetValueByKey(1) == vector<int>{ 2, 3 });
    REQUIRE(parent_table.GetValueByKey(4) == vector<int>{ 5 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 5);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }

  SECTION("3_test4") {
    Pkb pkb = parse(populate_dir + "3_test4.txt");

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 3, 4, 0, 8, 9, 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6, 7, 8 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 8 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3, 7 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2, 5, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_if = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kIf) == expected_if);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(4) == 8);

    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 1);
    REQUIRE(parent_table.GetValueByKey(4) == vector<int>{ 5, 6, 7 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 6);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(modifies_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 6);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "z"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }
}

TEST_CASE("Read/print/assign/if/while statments (2 level nesting) for Population") {

  SECTION("4_test1") {
    Pkb pkb = parse(populate_dir + "4_test1.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 5, 4, 0 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_if = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kIf) == expected_if);

    unordered_set<int> expected_while = { 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kWhile) == expected_while);
    
    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 1);
    REQUIRE(follows_table.GetValueByKey(1) == 2);

    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 2);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3 });
    REQUIRE(parent_table.GetValueByKey(3) == vector<int>{ 4, 5 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }
  
  SECTION("4_test2") {
    Pkb pkb = parse(populate_dir + "4_test2.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 4, 1, 5, 188, 0 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 6, 7, 9 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 3, 5, 8, 10 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_if = { 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kIf) == expected_if);

    unordered_set<int> expected_while = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kWhile) == expected_while);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 6);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 9);
    REQUIRE(follows_table.GetValueByKey(9) == 10);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(7) == 8);


    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 2);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3, 4, 7, 8 });
    REQUIRE(parent_table.GetValueByKey(4) == vector<int>{ 5, 6 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 7);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(10) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 8);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "z")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(9) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }
}

TEST_CASE("Read/print/assign/if/while statments (3 level nesting) for Population") {

  SECTION("5_test1") {
    Pkb pkb = parse(populate_dir + "5_test1.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "v"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")};
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 9, 10, 1, 0, 100, 199, 3, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 2, 7 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 8, 10 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 4, 5, 9 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_if = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kIf) == expected_if);

    unordered_set<int> expected_while = { 1, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kWhile) == expected_while);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 9);
    REQUIRE(follows_table.GetValueByKey(9) == 10);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 8);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    RelListTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 3);
    REQUIRE(parent_table.GetValueByKey(1) == vector<int>{ 2, 3, 8 });
    REQUIRE(parent_table.GetValueByKey(3) == vector<int>{ 4, 5, 6 });
    REQUIRE(parent_table.GetValueByKey(6) == vector<int>{ 7 });

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 8);
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});
    REQUIRE(modifies_table.GetValueByKey(9) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 8);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "v")});
    REQUIRE(uses_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y")});
    REQUIRE(uses_table.GetValueByKey(9) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(10) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
  }
}

TEST_CASE("Read/print/assign/call statments for Population") {

  SECTION("6_test1") {
    Pkb pkb = parse(populate_dir + "6_test1.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kProcIndex, "proc2") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "print"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc2"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc3"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2, 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_call = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kCall) == expected_call);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 4);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 5);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "print")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc2")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc3")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    
  }

  SECTION("6_test2") {
    Pkb pkb = parse(populate_dir + "6_test2.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kProcIndex, "proc2"), pkb.GetIndexByString(IndexTableType::kProcIndex, "proc3") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "f"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc3"), pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6, 7, 8 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2, 3, 7 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_call = { 6, 8 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kCall) == expected_call);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(7) == 8);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 7);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure")});
    REQUIRE(modifies_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 6);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "f")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc3")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "f"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc3")});
    REQUIRE(uses_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "y"), pkb.GetIndexByString(IndexTableType::kVarIndex, "f"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc3")});

  }

  SECTION("6_test3") {
    Pkb pkb = parse(populate_dir + "6_test3.txt");

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByString(IndexTableType::kProcIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kProcIndex, "proc2"), pkb.GetIndexByString(IndexTableType::kProcIndex, "procedure") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByString(IndexTableType::kVarIndex, "print"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc2"), pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_constant = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kConstant) == expected_constant);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6, 7, 8 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 4 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    unordered_set<int> expected_assign = { 2, 3, 7 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    unordered_set<int> expected_call = { 6, 8 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kCall) == expected_call);

    // check relation tables
    RelTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(7) == 8);

    RelListTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 7);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "print")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc2")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(modifies_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "print"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc2"), pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(modifies_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "print"), pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc2")});

    RelListTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 6);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "t")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "x"), pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure")});
    REQUIRE(uses_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByString(IndexTableType::kVarIndex, "proc1"), pkb.GetIndexByString(IndexTableType::kVarIndex, "procedure")});

  }
}
