# include "clause.h"

namespace pql_clause {
  class PatternClause : public Clause {
    public:
      PatternClause(pql::RelationshipToken* token, Pkb& pkb,
        std::unordered_map<std::string, std::vector<int>>* domain,
        std::vector<pql_table::Predicate>* predicates) :
        Clause{ token, pkb, domain, predicates } {}

     virtual ~PatternClause() = default;

    public:
      virtual void Evaluate() = 0;
  };

  class AssignPatternClause : virtual public PatternClause {
    public:
      AssignPatternClause(pql::RelationshipToken* token, Pkb& pkb,
        std::unordered_map<std::string, std::vector<int>>* domain,
        std::vector<pql_table::Predicate>* predicates) :
        PatternClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };
}