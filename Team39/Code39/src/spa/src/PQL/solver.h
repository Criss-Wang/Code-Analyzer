#include <string>
#include <vector>
#include <unordered_map>

#include "intertable.h"
#include "predicate.h"
#include "Token.h"

namespace pql_solver {

  class Solver {
    private:
      std::vector<pql_table::InterTable> tables_;
      std::vector<pql_table::Predicate>* predicates_;
      pql::Synonym* return_syn_;
      
    public:
      Solver(std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
             std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
             std::vector<pql_table::Predicate>* preds,
             std::vector<pql::Synonym>& syn_list, pql::Synonym* selected_syn);
     
    public:
      int GetTableIndex(std::string& name);
      
      void Consume(pql_table::Predicate& pred);

      std::vector<std::string> ExtractResult();

      std::vector<std::string> Solve();
  };

}