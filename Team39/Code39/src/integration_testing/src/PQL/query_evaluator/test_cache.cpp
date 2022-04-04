#include "../../../../spa/src/PQL/query_evaluator/cache/cache.h"
#include "../../../../spa/src/PKB/pkb.h"
#include "../../../../spa/src/Utility/CFG/control_flow_graph.h"
#include "../../../../spa/src/SP/design_extractor.h"

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
  pkb.AddInfoToTable(TableIdentifier::kConstant, 3, vector < int>{0});
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
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>{ "c" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>{ "b", "d" });

  //line 6 d = b + c
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 6);
  pkb.AddInfoToTable(TableIdentifier::kAssign, 6, "b + c");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 6, "b + c");
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 6, vector<string>{ "d" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>{ "b", "c" });

  //line 7 c = d * a
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 7);
  pkb.AddInfoToTable(TableIdentifier::kAssign, 7, "d * a");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 7, "d * a");
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 7, vector<string>{ "c" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{ "d", "a" });

  //line 8 b = d - c
  pkb.AddEntityToSet(EntityIdentifier::kAssign, 8);
  pkb.AddInfoToTable(TableIdentifier::kAssign, 8, "d - c");
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 8, "d - c");
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 8, vector<string>{ "b" });
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 8, vector<string>{ "d", "c" });

  //Add next relationship
  pkb.AddInfoToTable(TableIdentifier::kNext, 1, vector<int>{2});
  pkb.AddInfoToTable(TableIdentifier::kNext, 2, vector<int>{3});
  pkb.AddInfoToTable(TableIdentifier::kNext, 3, vector<int>{4, 5});
  pkb.AddInfoToTable(TableIdentifier::kNext, 4, vector<int>{6});
  pkb.AddInfoToTable(TableIdentifier::kNext, 5, vector<int>{6});
  pkb.AddInfoToTable(TableIdentifier::kNext, 6, vector<int>{7});
  pkb.AddInfoToTable(TableIdentifier::kNext, 7, vector<int>{8});
  pkb.AddInfoToTable(TableIdentifier::kNext, 8, vector<int>{2});

  //add cfg
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
  
  shared_ptr<cfg::GraphNode> head = cfg::CFG::GenerateCfg(tokens).GetHead();
  shared_ptr<cfg::CFG> cfg = make_shared<cfg::CFG>(head);
  pkb.AddCfg(cfg);

  PopulateNestedRelationships(pkb);

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
  
  shared_ptr<cfg::GraphNode> head = cfg::CFG::GenerateCfg(tokens).GetHead();
  pql_cache::Cache cache(&pkb1);


  unordered_set<pair<int, int>, pql_cache::hash_pair_fn> affects_ans_set = { make_pair(1, 4), make_pair(4, 6), make_pair(5, 6), make_pair(6, 7),
      make_pair(1, 7), make_pair(6, 8), make_pair(7, 8), make_pair(6, 5), make_pair(8, 5), make_pair(7, 6), make_pair(8, 6) };

  REQUIRE(cache.pair_cache_[pql::kAffects].empty()); //it is empty before the computation
  cache.ComputeAffectsRelationship(*head);
  REQUIRE(cache.pair_cache_[pql::kAffects] == affects_ans_set);
}

TEST_CASE("Checks the correctness of PopulateStarRelationship to populate the Star relationship") {
  SECTION("Next relationship") {
    //using the example above
    unordered_map<int, unordered_set<int>> next = {
      {1, {2}}, {2, {3}}, {3, {4, 5}}, {4, {6}}, {5, {6}}, {6, {7}}, {7, {8}}, {8, {2}}
    };

    unordered_map<int, unordered_set<int>> next_star_ans = {
      {1, {2, 3, 4, 5, 6, 7, 8}}, {2, {2, 3, 4, 5, 6, 7, 8}}, {3, {2, 3, 4, 5, 6, 7, 8}}, {4, {2, 3, 4, 5, 6, 7, 8}}, 
      {5, {2, 3, 4, 5, 6, 7, 8}}, {6, {2, 3, 4, 5, 6, 7, 8}}, {7, {2, 3, 4, 5, 6, 7, 8}}, {8, {2, 3, 4, 5, 6, 7, 8}}
    };

    unordered_map<int, unordered_set<int>> next_star;

    REQUIRE(next_star != next_star_ans);
    next_star = std::move(pql_cache::Cache::PopulateStarRelationship(next));
    REQUIRE(next_star == next_star_ans);
  }

  SECTION("Affects relationship") {
    unordered_map<int, unordered_set<int>> affects = {
      {1, {4, 7}}, {4, {6}}, {5, {6}}, {6, {7, 8, 5}}, {7, {8, 6}}, {8, {5, 6}}
    };

    unordered_map<int, unordered_set<int>> affects_star_ans = {
      {1, {4, 5, 6, 7, 8}}, {4, {5, 6, 7, 8}}, {5, {5, 6, 7, 8}}, 
      {6, {5, 6, 7, 8}}, {7, {5, 6, 7, 8}}, {8, {5, 6, 7, 8}}
    };

    unordered_map<int, unordered_set<int>> affects_star;

    REQUIRE(affects_star != affects_star_ans);
    affects_star = std::move(pql_cache::Cache::PopulateStarRelationship(affects));
    REQUIRE(affects_star == affects_star_ans);
  }
}

TEST_CASE("Check the correctness on generating the tables for Next*") {
  Pkb pkb1 = move(InitializePkb1());
  pql_cache::Cache cache(&pkb1);

  SECTION("Check Relation domain") {
    unordered_map<int, unordered_set<int>> next_star_ans = {
      {1, {2, 3, 4, 5, 6, 7, 8}}, {2, {2, 3, 4, 5, 6, 7, 8}}, {3, {2, 3, 4, 5, 6, 7, 8}}, {4, {2, 3, 4, 5, 6, 7, 8}}, 
      {5, {2, 3, 4, 5, 6, 7, 8}}, {6, {2, 3, 4, 5, 6, 7, 8}}, {7, {2, 3, 4, 5, 6, 7, 8}}, {8, {2, 3, 4, 5, 6, 7, 8}}
    };

    REQUIRE(cache.rel_cache_[pql::kNextT].empty());
    cache.GenerateNextTOrAffectsTRelDomain(pql::kNextT);
    REQUIRE(cache.rel_cache_[pql::kNextT] == next_star_ans);
  }


  SECTION("Check Inverse Relationship") {
    unordered_map<int, unordered_set<int>> inverse_next_star_ans = {
      {2, {1, 2, 3, 4, 5, 6, 7, 8}}, {3, {1, 2, 3, 4, 5, 6, 7, 8}}, {4, {1, 2, 3, 4, 5, 6, 7, 8}}, 
      {5, {1, 2, 3, 4, 5, 6, 7, 8}}, {6, {1, 2, 3, 4, 5, 6, 7, 8}}, {7, {1, 2, 3, 4, 5, 6, 7, 8}}, {8, {1, 2, 3, 4, 5, 6, 7, 8}}
    };

    REQUIRE(cache.inverse_rel_cache_[pql::kNextT].empty());
    cache.GenerateNextTOrAffectsTInverseRelDomain(pql::kNextT);
    REQUIRE(cache.inverse_rel_cache_[pql::kNextT] == inverse_next_star_ans);
  }

  SECTION("Check Pair Relation domain") {
    unordered_set<pair<int, int>, pql_cache::hash_pair_fn> next_star_pair_ans = {
      make_pair(1, 2), make_pair(1, 3), make_pair(1, 4), make_pair(1, 5), make_pair(1, 6), make_pair(1, 7), make_pair(1, 8),
      make_pair(2, 2), make_pair(2, 3), make_pair(2, 4), make_pair(2, 5), make_pair(2, 6), make_pair(2, 7), make_pair(2, 8),
      make_pair(3, 2), make_pair(3, 3), make_pair(3, 4), make_pair(3, 5), make_pair(3, 6), make_pair(3, 7), make_pair(3, 8),
      make_pair(4, 2), make_pair(4, 3), make_pair(4, 4), make_pair(4, 5), make_pair(4, 6), make_pair(4, 7), make_pair(4, 8),
      make_pair(5, 2), make_pair(5, 3), make_pair(5, 4), make_pair(5, 5), make_pair(5, 6), make_pair(5, 7), make_pair(5, 8),
      make_pair(6, 2), make_pair(6, 3), make_pair(6, 4), make_pair(6, 5), make_pair(6, 6), make_pair(6, 7), make_pair(6, 8),
      make_pair(7, 2), make_pair(7, 3), make_pair(7, 4), make_pair(7, 5), make_pair(7, 6), make_pair(7, 7), make_pair(7, 8),
      make_pair(8, 2), make_pair(8, 3), make_pair(8, 4), make_pair(8, 5), make_pair(8, 6), make_pair(8, 7), make_pair(8, 8)
    };

    REQUIRE(cache.pair_cache_[pql::kNextT].empty());
    cache.GenerateNextTOrAffectsTPairDomain(pql::kNextT);
    REQUIRE(cache.pair_cache_[pql::kNextT] == next_star_pair_ans);
  }
}

TEST_CASE("Check the correctness on generating the tables for Affect") {
  Pkb pkb1 = move(InitializePkb1());
  pql_cache::Cache cache(&pkb1);

  SECTION("Check Pair Relation domain") {
    unordered_set<pair<int, int>, pql_cache::hash_pair_fn> affects_star_pair_ans = {
      make_pair(1, 4), make_pair(4, 6), make_pair(5, 6), make_pair(6, 7), make_pair(1, 7), make_pair(6, 8), 
      make_pair(7, 8), make_pair(6, 5), make_pair(8, 5), make_pair(7, 6), make_pair(8, 6)
    };

    REQUIRE(cache.pair_cache_[pql::kAffects].empty());
    cache.GenerateAffectsPairDomain(pql::kAffects);
    REQUIRE(cache.pair_cache_[pql::kAffects] == affects_star_pair_ans);
  }

  SECTION("Check Relation domain") {
    unordered_map<int, unordered_set<int>> affects_ans = {
      {1, {4, 7}}, {4, {6}}, {5, {6}}, {6, {7, 8, 5}}, {7, {8, 6}}, {8, {5, 6}}
    };

    REQUIRE(cache.rel_cache_[pql::kAffects].empty());
    cache.GenerateAffectsRelDomain(pql::kAffects);
    REQUIRE(cache.rel_cache_[pql::kAffects] == affects_ans);
  }


  SECTION("Check Inverse Relationship") {
    unordered_map<int, unordered_set<int>> inverse_affects_ans = {
      {4, {1}}, {5, {6,8}}, {6, {4, 5, 7, 8}}, {7, {1, 6}}, {8, {6, 7}}
    };

    REQUIRE(cache.inverse_rel_cache_[pql::kAffects].empty());
    cache.GenerateAffectsInverseRelDomain(pql::kAffects);
    REQUIRE(cache.inverse_rel_cache_[pql::kAffects] == inverse_affects_ans);
  }
}

TEST_CASE("Check the correctness on generating the tables for Affects*") {
  Pkb pkb1 = move(InitializePkb1());
  pql_cache::Cache cache(&pkb1);

  SECTION("Check Relation domain") {
    unordered_map<int, unordered_set<int>> affects_star_ans = {
      {1, {4, 5, 6, 7, 8}}, {4, {5, 6, 7, 8}}, {5, {5, 6, 7, 8}},
      {6, {5, 6, 7, 8}}, {7, {5, 6, 7, 8}}, {8, {5, 6, 7, 8}}
    };

    REQUIRE(cache.rel_cache_[pql::kAffectsT].empty());
    cache.GenerateNextTOrAffectsTRelDomain(pql::kAffectsT);
    REQUIRE(cache.rel_cache_[pql::kAffectsT] == affects_star_ans);
  }


  SECTION("Check Inverse Relationship") {
    unordered_map<int, unordered_set<int>> inverse_affects_star_ans = {
      {4, {1}}, {5, {1, 4, 5, 6, 7, 8}}, {6, {1, 4, 5, 6, 7, 8}}, {7, {1, 4, 5, 6, 7, 8}}, {8, {1, 4, 5, 6, 7, 8}}
    };

    REQUIRE(cache.inverse_rel_cache_[pql::kAffectsT].empty());
    cache.GenerateNextTOrAffectsTInverseRelDomain(pql::kAffectsT);
    REQUIRE(cache.inverse_rel_cache_[pql::kAffectsT] == inverse_affects_star_ans);
  }

  SECTION("Check Pair Relation domain") {
    unordered_set<pair<int, int>, pql_cache::hash_pair_fn> affects_star_pair_ans = {
      make_pair(1, 4), make_pair(1, 5), make_pair(1, 6), make_pair(1, 7), make_pair(1, 8),
      make_pair(4, 5), make_pair(4, 6), make_pair(4, 7), make_pair(4, 8),
      make_pair(5, 5), make_pair(5, 6), make_pair(5, 7), make_pair(5, 8),
      make_pair(6, 5), make_pair(6, 6), make_pair(6, 7), make_pair(6, 8),
      make_pair(7, 5), make_pair(7, 6), make_pair(7, 7), make_pair(7, 8),
      make_pair(8, 5), make_pair(8, 6), make_pair(8, 7), make_pair(8, 8)
    };

    REQUIRE(cache.pair_cache_[pql::kAffectsT].empty());
    cache.GenerateNextTOrAffectsTPairDomain(pql::kAffectsT);
    REQUIRE(cache.pair_cache_[pql::kAffectsT] == affects_star_pair_ans);
  }
}
