#include <string>
#include <vector>

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
      back.val = pair.second;
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
      back.name = pair.second;
      lst.push_back(std::make_pair(front, back));
    }

    allowed_pairs_ = lst;
  }
}