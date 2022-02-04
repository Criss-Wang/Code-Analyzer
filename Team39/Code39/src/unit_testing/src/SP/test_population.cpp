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
	Token token1{ "name", "procedure", 1 };
	Token token2{ "name", "procName", 1 };
	Token token3{ "{", "{", 1 };
	Token token4{ "name", "read", 2 };
	Token token5{ "variable", "x", 2 };
	Token token6{ ";", ";", 2 };
	Token token7{ "name", "print", 3 };
	Token token8{ "variable", "y", 3 };
	Token token9{ ";", ";", 3 };
	Token token10{ "}", "}", 4 };
	vector<Token> input = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };
	
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
	Token token1{ "procedure", "", 1 };
	Token token2{ "name", "procName", 1 };
	Token token3{ "{", "", 1 };
	Token token4{ "read", "", 2 };
	Token token5{ "variable", "y", 2 };
	Token token6{ ";", "", 2 };
	Token token7{ "variable", "x", 3 };
	Token token8{ "operator", "=", 3 };
	Token token9{ "variable", "y", 3 };
	Token token10{ "operator", "+", 3 };
	Token token11{ "constant", "1", 3 };
	Token token12{ ";", "", 3 };
	Token token13{ "print", "", 4 };
	Token token14{ "variable", "x", 4 };
	Token token15{ ";", "", 4 };
	Token token16{ "}", "", 5 };
	vector<Token> input = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10, token11, token12,
		token13, token14, token15, token16 };

	Parse(input);

	RequirePopulation(1 == 1);
}