#include "PQL/parser.h"
#include "PQL/token.h"

#include <fstream>
#include "catch.hpp"

void RequireInvalid(std::string path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    CHECK_THROWS_AS(parser.Parse(), pql::ParseException);
  }
}

pql::Query GetQuery(std::string path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    parser.Parse();
    return parser.GetQuery();
  }
}

std::string valid_dir = "../../../../../../Tests39/pql/valid_queries/";
std::string invalid_dir = "../../../../../../Tests39/pql/invalid_queries/";

TEST_CASE("Invalid queries") {

  SECTION("With no Select clause") {
    RequireInvalid(invalid_dir + "1_test1.txt");
    RequireInvalid(invalid_dir + "1_test2.txt");
    RequireInvalid(invalid_dir + "1_test3.txt");
  }

  SECTION("With wrong spelling of keywords") {
    RequireInvalid(invalid_dir + "2_test4.txt");
    RequireInvalid(invalid_dir + "2_test5.txt");
    RequireInvalid(invalid_dir + "2_test6.txt");
    RequireInvalid(invalid_dir + "2_test7.txt");
  }

  SECTION("Select clause contains synonyms that are not declared") {
    RequireInvalid(invalid_dir + "3_test8.txt");
    RequireInvalid(invalid_dir + "3_test9.txt");
  }

  SECTION("With Modifies and Uses such that the first argument is wildcard") {
    RequireInvalid(invalid_dir + "4_test10.txt");
    RequireInvalid(invalid_dir + "4_test11.txt");
  }

}

TEST_CASE("Valid queries") {

  SECTION("With Select clause only") {
    pql::Query query = GetQuery(valid_dir + "1_test1.txt");
    REQUIRE(query.GetSuchThatClause().empty());
    REQUIRE(!query.GetPattern().has_value());
    REQUIRE(query.GetAllUsedSynonyms().size() == 1);
  }

  SECTION("With Select and such that clause") {
    pql::Query query = GetQuery(valid_dir + "2_test1.txt");
    REQUIRE(query.GetSuchThatClause().size() == 1);
    REQUIRE(!query.GetPattern().has_value());
    REQUIRE(query.GetAllUsedSynonyms().size() == 1);
  }

  SECTION("With Select and pattern clause") {
    pql::Query query = GetQuery(valid_dir + "3_test1.txt");
    REQUIRE(query.GetSuchThatClause().size() == 0);
    REQUIRE(query.GetPattern().has_value());
    REQUIRE(query.GetAllUsedSynonyms().size() == 2);
  }

  SECTION("With Select, such that and pattern clause") {
    pql::Query query = GetQuery(valid_dir + "4_test1.txt");
    REQUIRE(query.GetSuchThatClause().size() == 1);
    REQUIRE(query.GetPattern().has_value());
    REQUIRE(query.GetAllUsedSynonyms().size() == 3);
  }
}