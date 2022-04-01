#ifndef CLAUSE_H
#define CLAUSE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "../solver/predicate.h"
#include "../cache/cache.h"
#include "../../query_parser/token.h"
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

  template <typename T, typename R>
  std::vector<T> ExtractFirst(std::vector<std::pair<T, R>>& lst) {
    std::vector<T> res;

    for (std::pair<T, R>& ele : lst) {
      res.push_back(ele.first);
    }

    return res;
  }

  template <typename T, typename R>
  std::vector<R> ExtractSecond(std::vector<std::pair<T, R>>& lst) {
    std::vector<R> res;

    for (std::pair<T, R>& ele : lst) {
      res.push_back(ele.second);
    }

    return res;
  }

  template <typename T>
  std::vector<T> RemoveDuplicate(std::vector<T>& lst) {
    std::unordered_set<T> st;
    std::for_each(lst.begin(), lst.end(), [&st](const T& k) { st.insert(k); });
    std::vector<T> res(st.begin(), st.end());

    return res;
  }

  class Clause {
    public:
      pql::RelationshipTypes type_;

    public:
        virtual ~Clause() = default;

    public:
      virtual void Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
          std::vector<pql_table::Predicate>& predicates) = 0;

      virtual std::vector<std::string> GetInvovledSynonyms() = 0;

      virtual int GetPriority() = 0;
  };

  //We define the priority of clause in below:
  //with <-- (pattern, Follows, Calls, ModifiesS, Next, Parent) <-- (FollowsT, ParentT, 
  // CallsT, UsesS, NextT, UsesP, ModifiesP) <-- (Affects, AffectsT) 
}

#endif
