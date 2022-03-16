#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "../solver/predicate.h"
#include "../../../PKB/pkb.h"
#include "../query_evaluator_exceptions.h"

namespace pql_clause {
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

    public:
      Clause(pql::RelationshipToken* token);

      virtual ~Clause() = default;

    public:
      virtual void Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) = 0;
  };
}
