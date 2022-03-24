#include "../../../../spa/src/PQL/query_evaluator/cache/cache.h"
#include "../../../../spa/src/PKB/pkb.h"
#include "../../../../spa/src/Utility/CFG/control_flow_graph.h"

#include "catch.hpp"

using namespace std;

TEST_CASE("Test MergeTable function in cache") {
  unordered_map<int, unordered_set<int>> dst = {
		{ 1, { 1, 2 } },
		{ 2, { 3 } }
  };

	unordered_map<int, unordered_set<int>> src = {
	  { 1, { 1, 3, 4 } },
		{ 3, { 5 } }
	};

	unordered_map<int, unordered_set<int>> compare = {
		{ 1, { 1, 2, 3, 4 } },
		{ 2, { 3 } },
		{ 3, { 5 } }
	};
	
	REQUIRE(dst != compare);
	pql_cache::Cache::MergeTable(dst, src);
	REQUIRE(dst == compare);
}

Pkb InitializePkb1() {
  /*
  01  a = b * c + d;
  02  while (c > 0){
  03  if (d > 0) then {
  04		 b = a - 1;}
  		else {
  05		 c = b * d;}
  06	 d = b + c;
  07	 c = d * a;
  08	 b = d - c;}}
  */

  Pkb pkb;

  //add variable 
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "a");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "b");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "c");
  pkb.AddEntityToSet(EntityIdentifier::kVariable, "d");

  //line 1
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 1);	

  pkb.AddInfoToTable(TableIdentifier::kAssign, 1, "b * c + d");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 1, "b * c + d");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 1, vector<int>({}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 1, vector<string>{ "a" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 1, vector<string>{ "b", "c", "d" });

  //line 2 while (c > 0)
  pkb.AddEntityToSet(EntityIdentifier::kWhile, 2);
  pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);

  pkb.AddInfoToTable(TableIdentifier::kWhile, 2, vector<string>({ "c" }));
  pkb.AddInfoToTable(TableIdentifier::kConstant, 2, vector<int>({ 0 }));
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 2, vector<string>({ "c" }));

  //line 3 if (d > 0)
  pkb.AddEntityToSet(EntityIdentifier::kIf, 3);
  pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);

  pkb.AddInfoToTable(TableIdentifier::kIf, 3, vector<string>{"d"});
  pkb.AddInfoToTable(TableIdentifier::kConstant, 3, 0);
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 3, vector<string>{"d"});

  //line 4
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 4);

  pkb.AddInfoToTable(TableIdentifier::kAssign, 4, "a - 1");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 4, "a - 1");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 4, vector<int>({1}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 4, vector<string>{ "b" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 4, vector<string>{ "a" });

  //line 5 c = b * d
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 5);

  pkb.AddInfoToTable(TableIdentifier::kAssign, 5, "b * d");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 5, "b * d");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 5, vector<int>({}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>{ "c" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>{ "b", "d" });

  //line 6 d = b + c
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 6);

  pkb.AddInfoToTable(TableIdentifier::kAssign, 6, "b + c");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 6, "b + c");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 6, vector<int>({}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 6, vector<string>{ "d" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>{ "b", "c" });

  //line 7 c = d * a
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 7);

  pkb.AddInfoToTable(TableIdentifier::kAssign, 7, "d * a");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 7, "d * a");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 7, vector<int>({}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 7, vector<string>{ "c" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{ "d", "a" });

  //line 8 b = d - c
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 8);

  pkb.AddInfoToTable(TableIdentifier::kAssign, 8, "d - c");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 8, "d - c");
  pkb.AddInfoToTable(TableIdentifier::kConstant, 8, vector<int>({}));
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 8, vector<string>{ "b" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 8, vector<string>{ "d", "c" });

	return pkb;
}

TEST_CASE("Checks the correctness of constructing Affects relationship") {
  Pkb pkb1 = move(InitializePkb1());
  vector<CFGToken> tokens({
    CFGToken(),
    CFGToken(CFGTokenType::kAssign, 1),
    CFGToken(CFGTokenType::kWhile, 2),
    CFGToken(CFGTokenType::kIf, 3),
    CFGToken(CFGTokenType::kAssign, 4),
    CFGToken(CFGTokenType::kThenEnd, 0),
    CFGToken(CFGTokenType::kAssign, 5),
    CFGToken(CFGTokenType::kElseEnd, 0),
    CFGToken(CFGTokenType::kAssign, 6),
    CFGToken(CFGTokenType::kAssign, 7),
    CFGToken(CFGTokenType::kAssign, 8),
    CFGToken(CFGTokenType::kWhileEnd, 0),
    CFGToken(CFGTokenType::kEnd, 0)
    });
  
  shared_ptr<GraphNode> head = CFG::GenerateCfg(tokens);
  pql_cache::Cache cache(pkb1);

  vector<pair<int, int>> affect_lst = cache.ComputeAffectsRelationship(*head);

  vector<pair<int, int>> ans = { make_pair(1, 4), make_pair(4, 6), make_pair(5, 6), make_pair(6, 7),
      make_pair(1, 7), make_pair(6, 8), make_pair(7, 8), make_pair(6, 5), make_pair(8, 5), make_pair(7, 6), make_pair(8, 6) };

  unordered_set<pair<int,int>, pql_cache::hash_pair_fn> affect_set(affect_lst.begin(), affect_lst.end());
  unordered_set<pair<int,int>, pql_cache::hash_pair_fn> ans_set(ans.begin(), ans.end());

  REQUIRE(affect_set == ans_set);

}
