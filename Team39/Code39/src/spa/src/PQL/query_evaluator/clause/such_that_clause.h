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
      std::vector<std::string> GetInvovledSynonyms() override;

      int GetPriority();

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
          SuchThatClause{ left, is_synonym_left, right, is_synonym_right } {
        type_ = pql::RelationshipTypes::kFollows;
      }
  };

  class FollowsTClause : public SuchThatClause {
    public:
      FollowsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kFollowsT;
      }
  };

  class ParentClause : public SuchThatClause {
    public:
      ParentClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kParent;
      }
  };

  class ParentTClause : public SuchThatClause {
    public:
      ParentTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kParentT;
      }
  };

  class UsesSClause : public SuchThatClause {
    public:
      UsesSClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kUsesS;
      }
  };

  class ModifiesSClause : public SuchThatClause {
    public:
      ModifiesSClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kModifiesS;
      }
  };

  class UsesPClause : public SuchThatClause {
    public:
      UsesPClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
        type_ = pql::RelationshipTypes::kUsesP;
      }
  };

  class ModifiesPClause : public SuchThatClause {
    public:
      ModifiesPClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
        SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
          type_ = pql::RelationshipTypes::kModifiesP;
      }
  };

  class CallsClause : public SuchThatClause {
    public:
      CallsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) {
        type_ = pql::RelationshipTypes::kCalls;
      }
  };

  class CallsTClause : public SuchThatClause {
    public:
      CallsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kCallsT;
      }
  };

  class NextClause : public SuchThatClause {
    public:
      NextClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kNext;
      }
  };

  class NextTClause : public SuchThatClause {
    public:
      NextTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kNextT;
      }
  };

  class AffectsClause : public SuchThatClause {
    public:
      AffectsClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kAffects;
      }
  };

  class AffectsTClause : public SuchThatClause {
    public:
      AffectsTClause(std::string left, bool is_synonym_left, std::string right, bool is_synonym_right) :
          SuchThatClause(left, is_synonym_left, right, is_synonym_right) { 
        type_ = pql::RelationshipTypes::kAffectsT;
      }
  };
}