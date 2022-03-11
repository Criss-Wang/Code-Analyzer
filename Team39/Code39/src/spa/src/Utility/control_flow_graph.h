#pragma once

#include <string>

#include "graph_node.h"

using namespace std;

class CFG {
  public:
    ~CFG() = default;

    CFG();

    CFG(string proc_name);

    GraphNode* GetStartNode();

  private:
    string proc_name_;
    GraphNode* start_node_;

};
