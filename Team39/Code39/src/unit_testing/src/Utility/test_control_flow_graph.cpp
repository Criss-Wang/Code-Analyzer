#include "../../spa/src/Utility/CFG/control_flow_graph.h"
#include "../../spa/src/Utility/CFG/cfg_token.h"
#include "SP/tokenizer.h"
#include "SP/populator.h"

#include <fstream>
#include "catch.hpp"

using namespace std;

TEST_CASE("TEST GenerateCfg function") {
  vector<CFGToken> tokens({
    CFGToken(),
    CFGToken(CFGTokenType::kAssign, 1),
    CFGToken(CFGTokenType::kAssign, 2),
    CFGToken(CFGTokenType::kWhile, 3),
    CFGToken(CFGTokenType::kAssign, 4),
    CFGToken(CFGTokenType::kCall, 5),
    CFGToken(CFGTokenType::kAssign, 6),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kIf, 7),
    CFGToken(CFGTokenType::kAssign, 8),
    CFGToken(CFGTokenType::kThenEnd, 0),
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
    CFGToken(CFGTokenType::kIf, 20),
    CFGToken(CFGTokenType::kAssign, 21),
    CFGToken(CFGTokenType::kThenEnd, 0),
    CFGToken(CFGTokenType::kAssign, 22),
    CFGToken(CFGTokenType::kElseEnd, 0),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kEnd, 0),
  });

  shared_ptr<GraphNode> head = CFG::GenerateCfg(tokens1);

  REQUIRE(1 == 1);
}