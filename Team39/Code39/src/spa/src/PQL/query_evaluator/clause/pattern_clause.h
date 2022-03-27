# include "clause.h"

#include <utility>

namespace pql_clause {
  class PatternClause : public Clause {
   public:
    std::string pattern_synonym_name_;
    std::string left_;
    bool is_synonymy_left_;

    public:
      PatternClause(std::string& pattern_synonym_name, std::string left, bool is_synonym_left) :
        Clause{} {
        pattern_synonym_name_ = pattern_synonym_name;
        left_ = std::move(left);
        is_synonymy_left_ = is_synonym_left;
      }

    public:
      virtual void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates) = 0;

      void EvaluateLeft(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateLeftWildcard(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
      
      void EvaluateLeftEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
      
      void EvaluateLeftSyn(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
  };

  class AssignPatternClause : public PatternClause {
    public:
      std::string expression_;
      bool is_exact_;

    public:
      AssignPatternClause(std::string& pattern_synonym, std::string left, bool is_synonym_left, std::string& expr, bool is_exact) :
          PatternClause{ pattern_synonym, std::move(left), is_synonym_left } {
        expression_ = expr;
        is_exact_ = is_exact;
        type_ = pql::RelationshipTypes::kAssignPattern;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;

      void EvaluateExpr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain);
  };

  class IfPatternClause : public PatternClause {
    public:
      IfPatternClause(std::string& pattern_synonym, std::string left, bool is_synonym_left) :
          PatternClause{ pattern_synonym, std::move(left), is_synonym_left } {
         type_ = pql::RelationshipTypes::kIfPattern;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };

  class WhilePatternClause : public PatternClause {
    public:
      WhilePatternClause(std::string& pattern_synonym, std::string left, bool is_synonym_left) :
          PatternClause{ pattern_synonym, std::move(left), is_synonym_left } {
        type_ = pql::RelationshipTypes::kWhilePattern;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;
  };
}