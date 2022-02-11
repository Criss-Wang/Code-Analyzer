#include <vector>

#include "Token.h"

namespace pql_table {
  
  class Predicate {
    public:
      pql::Synonym first_syn;
      pql::Synonym second_syn;
      std::vector<std::pair<element, element>> allowed_pairs;
  };
}