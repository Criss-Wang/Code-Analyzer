#pragma once

#include <stack>

#include "control_flow_graph.h"

using namespace std;

CFG::CFG() {
  this->proc_name_ = "";
  this->start_node_ = nullptr;
}

CFG::CFG(string proc_name) {
  this->proc_name_ = proc_name;
  this->start_node_ = &GraphNode(NodeType::START);
}


GraphNode* CFG::GetStartNode() {
  return this->start_node_;
}

GraphNode* CFG::GenerateCFG(vector<CFGToken> tokens) {
  //We are guranteed that the size of tokens will be >= 3
  //Since it contains Start and End node, and stmtLst must contain at least one statement
  GraphNode head = GraphNode(NodeType::START);
  head.next_node_ = &GraphNode(tokens[1].type_, tokens[1].stmt_num_);
  GraphNode curr = *head.next_node_;

  //stack stores the latest if/while node
  stack<GraphNode> stack;

  int index = 2;

  while (index < tokens.size()) {
    if (tokens[index].type_ == CFGTokenType::)
  }

  return &head;
}
