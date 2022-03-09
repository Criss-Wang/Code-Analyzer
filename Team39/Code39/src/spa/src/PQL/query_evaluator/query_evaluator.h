#include <string>
#include <vector>

#include "../../PKB/pkb.h"
#include "../parser.h"
#include "../token.h"
#include "predicate.h"

namespace pql {
  static std::vector<std::string> EMPTY_LIST = {};
  static std::vector<std::string> BOOLEAN_TRUE_LIST = { "TRUE" };
  static std::vector<std::string> BOOLEAN_FALSE_LIST = { "FALSE" };

	std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb);

	void ConsumePatternWithoutSyn(pql::PatternToken& pattern_token, Pkb& pkb,
			std::unordered_map<std::string, std::vector<int>>& stmt_hashmap);

	void ConsumePatternWithSyn(pql::PatternToken& pattern_token, Pkb& pkb,
			std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
			std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
			std::vector<pql_table::Predicate>& predicates);
}