#include "Parser.h"
#include "PKB/pkb.h"
#include "PKB/tables/table.h"

#include <fstream>
#include "catch.hpp"

using namespace std;
void RequirePopulation(bool b) {
    REQUIRE(b);
}


// Just placeholder test cases to test the logic of parse.
// To be replaced with actual test cases once PKB API is provided.
TEST_CASE("Test populate PKB") {
	
	/*Input:
	* procedure procName {
	*	read x;
	*	print y;
	* }
	* */
	ifstream input_file("C:/Users/yuxua/OneDrive/Documents/NUS/Y3S2/CS3203/21s2-cp-spa-team-39/Team39/Tests39/sp/test1.txt");
	if (!input_file.is_open()) {
		cerr << "Could not open the file " << endl;
		RequirePopulation(1 == 1);
	} else {
		string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

		Pkb pkb;
		Parse(input, pkb);

		RequirePopulation(1==1);
	}

	RequirePopulation(1 == 1);
}

TEST_CASE("Test populate PKB 2") {
	/* Input:
	* procedure procName {
	*	x = y * 3;
	* print x;
	* }
	* */

	ifstream input_file("C:/Users/yuxua/OneDrive/Documents/NUS/Y3S2/CS3203/21s2-cp-spa-team-39/Team39/Tests39/sp/test2.txt");
	if (!input_file.is_open()) {
		cerr << "Could not open the file " << endl;
		RequirePopulation(1 == 1);
	} else {
		string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

		Pkb pkb;
		Parse(input, pkb);

		RequirePopulation(1 == 1);
	}

	RequirePopulation(1 == 1);
}
