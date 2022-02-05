#include "Parser.h"
#include "PKB/pkb.h"

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
	*/
	TokenTemp TokenTemp1{ "name", "procedure", 1 };
	TokenTemp TokenTemp2{ "name", "procName", 1 };
	TokenTemp TokenTemp3{ "{", "{", 1 };
	TokenTemp TokenTemp4{ "name", "read", 2 };
	TokenTemp TokenTemp5{ "variable", "x", 2 };
	TokenTemp TokenTemp6{ ";", ";", 2 };
	TokenTemp TokenTemp7{ "name", "print", 3 };
	TokenTemp TokenTemp8{ "variable", "y", 3 };
	TokenTemp TokenTemp9{ ";", ";", 3 };
	TokenTemp TokenTemp10{ "}", "}", 4 };
	vector<TokenTemp> input = { TokenTemp1, TokenTemp2, TokenTemp3, TokenTemp4, TokenTemp5, TokenTemp6, TokenTemp7, TokenTemp8, TokenTemp9, TokenTemp10 };
	
	Parse(input);
	cout << endl;

	RequirePopulation(1 == 1);
}

TEST_CASE("Test populate PKB 2") {

	/*Input:
	* procedure procName {
	*	read y;
	*	x = y + 1;
	*	print x;
	* }
	*/
	TokenTemp TokenTemp1{ "procedure", "", 1 };
	TokenTemp TokenTemp2{ "name", "procName", 1 };
	TokenTemp TokenTemp3{ "{", "", 1 };
	TokenTemp TokenTemp4{ "read", "", 2 };
	TokenTemp TokenTemp5{ "variable", "y", 2 };
	TokenTemp TokenTemp6{ ";", "", 2 };
	TokenTemp TokenTemp7{ "variable", "x", 3 };
	TokenTemp TokenTemp8{ "operator", "=", 3 };
	TokenTemp TokenTemp9{ "variable", "y", 3 };
	TokenTemp TokenTemp10{ "operator", "+", 3 };
	TokenTemp TokenTemp11{ "constant", "1", 3 };
	TokenTemp TokenTemp12{ ";", "", 3 };
	TokenTemp TokenTemp13{ "print", "", 4 };
	TokenTemp TokenTemp14{ "variable", "x", 4 };
	TokenTemp TokenTemp15{ ";", "", 4 };
	TokenTemp TokenTemp16{ "}", "", 5 };
	vector<TokenTemp> input = { TokenTemp1, TokenTemp2, TokenTemp3, TokenTemp4, TokenTemp5, TokenTemp6, TokenTemp7, TokenTemp8, TokenTemp9, TokenTemp10, TokenTemp11, TokenTemp12,
		TokenTemp13, TokenTemp14, TokenTemp15, TokenTemp16 };

	Parse(input);

	RequirePopulation(1 == 1);
}