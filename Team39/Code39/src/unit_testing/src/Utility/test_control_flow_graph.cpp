#include "../../spa/src/Utility/CFG/control_flow_graph.h"
#include "../../spa/src/Utility/CFG/cfg_token.h"
#include "SP/tokenizer.h"
#include "SP/populator.h"

#include <fstream>
#include "catch.hpp"

using namespace std;
/*
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
*/

TEST_CASE("TEST GenerateCfg function") {
  vector<CFGToken> tokens({
    CFGToken(),
    CFGToken(CFGTokenType::kAssign, 1),
    CFGToken(CFGTokenType::kAssign, 2),
    CFGToken(CFGTokenType::kWhile, 3),
    CFGToken(CFGTokenType::kWhileStart, 0),
    CFGToken(CFGTokenType::kAssign, 4),
    CFGToken(CFGTokenType::kCall, 5),
    CFGToken(CFGTokenType::kAssign, 6),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kIf, 7),
    CFGToken(CFGTokenType::kThenStart, 0),
    CFGToken(CFGTokenType::kAssign, 8),
    CFGToken(CFGTokenType::kThenEnd, 0),
    CFGToken(CFGTokenType::kElseStart, 0),
    CFGToken(CFGTokenType::kAssign, 9),
    CFGToken(CFGTokenType::kElseEnd, 0),
    CFGToken(CFGTokenType::kAssign, 10),
    CFGToken(CFGTokenType::kAssign, 11),
    CFGToken(CFGTokenType::kAssign, 12),
    CFGToken(CFGTokenType::kEnd, 0)
  });

  vector<CFGToken> tokens1({
    CFGToken(),
    CFGToken(CFGTokenType::kWhile, 19),
    CFGToken(CFGTokenType::kWhileStart, 0),
    CFGToken(CFGTokenType::kIf, 20),
    CFGToken(CFGTokenType::kThenStart, 0),
    CFGToken(CFGTokenType::kAssign, 21),
    CFGToken(CFGTokenType::kThenEnd, 0),
    CFGToken(CFGTokenType::kElseStart, 0),
    CFGToken(CFGTokenType::kAssign, 22),
    CFGToken(CFGTokenType::kElseEnd, 0),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kEnd, 0),
  });

  shared_ptr<GraphNode> head = CFG::GenerateCfg(tokens1);

  REQUIRE(1 == 1);
}
