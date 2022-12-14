#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../entity.h"
#include "cfg_token.h"

namespace cfg {
  enum class NodeType {
    START,
    STMT,
    IF,
    WHILE,
    END,
    THENEND,
    IFEND,
    WHILEEND
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

      GraphNode(NodeType type, int stmt_num);

      GraphNode(CFGToken& token);
 
      void append(CFGToken& token);

      std::shared_ptr<GraphNode> GetNext();

      std::shared_ptr<GraphNode> GetAlternative();

      NodeType GetNodeType();

      int GetStart();

      int GetEnd();

      EntityIdentifier GetStmtType(int stmt_num);

      bool equal(GraphNode& node);
  };
}

