#include "Parser.h"
#include "SP/validator.h"

#include "catch.hpp"
using namespace std;
void RequireValidation(bool b) {
    REQUIRE(b);
}

TEST_CASE("Valid programs") {
	
	SECTION("Test 1") {
		/*Input:
		* procedure procName {
		*	read y;
		* }
		*/
		Token token1{ "name", "procedure", 1 };
		Token token2{ "name", "procName", 1 };
		Token token3{ "{", "{", 1 };
		Token token4{ "name", "read", 2 };
		Token token5{ "variable", "y", 2 };
		Token token6{ ";", ";", 2 };
		Token token7{ "}", "}", 3 };
		vector<Token> input = { token1, token2, token3, token4, token5, token6, token7 };

		RequireValidation(Validate(input));
	}

	SECTION("Test 2") {
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

		RequireValidation(Validate(input));
	}

	SECTION("Test 3") {
		/*Input:
		* procedure procName {
		*	x = y * 3;
		* print x;
		* }
		*/
		Token token1{ "name", "procedure", 1 };
		Token token2{ "name", "procName", 1 };
		Token token3{ "{", "{", 1 };
		Token token4{ "variable", "x", 2 };
		Token token5{ "operator", "=", 2 };
		Token token6{ "variable", "y", 2 };
		Token token7{ "operator", "*", 2 };
		Token token8{ "variable", "3", 2 };
		Token token9{ ";", ";", 2 };
		Token token10{ "name", "print", 3 };
		Token token11{ "variable", "x", 3 };
		Token token12{ ";", ";", 3 };
		Token token13{ "}", "}", 4 };
		vector<Token> input = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10, token11,
			token12, token13 };

		RequireValidation(Validate(input));
	}

	SECTION("Test 4") {
		/*Input:
		* procedure procName {
		*	x = y + (5 + z);
		* }
		*/
		Token token1{ "name", "procedure", 1 };
		Token token2{ "name", "procName", 1 };
		Token token3{ "{", "{", 1 };
		Token token4{ "variable", "x", 2 };
		Token token5{ "operator", "=", 2 };
		Token token6{ "variable", "y", 2 };
		Token token7{ "operator", "+", 2 };
		Token token8{ "bracket", "(", 2 };
		Token token9{ "variable", "5", 2 };
		Token token10{ "operator", "+", 2 };
		Token token11{ "variable", "z", 2 };
		Token token12{ "bracket", ")", 2 };
		Token token13{ ";", ";", 2 };
		Token token14{ "}", "}", 3 };
		vector<Token> input = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10, token11,
			token12, token13, token14 };

		RequireValidation(Validate(input));
	}
}

TEST_CASE("Invalid Programs") {

	/*Input:
	* procedre procName {
	*	read y;
	*	x = y + 1;
	*	print x;
	* }
	*/
	Token token1{ "name", "procedre", 1 };
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

	RequireValidation(!Validate(input));
}