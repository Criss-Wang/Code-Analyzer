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
        left_ = move(left); 
        is_synonym_left_ = is_synonym_left;
        right_ = move(right);
        is_synonym_right_ = is_synonym_right;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
 
      void EvaluateWildWild(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
     
      void EvaluateWildEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
      
      void EvaluateWildSyn(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntWild(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntSyn(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateSynWild(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateSynEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateSynSyn(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
  };

  class FollowsClause : public SuchThatClause {
  public:
      FollowsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause{ left, is_synonym_left, right, is_synonym_right } { // review: std::move
          type_ = pql::RelationshipTypes::kFollows;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class FollowsTClause : public SuchThatClause {
    public:
      FollowsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kFollowsT;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class ParentClause : public SuchThatClause {
    public:
      ParentClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kParent;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class ParentTClause : public SuchThatClause {
    public:
      ParentTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kParentT;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class UsesSClause : public SuchThatClause {
    public:
      UsesSClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kUsesS;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class ModifiesSClause : public SuchThatClause {
    public:
      ModifiesSClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kModifiesS;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class UsesPClause : public SuchThatClause {
  public:
      UsesPClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kUsesP;
      }

  public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class ModifiesPClause : public SuchThatClause {
  public:
    ModifiesPClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
        SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kModifiesP;
      }

  public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class CallsClause : public SuchThatClause {
    public:
      CallsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kCalls;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class CallsTClause : public SuchThatClause {
    public:
      CallsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kCallsT;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class NextClause : public SuchThatClause {
    public:
      NextClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kNext;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class NextTClause : public SuchThatClause {
    public:
      NextTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kNextT;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class AffectsClause : public SuchThatClause {
    public:
      AffectsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kAffects;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };

  class AffectsTClause : public SuchThatClause {
    public:
      AffectsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { // review: std::move
          type_ = pql::RelationshipTypes::kAffectsT;
      }

    public:
      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) {
        SuchThatClause::Evaluate(cache, domain, predicates);
      };
  };
}