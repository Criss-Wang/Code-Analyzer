#pragma once

#include <stack>

#include "control_flow_graph.h"

using namespace std;

//CFG::CFG() {
//  this->proc_name_ = "";
//  this->start_node_ = nullptr;
//}
//
//CFG::CFG(string proc_name) {
//  this->proc_name_ = proc_name;
//  this->start_node_ = &GraphNode(NodeType::START);
//}
//
//
//GraphNode* CFG::GetStartNode() {
//  return this->start_node_;
//}
//
//CFG::~CFG() {
//  free(start_node_);
//}
//
//CFG::CFG(vector<CFGToken>& tokens) {
//  start_node_ = new GraphNode(NodeType::START);
//  GenerateCfg(start_node_, tokens);
//}

void Connect(shared_ptr<GraphNode>& source, shared_ptr<GraphNode>& dst) {
  if (!source->next_node_) {
    //add to the next_node if there isn't one yet
    source->next_node_ = dst;
  } else {
    source->alternative_node_ = dst;
  }
}

std::shared_ptr<GraphNode> CFG::GenerateCfg(vector<CFGToken>& tokens) {
  //We are guranteed that the size of tokens will be >= 3
  //Since it contains Start and End node, and stmtLst must contain at least one statement
  shared_ptr<GraphNode> head = make_shared<GraphNode>(NodeType::START);
  shared_ptr<GraphNode> curr = head;
  //stack stores the latest if/while node
  stack<shared_ptr<GraphNode>> stack;

  for (int index = 1; index < tokens.size(); index++) {

    //Need to create a new node when the previous node is If/While Node or
    //The current node is If/While Node
    if (tokens[index].type_ == CFGTokenType::kWhile) {
      shared_ptr<GraphNode> while_node = make_shared<GraphNode>(tokens[index]);
      shared_ptr<GraphNode> next_node = make_shared<GraphNode>(NodeType::WHILESTART);
      
      Connect(curr, while_node);
      Connect(while_node, next_node);
      stack.push(while_node);
      
      curr = next_node;

    } else if (tokens[index].type_ == CFGTokenType::kIf) {
      shared_ptr<GraphNode> if_node = make_shared<GraphNode>(tokens[index]);
      shared_ptr<GraphNode> next_node = make_shared<GraphNode>(NodeType::IFSTART);

      Connect(curr, if_node);
      Connect(if_node, next_node);
      stack.push(if_node);

      curr = next_node;
    } else if (tokens[index].type_ == CFGTokenType::kThenEnd) {
      //need to push the curr node to the stack
      //and move the ptr to the nearest if node
      shared_ptr<GraphNode> if_node = stack.top();
      stack.pop();
      stack.push(curr);
      curr = if_node;

    } else if (tokens[index].type_ == CFGTokenType::kWhileEnd) {
      shared_ptr<GraphNode> while_node = stack.top();
      stack.pop();

      shared_ptr<GraphNode> next_node = make_shared<GraphNode>(NodeType::WHILEEND);

      //points curr to while_node
      Connect(curr, next_node);
      Connect(next_node, while_node);
      curr = while_node;

    } else if (tokens[index].type_ == CFGTokenType::kElseEnd) {
      shared_ptr<GraphNode> then_node = stack.top();
      stack.pop();
      shared_ptr<GraphNode> next_node = make_shared<GraphNode>(NodeType::IFEND);
     
      //connect the last node in then stmtlst and else stmtlst to the next_node
      Connect(then_node, next_node);
      Connect(curr, next_node);
      curr = next_node;

    } else if (tokens[index].type_ == CFGTokenType::kEnd) {
      shared_ptr<GraphNode> end_node = make_shared<GraphNode>(NodeType::END);
      Connect(curr, end_node);
    } else {
      //curr node and the curr node can be join into the same node
      curr->append(tokens[index]);
    }
  }

  return head;
}
