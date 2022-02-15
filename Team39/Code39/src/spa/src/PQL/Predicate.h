#include <string>
#include <vector>

#include "Token.h"

namespace pql_table {
  
  class Predicate {
    public:
      pql::Synonym first_syn_;
      pql::Synonym second_syn_;
      std::vector<std::pair<pql_table::element, pql_table::element>> allowed_pairs_;

    public:
      Predicate(pql::Synonym& first, pql::Synonym& second, std::vector<std::pair<int, int>> pairs);

      Predicate(pql::Synonym& first, pql::Synonym& second, std::vector<std::pair<int, std::string>> pairs);
  };
}