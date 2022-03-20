#pragma once

#include <string>
#include <memory>

#include "graph_node.h"
#include "cfg_token.h"

class CFG {
  public:
    ~CFG() = default;

//    CFG();

//    CFG(string proc_name);

    //CFG(std::vector<CFGToken>& tokens);

    //GraphNode* GetStartNode();

    static std::shared_ptr<GraphNode> GenerateCfg(std::vector<CFGToken>& tokens);

  private:
    //string proc_name_;
    GraphNode* start_node_;

};
