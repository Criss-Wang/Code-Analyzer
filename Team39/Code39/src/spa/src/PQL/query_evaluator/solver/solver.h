#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "intertable.h"
#include "../../query_parser/query.h"
#include "../cache/cache.h"
#include "../clause/clause.h"

namespace pql_solver {

  class Solver {
    public:
      pql::Query* query_;
      pql_cache::Cache* cache_;
      std::vector<pql_table::InterTable> tables_;
      
    public:
      Solver(pql::Query* query, pql_cache::Cache* cache);
     
    public:
      int GetTableIndex(std::string& name);
      
      void Consume(pql_table::Predicate& pred);

      pql_table::InterTable ExtractResult();

      pql_table::InterTable Solve();
    
    private:
        std::vector<pql_table::InterTable> GetReturnTables();
  };

  class Ufds {
    private:
      std::vector<int> parent_;
      std::vector<int> rank_;
      std::unordered_map<int, std::vector<std::shared_ptr<pql_clause::Clause>>> syn_to_clauses_map_;
      std::unordered_map<std::string, int> name_to_idx_map_;
      std::unordered_map<int, pql::Synonym*> idx_to_syn_map_;
      std::vector<pql::Synonym>* synonyms_;
      std::vector<std::shared_ptr<pql_clause::Clause>>* clauses_;
      std::vector<std::vector<pql::Synonym>> syn_groups_;
      std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>> clause_groups_;

    public:
      Ufds(std::vector<pql::Synonym>* synonyms, std::vector<std::shared_ptr<pql_clause::Clause>>* clauses);

      int Find(int idx);

      int Union(int i, int j);

      void Group();

      std::pair<std::vector<std::vector<pql::Synonym>>,
          std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>>> GetGroupings();
  };
}