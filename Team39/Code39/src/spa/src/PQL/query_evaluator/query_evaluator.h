#include <string>
#include <vector>

#include "../../PKB/pkb.h"
#include "../query.h"

namespace pql {
  static std::vector<std::string> EMPTY_LIST = {};
  static std::vector<std::string> BOOLEAN_TRUE_LIST = { "TRUE" };
  static std::vector<std::string> BOOLEAN_FALSE_LIST = { "FALSE" };

	std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb);
}