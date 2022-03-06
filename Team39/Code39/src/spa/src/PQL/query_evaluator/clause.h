#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "../token.h"
#include "predicate.h"
#include "../../PKB/pkb.h"
#include "query_evaluator_exceptions.h"

namespace pql {
  template <typename T>
  std::vector<T> Intersect(const std::vector<T>& lst1, const std::vector<T>& lst2) {
    //refer from stack overflow https://stackoverflow.com/questions/38993415/how-to-apply-the-intersection-between-two-lists-in-c
    std::vector<T> res;
    std::unordered_set<T> st;
    std::for_each(lst2.begin(), lst2.end(), [&st](const T& k) { st.insert(k); });
    std::for_each(lst1.begin(), lst1.end(),
        [&st, &res](const T& k) {
            auto iter = st.find(k);
            if (iter != st.end()) {
                res.push_back(k);
                st.erase(iter);
            }
        }
    );
    return res;
  }

  template <typename T>
  void UpdateHashmap(std::unordered_map<std::string, std::vector<T>>& hmap, std::string name, const std::vector<T>& lst) {
    std::vector<T> oriLst = hmap[name];
    std::vector<T> inter = Intersect<T>(oriLst, lst);

    //the domain is empty after intersection
    if (inter.size() == 0) {
        throw pql_exceptions::EmptyDomainException();
    }

    hmap[name] = inter;
  }

  class Clause {
    public:
      pql::RelationshipToken* token_;
      Pkb pkb_;
      std::unordered_map<std::string, std::vector<int>>* stmt_hashmap_;
      std::unordered_map<std::string, std::vector<std::string>>* var_hashmap_;
      std::vector<pql_table::Predicate>* predicates_;

    public:
      Clause(pql::RelationshipToken* token, Pkb& pkb,
             std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
             std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
             std::vector<pql_table::Predicate>* predicates);

      virtual ~Clause() = default;

    public:
      virtual void ExtractRelExist() = 0;

      virtual void ExtractRelDomain() = 0;

      virtual void ExtractInverseRelDomain() = 0;

      virtual void ExtractRelPair() = 0;

      virtual void Evaluate() = 0;
  };

  class FollowsClause : public Clause {
    public:
      FollowsClause(pql::RelationshipToken* token, Pkb& pkb,
                    std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
                    std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
                    std::vector<pql_table::Predicate>* predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist() override;

      void ExtractRelDomain() override;

      void ExtractInverseRelDomain() override;

      void ExtractRelPair() override;

      void Evaluate() override;
  };

  class FollowsTClause : public Clause {
    public:
      FollowsTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
          std::vector<pql_table::Predicate>* predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist() override;

      void ExtractRelDomain() override;

      void ExtractInverseRelDomain() override;

      void ExtractRelPair() override;

      void Evaluate() override;
  };

  class ParentClause : public Clause {
    public:
      ParentClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
          std::vector<pql_table::Predicate>* predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist() override;

      void ExtractRelDomain() override;

      void ExtractInverseRelDomain() override;

      void ExtractRelPair() override;

      void Evaluate() override;
  };

  class ParentTClause : public Clause {
    public:
      ParentTClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
          std::vector<pql_table::Predicate>* predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist() override;

      void ExtractRelDomain() override;

      void ExtractInverseRelDomain() override;

      void ExtractRelPair() override;

      void Evaluate() override;
  };

  class UsesSClause : public Clause {
    public:
      UsesSClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
          std::vector<pql_table::Predicate>* predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist() override;

      void ExtractRelDomain() override;

      void ExtractInverseRelDomain() override;

      void ExtractRelPair() override;

      void Evaluate() override;
  };

  class ModifiesSClause : public Clause {
    public:
      ModifiesSClause(pql::RelationshipToken* token, Pkb& pkb,
          std::unordered_map<std::string, std::vector<int>>* stmt_hashmap,
          std::unordered_map<std::string, std::vector<std::string>>* var_hashmap,
          std::vector<pql_table::Predicate>* predicates) :
          Clause(token, pkb, stmt_hashmap, var_hashmap, predicates) {}

    public:
      void ExtractRelExist() override;

      void ExtractRelDomain() override;

      void ExtractInverseRelDomain() override;

      void ExtractRelPair() override;

      void Evaluate() override;
  };
}
