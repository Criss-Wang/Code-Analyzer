#include <string>
#include <vector>
#include <unordered_map>

#include "intertable.h"

namespace pql_solver {

  class Solver {
    public:
      std::vector<pql_table::InterTable> tables_;
      std::vector<pql_table::Predicate>* predicates_;
      std::vector<pql::AttrRef> return_syns_;
      bool is_return_boolean_;
      
    public:
      Solver(std::unordered_map<std::string, std::vector<int>>* domain,
             std::vector<pql_table::Predicate>* preds,
             std::vector<pql::Synonym>& syn_list, std::vector<pql::AttrRef>& selected_syns,
             bool is_return_boolean);
     
    public:
      int GetTableIndex(std::string& name);
      
      void Consume(pql_table::Predicate& pred);

      pql_table::InterTable ExtractResult();

      pql_table::InterTable Solve();
    
    private:
        std::vector<pql_table::InterTable> GetReturnTables();
  };

}