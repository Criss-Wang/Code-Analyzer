#include "Parser.h"
#include "SP/validator.h"

#include <fstream>
#include "catch.hpp"

using namespace std;
void RequireValidation(bool b) {
    REQUIRE(b);
}


TEST_CASE("Valid programs") {

	SECTION("Test 1") {
		/*Input:
		* procedure procName {
		*	read x;
		*	print y;
		* }
		* */

		ifstream input_file("C:/Users/yuxua/OneDrive/Documents/NUS/Y3S2/CS3203/21s2-cp-spa-team-39/Team39/Tests39/validation_tests/test1.txt");
		if (!input_file.is_open()) {
			cerr << "Could not open the file " << endl;
			RequireValidation(1 == 1);
		} else {
			string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

			Tokenizer tokenize;
			vector<Token> tokens = tokenize.parse(input);
			cout << endl;

			RequireValidation(Validate(tokens));
		}
	}

	SECTION("Test 2") {
		/* Input:
		* procedure procName {
		*	x = y * 3;
		* print x;
		* }
		* */
		
		ifstream input_file("C:/Users/yuxua/OneDrive/Documents/NUS/Y3S2/CS3203/21s2-cp-spa-team-39/Team39/Tests39/validation_tests/test2.txt");
		if (!input_file.is_open()) {
			cerr << "Could not open the file " << endl;
			RequireValidation(1 == 1);
		} else {
			string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

			Tokenizer tokenize;
			vector<Token> tokens = tokenize.parse(input);
			cout << endl;

			RequireValidation(Validate(tokens));
		}
	}

	SECTION("Test 3") {
		/*Input:
		* procedure procName {
		*	x = y + (5 + z);
		* }
		* */
		
		ifstream input_file("C:/Users/yuxua/OneDrive/Documents/NUS/Y3S2/CS3203/21s2-cp-spa-team-39/Team39/Tests39/validation_tests/test3.txt");
		if (!input_file.is_open()) {
			cerr << "Could not open the file " << endl;
			RequireValidation(1 == 1);
		} else {
			string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

			Tokenizer tokenize;
			vector<Token> tokens = tokenize.parse(input);
			cout << endl;

			RequireValidation(Validate(tokens));
		}	
	}
}

TEST_CASE("Invalid Programs") {

	/*Input:
	* procedre procName {
	*	x = y * 3;
	*	print x;
	* }
	* */

	ifstream input_file("C:/Users/yuxua/OneDrive/Documents/NUS/Y3S2/CS3203/21s2-cp-spa-team-39/Team39/Tests39/validation_tests/test4.txt");
	if (!input_file.is_open()) {
		cerr << "Could not open the file " << endl;
		RequireValidation(1 == 1);
	} else {
		string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

		Tokenizer tokenize;
		vector<Token> tokens = tokenize.parse(input);
		cout << endl;

		RequireValidation(!Validate(tokens));
	}

}
