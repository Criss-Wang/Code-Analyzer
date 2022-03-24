#include "SP/parser.h"
#include "SP/validator.h"
#include "SP/sp_exceptions.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

void RequireValid(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    REQUIRE_NOTHROW(Parser(input));
    Parser parser(input);
    REQUIRE_NOTHROW(parser.Validate());
  }
}

void RequireInvalidSyntax(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    try {
      Parser parser(input);
      parser.Validate();
      REQUIRE(0 == 1);
    } catch (InvalidSyntaxException) {
      REQUIRE(1 == 1);
    }
  }
}

void RequireInvalidSemantic(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    REQUIRE_NOTHROW(Parser(input));
    Parser parser(input);
    REQUIRE_THROWS_AS(parser.Validate(), InvalidSemanticException);
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

    RequireInvalidSyntax(invalid_dir + "1_test1.txt");
    RequireInvalidSyntax(invalid_dir + "1_test2.txt");
    RequireInvalidSyntax(invalid_dir + "1_test3.txt");
    RequireInvalidSyntax(invalid_dir + "1_test4.txt");
    RequireInvalidSyntax(invalid_dir + "1_test5.txt");
    RequireInvalidSyntax(invalid_dir + "1_test6.txt");

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

    RequireInvalidSyntax(invalid_dir + "2_test1.txt");
    RequireInvalidSyntax(invalid_dir + "2_test2.txt");
    RequireInvalidSyntax(invalid_dir + "2_test3.txt");
    RequireInvalidSyntax(invalid_dir + "2_test4.txt");
    RequireInvalidSyntax(invalid_dir + "2_test5.txt");
    RequireInvalidSyntax(invalid_dir + "2_test6.txt");
    RequireInvalidSyntax(invalid_dir + "2_test7.txt");

  }

}

TEST_CASE("Read/print/assign/if/while statments (1 level nesting) for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "3_test1.txt");
    RequireValid(valid_dir + "3_test2.txt");
    RequireValid(valid_dir + "3_test3.txt");
    RequireValid(valid_dir + "3_test4.txt");

  }

  SECTION("Invalid Programs") {

    RequireInvalidSyntax(invalid_dir + "3_test1.txt");
    RequireInvalidSyntax(invalid_dir + "3_test2.txt");
    RequireInvalidSyntax(invalid_dir + "3_test3.txt");

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

TEST_CASE("Read/print/assign/call statments for Validation") {

  SECTION("Valid Programs") {

    RequireValid(valid_dir + "6_test1.txt");
    RequireValid(valid_dir + "6_test2.txt");
    RequireValid(valid_dir + "6_test3.txt");

  }

  SECTION("Invalid Programs") {

    RequireInvalidSemantic(invalid_dir + "6_test1.txt");
    RequireInvalidSemantic(invalid_dir + "6_test2.txt");
    RequireInvalidSemantic(invalid_dir + "6_test3.txt");

  }
}
