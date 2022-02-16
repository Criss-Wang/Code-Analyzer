#include <string>
#include <vector>
#include <unordered_map>

#include "Token.h"
#include "predicate.h"
#include "../PKB/pkb.h"

namespace pql {
  
  class Clause {
    public:
      pql::RelationshipToken* token_;
      Pkb pkb_;
      std::unordered_map<std::string, std::vector<int>> stmt_hashmap_;
      std::unordered_map<std::string, std::vector<std::string>> var_hashmap_; 
      std::vector<pql_table::Predicate> predicates_;

    public:
      Clause(pql::RelationshipToken* token, Pkb& pkb,
             std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
             std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
             std::vector<pql_table::Predicate>& predicates);

    public:
      void ExtractRelExist() { return; };
      
      void ExtractRelDomain() { return; };

      void ExtractInverseRelDomain() { return; };

      void Evaluate() { return; };
  };

  class FollowsClause : public Clause {
    public:
      FollowsClause(pql::RelationshipToken* token, Pkb& pkb,
                    std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
                    std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
                    std::vector<pql_table::Predicate>& predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist();

      void ExtractRelDomain();

      void ExtractInverseRelDomain();

      void Evaluate();
  };

  class FollowsTClause : public Clause {
    public:
      FollowsTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
          std::vector<pql_table::Predicate>& predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist();

      void ExtractRelDomain();

      void ExtractInverseRelDomain();

      void Evaluate();
  };

  class ParentClause : public Clause {
    public:
      ParentClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
          std::vector<pql_table::Predicate>& predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist();

      void ExtractRelDomain();

      void ExtractInverseRelDomain();

      void Evaluate();
  };

  class ParentTClause : public Clause {
    public:
      ParentTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
          std::vector<pql_table::Predicate>& predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist();

      void ExtractRelDomain();

      void ExtractInverseRelDomain();

      void Evaluate();
  };

  class UsesSClause : public Clause {
    public:
      UsesSClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
          std::vector<pql_table::Predicate>& predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist();

      void ExtractRelDomain();

      void ExtractInverseRelDomain();

      void Evaluate();
  };

  class ModifiesSClause : public Clause {
    public:
      ModifiesSClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
          std::vector<pql_table::Predicate>& predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist();

      void ExtractRelDomain();

      void ExtractInverseRelDomain();

      void Evaluate();
  };
}
