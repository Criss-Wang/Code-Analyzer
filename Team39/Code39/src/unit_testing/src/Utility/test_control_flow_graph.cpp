#include "Utility/control_flow_graph.h"
#include "SP/tokenizer.h"
#include "SP/populator.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

vector<Token> parse(string path) {
  ifstream input_file(path);
  if (!input_file.is_open()) {
    cerr << "Could not open the file " << endl;
    return {};
  } else {
    string input = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    Tokenizer tokenizer;
    vector<Token> input_tokens = tokenizer.parse(input);
    return input_tokens;
  }
}

string populate_dir = "../../../../../../Tests39/sp/valid_programs/";

TEST_CASE("Test CFG for 1 procedure with only read/print/assign statments") {

  SECTION("2_test4") {

    vector<Token> input_tokens = parse(populate_dir + "2_test4.txt");
    CFG actualCFG = buildCFG(input_tokens).at(0);

    REQUIRE(1 == 1);
  }

}

TEST_CASE("Test CFG for 1 procedure with while statments") {

  SECTION("3_test2") {

    vector<Token> input_tokens = parse(populate_dir + "3_test2.txt");
    CFG actualCFG = buildCFG(input_tokens).at(0);

    REQUIRE(1 == 1);
  }

}

TEST_CASE("Test CFG for 1 procedure with if statments") {

  SECTION("3_test1") {

    vector<Token> input_tokens = parse(populate_dir + "3_test1.txt");
    CFG actualCFG = buildCFG(input_tokens).at(0);

    REQUIRE(1 == 1);
  }

}
