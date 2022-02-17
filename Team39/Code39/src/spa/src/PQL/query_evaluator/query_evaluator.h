#include <string>
#include <vector>

#include "../../PKB/pkb.h"
#include "../Parser.h"
#include "../Token.h"

namespace pql {
	std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb);
}