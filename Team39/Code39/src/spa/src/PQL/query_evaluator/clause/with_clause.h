#include <memory>
#include <utility>

#include "clause.h"

namespace pql_clause {
  class WithClause : public Clause {
    public:
      std::shared_ptr<pql::AttrRef> left_attr_;
      std::string left_entity_;
      bool is_attr_ref_left_;
      std::shared_ptr<pql::AttrRef> right_attr_;
      std::string right_entity_;
      bool is_attr_ref_right_;
    
    public:
      WithClause(std::shared_ptr<pql::AttrRef> left_attr, std::string left_entity, bool is_attr_ref_left, 
          std::shared_ptr<pql::AttrRef> right_attr, std::string right_entity, bool is_attr_ref_right) :
        Clause{} {
          left_attr_ = std::move(left_attr);
          left_entity_ = std::move(left_entity);
          is_attr_ref_left_ = is_attr_ref_left;
          right_attr_ = std::move(right_attr);
          right_entity_ = std::move(right_entity);
          is_attr_ref_right_ = is_attr_ref_right;
          type_ = pql::kWith;
      }

    public:
      std::vector<std::string> GetInvovledSynonyms() override;

      int GetPriority() override;

      void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
     
      void EvaluateEntEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntAttr(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrAttr(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrAttrVar(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrAttrNum(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
  };

}