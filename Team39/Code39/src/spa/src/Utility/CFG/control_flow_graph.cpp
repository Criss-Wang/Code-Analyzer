#pragma once

#include <stack>

#include "control_flow_graph.h"

using namespace std;

namespace cfg {
  CFG::CFG(shared_ptr<GraphNode> head) {
    start_node_ = head;
  }

  shared_ptr<GraphNode> CFG::GetHead() {
    return start_node_;
  }

  void Connect(shared_ptr<GraphNode>& source, shared_ptr<GraphNode>& dst) {
    if (!source->next_node_) {
      //add to the next_node if there isn't one yet
      source->next_node_ = dst;
    } else {
      source->alternative_node_ = dst;
    }
  }

  typedef void (*GenerateCfgForToken)(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token);

  void GenerateCfgForIf(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    shared_ptr<GraphNode> if_node = make_shared<GraphNode>(token);
    
    Connect(curr, if_node);
    stack.push(if_node);
    curr = if_node;
  }

  void GenerateCfgForWhile(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    shared_ptr<GraphNode> while_node = make_shared<GraphNode>(token);
    
    Connect(curr, while_node);
    stack.push(while_node);
    curr = while_node;
  }

  void GenerateCfgForThenEnd(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    shared_ptr<GraphNode> if_node = stack.top();
    stack.pop();
    shared_ptr<GraphNode> then_end_node = make_shared<GraphNode>(NodeType::THENEND, 0);

    Connect(curr, then_end_node);
    //The then_end_node is pushed to the stack because will need to connect it with if_end_node when else branch is done
    stack.push(then_end_node);

    //We move the curr to the nearest if node to start constructing its else branch
    curr = if_node;
  }

  void GenerateCfgForWhileEnd(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    shared_ptr<GraphNode> while_node = stack.top();
    stack.pop();

    shared_ptr<GraphNode> next_node = make_shared<GraphNode>(NodeType::WHILEEND, 0);

    //points curr to while_node
    Connect(curr, next_node);
    Connect(next_node, while_node);
    curr = while_node;
  }

  void GenerateCfgForElseEnd(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    shared_ptr<GraphNode> then_node = stack.top();
    stack.pop();
    shared_ptr<GraphNode> next_node = make_shared<GraphNode>(NodeType::IFEND, 0);

    //connect the last node in then stmtlst and else stmtlst to the next_node
    Connect(then_node, next_node);
    Connect(curr, next_node);
    curr = next_node;
  }

  void GenerateCfgForEnd(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    shared_ptr<GraphNode> end_node = make_shared<GraphNode>(NodeType::END, 0);
    Connect(curr, end_node);
  }

  void GenerateCfgForStmt(shared_ptr<GraphNode>& curr, stack<shared_ptr<GraphNode>>& stack, CFGToken& token) {
    if (curr->GetNodeType() != NodeType::STMT) {
      //The token type can only be stmt node up to this point
      //need to create a new node if the curr is a while/if node or at ifend/while end
      shared_ptr<GraphNode> new_node = make_shared<GraphNode>(token);
      Connect(curr, new_node);
      curr = new_node;
    } else {
      //curr node and the curr node can be join into the same node
      curr->append(token);
    }
  }

  const unordered_map<CFGTokenType, GenerateCfgForToken> GenerateCfgForTokenMap = {
    { CFGTokenType::kWhile, &GenerateCfgForWhile },
    { CFGTokenType::kIf, &GenerateCfgForIf },
    { CFGTokenType::kThenEnd, &GenerateCfgForThenEnd },
    { CFGTokenType::kWhileEnd, &GenerateCfgForWhileEnd },
    { CFGTokenType::kElseEnd, &GenerateCfgForElseEnd },
    { CFGTokenType::kEnd, &GenerateCfgForEnd },
    { CFGTokenType::kAssign, &GenerateCfgForStmt },
    { CFGTokenType::kCall, &GenerateCfgForStmt },
    { CFGTokenType::kRead, &GenerateCfgForStmt },
    { CFGTokenType::kPrint, &GenerateCfgForStmt },
  };

  CFG CFG::GenerateCfg(vector<CFGToken>& tokens) {
    //We are guranteed that the size of tokens will be >= 3
    //Since it contains Start and End node, and stmtLst must contain at least one statement
    shared_ptr<GraphNode> head = make_shared<GraphNode>(NodeType::START, 0);
    shared_ptr<GraphNode> curr = head;
    //stack stores the latest if/while node
    stack<shared_ptr<GraphNode>> stack;

    for (int index = 1; index < tokens.size(); index++) {
      GenerateCfgForToken fn = GenerateCfgForTokenMap.at(tokens[index].type_);
      (*fn)(curr, stack, tokens[index]);
    }

    return CFG(head);
  }

  bool CFG::equal(CFG& cfg) {
    return start_node_->equal(*cfg.start_node_);
  }
}
