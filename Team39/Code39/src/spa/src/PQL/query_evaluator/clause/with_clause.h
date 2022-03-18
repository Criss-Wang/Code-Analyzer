#include "clause.h"

namespace pql_clause {
  class WithClause : public Clause {
    public:
      pql::AttrRef* left_;
      bool is_attr_ref_left_;
      pql::AttrRef* right_;
      bool is_attr_ref_right_;
    
    public:
      WithClause(pql::AttrRef* left, bool is_attr_ref_left, pql::AttrRef* right, bool is_attr_ref_right) :
        Clause{} {
          left_ = left;
          is_attr_ref_left_ = is_attr_ref_left;
          right_ = right;
          is_attr_ref_right_ = is_attr_ref_right;
      }

    public:
      void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
        std::vector<pql_table::Predicate>& predicates);
  };

}