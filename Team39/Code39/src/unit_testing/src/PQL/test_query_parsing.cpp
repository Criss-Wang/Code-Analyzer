#include "PQL/query_parser/parser.h"
#include "PQL/query_parser/token.h"

#include <fstream>
#include "catch.hpp"

void RequireSyntaxInvalidQuery(std::string path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    parser.Parse();
    CHECK_THROWS_AS(parser.Parse(), pql::ParseException);
  }
}

void RequireSemanticsInvalidQuery(std::string path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    parser.Parse();
    REQUIRE(!parser.GetQuery().IsSemanticallyValid());
  }
}

pql::Query GetQuery(std::string path) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
    return {};
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    parser.Parse();
    return parser.GetQuery();
  }
}

void RequireValidQuery(std::string path, int result_attr_ref_size, int clause_size, int used_synonyms_size) {
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cerr << "Could not open the file " << std::endl;
  } else {
    std::string query = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    pql::Parser parser = pql::Parser(query);
    parser.Parse();
    REQUIRE(parser.GetQuery().GetAttrRef().size() == result_attr_ref_size);
    REQUIRE(parser.GetQuery().GetClauses().size() == clause_size);
    REQUIRE(parser.GetQuery().GetAllUsedSynonyms().size() == used_synonyms_size);
  }
}

std::string valid_queries_dir = "../../../../../../Tests39/pql/valid_queries/";
std::string invalid_queries_dir = "../../../../../../Tests39/pql/invalid_queries/";

TEST_CASE("Invalid queries") {

  SECTION("With no Select clause") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "1_test1.txt");
    RequireSyntaxInvalidQuery(invalid_queries_dir + "1_test2.txt");
    RequireSyntaxInvalidQuery(invalid_queries_dir + "1_test3.txt");
  }


  SECTION("With wrong spelling of keywords") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "2_test1.txt"); //Declaration keyword spelled wrongly
    RequireSyntaxInvalidQuery(invalid_queries_dir + "2_test2.txt"); //First letter of Select keyword not in capital letter
    RequireSyntaxInvalidQuery(invalid_queries_dir + "2_test3.txt"); //Missing space between such that
    RequireSyntaxInvalidQuery(invalid_queries_dir + "2_test4.txt"); //Misspelled keyword for relationship
  }

  SECTION("Select clause contains synonyms that are not declared") {
    RequireSemanticsInvalidQuery(invalid_queries_dir + "3_test1.txt");
    RequireSemanticsInvalidQuery(invalid_queries_dir + "3_test2.txt");
  }

  SECTION("With Modifies and Uses such that the first argument is wildcard") {
    RequireSemanticsInvalidQuery(invalid_queries_dir + "4_test1.txt"); //Modifies
    RequireSemanticsInvalidQuery(invalid_queries_dir + "4_test2.txt"); //Uses
  }

  SECTION("With Modifies and Uses such that the second argument is not a variable") {
    RequireSemanticsInvalidQuery(invalid_queries_dir + "5_test1.txt"); //Modifies(s, s1)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "5_test2.txt"); //Modifies(s, 3)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "5_test3.txt"); //Modifies(s, "10")
    RequireSemanticsInvalidQuery(invalid_queries_dir + "5_test4.txt"); //Uses(s, s1)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "5_test5.txt"); //Uses(s, 3)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "5_test6.txt"); //Uses(s, "10")
  }

  SECTION("Assign Pattern clause such that the second argument is not a valid expression") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "6_test1.txt"); //pattern a(_, "x +")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "6_test2.txt"); //pattern a(_, "x (3 + 3)")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "6_test3.txt"); //pattern a(_, "(3 + y / 3")
  }

  SECTION("Select clause such that invalid attributes are used") {
    RequireSemanticsInvalidQuery(invalid_queries_dir + "7_test1.txt"); //stmt.procName
    RequireSyntaxInvalidQuery(invalid_queries_dir + "7_test2.txt"); //proc.Procname
    RequireSemanticsInvalidQuery(invalid_queries_dir + "7_test3.txt"); //read.value
    RequireSyntaxInvalidQuery(invalid_queries_dir + "7_test4.txt"); //variable.var
    RequireSemanticsInvalidQuery(invalid_queries_dir + "7_test5.txt"); //<r.varName, r.value>
    RequireSyntaxInvalidQuery(invalid_queries_dir + "7_test6.txt"); //<ifs.stmt#, w.stmt #>
  }

  SECTION("Repeated synonym names declared") {
    RequireSemanticsInvalidQuery(invalid_queries_dir + "8_test1.txt"); //assign a, a;
    RequireSemanticsInvalidQuery(invalid_queries_dir + "8_test2.txt"); //assign a; if a;
  }

  SECTION("Syntactically invalid arguments in Follows/Follows*") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "9_test1.txt"); //Follows("a", a)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "9_test2.txt"); //Follows(a, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "9_test3.txt"); //Follows("a", "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "9_test4.txt"); //Follows*("a", a)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "9_test5.txt"); //Follows*(a, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "9_test6.txt"); //Follows*("a", "a")
  }

  SECTION("Syntactically invalid arguments in Parent/Parent*") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "10_test1.txt"); //Parent("a", a)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "10_test2.txt"); //Parent(ifs, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "10_test3.txt"); //Parent("ifs", "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "10_test4.txt"); //Parent*("a", a)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "10_test5.txt"); //Parent*(ifs, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "10_test6.txt"); //Parent*("ifs", "a")
  }

  SECTION("Syntactically invalid second arguments in Uses") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "11_test1.txt"); //Uses(s, 1)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "11_test2.txt"); //Uses(p, 1)
  }

  SECTION("Syntactically invalid second arguments in Modifies") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "12_test1.txt"); //Modifies(s, 1)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "12_test2.txt"); //Modifies(p, 1)
  }

  SECTION("Syntactically invalid arguments in Calls/Calls*") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "13_test1.txt"); //Calls(p, 1)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "13_test2.txt"); //Calls(1, p)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "13_test3.txt"); //Calls(1, 2)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "13_test4.txt"); //Calls*(p, 1)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "13_test5.txt"); //Calls*(1, p)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "13_test6.txt"); //Calls*(1, 2)
  }

  SECTION("Syntactically invalid arguments in Next/Next*") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "14_test1.txt"); //Next(s, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "14_test2.txt"); //Next("a", s)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "14_test3.txt"); //Next("a", "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "14_test4.txt"); //Next*(s, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "14_test5.txt"); //Next*("a", s)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "14_test6.txt"); //Next*("a", "a")
  }

  SECTION("Syntactically invalid arguments in Affects/Affects*") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "15_test1.txt"); //Affects(a, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "15_test2.txt"); //Affects("a", a)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "15_test3.txt"); //Affects("a", "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "15_test4.txt"); //Affects*(a, "a")
    RequireSyntaxInvalidQuery(invalid_queries_dir + "15_test5.txt"); //Affects*("a", a)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "15_test6.txt"); //Affects*("a", "a")
  }

  SECTION("Syntactically invalid first argument in pattern clauses") {
    RequireSyntaxInvalidQuery(invalid_queries_dir + "16_test1.txt"); //pattern a(1, _)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "16_test2.txt"); //pattern ifs(1, _, _)
    RequireSyntaxInvalidQuery(invalid_queries_dir + "16_test3.txt"); //pattern w(1, _)
  }

  SECTION("Semantically invalid pattern synonym") {
    RequireSemanticsInvalidQuery(invalid_queries_dir + "17_test1.txt"); //pattern p(_, _)
    RequireSemanticsInvalidQuery(invalid_queries_dir + "17_test2.txt"); //pattern p(_, _, _)
  }
}

TEST_CASE("Valid queries") {

  SECTION("With Select clause only") {
    RequireValidQuery(valid_queries_dir + "1_test1.txt", 1, 0, 1);
  }

  SECTION("With Select and such that clause") {
    RequireValidQuery(valid_queries_dir + "2_test1.txt", 1, 1, 1); //Parent(1, 2)
    RequireValidQuery(valid_queries_dir + "2_test2.txt", 1, 1, 1); //Parent(1, _)
    RequireValidQuery(valid_queries_dir + "2_test3.txt", 1, 1, 1); //Parent(s, _)
    RequireValidQuery(valid_queries_dir + "2_test4.txt", 1, 1, 2); //Parent(s, s1)
    RequireValidQuery(valid_queries_dir + "2_test5.txt", 1, 1, 1); //Parent(_, s)
    RequireValidQuery(valid_queries_dir + "2_test6.txt", 1, 1, 1); //Parent(_, 9)
    RequireValidQuery(valid_queries_dir + "2_test7.txt", 1, 1, 1); //Parent(_, _)
    RequireValidQuery(valid_queries_dir + "2_test8.txt", 1, 1, 2); //Modifies(s, v)
  }

  SECTION("With Select and pattern clause") {
    RequireValidQuery(valid_queries_dir + "3_test1.txt", 1, 1, 2); //pattern a(_, _)
    RequireValidQuery(valid_queries_dir + "3_test2.txt", 1, 1, 1); //pattern a(_, "x")
    RequireValidQuery(valid_queries_dir + "3_test3.txt", 1, 1, 2); //pattern a(v, "1")
  }

  SECTION("With Select, such that and pattern clause") {
    RequireValidQuery(valid_queries_dir + "4_test1.txt", 1, 2, 3);
  }

  SECTION("With Select and pattern clause with expression") {
    RequireValidQuery(valid_queries_dir + "5_test1.txt", 1, 1, 2); //pattern a(_, _"x + 1"_)
    RequireValidQuery(valid_queries_dir + "5_test2.txt", 1, 1, 2); //pattern a(_, "(3 % 4) / x")
    RequireValidQuery(valid_queries_dir + "5_test3.txt", 1, 1, 3); //pattern a(v, "x+1*2")
  }

  SECTION("With Select, such that and pattern clause with expression") {
    RequireValidQuery(valid_queries_dir + "5_test4.txt", 1, 2, 3);
  }

  SECTION("With Select clause with synonym and attribute") {
    RequireValidQuery(valid_queries_dir + "6_test1.txt", 1, 0, 1); //Select p.procName
    RequireValidQuery(valid_queries_dir + "6_test2.txt", 1, 1, 1); //Select a.stmt#
    RequireValidQuery(valid_queries_dir + "6_test3.txt", 1, 2, 3); //Select v.varName
    RequireValidQuery(valid_queries_dir + "6_test4.txt", 1, 2, 3); //Select c.value
  }

  SECTION("With Select tuple and attribute") {
    RequireValidQuery(valid_queries_dir + "6_test5.txt", 2, 0, 2); //<c.value, s.stmt#>
    RequireValidQuery(valid_queries_dir + "6_test6.txt", 2, 1, 3); //<v.varName, pr.procName>
    RequireValidQuery(valid_queries_dir + "6_test7.txt", 2, 0, 1); //<r.stmt#, r .varName>
    RequireValidQuery(valid_queries_dir + "6_test8.txt", 3, 0, 2); //<p, p.varName, pr>
  }

  SECTION("With Select and with clause") {
    RequireValidQuery(valid_queries_dir + "7_test1.txt", 1, 1, 1); //s.stmt# = 1
    RequireValidQuery(valid_queries_dir + "7_test2.txt", 1, 1, 1); //p.procName = "proc"
    RequireValidQuery(valid_queries_dir + "7_test3.txt", 1, 1, 1); //v.varName = "var"
    RequireValidQuery(valid_queries_dir + "7_test4.txt", 1, 1, 1); //c.value = 3
  }

  SECTION("Implicit and") {
    RequireValidQuery(valid_queries_dir + "8_test1.txt", 1, 2, 1);
    RequireValidQuery(valid_queries_dir + "8_test2.txt", 2, 3, 2);
    RequireValidQuery(valid_queries_dir + "8_test3.txt", 1, 2, 2);
    RequireValidQuery(valid_queries_dir + "8_test4.txt", 2, 2, 2);
    RequireValidQuery(valid_queries_dir + "8_test5.txt", 2, 2, 2);
  }

  SECTION("Explicit and") {
    RequireValidQuery(valid_queries_dir + "9_test1.txt", 1, 2, 1);
    RequireValidQuery(valid_queries_dir + "9_test2.txt", 2, 3, 2);
    RequireValidQuery(valid_queries_dir + "9_test3.txt", 1, 2, 2);
    RequireValidQuery(valid_queries_dir + "9_test4.txt", 2, 2, 2);
    RequireValidQuery(valid_queries_dir + "9_test5.txt", 2, 2, 2);
  }
}
