#include <string>
#include <vector>
#include <unordered_map>

#include "intertable.h"
#include "../../query_parser/query.h"

namespace pql_solver {

  class Solver {
    public:
      pql::Query* query_;
      std::vector<pql_table::InterTable> tables_;
      
    public:
      Solver(pql::Query* query);
     
    public:
      int GetTableIndex(std::string& name);
      
      void Consume(pql_table::Predicate& pred);

      pql_table::InterTable ExtractResult();

      pql_table::InterTable Solve();
    
    private:
        std::vector<pql_table::InterTable> GetReturnTables();
  };

}