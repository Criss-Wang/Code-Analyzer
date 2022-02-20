#include <string>
#include <vector>
#include <unordered_set>

#include "predicate.h"

namespace pql_table {

  Predicate::Predicate(std::string& first, std::string& second, std::vector<std::pair<int, int>> pairs) {
    first_syn_ = first;
    second_syn_ = second;
    std::vector<std::pair<element, element>> lst;
    
    for (auto& pair : pairs) {
      element front;
      element back;
      front.val = pair.first;
      front.name = "";
      back.val = pair.second;
      back.name = "";
      lst.push_back(std::make_pair(front, back));
    }

    allowed_pairs_ = lst;
  }

  Predicate::Predicate(std::string& first, std::string& second, std::vector<std::pair<int, std::string>> pairs) {
    first_syn_ = first;
    second_syn_ = second;
    std::vector<std::pair<element, element>> lst;

    for (auto& pair : pairs) {
      element front;
      element back;
      front.val = pair.first;
      front.name = "";
      back.val = 0;
      back.name = pair.second;
      lst.push_back(std::make_pair(front, back));
    }

    allowed_pairs_ = lst;
  }

  bool Predicate::equal(Predicate& p) {
    if (first_syn_ != p.first_syn_ || second_syn_ != p.second_syn_) {
      return false;
    }

    std::unordered_set<std::pair<element, element>, hash_pair_fn> s1(allowed_pairs_.begin(), allowed_pairs_.end());
    std::unordered_set<std::pair<element, element>, hash_pair_fn> s2(p.allowed_pairs_.begin(), p.allowed_pairs_.end());
    
    return s1 == s2;
  }
}