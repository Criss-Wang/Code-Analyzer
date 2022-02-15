#include <string>
#include <vector>

#include "predicate.h"

namespace pql_table {

  Predicate::Predicate(pql::Synonym& first, pql::Synonym& second, std::vector<std::pair<int, int>> pairs) {
    first_syn_ = first;
    second_syn_ = second;
    std::vector<std::pair<pql_table::element, pql_table::element>> lst;
    
    for (auto& pair : pairs) {
      pql_table::element front;
      pql_table::element back;
      front.val = pair.first;
      back.val = pair.second;
      lst.push_back(std::make_pair(front, back));
    }

    allowed_pairs_ = lst;
  }

  Predicate::Predicate(pql::Synonym& first, pql::Synonym& second, std::vector<std::pair<int, std::string>> pairs) {
    first_syn_ = first;
    second_syn_ = second;
    std::vector<std::pair<pql_table::element, pql_table::element>> lst;

    for (auto& pair : pairs) {
      pql_table::element front;
      pql_table::element back;
      front.val = pair.first;
      back.name = pair.second;
      lst.push_back(std::make_pair(front, back));
    }

    allowed_pairs_ = lst;
  }
}