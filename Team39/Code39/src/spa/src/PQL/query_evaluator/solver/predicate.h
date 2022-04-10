#pragma once

#include <string>
#include <vector>

#include "../../query_parser/token.h"
#include "../../../Utility/entity.h"

namespace pql_table {
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

      int GetPredSize();

      bool equal(Predicate &p);
  };
}