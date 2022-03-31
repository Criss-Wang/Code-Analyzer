#include <string>
#include <vector>
#include <unordered_map>

namespace pql_solver {
  class Ufds {
    private:
      std::vector<int> parent_;
      std::unordered_map<std::string, int> name_to_idx_map_;


  };
}