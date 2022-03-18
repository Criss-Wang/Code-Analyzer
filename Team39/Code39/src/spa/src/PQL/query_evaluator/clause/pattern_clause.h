# include "clause.h"

namespace pql_clause {
  class PatternClause : public Clause {
   public:
    std::string pattern_synonym_name_;
    pql::Ref left_;
    bool is_synonymy_left_;

    public:
      PatternClause(std::string& pattern_synonym_name, pql::Ref left, bool is_synonym_left) :
        Clause{} {
        pattern_synonym_name_ = pattern_synonym_name;
        left_ = left;
        is_synonymy_left_ = is_synonym_left;
      }

     virtual ~PatternClause() = default;

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
      AssignPatternClause(std::string& pattern_synonym, pql::Ref left, bool is_synonym_left, std::string& expr, bool is_exact) :
          PatternClause{ pattern_synonym, left, is_synonym_left } {
        expression_ = expr;
        is_exact_ = is_exact;
        type_ = pql::RelationshipTypes::kAssignPattern;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) override;

      void EvaluateExpr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain);
  };
}