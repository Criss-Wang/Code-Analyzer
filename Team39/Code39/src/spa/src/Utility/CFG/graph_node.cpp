#pragma once

#include <unordered_set>
#include <unordered_map>

#include "graph_node.h"
#include "cfg_token.h"

using namespace std;

namespace cfg {
  //Set of token type that does not have a statement number
  const unordered_set<NodeType> NodeTypeWithoutStmtNumSet = 
    { NodeType::START, NodeType::END, NodeType::IFEND, NodeType::THENEND, NodeType::WHILEEND };

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
  GraphNode::GraphNode(NodeType type) {
    /*if (NodeTypeWithoutStmtNumSet.find(type) == NodeTypeWithoutStmtNumSet.end()) {
      throw exception();
    }*/

    type_ = type;
    start_ = 0;
    end_ = 0;
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
}
