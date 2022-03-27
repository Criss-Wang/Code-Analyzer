#include "clause.h"
#include <memory>
#include <utility>

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
      WithClause(std::shared_ptr<pql::AttrRef> left_attr, std::string& left_entity, bool is_attr_ref_left, 
          std::shared_ptr<pql::AttrRef> right_attr, std::string& right_entity, bool is_attr_ref_right) :
        Clause{} {
          left_attr_ = std::move(left_attr);
          left_entity_ = left_entity;
          is_attr_ref_left_ = is_attr_ref_left;
          right_attr_ = std::move(right_attr);
          right_entity_ = right_entity;
          is_attr_ref_right_ = is_attr_ref_right;
          type_ = pql::kWith;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
     
      void EvaluateEntEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateEntAttr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrAttr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrAttrVar(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);

      void EvaluateAttrAttrNum(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
  };

}