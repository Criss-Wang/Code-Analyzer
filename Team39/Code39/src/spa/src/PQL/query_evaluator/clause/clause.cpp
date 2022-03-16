#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "clause.h"

namespace pql_clause {
  Clause::Clause(pql::RelationshipToken* token) {
    token_ = token;
  }

  pql::RelationshipTypes Clause::GetType() {
    return type_;
  }

  pql::RelationshipToken Clause::GetToken() {
    return token_;
  }
}