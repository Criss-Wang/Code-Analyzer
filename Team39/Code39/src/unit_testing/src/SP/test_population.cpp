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
		unordered_set<string> expectedProc = { "procName" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kProc) == expectedProc);

		unordered_set<string> expectedVariable = { "x" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kVariable) == expectedVariable);

		unordered_set<int> expectedStmt = { 1 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kStmt) == expectedStmt);

		unordered_set<int> expectedRead = { 1 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kRead) == expectedRead);
	}

	SECTION("1_test2") {
		vector<Token> input_tokens = parse(populate_dir + "1_test2.txt");
		Pkb pkb;
		populate(input_tokens, pkb);

		// check entity sets
		unordered_set<string> expectedProc = { "procName" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kProc) == expectedProc);

		unordered_set<string> expectedVariable = { "x" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kVariable) == expectedVariable);

		unordered_set<int> expectedStmt = { 1 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kStmt) == expectedStmt);

		unordered_set<int> expectedPrint = { 1 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kPrint) == expectedPrint);

	}

	SECTION("1_test3") {
		vector<Token> input_tokens = parse(populate_dir + "1_test3.txt");
		Pkb pkb;
		populate(input_tokens, pkb);

		// check entity tables
		unordered_set<string> expectedProc = { "procName" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kProc) == expectedProc);

		unordered_set<string> expectedVariable = { "x", "y" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kVariable) == expectedVariable);

		unordered_set<int> expectedStmt = { 1, 2, 3 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kStmt) == expectedStmt);

		unordered_set<int> expectedRead = { 1, 2 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kRead) == expectedRead);

		unordered_set<int> expectedPrint = { 3 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kPrint) == expectedPrint);

	}

	SECTION("1_test4") {
		vector<Token> input_tokens = parse(populate_dir + "1_test4.txt");
		Pkb pkb;
		populate(input_tokens, pkb);

		// check entity tables
		unordered_set<string> expectedProc = { "procName" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kProc) == expectedProc);

		unordered_set<string> expectedVariable = { "x", "yhhs", "x09", "read", "print", "z" };
		REQUIRE(pkb.GetAllEntityString(EntityIdentifier::kVariable) == expectedVariable);

		unordered_set<int> expectedStmt = { 1, 2, 3, 4, 5, 6 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kStmt) == expectedStmt);

		unordered_set<int> expectedRead = { 1, 2, 5 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kRead) == expectedRead);

		unordered_set<int> expectedPrint = { 3, 4, 6 };
		REQUIRE(pkb.GetAllEntityInt(EntityIdentifier::kPrint) == expectedPrint);

	}
}

/*
TEST_CASE("Read/print/assign statments for Population") {

	RequirePopulate(populate_dir + "2_test1.txt");
	RequirePopulate(populate_dir + "2_test2.txt");
	RequirePopulate(populate_dir + "2_test3.txt");
	RequirePopulate(populate_dir + "2_test4.txt");

}

TEST_CASE("Read/print/assign/if/while statments (1 level nesting) for Population") {

	RequirePopulate(populate_dir + "3_test1.txt");
	RequirePopulate(populate_dir + "3_test2.txt");
	RequirePopulate(populate_dir + "3_test3.txt");
	RequirePopulate(populate_dir + "3_test4.txt");
  
}

TEST_CASE("Read/print/assign/if/while statments (2 level nesting) for Population") {

	RequirePopulate(populate_dir + "4_test1.txt");
	RequirePopulate(populate_dir + "4_test2.txt");
	RequirePopulate(populate_dir + "4_test3.txt");

}
*/