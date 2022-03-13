#pragma once

#include <string>

#include "graph_node.h"
#include "cfg_token.h"

class CFG {
  public:
    ~CFG() = default;

    CFG();

    CFG(string proc_name);

    GraphNode* GetStartNode();

    GraphNode* GenerateCFG(std::vector<CFGToken> tokens);

  private:
    string proc_name_;
    GraphNode* start_node_;

};
