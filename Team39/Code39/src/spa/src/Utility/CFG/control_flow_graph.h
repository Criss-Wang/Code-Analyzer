#pragma once

#include <string>
#include <memory>

#include "graph_node.h"
#include "cfg_token.h"

namespace cfg {
  class CFG {
    private:
      std::shared_ptr<GraphNode> start_node_;

    public:
      CFG(std::shared_ptr<GraphNode>& head);
 
      static CFG GenerateCfg(std::vector<CFGToken>& tokens);

      std::shared_ptr<GraphNode> GetHead();

      bool equal(CFG& cfg);
  };
}
