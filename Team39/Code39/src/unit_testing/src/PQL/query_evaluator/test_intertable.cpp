#include <vector>
#include <string>

#include "../../../../spa/src/PQL/query_evaluator/solver/intertable.h"
#include "../../../../spa/src/PQL/query_evaluator/solver/predicate.h"
#include "../../../../spa/src/PQL/query_evaluator/query_evaluator_exceptions.h"
#include "../../../../spa/src/PQL/query_parser/token.h"
#include "../../../../spa/src/Utility/entity.h"
#include "catch.hpp"

using namespace std;

//Only test until table with three columns because in iteration 1, there is at most 
//3 columns in a table

// table1 represents table with one col, table2 represents table with 2 col, ...
/*-------------------------------------------------------TABLES with one column ------------------------------------------------------------------ */
//Table with one column and elements are integer
static vector<int> table1_col({ 1,2,3,4,5 });
static pql::Synonym table1_syn("s1", EntityIdentifier::kStmt);
static pql_table::InterTable table1 = pql_table::InterTable(table1_syn, table1_col);

static vector<int> table1_out_order_col({ 3,2,4,5,1 });
static pql_table::InterTable table1_out_order = pql_table::InterTable(table1_syn, table1_out_order_col);

static vector<int> table1_duplicate_col({ 1,1,1,2,3,3,4,4,4,4,5 });
static pql_table::InterTable table1_duplicate = pql_table::InterTable(table1_syn, table1_duplicate_col);
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------TABLES with two columns ------------------------------------------------------------------ */ 
// standard table with 2 columns
static vector<int> table2_first_col({ 2, 4, 6, 8, 10 });
static vector<int> table2_second_col({ 1, 3, 5, 7, 9 });
static pql::Synonym table2_first_syn("s", EntityIdentifier::kStmt);
static vector<vector<int>> table2_rows = { {2, 1}, {4, 3}, {6, 5}, {8, 7}, {10, 9} };
static pql::Synonym table2_second_syn("v", EntityIdentifier::kVariable);
static vector<string> table2_header({ table2_first_syn.GetName(), table2_second_syn.GetName() });
static pql_table::InterTable table2(table2_header, table2_rows);

// table with 2 columns that have duplicate rows
static vector<vector<int>> table2_duplicate_rows = { {2, 1}, {4, 3}, {6, 5}, {8, 7}, {10, 9}, {2, 1}, {4, 3}, {6, 5}, {8, 7}, {2, 1} };
static pql_table::InterTable table2_duplicate(table2_header, table2_duplicate_rows);

//tables used for Merge, Filter and mergeAndFilter
static pql_table::InterTable table1_merge1(table2_first_syn, table2_first_col);
static pql_table::InterTable table1_merge2(table2_second_syn, table2_second_col);
static vector<vector<int>> table2_merge_rows = { {2, 1}, {2, 3}, {2, 5}, {2, 7}, {2, 9}, {4, 1}, {4, 3}, {4, 5}, {4, 7}, {4, 9}, 
																								 {6, 1}, {6, 3}, {6, 5}, {6, 7}, {6, 9}, {8, 1}, {8, 3}, {8, 5}, {8, 7}, {8, 9}, 
																								 {10, 1}, {10, 3}, {10, 5}, {10, 7}, {10, 9} };
static pql_table::InterTable table2_merge(table2_header, table2_merge_rows);

static vector<pair<int, int>> nonempty_pred_lst = {make_pair(2, 1), make_pair(2, 5), make_pair(100, 8), make_pair(8, 9), 
		make_pair(2, 10), make_pair(2, 57), make_pair(6, 3), make_pair(10, 7) };
static string first_syn_name = table2_first_syn.GetName();
static string second_syn_name = table2_second_syn.GetName();
static pql_table::Predicate nonempty_predicates(first_syn_name, second_syn_name, nonempty_pred_lst);

static vector<pair<int, int>> empty_pred_lst = { make_pair(1, 3), make_pair(7, 5), make_pair(100, 8),
		make_pair(2, 10), make_pair(2, 57) };
static pql_table::Predicate empty_predicates(first_syn_name, second_syn_name, empty_pred_lst);

static vector<vector<int>> table2_filter_rows = { {2, 1}, {2, 5}, {6, 3}, {10, 7}, {8, 9} };
static pql_table::InterTable table2_filter(table2_header, table2_filter_rows);

// tables with two columns to be merged into table with four columns
static pql::Synonym table4_first_syn("s", EntityIdentifier::kStmt);
static pql::Synonym table4_second_syn("s1", EntityIdentifier::kStmt);
static pql::Synonym table4_third_syn("c", EntityIdentifier::kCall);
static pql::Synonym table4_fourth_syn("p", EntityIdentifier::kProc);
static string table4_first_syn_name = table4_first_syn.GetName();
static string table4_second_syn_name = table4_second_syn.GetName();
static string table4_third_syn_name = table4_third_syn.GetName();
static string table4_fourth_syn_name = table4_fourth_syn.GetName();

static vector<string> table2_merge_into_table4_1_header({ table4_first_syn.GetName(), table4_second_syn.GetName() });
static vector<vector<int>> table2_merge_into_table4_1_rows = { {3, 13}, {19, 14}, {2, 16}, {10, 18}, {9, 5} };
static pql_table::InterTable table2_merge_into_table4_1(table2_merge_into_table4_1_header, table2_merge_into_table4_1_rows);

static vector<string> table2_merge_into_table4_2_header({ table4_third_syn.GetName(), table4_fourth_syn.GetName() });
static vector<vector<int>> table2_merge_into_table4_2_rows = { {14, 5}, {2, 14}, {10, 9} };
static pql_table::InterTable table2_merge_into_table4_2(table2_merge_into_table4_2_header, table2_merge_into_table4_2_rows);

/*--------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------TABLES with four columns ---------------------------------------------------------------------- */
static vector<string> table4_header({ table4_first_syn.GetName(), table4_second_syn.GetName(), table4_third_syn.GetName(), table4_fourth_syn.GetName() });
static vector<vector<int>> table4_rows = { {3, 13, 14, 5}, {3, 13, 2, 14}, {3, 13, 10, 9}, {19, 14, 14, 5}, {19, 14, 2, 14}, {19, 14, 10, 9},
		{2, 16, 14, 5}, {2, 16, 2, 14}, {2, 16, 10, 9}, {10, 18, 14, 5}, {10, 18, 2, 14}, {10, 18, 10, 9}, {9, 5, 14, 5}, {9, 5, 2, 14}, {9, 5, 10, 9} };
static pql_table::InterTable table4(table4_header, table4_rows);

static vector<vector<int>> table4_duplicate_rows = { {3, 13, 14, 5}, {3, 13, 2, 14}, {3, 13, 10, 9}, {19, 14, 14, 5}, {19, 14, 2, 14}, {19, 14, 10, 9},
		{2, 16, 14, 5}, {2, 16, 2, 14}, {2, 16, 10, 9}, {10, 18, 14, 5}, {10, 18, 2, 14}, {10, 18, 10, 9}, {9, 5, 14, 5}, {9, 5, 2, 14}, {9, 5, 10, 9},
    {3, 13, 14, 5}, {3, 13, 2, 14}, {3, 13, 10, 9}, {19, 14, 14, 5}, {19, 14, 2, 14}, {2, 16, 14, 5}, {2, 16, 2, 14}, {2, 16, 14, 5}, {2, 16, 2, 14} };
static pql_table::InterTable table4_duplicate(table4_header, table4_duplicate_rows);

static vector<pair<int, int>> table4_pred_lst_1 = { make_pair(1, 2), make_pair(7, 5), make_pair(13, 14),
		make_pair(13, 10), make_pair(14, 10), make_pair(16, 14), make_pair(16, 2), make_pair(18, 10) };
static pql_table::Predicate table4_pred_1(table4_second_syn_name, table4_third_syn_name, table4_pred_lst_1);

static vector<vector<int>> table4_filter_1_rows = { {3, 13, 14, 5}, {3, 13, 10, 9}, {19, 14, 10, 9}, {2, 16, 14, 5}, {2, 16, 2, 14}, {10, 18, 10, 9}};
static pql_table::InterTable table4_filter_1(table4_header, table4_filter_1_rows);

static vector<pair<int, int>> table4_pred_lst_2 = { make_pair(13, 3), make_pair(18, 10) };
static pql_table::Predicate table4_pred_2(table4_second_syn_name, table4_first_syn_name, table4_pred_lst_2);

static vector<vector<int>> table4_filter_2_rows = { {3, 13, 14, 5}, {3, 13, 2, 14}, {3, 13, 10, 9}, {10, 18, 14, 5}, {10, 18, 2, 14}, {10, 18, 10, 9} };
static pql_table::InterTable table4_filter_2(table4_header, table4_filter_2_rows);
/*---------------------------------------------------------------------------------------------------------------------------------------------------------*/

TEST_CASE("Check equality of InterTable") {
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
	  REQUIRE(!table1.equal(table1_duplicate)); // should not be equal since number of occurence of each elements are not same
		REQUIRE(table1.equal(table1_duplicate.Deduplicate())); // should be equal

		REQUIRE(!table2.equal(table2_duplicate));
		REQUIRE(table2.equal(table2_duplicate.Deduplicate()));

		REQUIRE(!table4_duplicate.equal(table4));
		REQUIRE(table4_duplicate.Deduplicate().equal(table4));
	}
}

TEST_CASE("Check merge function of Intertable") {
	
	SECTION("Merge two tables of column one") {
		REQUIRE(!table1_merge1.equal(table2_merge));
		REQUIRE(!table1_merge2.equal(table2_merge));
		REQUIRE(table1_merge1.Merge(table1_merge2).equal(table2_merge));
	}

	SECTION("Merge two tables of column two") {
	  REQUIRE(!table2_merge_into_table4_1.equal(table4));
		REQUIRE(!table2_merge_into_table4_2.equal(table4));
		REQUIRE(table2_merge_into_table4_1.Merge(table2_merge_into_table4_2).equal(table4));
	}
}

TEST_CASE("Check filter function of Intertable") {
  
  SECTION("Filter tables of two columns with content") {
		REQUIRE(!table2_merge.equal(table2_filter));
		REQUIRE(table2_merge.Filter(nonempty_predicates).equal(table2_filter));
  }

	SECTION("The table of two columns after filter will be empty") {
	  CHECK_THROWS_AS(table2_merge.Filter(empty_predicates), pql_exceptions::EmptyTableException);
	}

	SECTION("Filter tables of four columns with content") {
	  REQUIRE(!table4.equal(table4_filter_1));
		REQUIRE(table4.Filter(table4_pred_1).equal(table4_filter_1));

		REQUIRE(!table4.equal(table4_filter_2));
		REQUIRE(table4.Filter(table4_pred_2).equal(table4_filter_2));
	}
}

TEST_CASE("Check MergeAndFilter function of Intertable") {

  SECTION("MergeAndFilter tables of one column with content") {
		REQUIRE(!table1_merge1.equal(table2_filter));
		REQUIRE(table1_merge1.MergeAndFilter(table1_merge2, nonempty_predicates).equal(table2_filter));
  }

	SECTION("The table with two columns after merge and filter will be empty") {
		CHECK_THROWS_AS(table1_merge1.MergeAndFilter(table1_merge2, empty_predicates).equal(table2_filter), pql_exceptions::EmptyTableException);
	}

	SECTION("MergeAndFilter tables of two columns with content") {
	  REQUIRE(table2_merge_into_table4_1.MergeAndFilter(table2_merge_into_table4_2, table4_pred_1).equal(table4_filter_1));
	}
}

TEST_CASE("Check GetColByIndices function of InterTable") {
	SECTION("GetColByIndices on tables with four column") {
	  
	  SECTION("Get one column") {
			vector<string> res1_header = { table4_first_syn_name };
			vector<vector<int>> res1_rows = { { 3 }, { 19 }, { 2 }, { 10 }, { 9 } };
			pql_table::InterTable res1_table(res1_header, res1_rows);
			
			vector<vector<int>> res1_duplicate_rows = { { 3 }, { 3 }, { 3 }, { 19 }, { 19 }, { 19 }, { 2 }, { 2 }, { 2 }, 
					{ 10 }, { 10 }, { 10 }, { 10 }, { 9 }, { 9 }, { 9 } };
			pql_table::InterTable res1_duplicate_table(res1_header, res1_duplicate_rows);

			vector<int> index_lst = { 0 };
			REQUIRE(table4.GetColsByIndices(index_lst).equal(res1_table));
			REQUIRE(!table4.GetColsByIndices(index_lst).equal(res1_duplicate_table)); //the GetColsByIndices return an InterTable without any duplication
		}
		
		SECTION("Get two columns") {
		  vector<string> res2_header = { table4_second_syn_name, table4_fourth_syn_name };
		  vector<vector<int>> res2_rows = { { 13, 5 }, { 13, 14 }, { 13, 9 }, { 14, 5 }, { 14, 14 }, { 14, 9 }, 
					{ 16, 5 }, { 16, 14 }, { 16, 9 }, { 18, 5 }, { 18, 14 }, { 18, 9 }, { 5, 5 }, { 5, 14 }, { 5, 9 } };
		  pql_table::InterTable res2_table(res2_header, res2_rows);
		  vector<int> index_lst = { 1, 3 };
		  REQUIRE(table4.GetColsByIndices(index_lst).equal(res2_table));
		}

		SECTION("Get three columns") {
		  vector<string> res3_header = { table4_second_syn_name, table4_third_syn_name, table4_fourth_syn_name };
		  vector<vector<int>> res3_rows = { { 13, 14, 5 }, { 13, 2, 14 }, { 13, 10, 9 }, { 14, 14, 5 }, { 14, 2, 14 }, { 14, 10, 9 },
				{ 16, 14, 5 }, { 16, 2, 14 }, { 16, 10, 9 }, { 18, 14, 5 }, { 18, 2, 14 }, { 18, 10, 9 }, { 5, 14, 5 }, { 5, 2, 14 }, { 5, 10, 9 } };
		  pql_table::InterTable res3_table(res3_header, res3_rows);
		  vector<int> index_lst = { 1, 2, 3 };
		  REQUIRE(table4.GetColsByIndices(index_lst).equal(res3_table));
		}

	}
}
