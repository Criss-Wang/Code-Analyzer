#include <string>
#include <vector>

#include "../../PKB/pkb.h"
#include "PQL/query_parser/parser.h"
#include "PQL/query_parser/token.h"
#include "predicate.h"

namespace pql {
	std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb);

	void ConsumePatternWithoutSyn(pql::PatternToken& pattern_token, Pkb& pkb,
			std::unordered_map<std::string, std::vector<int>>& stmt_hashmap);

	void ConsumePatternWithSyn(pql::PatternToken& pattern_token, Pkb& pkb,
			std::unordered_map<std::string, std::vector<int>>& stmt_hashmap,
			std::unordered_map<std::string, std::vector<std::string>>& var_hashmap,
			std::vector<pql_table::Predicate>& predicates);
}