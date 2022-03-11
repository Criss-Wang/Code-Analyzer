#pragma once
#include "tokenizer.h"
#include "PKB/pkb.h"
#include "Utility/control_flow_graph.h"

using namespace std;

void populate(vector<Token> inputTokens, Pkb& pkb);

vector<CFG> buildCFG(vector<Token> inputTokens);