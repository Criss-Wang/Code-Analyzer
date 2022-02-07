#include <string>
#include <list>

#include "../PKB/pkb.h"

namespace pql {
	std::list<std::string> evaluateQuery(Query query, Pkb pkb);
}