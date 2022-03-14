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
  shared_ptr<GraphNode> first_node = make_shared<GraphNode>(tokens[1]);
  head->next_node_ = first_node;
  shared_ptr<GraphNode> curr = first_node;

  //stack stores the latest if/while node
  stack<shared_ptr<GraphNode>> stack;

  if (tokens[1].type_ == CFGTokenType::kIf || tokens[1].type_ == CFGTokenType::kWhile) {
    stack.push(curr);
  }

  int index = 2;

  while (index < tokens.size()) {

    if (tokens[index].type_ == CFGTokenType::kWhileStart
        || tokens[index].type_ == CFGTokenType::kThenStart
        || tokens[index].type_ == CFGTokenType::kElseStart) {
      index++;
      continue;
    }

    //Need to create a new node when the previous node is If/While Node or
    //The current node is If/While Node
    if (curr->type_ == NodeType::IF || curr->type_ == NodeType::WHILE ||
        tokens[index].type_ == CFGTokenType::kWhile || tokens[index].type_ == CFGTokenType::kIf) {
      shared_ptr<GraphNode> new_node = tokens[index].type_ == CFGTokenType::kEnd ? make_shared<GraphNode>(NodeType::END)
                                                                                 : make_shared<GraphNode>(tokens[index]);
      Connect(curr, new_node);

      if (tokens[index].type_ == CFGTokenType::kWhile || tokens[index].type_ == CFGTokenType::kIf) {
        stack.push(new_node);
      }

      curr = new_node;

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
      //points curr to while_node
      Connect(curr, while_node);
      curr = while_node;

    } else if (tokens[index].type_ == CFGTokenType::kElseEnd) {
      shared_ptr<GraphNode> then_node = stack.top();
      stack.pop();
      shared_ptr<GraphNode> next_node;
      int next_index = index + 1;

      if (tokens[next_index].type_ == CFGTokenType::kThenEnd
          || tokens[next_index].type_ == CFGTokenType::kWhileEnd
          || tokens[next_index].type_ == CFGTokenType::kElseEnd) {
        next_node = make_shared<GraphNode>(NodeType::DUMMY);
      } else if (tokens[next_index].type_ == CFGTokenType::kEnd) {
        next_node = make_shared<GraphNode>(NodeType::END);
        index++;
      } else {
        next_node = make_shared<GraphNode>(tokens[next_index]);
        index++;
      }

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

    index++;
  }

  return head;
}
