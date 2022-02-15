#include <string>
#include <vector>

#include "intertable.h"
#include "predicate.h"
#include "Token.h"

namespace pql_solver {

  class Solver {
    private:
      std::vector<pql_table::InterTable> tables_;
      std::vector<pql_table::Predicate> predicates_;
      std::vector<pql::Synonym> all_syn_
      pql::Synonym return_syn_;
      
    public:
      Solver()

    public:
      
  };

}