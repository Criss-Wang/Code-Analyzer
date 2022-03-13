#pragma once

#include <string>
#include <vector>
#include <unordered_map>

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

    ~GraphNode() = default;

    GraphNode();

    GraphNode(NodeType type);

    void AddStmtNum(int stmt_num);

    void InsertNextNode(GraphNode* node);

    void InsertAlternativeNode(GraphNode* node);

    NodeType GetNodeType();

    GraphNode* GetNextNode();

    GraphNode* GetAlternativeNode();

  private:
    NodeType type_;
    int start_;
    int end_;
    std::unordered_map<> stmt_num_;
    GraphNode* next_node_;
    GraphNode* alternative_node_;

};
