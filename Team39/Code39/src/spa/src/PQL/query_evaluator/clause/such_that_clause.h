#include "clause.h"

namespace pql_clause {
  class SuchThatClause : public Clause {
    public:
      SuchThatClause(pql::RelationshipToken* token) :
          Clause{ token } {}
    
      virtual ~SuchThatClause() = default;

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
 
      void EvaluateWildWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
     
      void EvaluateWildEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
      
      void EvaluateWildSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateSynWild(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateSynEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateSynSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
  };

  class FollowsClause : virtual public SuchThatClause {
  public:
      FollowsClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kFollows;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class FollowsTClause : public SuchThatClause {
    public:
      FollowsTClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kFollowsT;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ParentClause : public SuchThatClause {
    public:
      ParentClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kParent;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ParentTClause : public SuchThatClause {
    public:
      ParentTClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kParentT;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class UsesSClause : public SuchThatClause {
    public:
      UsesSClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kUsesS;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ModifiesSClause : public SuchThatClause {
    public:
      ModifiesSClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kModifiesS;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class UsesPClause : public SuchThatClause {
  public:
      UsesPClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kUsesP;
      }

  public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ModifiesPClause : public SuchThatClause {
  public:
      ModifiesPClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kModifiesP;
      }

  public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class CallsClause : public SuchThatClause {
    public:
      CallsClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kCalls;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class CallsTClause : public SuchThatClause {
    public:
      CallsTClause(pql::RelationshipToken* token) :
          SuchThatClause(token) {
          type_ = pql::RelationshipTypes::kCallsT;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };
}