#pragma once

#include <string>
#include <vector>

using namespace std;

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
    vector<int> stmt_num_;
    GraphNode* next_node_;
    GraphNode* alternative_node_;

};
