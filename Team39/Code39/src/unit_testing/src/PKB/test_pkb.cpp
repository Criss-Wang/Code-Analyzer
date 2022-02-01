
#include "TNode.h"
#include "PKB/pkb.h"
#include "PKB/entity_table.h"
#include "PKB/results.h"

#include "catch.hpp"

using namespace std;
void require(const bool b) {
    REQUIRE(b);
}



TEST_CASE("Testing StmtTable", "[Statement Table]") {
    int invalid_prop_idx = -1;
    int invalid_stmt_id = -1;
    int signal;

    SECTION("AddEntity testing")
    {
        auto test_stmt_table = StmtTable();
        int first_stmt = 1;

        signal = test_stmt_table.AddEntity(first_stmt, StmtTable::assign_idx_);
        require(signal == 1);

        signal = test_stmt_table.AddEntity(invalid_stmt_id, StmtTable::assign_idx_);
        require(signal == 0);

        signal = test_stmt_table.AddEntity(first_stmt, invalid_prop_idx);
        require(signal == 0);

        signal = test_stmt_table.AddEntity(first_stmt, StmtTable::print_idx_);
        require(signal == 0);
    }

    SECTION("GetPropByKey testing")
    {
        auto test_stmt_table = StmtTable();
        int first_stmt = 1;
        int second_stmt = 2;
        signal = test_stmt_table.AddEntity(first_stmt, StmtTable::assign_idx_);

        int prop_idx = test_stmt_table.GetPropByKey(first_stmt);
        require(signal == 1);
        require(prop_idx == StmtTable::assign_idx_);

        // TODO(Zhenlin) - Include a wrapper fn for this check style later
        REQUIRE_THROWS_AS(test_stmt_table.GetPropByKey(second_stmt), invalid_argument);
        REQUIRE_THROWS_WITH(test_stmt_table.GetPropByKey(second_stmt), "Invalid key");

    }

    SECTION("GetStmtLstByProp testing")
    {
        auto test_stmt_table = StmtTable();
        int first_assign_stmt = 1;
        int second_assign_stmt = 2;
        int read_stmt= 3;
        int print_stmt = 4;
        int call_stmt = 5;
        int while_stmt = 7;
        vector<int> result_list;
        vector<int> expected_list;

        signal = test_stmt_table.AddEntity(first_assign_stmt, StmtTable::assign_idx_);
        signal = test_stmt_table.AddEntity(second_assign_stmt, StmtTable::assign_idx_);
        signal = test_stmt_table.AddEntity(read_stmt, StmtTable::read_idx_);
        signal = test_stmt_table.AddEntity(print_stmt, StmtTable::print_idx_);
        signal = test_stmt_table.AddEntity(call_stmt, StmtTable::call_idx_);
        signal = test_stmt_table.AddEntity(while_stmt, StmtTable::while_idx_);

        result_list = test_stmt_table.GetStmtLstByProp(StmtTable::assign_idx_);
        expected_list = { first_assign_stmt, second_assign_stmt };
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));

        result_list = test_stmt_table.GetStmtLstByProp(StmtTable::read_idx_);
        expected_list = { read_stmt};
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));

        result_list = test_stmt_table.GetStmtLstByProp(StmtTable::print_idx_);
        expected_list = { print_stmt};
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));

        result_list = test_stmt_table.GetStmtLstByProp(StmtTable::call_idx_);
        expected_list = { call_stmt };
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));

    	result_list = test_stmt_table.GetStmtLstByProp(StmtTable::while_idx_);
        expected_list = { while_stmt };
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));

        result_list = test_stmt_table.GetStmtLstByProp(StmtTable::if_idx_);
        expected_list = {};
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));

        result_list = test_stmt_table.GetStmtLstByProp(invalid_prop_idx);
        expected_list = {};
        require(equal(result_list.begin(), result_list.end(), expected_list.begin()));
    }

}

TEST_CASE("Testing NonStmtIdTable", "[Non-Statemetn ID Tables]")
{
    string first_var_name = "x";
    int signal;

    SECTION("AddEntityByName Testing")
    {
        auto test_var_table = NonStmtIdTable(NonStmtIdTable::var_id_);

        signal = test_var_table.AddEntityByName(first_var_name);
        require(signal == 1);

        signal = test_var_table.AddEntityByName(first_var_name);
        require(signal == 0);
    }

    SECTION("GetEntityById testing")
    {
        string actual_result;

    	int valid_entity_id = 1;
        int invalid_entity_id_1 = 5;
        int invalid_entity_id_2 = -1;
        auto test_var_table = NonStmtIdTable(NonStmtIdTable::var_id_);
        signal = test_var_table.AddEntityByName(first_var_name);

        actual_result = test_var_table.GetEntityById(valid_entity_id);
        require(actual_result == first_var_name);

        actual_result = test_var_table.GetEntityById(invalid_entity_id_1);
        require(actual_result.empty());

        actual_result = test_var_table.GetEntityById(invalid_entity_id_2);
        require(actual_result.empty());
    }
}

// TODO(Zhenlin): Add tests after gettign implementations from Source Processor side
TEST_CASE("Testing PKB", "PKB")
{
	
}


