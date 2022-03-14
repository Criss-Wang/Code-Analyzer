#include "clause.h"

namespace pql_clause {
  class SuchThatClause : public Clause {
    public:
      SuchThatClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          Clause{ token, pkb, domain, predicates } {}
    
      virtual ~SuchThatClause() = default;

    public:
      virtual void Evaluate() = 0;
  };

  class FollowsClause : virtual public SuchThatClause {
  public:
      FollowsClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class FollowsTClause : public SuchThatClause {
    public:
      FollowsTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class ParentClause : public SuchThatClause {
    public:
      ParentClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class ParentTClause : public SuchThatClause {
    public:
      ParentTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class UsesSClause : public SuchThatClause {
    public:
      UsesSClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class ModifiesSClause : public SuchThatClause {
    public:
      ModifiesSClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class UsesPClause : public SuchThatClause {
  public:
      UsesPClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

  public:
      void Evaluate() override;
  };

  class ModifiesPClause : public SuchThatClause {
  public:
      ModifiesPClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

  public:
      void Evaluate() override;
  };

  class CallsClause : public SuchThatClause {
    public:
      CallsClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };

  class CallsTClause : public SuchThatClause {
    public:
      CallsTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* domain,
          std::vector<pql_table::Predicate>* predicates) :
          SuchThatClause(token, pkb, domain, predicates) {}

    public:
      void Evaluate() override;
  };
}