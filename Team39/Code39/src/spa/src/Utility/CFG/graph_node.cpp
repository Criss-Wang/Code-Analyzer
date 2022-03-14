#pragma once

#include <unordered_set>
#include <unordered_map>

#include "graph_node.h"
#include "cfg_token.h"

using namespace std;

//Set of token type that does not have a statement number
const unordered_set<NodeType> NodeTypeWithoutStmtNumSet{ NodeType::START, NodeType::END, NodeType::DUMMY };

//Map valid statment types to EntityIdentifier
const unordered_map<CFGTokenType, EntityIdentifier> NodeTypeToEntityIdentifierMap{
  {CFGTokenType::kAssign, EntityIdentifier::kAssign}, 
  {CFGTokenType::kRead, EntityIdentifier::kRead},
  {CFGTokenType::kPrint, EntityIdentifier::kPrint},
  {CFGTokenType::kCall, EntityIdentifier::kCall},
  {CFGTokenType::kWhile, EntityIdentifier::kWhile},
  {CFGTokenType::kIf, EntityIdentifier::kIf},
};


GraphNode::GraphNode() {
 /* this->type_ = NodeType::DUMMY;
  this->stmt_num_ = {};
  this->next_node_ = nullptr;
  this->alternative_node_ = nullptr;*/
}

//This constructor will be called to create Start, Dummy and End node only
GraphNode::GraphNode(NodeType type) {
  //prevent NodeType different than Start/Dummy/End to create a node
  if (NodeTypeWithoutStmtNumSet.find(type) == NodeTypeWithoutStmtNumSet.end()) {
    throw exception();
  }

  type_ = type;
  start_ = 0;
  end_ = 0;
  next_node_ = nullptr;
  alternative_node_ = nullptr;
}

//This constructor will be called to create stmt nodes only
GraphNode::GraphNode(CFGToken token) {
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
