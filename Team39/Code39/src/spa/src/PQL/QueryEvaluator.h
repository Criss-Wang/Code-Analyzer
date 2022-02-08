#include <string>
#include <list>

#include "../PKB/pkb.h"
#include "Parser.h"

using namespace std;

namespace pql {
	std::list<std::string> evaluateQuery(Query query, Pkb& pkb);
}