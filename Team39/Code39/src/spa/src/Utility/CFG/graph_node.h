#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../entity.h"
#include "cfg_token.h"

enum class NodeType {
  START,
  STMT,
  IF,
  WHILE,
  END,
  DUMMY
};

class GraphNode {
  public:
    NodeType type_;
    int start_;
    int end_;
    //stmt_type_ stores the entity type of the stmt within this node for Affect/* to quickly check the type
    std::unordered_map<int, EntityIdentifier> stmt_type_;
    std::shared_ptr<GraphNode> next_node_;
    std::shared_ptr<GraphNode> alternative_node_;

  public:
    ~GraphNode() = default;

    GraphNode();

    GraphNode(NodeType type);

    GraphNode(CFGToken token);

    void append(CFGToken& token);
};
