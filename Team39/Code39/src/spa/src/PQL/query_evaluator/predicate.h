#pragma once

#include <string>
#include <vector>

#include "../Token.h"
#include "../../Utility/entity.h"

namespace pql_table {

  typedef struct element {
    int val;
    std::string name;

    bool operator< (const element& e) const {
      if (val == e.val) {
        return name < e.name;
      } else {
        return val < e.val;
      }
    }

    bool operator==(const element& e) const {
      return val == e.val && name == e.name;
    }
  } element;

    //specialized hash function for struct element
  struct hash_fn {
    std::size_t operator() (const element& e) const {
      std::size_t h1 = std::hash<int>{}(e.val);
      std::size_t h2 = std::hash<std::string>{}(e.name);

      return h1 ^ h2;
    }
  };

    //specialized hash function for pair of struct element
  struct hash_pair_fn {
    std::size_t operator() (const std::pair<element, element>& p) const {
      std::size_t h1 = std::hash<int>{}(p.first.val);
      std::size_t h2 = std::hash<std::string>{}(p.first.name);
      std::size_t h3 = std::hash<int>{}(p.second.val);
      std::size_t h4 = std::hash<std::string>{}(p.second.name);

      return h1 ^ h2 ^ h3 ^ h4;
    }
  };

  struct hash_vector_fn {
    std::size_t operator()(const std::vector<element>& elements) const {
      size_t answer = 0;
      for (element e : elements) {
        std::size_t h1 = std::hash<int>{}(e.val);
        std::size_t h2 = std::hash<std::string>{}(e.name);
        answer ^= (h1 ^ h2) + 0x9e3779b9 + (answer << 6) + (answer >> 2);
      }
      return answer;
    }
  };

  class Predicate {
    public:
      std::string first_syn_;
      std::string second_syn_;
      std::vector<std::pair<element, element>> allowed_pairs_;

    public:
      Predicate(std::string& first, std::string& second, std::vector<std::pair<int, int>> pairs);

      Predicate(std::string& first, std::string& second, std::vector<std::pair<int, std::string>> pairs);

      bool equal(Predicate &p);
  };
}