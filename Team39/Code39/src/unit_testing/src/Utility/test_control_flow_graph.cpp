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

  vector<CFGToken> tokens2({
    CFGToken(CFGTokenType::kStart, 0),
    CFGToken(CFGTokenType::kWhile, 1),
    CFGToken(CFGTokenType::kWhile, 2),
    CFGToken(CFGTokenType::kAssign, 3),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kEnd, 0),
  });

  vector<CFGToken> tokens3({
      CFGToken(),
      CFGToken(CFGTokenType::kAssign, 10),
      CFGToken(CFGTokenType::kAssign, 11),
      CFGToken(CFGTokenType::kAssign, 12),
      CFGToken(CFGTokenType::kCall, 13),
      CFGToken(CFGTokenType::kWhile, 14),
      CFGToken(CFGTokenType::kAssign, 15),
      CFGToken(CFGTokenType::kAssign, 16),
      CFGToken(CFGTokenType::kAssign, 17),
      CFGToken(CFGTokenType::kCall, 18),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kIf, 19),
      CFGToken(CFGTokenType::kAssign, 20),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kAssign, 21),
      CFGToken(CFGTokenType::kAssign, 22),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kAssign, 23),
      CFGToken(CFGTokenType::kPrint, 24),
      CFGToken(CFGTokenType::kPrint, 25),
      CFGToken(CFGTokenType::kEnd, 0)
   });

    shared_ptr<cfg::GraphNode> head = cfg::CFG::GenerateCfg(tokens3).GetHead();
}
