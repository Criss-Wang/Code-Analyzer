#include <string> 
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "query_evaluator.h"
#include "cache/cache.h"
#include "solver/solver.h"
#include "query_evaluator_exceptions.h"
#include "formatter/formatter.h"

namespace pql {
  std::vector<std::string> EvaluateQuery(Query* query, Pkb* pkb) {
    try {
      if (!query->IsSemanticallyValid()) {
        throw pql_exceptions::SemanticallyInvalidException();
      }
      
      std::vector<pql::AttrRef> selected_syns = query->GetAttrRef();
      pql_cache::Cache cache(pkb);

      pql_solver::Solver solver(query, &cache);
      pql_table::InterTable table = solver.Solve();
      pql_formatter::Formatter formatter(&cache);
      
      return formatter.FormatRawInput(table, selected_syns);

    } catch (pql_exceptions::TrueResultException& e) {
      return BOOLEAN_TRUE_LIST;
    } catch (pql_exceptions::EmptyResultException e) {      
      if (query->GetBoolean()) {
        return BOOLEAN_FALSE_LIST;
      }
      return EMPTY_LIST;
    }
  }
}