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
		TokenTemp TokenTemp1{ "name", "procedure", 1 };
		TokenTemp TokenTemp2{ "name", "procName", 1 };
		TokenTemp TokenTemp3{ "{", "{", 1 };
		TokenTemp TokenTemp4{ "name", "read", 2 };
		TokenTemp TokenTemp5{ "variable", "y", 2 };
		TokenTemp TokenTemp6{ ";", ";", 2 };
		TokenTemp TokenTemp7{ "}", "}", 3 };
		vector<TokenTemp> input = { TokenTemp1, TokenTemp2, TokenTemp3, TokenTemp4, TokenTemp5, TokenTemp6, TokenTemp7 };

		RequireValidation(Validate(input));
	}

	SECTION("Test 2") {
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

		RequireValidation(Validate(input));
	}

	SECTION("Test 3") {
		/*Input:
		* procedure procName {
		*	x = y * 3;
		* print x;
		* }
		*/
		TokenTemp TokenTemp1{ "name", "procedure", 1 };
		TokenTemp TokenTemp2{ "name", "procName", 1 };
		TokenTemp TokenTemp3{ "{", "{", 1 };
		TokenTemp TokenTemp4{ "variable", "x", 2 };
		TokenTemp TokenTemp5{ "operator", "=", 2 };
		TokenTemp TokenTemp6{ "variable", "y", 2 };
		TokenTemp TokenTemp7{ "operator", "*", 2 };
		TokenTemp TokenTemp8{ "variable", "3", 2 };
		TokenTemp TokenTemp9{ ";", ";", 2 };
		TokenTemp TokenTemp10{ "name", "print", 3 };
		TokenTemp TokenTemp11{ "variable", "x", 3 };
		TokenTemp TokenTemp12{ ";", ";", 3 };
		TokenTemp TokenTemp13{ "}", "}", 4 };
		vector<TokenTemp> input = { TokenTemp1, TokenTemp2, TokenTemp3, TokenTemp4, TokenTemp5, TokenTemp6, TokenTemp7, TokenTemp8, TokenTemp9, TokenTemp10, TokenTemp11,
			TokenTemp12, TokenTemp13 };

		RequireValidation(Validate(input));
	}

	SECTION("Test 4") {
		/*Input:
		* procedure procName {
		*	x = y + (5 + z);
		* }
		*/
		TokenTemp TokenTemp1{ "name", "procedure", 1 };
		TokenTemp TokenTemp2{ "name", "procName", 1 };
		TokenTemp TokenTemp3{ "{", "{", 1 };
		TokenTemp TokenTemp4{ "variable", "x", 2 };
		TokenTemp TokenTemp5{ "operator", "=", 2 };
		TokenTemp TokenTemp6{ "variable", "y", 2 };
		TokenTemp TokenTemp7{ "operator", "+", 2 };
		TokenTemp TokenTemp8{ "bracket", "(", 2 };
		TokenTemp TokenTemp9{ "variable", "5", 2 };
		TokenTemp TokenTemp10{ "operator", "+", 2 };
		TokenTemp TokenTemp11{ "variable", "z", 2 };
		TokenTemp TokenTemp12{ "bracket", ")", 2 };
		TokenTemp TokenTemp13{ ";", ";", 2 };
		TokenTemp TokenTemp14{ "}", "}", 3 };
		vector<TokenTemp> input = { TokenTemp1, TokenTemp2, TokenTemp3, TokenTemp4, TokenTemp5, TokenTemp6, TokenTemp7, TokenTemp8, TokenTemp9, TokenTemp10, TokenTemp11,
			TokenTemp12, TokenTemp13, TokenTemp14 };

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
	TokenTemp TokenTemp1{ "name", "procedre", 1 };
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

	RequireValidation(!Validate(input));
}