#include "SP/tokenizer.h"
#include "SP/populator.h"
#include "PKB/pkb.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

void RequirePopulate(string path) {
	ifstream input_file(path);
	if (!input_file.is_open()) {
		cerr << "Could not open the file " << endl;
	} else {
		string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		Tokenizer tokenizer;
		vector<Token> input_tokens = tokenizer.parse(input);
		Pkb pkb;
		populate(input_tokens, pkb);
		REQUIRE(1 == 1);
	}
}

string populate_dir = "../../../../../../Tests39/sp/valid_programs/";

TEST_CASE("Read/print statements for Population") {

	RequirePopulate(populate_dir + "1_test1.txt");
	RequirePopulate(populate_dir + "1_test2.txt");
	RequirePopulate(populate_dir + "1_test3.txt");
	RequirePopulate(populate_dir + "1_test4.txt");
  
}

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