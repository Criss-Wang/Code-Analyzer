#pragma once

#include <string>
#include <vector>

#include "../../query_parser/token.h"
#include "../../../Utility/entity.h"

namespace pql_table {

  //specialized hash function for pair of int
  struct hash_pair_fn {
    std::size_t operator() (const std::pair<int, int>& p) const {
      std::size_t h1 = std::hash<int>{}(p.first);
      std::size_t h2 = std::hash<int>{}(p.second);

      return h1 ^ h2;
    }
  };

  struct hash_vector_fn {
    std::size_t operator()(const std::vector<int>& elements) const {
      size_t answer = 0;
      for (int e : elements) {
        std::size_t h1 = std::hash<int>{}(e);
        answer ^= h1 + 0x9e3779b9 + (answer << 6) + (answer >> 2);
      }
      return answer;
    }
  };

  class Predicate {
    public:
      std::string first_syn_;
      std::string second_syn_;
      std::vector<std::pair<int, int>> allowed_pairs_;

    public:
      Predicate(std::string& first, std::string& second, std::vector<std::pair<int, int>>& pairs);

      bool equal(Predicate &p);
  };
}