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
  //prevent NodeType different than Start/End to create a node
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
GraphNode::GraphNode(CFGTokenType token_type, int stmt_num) {
  if (token_type == CFGTokenType::kIf) {
    type_ = NodeType::IF;
  } else if (token_type == CFGTokenType::kWhile) {
    type_ = NodeType::WHILE;
  } else {
    type_ = NodeType::STMT;
  }

  start_ = stmt_num;
  end_ = stmt_num;
  next_node_ = nullptr;
  alternative_node_ = nullptr;
  stmt_type_[stmt_num] = NodeTypeToEntityIdentifierMap.at(token_type);
}
