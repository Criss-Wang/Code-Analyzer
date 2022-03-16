#include "clause.h"

namespace pql_clause {
  class SuchThatClause : public Clause {
    public:
      SuchThatClause(pql::RelationshipToken* token) :
          Clause{ token } {}
    
      virtual ~SuchThatClause() = default;

    public:
      virtual void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) = 0;
  };

  class FollowsClause : virtual public SuchThatClause {
  public:
      FollowsClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class FollowsTClause : public SuchThatClause {
    public:
      FollowsTClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class ParentClause : public SuchThatClause {
    public:
      ParentClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class ParentTClause : public SuchThatClause {
    public:
      ParentTClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class UsesSClause : public SuchThatClause {
    public:
      UsesSClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class ModifiesSClause : public SuchThatClause {
    public:
      ModifiesSClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class CallsClause : public SuchThatClause {
    public:
      CallsClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };

  class CallsTClause : public SuchThatClause {
    public:
      CallsTClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {}

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) override;
  };
}