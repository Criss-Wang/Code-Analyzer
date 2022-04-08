#pragma once

#include <unordered_set>
#include <unordered_map>

#include "graph_node.h"
#include "cfg_token.h"

using namespace std;

namespace cfg {
  //Map valid statment types to EntityIdentifier
  const unordered_map<CFGTokenType, EntityIdentifier> NodeTypeToEntityIdentifierMap{
    {CFGTokenType::kAssign, EntityIdentifier::kAssign}, 
    {CFGTokenType::kRead, EntityIdentifier::kRead},
    {CFGTokenType::kPrint, EntityIdentifier::kPrint},
    {CFGTokenType::kCall, EntityIdentifier::kCall},
    {CFGTokenType::kWhile, EntityIdentifier::kWhile},
    {CFGTokenType::kIf, EntityIdentifier::kIf},
  };

  //This constructor will be called to create nodes without stmt number only
  GraphNode::GraphNode(NodeType type, int stmt_num) {
    type_ = type;
    start_ = stmt_num;
    end_ = stmt_num;
    next_node_ = nullptr;
    alternative_node_ = nullptr;
  }

  //This constructor will be called to create stmt nodes only
  GraphNode::GraphNode(CFGToken& token) {
    if (token.type_ == CFGTokenType::kIf) {
      type_ = NodeType::IF;
    } else if (token.type_ == CFGTokenType::kWhile) {
      type_ = NodeType::WHILE;
    } else {
      type_ = NodeType::STMT;
    }

    start_ = token.stmt_num_;
    end_ = token.stmt_num_;
    next_node_ = nullptr;
    alternative_node_ = nullptr;
    stmt_type_[token.stmt_num_] = NodeTypeToEntityIdentifierMap.at(token.type_);
  }

  //append the token to the curr node
  void GraphNode::append(CFGToken& token) {
    //Move the end by 1 and add the current statemnt type into stmt_type
    end_ = token.stmt_num_;
    stmt_type_[token.stmt_num_] = NodeTypeToEntityIdentifierMap.at(token.type_);
  }

  std::shared_ptr<GraphNode> GraphNode::GetNext() {
    return next_node_;
  }

  std::shared_ptr<GraphNode> GraphNode::GetAlternative() {
    return alternative_node_;
  }

  NodeType GraphNode::GetNodeType() {
    return type_;
  }

  int GraphNode::GetStart() {
    return start_;
  }

  int GraphNode::GetEnd() {
    return end_;
  }

  EntityIdentifier GraphNode::GetStmtType(int stmt_num) {
    return stmt_type_[stmt_num];
  }

  bool Helper(GraphNode& n1, GraphNode& n2, unordered_set<int>& visited) {
    bool is_attribute_equal = n1.type_ == n2.type_ && n1.start_ == n2.start_ && n1.end_ == n2.end_ && n1.stmt_type_ == n2.stmt_type_;
    //both must be nullptr or not nullptr at the same time
    bool is_next_node_same = (!n1.next_node_ && !n2.next_node_) || (n1.next_node_ && n2.next_node_); 
    bool is_alternative_node_same = (!n1.alternative_node_ && !n2.alternative_node_) || (n1.alternative_node_ && n2.alternative_node_);

    if (!is_attribute_equal || !is_next_node_same || !is_alternative_node_same) {
      return false;
    }

    visited.insert(n1.GetStart());

    if (n1.next_node_ && visited.find(n1.next_node_->GetStart()) == visited.end()) {
      if (!Helper(*n1.next_node_, *n2.next_node_, visited)) {
        return false;
      }
    }

    if (n1.alternative_node_ && visited.find(n1.alternative_node_->GetStart()) == visited.end()) {
      if (!Helper(*n1.alternative_node_, *n2.alternative_node_, visited)) {
        return false;
      }
    }

    return true;
  }

  bool GraphNode::equal(GraphNode& node) {
    //We use the start to represent the node, since each node will have a unique start_stmt
    unordered_set<int> visited;

    return Helper(*this, node, visited);
  }
}
