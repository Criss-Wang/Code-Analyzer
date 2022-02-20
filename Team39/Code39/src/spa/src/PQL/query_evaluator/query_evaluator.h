#include <string>
#include <vector>

#include "../../PKB/pkb.h"
#include "../parser.h"
#include "../token.h"

namespace pql {
	std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb);
}