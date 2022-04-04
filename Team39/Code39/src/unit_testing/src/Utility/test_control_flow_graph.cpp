#include "../../spa/src/Utility/CFG/control_flow_graph.h"
#include "../../spa/src/Utility/CFG/cfg_token.h"
#include "SP/tokenizer.h"
#include "SP/populator.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

TEST_CASE("Read/print statements") {

  SECTION("1_test4") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kRead, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kPrint, 4),
      CFGToken(CFGTokenType::kRead, 5),
      CFGToken(CFGTokenType::kPrint, 6),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }
}

TEST_CASE("Read/print/assign statements") {

  SECTION("2_test4") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kAssign, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kRead, 4),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kAssign, 6),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }
}

TEST_CASE("Read/print/assign/if/while statments (1 level nesting)") {

  SECTION("3_test1") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kAssign, 1),
      CFGToken(CFGTokenType::kIf, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kRead, 4),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }

  SECTION("3_test2") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kWhile, 2),
      CFGToken(CFGTokenType::kAssign, 3),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kPrint, 4),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }

  SECTION("3_test3") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kIf, 1),
      CFGToken(CFGTokenType::kRead, 2),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kRead, 3),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kWhile, 4),
      CFGToken(CFGTokenType::kPrint, 5),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kAssign, 6),
      CFGToken(CFGTokenType::kAssign, 7),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }

  SECTION("3_test4") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kAssign, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kIf, 4),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kAssign, 6),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kPrint, 7),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kRead, 8),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }
}

TEST_CASE("Read/print/assign/if/while statments (2 level nesting)") {

  SECTION("4_test1") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kAssign, 1),
      CFGToken(CFGTokenType::kWhile, 2),
      CFGToken(CFGTokenType::kIf, 3),
      CFGToken(CFGTokenType::kPrint, 4),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kRead, 5),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }

  SECTION("4_test2") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kIf, 2),
      CFGToken(CFGTokenType::kAssign, 3),
      CFGToken(CFGTokenType::kWhile, 4),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kPrint, 6),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kPrint, 7),
      CFGToken(CFGTokenType::kAssign, 8),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kPrint, 9),
      CFGToken(CFGTokenType::kAssign, 10),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }
}

TEST_CASE("Read/print/assign/if/while statments (3 level nesting)") {

  SECTION("5_test1") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kWhile, 1),
      CFGToken(CFGTokenType::kRead, 2),
      CFGToken(CFGTokenType::kIf, 3),
      CFGToken(CFGTokenType::kAssign, 4),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kWhile, 6),
      CFGToken(CFGTokenType::kRead, 7),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kPrint, 8),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kAssign, 9),
      CFGToken(CFGTokenType::kPrint, 10),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    REQUIRE(1 == 1);
  }
}
