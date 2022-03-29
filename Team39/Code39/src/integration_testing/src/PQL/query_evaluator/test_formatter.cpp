#include "../../../../spa/src/PQL/query_evaluator/formatter/formatter.h"
#include "../../../../spa/src/PQL/query_evaluator/cache/cache.h"
#include "../../../../spa/src/SP/design_extractor.h"
#include "test_util.h"

#include "catch.hpp"

Pkb pkb_formatter = std::move(InitializePkb());
pql_cache::Cache cache_formatter(&pkb_formatter);
pql_formatter::Formatter formatter(&cache_formatter);
/*
Similar to testing with clause, we split AttrRef into 3 groups: (all AttrRef in same group behaves the same)
1) stmt.stmt#, read.stmt#, print.stmt#, call.stmt#, while.stmt#, if.stmt#, assign.stmt#, constant.value
2) procedure.procName, variable.varName,
3) call.procName, read.varName, print.varName
*/
TEST_CASE("Check correctness when the tuple consists of AttrRef in different group") {
  //Select <stmt.stmt#, constant.value, procedure.procName, print.varName>
  pql::Synonym first_syn("s", EntityIdentifier::kStmt);
  pql::AttrRef first_attr_ref(first_syn, AttrIdentifier::kStmtNum);
  pql::Synonym second_syn("const", EntityIdentifier::kConstant);
  pql::AttrRef second_attr_ref(second_syn, AttrIdentifier::kValue);
  pql::Synonym third_syn("proc", EntityIdentifier::kProc);
  pql::AttrRef third_attr_ref(third_syn, AttrIdentifier::kProcName);
  pql::Synonym fourth_syn("pn", EntityIdentifier::kPrint);
  pql::AttrRef fourth_attr_ref(fourth_syn, AttrIdentifier::kVarName);

  vector<pql::AttrRef> return_attr_refs = { first_attr_ref, second_attr_ref, third_attr_ref, fourth_attr_ref };

  vector<string> headers = { "s", "const", "proc", "pn" };
  vector<vector<int>> rows = { {10, 5, 0, 6}, {10, 27, 1, 24}, {25, 1, 0, 25} };
  pql_table::InterTable table(headers, rows);

  vector<string> std_ans = { "10 5 main flag", "10 27 readPoint x", "25 1 main y" };

  vector<string> res = formatter.FormatRawInput(table, return_attr_refs);

  REQUIRE(res == std_ans);
}


TEST_CASE("Check correctness when the synonym appears multiple times") {
  //Select <call.stmt#, call.procName, call.stmt#>
  pql::Synonym first_syn("c", EntityIdentifier::kCall);
  pql::AttrRef first_attr_ref(first_syn, AttrIdentifier::kStmtNum);
  pql::Synonym second_syn("c", EntityIdentifier::kCall);
  pql::AttrRef second_attr_ref(second_syn, AttrIdentifier::kProcName);
  pql::Synonym third_syn("c", EntityIdentifier::kCall);
  pql::AttrRef third_attr_ref(third_syn, AttrIdentifier::kStmtNum);

  vector<pql::AttrRef> return_attr_refs = { first_attr_ref, second_attr_ref, third_attr_ref };

  vector<string> headers = { "c" };
  vector<vector<int>> rows = { {2}, {3}, {18}, {13} };
  pql_table::InterTable table(headers, rows);

  vector<string> std_ans = { "2 computeCentroid 2", "3 x 3", "18 readPoint 18", "13 readPoint 13"};

  vector<string> res = formatter.FormatRawInput(table, return_attr_refs);

  REQUIRE(res == std_ans);
}
