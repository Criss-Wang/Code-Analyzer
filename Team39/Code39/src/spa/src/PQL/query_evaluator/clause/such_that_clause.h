#include "clause.h"

namespace pql_clause {
  class SuchThatClause : public Clause {
    public:
      std::string left_;
      bool is_synonym_left_;
      std::string right_;
      bool is_synonym_right_;

    public:
      SuchThatClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          Clause{} {
        left_ = left;
        is_synonym_left_ = is_synonym_left;
        right_ = right;
        is_synonym_right_ = is_synonym_right;
      }
    
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
      FollowsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause{ left, is_synonym_left, right, is_synonym_right } {
          type_ = pql::RelationshipTypes::kFollows;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class FollowsTClause : public SuchThatClause {
    public:
      FollowsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kFollowsT;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ParentClause : public SuchThatClause {
    public:
      ParentClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kParent;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ParentTClause : public SuchThatClause {
    public:
      ParentTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kParentT;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class UsesSClause : public SuchThatClause {
    public:
      UsesSClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kUsesS;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ModifiesSClause : public SuchThatClause {
    public:
      ModifiesSClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kModifiesS;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class UsesPClause : public SuchThatClause {
  public:
      UsesPClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kUsesP;
      }

  public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class ModifiesPClause : public SuchThatClause {
  public:
    ModifiesPClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
        SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kModifiesP;
      }

  public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class CallsClause : public SuchThatClause {
    public:
      CallsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kCalls;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class CallsTClause : public SuchThatClause {
    public:
      CallsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kCallsT;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };
}