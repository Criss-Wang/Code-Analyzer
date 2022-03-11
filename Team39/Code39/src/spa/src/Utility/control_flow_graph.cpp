#pragma once

#include "control_flow_graph.h"

using namespace std;

CFG::CFG() {
  this->proc_name_ = "";
  this->start_node_ = nullptr;
}

CFG::CFG(string proc_name) {
  this->proc_name_ = proc_name;
  this->start_node_ = new GraphNode(NodeType::START);
}


GraphNode* CFG::GetStartNode() {
  return this->start_node_;
}
