# include "clause.h"

namespace pql_clause {
  class PatternClause : public Clause {
    public:
      PatternClause(pql::RelationshipToken* token) :
        Clause{ token } {}

     virtual ~PatternClause() = default;

    public:
      virtual void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) = 0;
  };

  class AssignPatternClause : virtual public PatternClause {
    public:
      AssignPatternClause(pql::RelationshipToken* token) :
        PatternClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };
}