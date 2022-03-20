#include "SP/tokenizer.h"
#include "SP/populator.h"
#include "PKB/pkb.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

vector<Token> parse(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
    return {};
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    Tokenizer tokenizer;
    vector<Token> input_tokens = tokenizer.parse(input);
    return input_tokens;
  }
}

string populate_dir = "../../../../../../Tests39/sp/valid_programs/";

TEST_CASE("Read/print statements for Population") {

  SECTION("1_test1") {
    vector<Token> input_tokens = parse(populate_dir + "1_test1.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    // check relation tables
    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 1);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("1_test2") {
    vector<Token> input_tokens = parse(populate_dir + "1_test2.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_print = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    // check relation tables
    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 1);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("1_test3") {
    vector<Token> input_tokens = parse(populate_dir + "1_test3.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1, 2, 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 2 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    // check relation tables
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 2);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 1);
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("1_test4") {
    vector<Token> input_tokens = parse(populate_dir + "1_test4.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("yhhs"), pkb.GetIndexByVar("x09"), pkb.GetIndexByVar("read"), pkb.GetIndexByVar("print"), pkb.GetIndexByVar("z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1, 2, 3, 4, 5, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_read = { 1, 2, 5 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kRead) == expected_read);

    unordered_set<int> expected_print = { 3, 4, 6 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kPrint) == expected_print);

    // check relation tables
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 3);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("yhhs")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("print")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x09")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("read")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("z")});
  }
}

TEST_CASE("Read/print/assign statments for Population") {

  SECTION("2_test1") {
    vector<Token> input_tokens = parse(populate_dir + "2_test1.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kVariable) == expected_variable);

    unordered_set<int> expected_stmt = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kStmt) == expected_stmt);

    unordered_set<int> expected_assign = { 1 };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kAssign) == expected_assign);

    // check relation tables
    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 1);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 1);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
  }

  SECTION("2_test2") {
    vector<Token> input_tokens = parse(populate_dir + "2_test2.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 2);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 2);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("2_test3") {
    vector<Token> input_tokens = parse(populate_dir + "2_test3.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 2);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 2);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("y")});
  }

  SECTION("2_test4") {
    vector<Token> input_tokens = parse(populate_dir + "2_test4.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 5);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("z")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("z"), pkb.GetIndexByVar("y")});

  }
}


TEST_CASE("Read/print/assign/if/while statments (1 level nesting) for Population") {

  SECTION("3_test1") {
    vector<Token> input_tokens = parse(populate_dir + "3_test1.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(3) == 4);

    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 1);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3, 4, 5 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("z")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("3_test2") {
    vector<Token> input_tokens = parse(populate_dir + "3_test2.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 2);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 4);

    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 1);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 3);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("3_test3") {
    vector<Token> input_tokens = parse(populate_dir + "3_test3.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 3);
    REQUIRE(follows_table.GetValueByKey(1) == 4);
    REQUIRE(follows_table.GetValueByKey(4) == 6);
    REQUIRE(follows_table.GetValueByKey(6) == 7);

    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 2);
    REQUIRE(parent_table.GetValueByKey(1) == vector<int>{ 2, 3 });
    REQUIRE(parent_table.GetValueByKey(4) == vector<int>{ 5 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 5);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("x")});
  }

  SECTION("3_test4") {
    vector<Token> input_tokens = parse(populate_dir + "3_test4.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity tables
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(4) == 8);

    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 1);
    REQUIRE(parent_table.GetValueByKey(4) == vector<int>{ 5, 6, 7 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 6);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("z")});
    REQUIRE(modifies_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 6);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("z"), pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("x")});
  }
}

TEST_CASE("Read/print/assign/if/while statments (2 level nesting) for Population") {

  SECTION("4_test1") {
    vector<Token> input_tokens = parse(populate_dir + "4_test1.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 1);
    REQUIRE(follows_table.GetValueByKey(1) == 2);

    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 2);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3 });
    REQUIRE(parent_table.GetValueByKey(3) == vector<int>{ 4, 5 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
  }
  
  SECTION("4_test2") {
    vector<Token> input_tokens = parse(populate_dir + "4_test2.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 6);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 9);
    REQUIRE(follows_table.GetValueByKey(9) == 10);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(7) == 8);


    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 2);
    REQUIRE(parent_table.GetValueByKey(2) == vector<int>{ 3, 4, 7, 8 });
    REQUIRE(parent_table.GetValueByKey(4) == vector<int>{ 5, 6 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 7);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(10) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 8);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("z")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(9) == vector<int>{pkb.GetIndexByVar("x")});
  }
}

TEST_CASE("Read/print/assign/if/while statments (3 level nesting) for Population") {

  SECTION("5_test1") {
    vector<Token> input_tokens = parse(populate_dir + "5_test1.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("procName") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("v"), pkb.GetIndexByVar("t")};
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 9);
    REQUIRE(follows_table.GetValueByKey(9) == 10);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 8);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    ParentTable parent_table = *pkb.GetParentTable();
    REQUIRE(parent_table.GetTableSize() == 3);
    REQUIRE(parent_table.GetValueByKey(1) == vector<int>{ 2, 3, 8 });
    REQUIRE(parent_table.GetValueByKey(3) == vector<int>{ 4, 5, 6 });
    REQUIRE(parent_table.GetValueByKey(6) == vector<int>{ 7 });

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 8);
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("v")});
    REQUIRE(modifies_table.GetValueByKey(9) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("v")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("v")});
    REQUIRE(modifies_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("v")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 8);
    REQUIRE(uses_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("v")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("v")});
    REQUIRE(uses_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("x")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("v")});
    REQUIRE(uses_table.GetValueByKey(8) == vector<int>{pkb.GetIndexByVar("y")});
    REQUIRE(uses_table.GetValueByKey(9) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(10) == vector<int>{pkb.GetIndexByVar("x")});
  }
}

TEST_CASE("Read/print/assign/call statments for Population") {

  SECTION("6_test1") {
    vector<Token> input_tokens = parse(populate_dir + "6_test1.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("proc1"), pkb.GetIndexByProc("proc2") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("print"), pkb.GetIndexByVar("x"), pkb.GetIndexByVar("proc1"), pkb.GetIndexByVar("proc2"), pkb.GetIndexByVar("proc3"), pkb.GetIndexByVar("t") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 4);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(3) == 4);
    REQUIRE(follows_table.GetValueByKey(5) == 6);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 4);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("print")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("proc2")});
    REQUIRE(modifies_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("x")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 3);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("proc1")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("proc3")});
    REQUIRE(uses_table.GetValueByKey(6) == vector<int>{pkb.GetIndexByVar("t")});
    
  }

  SECTION("6_test2") {
    vector<Token> input_tokens = parse(populate_dir + "6_test2.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("proc1"), pkb.GetIndexByProc("proc2"), pkb.GetIndexByProc("proc3") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("proc1"), pkb.GetIndexByVar("x"), pkb.GetIndexByVar("y"), pkb.GetIndexByVar("f"), pkb.GetIndexByVar("proc3"), pkb.GetIndexByVar("procedure"), pkb.GetIndexByVar("t") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(7) == 8);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 5);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("proc1")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("procedure")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("x")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("y"), pkb.GetIndexByVar("f")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("proc3")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("t")});

  }

  SECTION("6_test3") {
    vector<Token> input_tokens = parse(populate_dir + "6_test3.txt");
    Pkb pkb;
    populate(input_tokens, pkb);

    // check entity sets
    unordered_set<int> expected_proc = { pkb.GetIndexByProc("proc1"), pkb.GetIndexByProc("proc2"), pkb.GetIndexByProc("procedure") };
    REQUIRE(pkb.GetAllEntity(EntityIdentifier::kProc) == expected_proc);

    unordered_set<int> expected_variable = { pkb.GetIndexByVar("print"), pkb.GetIndexByVar("x"), pkb.GetIndexByVar("proc1"), pkb.GetIndexByVar("proc2"), pkb.GetIndexByVar("procedure"), pkb.GetIndexByVar("t") };
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
    FollowsTable follows_table = *pkb.GetFollowsTable();
    REQUIRE(follows_table.GetTableSize() == 5);
    REQUIRE(follows_table.GetValueByKey(1) == 2);
    REQUIRE(follows_table.GetValueByKey(2) == 3);
    REQUIRE(follows_table.GetValueByKey(4) == 5);
    REQUIRE(follows_table.GetValueByKey(5) == 6);
    REQUIRE(follows_table.GetValueByKey(7) == 8);

    ModifiesStmtToVariablesTable modifies_table = *pkb.GetModifiesStmtToVariablesTable();
    REQUIRE(modifies_table.GetTableSize() == 5);
    REQUIRE(modifies_table.GetValueByKey(1) == vector<int>{pkb.GetIndexByVar("print")});
    REQUIRE(modifies_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("proc2")});
    REQUIRE(modifies_table.GetValueByKey(4) == vector<int>{pkb.GetIndexByVar("x")});
    REQUIRE(modifies_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("t")});

    UsesStmtToVariablesTable uses_table = *pkb.GetUsesStmtToVariablesTable();
    REQUIRE(uses_table.GetTableSize() == 4);
    REQUIRE(uses_table.GetValueByKey(2) == vector<int>{pkb.GetIndexByVar("proc1")});
    REQUIRE(uses_table.GetValueByKey(3) == vector<int>{pkb.GetIndexByVar("procedure")});
    REQUIRE(uses_table.GetValueByKey(5) == vector<int>{pkb.GetIndexByVar("t")});
    REQUIRE(uses_table.GetValueByKey(7) == vector<int>{pkb.GetIndexByVar("x")});

  }
}
