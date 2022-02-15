#include <vector>
#include <string>

#include "../../../spa/src/PQL/intertable.h"
#include "../../../spa/src/Utility/Entity.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("Check equality of InterTable") {
	vector<int> table1_in_order_list({ 1,2,3,4,5,6,7,8,9,10 });
	vector<int> table1_out_order_list({ 2,4,5,6,8,10,3,1,7,9 });
	pql::Synonym table1_synonym = Synonym("s1", EntityIdentifier::kStmt);
	pql_table::InterTable table1 = InterTable(table1_synonym, table1_in_order_list);
	pql_table::InterTable table2 = InterTable(table1_synonym, table1_out_order_list);

	SECTION("Tables should be equal if the contents are the same, order does not matter") {
		REQUIRE(table1.equal(table2));
	}
}
