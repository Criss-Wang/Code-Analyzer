#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "clause.h"

namespace pql_clause {
  bool IsNumber(const std::string& str) {
    for (char const& c : str) {
      if (std::isdigit(c) == 0) {
        return false;
      }
    }
    return true;
  }
 
  Clause::Clause(pql::RelationshipToken* token) {
    token_ = token;
  }
}