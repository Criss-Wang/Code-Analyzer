#include <vector>
#include <string>

#include "../../../../spa/src/PQL/query_evaluator/intertable.h"
#include "../../../../spa/src/PQL/query_evaluator/predicate.h"
#include "../../../../spa/src/PQL/Token.h"
#include "../../../../spa/src/Utility/Entity.h"
#include "catch.hpp"

using namespace std;

//Only test until table with three columns because in iteration 1, there is at most 
//3 columns in a table

// table1 represents table with one col, table2 represents table with 2 col, ...
// TABLES with one column
// TODO: tables with string
static vector<int> table1_col({ 1,2,3,4,5 });
static pql::Synonym table1_syn("s1", EntityIdentifier::kStmt);
static pql_table::InterTable table1 = pql_table::InterTable(table1_syn, table1_col);

static vector<int> table1_out_order_col({ 3,2,4,5,1 });
static pql_table::InterTable table1_out_order = pql_table::InterTable(table1_syn, table1_out_order_col);

static vector<int> table1_duplicate_col({ 1,1,1,2,3,3,4,4,4,4,5 });
static pql_table::InterTable table1_duplicate = pql_table::InterTable(table1_syn, table1_duplicate_col);

// TABLES with two columns
static vector<int> table2_first_col({ 2,4,6,8 });
static vector<string> table2_second_col({ "a", "b", "c", "d" });
static pql::Synonym table2_first_syn("s", EntityIdentifier::kStmt);
static pql::Synonym table2_second_syn("v", EntityIdentifier::kVariable);
static vector<string> table2_header({ table2_first_syn.GetName(), table2_second_syn.GetName() });
static vector<vector<pql_table::element>> table2_rows;
static pql_table::InterTable table2(table2_header, table2_rows);

static vector<int> table2_duplicate_first_col({ 2,2,2,4,6,8,8 });
static vector<string> table2_duplicate_second_col({ "a","a","a","b","c","d","d"});
static vector<vector<pql_table::element>> table2_duplicate_rows;
static pql_table::InterTable table2_duplicate(table2_header, table2_duplicate_rows);

static pql_table::InterTable table1_merge1(table2_first_syn, table2_first_col);
static pql_table::InterTable table1_merge2(table2_second_syn, table2_second_col);
static vector<int> table2_merge_first_col({ 2,2,2,2,4,4,4,4,6,6,6,6,8,8,8,8 });
static vector<string> table2_merge_second_col({ "a", "b", "c", "d", "a", "b", "c", "d", "a", "b", "c", "d", "a", "b", "c", "d" });
static vector<vector<pql_table::element>> table2_merge_rows;
static pql_table::InterTable table2_merge(table2_header, table2_merge_rows);

static vector<int> table2_first_empty_col({});
static pql_table::InterTable table1_empty_merge1(table2_first_syn, table2_first_empty_col);
static vector<vector<pql_table::element>> table2_empty_rows;
static pql_table::InterTable table2_empty(table2_header, table2_empty_rows);

static void InitializeList(vector<int>* col1, vector<string>* col2, vector<vector<pql_table::element>>* rows) {
	(*rows).clear();
  for (int index = 0; index < col1->size(); index++) {
		pql_table::element first;
		pql_table::element second;

		first.val = (*col1)[index];
		second.name = (*col2)[index];

		vector<pql_table::element> row({ first, second });
		(*rows).push_back(row);
  }
}

static void Initialize() {
  InitializeList(&table2_first_col, &table2_second_col, &table2_rows);
	InitializeList(&table2_duplicate_first_col, &table2_duplicate_second_col, &table2_duplicate_rows);
	InitializeList(&table2_merge_first_col, &table2_merge_second_col, &table2_merge_rows);
	table2 = pql_table::InterTable(table2_header, table2_rows);
	table2_duplicate = pql_table::InterTable(table2_header, table2_duplicate_rows);
	table2_merge = pql_table::InterTable(table2_header, table2_merge_rows);
}


TEST_CASE("Check equality of InterTable") {
	Initialize();

	SECTION("Tables should be equal if the contents are the same, order does not matter") {
		REQUIRE(table1.equal(table1_out_order));
	}

	SECTION("Tables that are the same should have same number of occurence of each element") {
	  REQUIRE(!table1.equal(table1_duplicate));
		REQUIRE(!table1_out_order.equal(table1_duplicate));
		REQUIRE(!table2.equal(table2_duplicate));
	}

	SECTION("Tables with different number of columns shoud not be equal") {
	  REQUIRE(!table1.equal(table2));
	}
}

TEST_CASE("Check Deduplicate function of Intertable") {

	SECTION("Deduplicate will only keep unique rows") {
	  Initialize();
	  REQUIRE(!table1.equal(table1_duplicate)); // should not be equal since number of occurence of each elements are not same
		table1_duplicate.Deduplicate();
		REQUIRE(table1.equal(table1_duplicate)); // should be equal

		REQUIRE(!table2.equal(table2_duplicate));
		table2_duplicate.Deduplicate();
		REQUIRE(table2.equal(table2_duplicate)); 
	}
}

TEST_CASE("Check merge function of Intertable") {
	
	SECTION("Merge two tables with content") {
		Initialize();
		REQUIRE(!table1_merge1.equal(table2_merge));
		table1_merge1.Merge(table1_merge2);
		REQUIRE(table1_merge1.equal(table2_merge));
	}
	
	SECTION("One of the table is empty") {
		Initialize();
		REQUIRE(!table1_empty_merge1.equal(table2_empty));
		table1_empty_merge1.Merge(table1_merge2);
		REQUIRE(table1_empty_merge1.equal(table2_empty));
	}
}

TEST_CASE("Check filter function of Intertable") {

}