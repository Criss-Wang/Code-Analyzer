#pragma once

#include <typeinfo>
#include <iostream>

#include "graph_node.h"

using namespace std;

GraphNode::GraphNode() {
	this->type_ = NodeType::DUMMY;
	this->stmt_num_ = {};
	this->next_node_ = nullptr;
	this->alternative_node_ = nullptr;
}

GraphNode::GraphNode(NodeType type) {
	this->type_ = type;
	this->stmt_num_ = {};
	this->next_node_ = nullptr;
	this->alternative_node_ = nullptr;
}

void GraphNode::AddStmtNum(int stmt_num) {
	this->stmt_num_.push_back(stmt_num);
}

void GraphNode::InsertNextNode(GraphNode* node) {
  this->next_node_ = node;
}

void GraphNode::InsertAlternativeNode(GraphNode* node) {
	this->alternative_node_ = node;
}

NodeType GraphNode::GetNodeType() {
	return this->type_;
}

GraphNode* GraphNode::GetNextNode() {
	return this->next_node_;
}

GraphNode* GraphNode::GetAlternativeNode() {
	return this->alternative_node_;
}
