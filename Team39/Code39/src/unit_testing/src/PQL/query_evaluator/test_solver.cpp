//#include "../../spa/src/PQL/query_evaluator/solver.h"
//
//#include "catch.hpp"
//
//using namespace std;
//
//static void InitializeList(vector<int>* col1, vector<string>* col2, vector<vector<pql_table::element>>* rows) {
//  (*rows).clear();
//  for (int index = 0; index < col1->size(); index++) {
//    pql_table::element first;
//    pql_table::element second;
//
//    first.val = (*col1)[index];
//    first.name = "";
//    second.val = 0;
//    second.name = (*col2)[index];
//
//    vector<pql_table::element> row({ first, second });
//    (*rows).push_back(row);
//  }
//}
//
//
//TEST_CASE("Check correctness for Consume") {
//  //assign a;
//  //variable v;
//  
//  unordered_map<string, vector<int>> stmt_hashmap;
//  unordered_map<string, vector<string>> var_hashmap;
//  std::vector<pql_table::Predicate> predicates;
//  vector<int> assign_domain({ 1,2,3,5,6,7,9,10,11,12 });
//  vector<string> var_domain({ "count", "cenX", "cenY", "x", "y", "flag", "normSq" });
//
//  stmt_hashmap["a"] = assign_domain;
//  var_hashmap["v"] = var_domain;
//
//  pql::Synonym syn1("a", EntityIdentifier::kAssign);
//  pql::Synonym syn2("v", EntityIdentifier::kVariable);
//
//  pql_solver::Solver solver(&stmt_hashmap, &var_hashmap, &predicates, vector<pql::Synonym>({syn1, syn2}), &syn1);
//
//  vector<pair<int, string>> pred_merge_filter_lst({make_pair(1,"count"),make_pair(2,"cenX"),make_pair(3,"cenY"),make_pair(5,"count"),
//        make_pair(6,"cenX"),make_pair(7,"cenY"),make_pair(9,"flag"),make_pair(10,"cenX"),make_pair(11,"cenY"),make_pair(12,"normSq"),
//        make_pair(4,"cenX"),make_pair(4,"cenY"),make_pair(4,"count"), make_pair(8,"cenX"),make_pair(8,"cenY"),make_pair(8,"flag"),
//        make_pair(13, "x")});
//
//  pql_table::Predicate pred_merge_filter(syn1.GetName(), syn2.GetName(), pred_merge_filter_lst);
//
//  //before merging
//  pql_table::InterTable expected_table_idx0(syn1, assign_domain);
//  pql_table::InterTable expected_table_idx1(syn2, var_domain);
//
//  REQUIRE(solver.tables_.size() == 2); // the solver currently should have two tables
//  REQUIRE(solver.tables_[0].equal(expected_table_idx0));
//  REQUIRE(solver.tables_[1].equal(expected_table_idx1));
//
//  //Perform merge and filter
//  solver.Consume(pred_merge_filter);
//
//  vector<string> expected_table_after_merge_filter_header({"a", "v"});
//  vector<int> expected_table_after_merge_filter_col1({ 1, 2, 3, 5, 6, 7, 9, 10, 11, 12});
//  vector<string> expected_table_after_merge_filter_col2({ "count", "cenX", "cenY", "count", "cenX", "cenY", "flag", "cenX", "cenY", "normSq"});
//  vector<vector<pql_table::element>> expected_table_after_merge_filter_rows;
//  InitializeList(&expected_table_after_merge_filter_col1, &expected_table_after_merge_filter_col2, &expected_table_after_merge_filter_rows);
//  pql_table::InterTable expected_table_after_merge_filter(expected_table_after_merge_filter_header, expected_table_after_merge_filter_rows);
//
//  REQUIRE(solver.tables_.size() == 1);
//  REQUIRE(solver.tables_[0].equal(expected_table_after_merge_filter));
//
//  //Perform filter
//  vector<pair<int, string>> pred_filter_lst({ make_pair(6,"cenX"),make_pair(10,"cenX"),make_pair(12,"normSq") });
//  pql_table::Predicate pred_filter(syn1.GetName(), syn2.GetName(), pred_filter_lst);
//
//  solver.Consume(pred_filter);
//
//  vector<string> expected_table_after_filter_header({ "a", "v" });
//  vector<int> expected_table_after_filter_col1({ 6, 10, 12 });
//  vector<string> expected_table_after_filter_col2({ "cenX", "cenX", "normSq" });
//  vector<vector<pql_table::element>> expected_table_after_filter_rows;
//  InitializeList(&expected_table_after_filter_col1, &expected_table_after_filter_col2, &expected_table_after_filter_rows);
//  pql_table::InterTable expected_table_after_filter(expected_table_after_filter_header, expected_table_after_filter_rows);
//
//  REQUIRE(solver.tables_.size() == 1);
//  REQUIRE(solver.tables_[0].equal(expected_table_after_filter));
//}