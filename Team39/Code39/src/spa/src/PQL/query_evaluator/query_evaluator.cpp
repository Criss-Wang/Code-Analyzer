#include <stdio.h>
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "query_evaluator.h"
#include "cache/cache.h"
#include "solver/solver.h"
#include "clause/clause.h"
#include "query_evaluator_exceptions.h"
#include "formatter/formatter.h"

namespace pql {
  void GetAllDomain(std::vector<pql::Synonym>& synonyms, std::unordered_map<std::string, std::vector<int>>& domain, Pkb& pkb) {
    //domain stores <synonym.name, domain> pair.
    for (pql::Synonym& synonym : synonyms) {
      std::unordered_set<int> domain_set = pkb.GetAllEntity(synonym.GetDeclaration());
      std::vector<int> domain_list(std::begin(domain_set), std::end(domain_set));
      domain.insert({ synonym.GetName(), domain_list });
    }
  }

  std::vector<std::string> EvaluateQuery(Query* query, Pkb* pkb) {
    try {
      if (!query->IsSemanticallyValid()) {
        throw pql_exceptions::SemanticallyInvalidException();
      }

      /*bool is_return_boolean = query.GetBoolean();
      std::vector<shared_ptr<pql_clause::Clause>> clauses = query.GetClauses();
      std::vector<pql::Synonym> synonyms = query.GetAllUsedSynonyms();
      std::vector<pql_table::Predicate> predicates;
      std::unordered_map<std::string, std::vector<int>> domain;*/

      /*GetAllDomain(synonyms, domain, *pkb);

      for (auto& clause : clauses) {
        clause->Evaluate(cache, domain, predicates);
      }*/
      
      std::vector<pql::AttrRef> selected_syns = query->GetAttrRef();

      pql_cache::Cache cache(pkb);

      pql_solver::Solver solver(query);
      pql_table::InterTable table = solver.Solve();
      pql_formatter::Formatter formatter(&cache);
      
      return formatter.FormatRawInput(table, selected_syns);

    } catch (pql_exceptions::TrueResultException e) {
      return BOOLEAN_TRUE_LIST;
    } catch (pql_exceptions::EmptyResultException e) {      
      if (query->GetBoolean()) {
        return BOOLEAN_FALSE_LIST;
      }
      return EMPTY_LIST;
    }
  }
}
