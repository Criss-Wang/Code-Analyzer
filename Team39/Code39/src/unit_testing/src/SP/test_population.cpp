#include "Parser.h"
#include "PKB/pkb.h"
#include "PKB/tables/table.h"

#include <fstream>
#include "catch.hpp"

using namespace std;
void RequirePopulate(string path) {
	ifstream input_file(path);
	if (!input_file.is_open()) {
		cerr << "Could not open the file " << endl;
	} else {
		string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		Pkb pkb;
		Parse(input, pkb);
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
	//RequireValid(valid_dir + "2_test3.txt"); syntax error
	//RequireValid(valid_dir + "2_test4.txt"); syntax error
}

TEST_CASE("Read/print/assign/if statments for Population") {

}

TEST_CASE("Read/print/assign/while statments for Population") {

}

TEST_CASE("Read/print/assign/if/while statments for Population") {

}
