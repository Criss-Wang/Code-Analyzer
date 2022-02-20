#include "PQL/parser.h"
#include "PQL/token.h"

#include <fstream>
#include "catch.hpp"

void RequireInvalidQuery(std::string path) {
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

void RequireValidQuery(std::string path, int such_that_clause_size, int pattern_clause_exist, int used_synonyms_size) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    parser.Parse();
    REQUIRE(parser.GetQuery().GetSuchThatClause().size() == such_that_clause_size);
    REQUIRE(parser.GetQuery().GetPattern().has_value() == pattern_clause_exist);
    REQUIRE(parser.GetQuery().GetAllUsedSynonyms().size() == used_synonyms_size);
  }
}

std::string valid_queries_dir = "../../../../../../Tests39/pql/valid_queries/";
std::string invalid_queries_dir = "../../../../../../Tests39/pql/invalid_queries/";

TEST_CASE("Invalid queries") {

  SECTION("With no Select clause") {
    RequireInvalidQuery(invalid_queries_dir + "1_test1.txt");
    RequireInvalidQuery(invalid_queries_dir + "1_test2.txt");
    RequireInvalidQuery(invalid_queries_dir + "1_test3.txt");
  }

  SECTION("With wrong spelling of keywords") {
    //RequireInvalidQuery(invalid_queries_dir + "2_test1.txt"); //Declaration keyword spelled wrongly
    RequireInvalidQuery(invalid_queries_dir + "2_test2.txt"); //First letter of Select keyword not in capital letter
    RequireInvalidQuery(invalid_queries_dir + "2_test3.txt"); //Missing space between such that
    RequireInvalidQuery(invalid_queries_dir + "2_test4.txt"); //Misspelled keyword for relationship
  }

  SECTION("Select clause contains synonyms that are not declared") {
    RequireInvalidQuery(invalid_queries_dir + "3_test1.txt");
    RequireInvalidQuery(invalid_queries_dir + "3_test2.txt");
  }

  SECTION("With Modifies and Uses such that the first argument is wildcard") {
    RequireInvalidQuery(invalid_queries_dir + "4_test1.txt"); //Modifies
    RequireInvalidQuery(invalid_queries_dir + "4_test2.txt"); //Uses
  }

  SECTION("With Modifies and Uses such that the second argument is not a variable") {
    RequireInvalidQuery(invalid_queries_dir + "5_test1.txt"); //Modifies(s, s1)
    RequireInvalidQuery(invalid_queries_dir + "5_test2.txt"); //Modifies(s, 3)
    RequireInvalidQuery(invalid_queries_dir + "5_test3.txt"); //Modifies(s, "10")
    RequireInvalidQuery(invalid_queries_dir + "5_test4.txt"); //Uses(s, s1)
    RequireInvalidQuery(invalid_queries_dir + "5_test5.txt"); //Uses(s, 3)
    RequireInvalidQuery(invalid_queries_dir + "5_test6.txt"); //Uses(s, "10")
  }

}
//
TEST_CASE("Valid queries") {

  SECTION("With Select clause only") {
    RequireValidQuery(valid_queries_dir + "1_test1.txt", 0, 0, 1);
  }

  SECTION("With Select and such that clause") {
    RequireValidQuery(valid_queries_dir + "2_test1.txt", 1, 0, 1); //Parent(1, 2)
    RequireValidQuery(valid_queries_dir + "2_test2.txt", 1, 0, 1); //Parent(1, _)
    RequireValidQuery(valid_queries_dir + "2_test3.txt", 1, 0, 1); //Parent(s, _)
    RequireValidQuery(valid_queries_dir + "2_test4.txt", 1, 0, 2); //Parent(s, s1)
    RequireValidQuery(valid_queries_dir + "2_test5.txt", 1, 0, 1); //Parent(_, s)
    RequireValidQuery(valid_queries_dir + "2_test6.txt", 1, 0, 1); //Parent(_, 9)
    RequireValidQuery(valid_queries_dir + "2_test7.txt", 1, 0, 1); //Parent(_, _)
    RequireValidQuery(valid_queries_dir + "2_test8.txt", 1, 0, 2); //Modifies(s, v)
  }

  SECTION("With Select and pattern clause") {
    RequireValidQuery(valid_queries_dir + "3_test1.txt", 0, 1, 2); //pattern a(_, _)
    RequireValidQuery(valid_queries_dir + "3_test2.txt", 0, 1, 1); //pattern a(_, "x")
    RequireValidQuery(valid_queries_dir + "3_test3.txt", 0, 1, 2); //pattern a(v, "1")
  }

  SECTION("With Select, such that and pattern clause") {
    RequireValidQuery(valid_queries_dir + "4_test1.txt", 1, 1, 3);
  }
}