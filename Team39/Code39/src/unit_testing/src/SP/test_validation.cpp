#include "parser.h"
#include "SP/validator.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

void RequireValid(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    Tokenizer tokenize;
    vector<Token> tokens = tokenize.parse(input);
    REQUIRE(!tokens.empty());
    REQUIRE(Validate(tokens));
  }
}

void RequireInvalid(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    Tokenizer tokenize;
    vector<Token> tokens = tokenize.parse(input);
    REQUIRE(!Validate(tokens));
  }
}

string valid_dir = "../../../../../../Tests39/sp/valid_programs/";
string invalid_dir = "../../../../../../Tests39/sp/invalid_programs/";

TEST_CASE("Read/print statements for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "1_test1.txt");
    RequireValid(valid_dir + "1_test2.txt");
    RequireValid(valid_dir + "1_test3.txt");
    RequireValid(valid_dir + "1_test4.txt");

  }

  SECTION("Invalid Programs") {

    RequireInvalid(invalid_dir + "1_test1.txt");
    RequireInvalid(invalid_dir + "1_test2.txt");
    RequireInvalid(invalid_dir + "1_test3.txt");
    RequireInvalid(invalid_dir + "1_test4.txt");
    RequireInvalid(invalid_dir + "1_test5.txt");
    RequireInvalid(invalid_dir + "1_test6.txt");

  }
}

TEST_CASE("Read/print/assign statments for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "2_test1.txt");
    RequireValid(valid_dir + "2_test2.txt");
    RequireValid(valid_dir + "2_test3.txt");
    RequireValid(valid_dir + "2_test4.txt");

  }

  SECTION("Invalid Programs") {

    RequireInvalid(invalid_dir + "2_test1.txt");
    RequireInvalid(invalid_dir + "2_test2.txt");
    RequireInvalid(invalid_dir + "2_test3.txt");
    RequireInvalid(invalid_dir + "2_test4.txt");
    RequireInvalid(invalid_dir + "2_test5.txt");
    RequireInvalid(invalid_dir + "2_test6.txt");

  }

}

TEST_CASE("Read/print/assign/if/while statments (1 level nesting) for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "3_test1.txt");
    RequireValid(valid_dir + "3_test2.txt");
    RequireValid(valid_dir + "3_test3.txt");
    RequireValid(valid_dir + "3_test4.txt");

  }
}

TEST_CASE("Read/print/assign/if/while statments (2 level nesting) for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "4_test1.txt");
    RequireValid(valid_dir + "4_test2.txt");

  }
}

TEST_CASE("Read/print/assign/if/while statments (3 level nesting) for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "5_test1.txt");

  }
}
