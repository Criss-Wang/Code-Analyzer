//#include "../../spa/src/PKB/pkb.h"
//#include "../../spa/src/SP/design_extractor.h"
//#include "../../spa/src/Utility/entity.h"
//#include "../../spa/src/PQL/query_evaluator/predicate.h"
//
//// This file contains the pkb that are going to be used in the test files in query_evaluator
//// The SIMPLE program we will be extracting from (taken from CS3203 wiki):
///*
//procedure computeCentroid {
//01      count = 0;
//02      cenX = 0;
//03      cenY = 0;
//04      while ((x != 0) && (y != 0)) {
//05          count = count + 1;
//06          cenX = cenX + x;
//07          cenY = cenY + y;
//        }
//08      if (count == 0) then {
//09          flag = 1;
//        } else {
//10          cenX = cenX / count;
//11          cenY = cenY / count;
//        }
//12      normSq = cenX * cenX + cenY * cenY;
//13      read x;
//14      print y;
//    }
//*/
//
//using namespace std;
//
//static Pkb pkb;
//
//static void InitializePkb() {
//    // add all statements first
//    for (int stmt_num = 1; stmt_num <= 14; stmt_num++) {
//      pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
//    }
//
//    //line 1
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 1);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "count");
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 1, "0");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 1, "0");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 1, vector<int>({ 0 }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 1, vector<string>{ "count" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 1, vector<string>{});  
//
//    //line 2
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 2);
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenX");
//    
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 2, "0");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 2, "0");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 2, vector<int>({ 0 }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 2, vector<string>{ "cenX" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 2, vector<string>{});
//
//    //line 3
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 3);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenY");
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 3, "0");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 3, "0");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 3, vector<int>({ 0 }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 3, vector<string>{ "cenY" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 3, vector<string>{});
//
//    //line 4
//    pkb.AddEntityToSet(EntityIdentifier::kWhile, 4);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);
//
//    pkb.AddInfoToTable(TableIdentifier::kWhile, 4, vector<string>({ "x","y" }));
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 4, vector<int>({ 0 }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 4, vector<string>({ "x","y" }));
//
//    //line 5
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 5);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "count");
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 1);
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 5, "count + 1");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 5, "count + 1");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 5, vector<int>({1}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>{ "count" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>{ "count" });
//
//    //line 6
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 6);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenX");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 6, "cenX + x");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 6, "cenX + x");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 6, vector<int>({}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 6, vector<string>{ "cenX" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>{ "cenX", "x" });
//
//    //line 7
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 7);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenY");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 7, "cenY + y");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 7, "cenY + y");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 7, vector<int>({}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 7, vector<string>{ "cenY" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{ "cenY", "y" });
//
//    //line 8
//    pkb.AddEntityToSet(EntityIdentifier::kIf, 8);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "count");
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);
//
//    pkb.AddInfoToTable(TableIdentifier::kIf, 8, vector<string>{"count"});
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 8, 0);
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 8, vector<string>{"count"});
//
//    //line 9
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 9);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "flag");
//    pkb.AddEntityToSet(EntityIdentifier::kConstant, 1);
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 9, "1");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 9, "1");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 9, vector<int>({1}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 9, vector<string>{ "flag" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 9, vector<string>{});
//        
//    //line 10
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 10);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenX");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "count");
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 10, "cenX / count");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 10, "cenX / count");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 10, vector<int>({}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 10, vector<string>{ "cenX" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 10, vector<string>{ "cenX", "count" });
//
//    //line 11
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 11);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenY");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "count");
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 11, "cenY / count");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 11, "cenY / count");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 11, vector<int>({}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 11, vector<string>{ "cenY" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 11, vector<string>{ "cenY", "count" });
//    
//    //line 12
//    pkb.AddEntityToSet(EntityIdentifier::kAssign, 12);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "normSq");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenX");
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenY");
//
//    pkb.AddInfoToTable(TableIdentifier::kAssign, 12, "cenX * cenX + cenY * cenY");
//    pkb.AddInfoToTable(TableIdentifier::kPattern, 12, "cenX * cenX + cenY * cenY");
//    pkb.AddInfoToTable(TableIdentifier::kConstant, 12, vector<int>({}));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 12, vector<string>{ "normSq" });
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 10, vector<string>{ "cenX", "cenY" });
//
//    //line 13
//    pkb.AddEntityToSet(EntityIdentifier::kRead, 13);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
//
//    pkb.AddInfoToTable(TableIdentifier::kRead, 13, "x");
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 13, vector<string>{"x"});
//
//    //line 14
//    pkb.AddEntityToSet(EntityIdentifier::kPrint, 14);
//    pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
//
//    pkb.AddInfoToTable(TableIdentifier::kPrint, 14, "y");
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 14, vector<string>{ "y" });
//    
//    //add Follows relationship
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 1, 2);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 3);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 3, 4);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 5, 6);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 6, 7);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 4, 8);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 10, 11);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 8, 12);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 12, 13);
//    pkb.AddInfoToTable(TableIdentifier::kFollows, 13, 14);
//
//    //add Parent relationship
//    pkb.AddInfoToTable(TableIdentifier::kParent, 4, vector<int>({5, 6, 7}));
//    pkb.AddInfoToTable(TableIdentifier::kParent, 8, vector<int>({ 9,10,11 }));
//
//    //add Uses relationship
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 4, vector<string>({ "x", "y" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 5, vector<string>({ "count" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>({ "cenX", "x" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>({ "cenY", "y" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 8, vector<string>({ "count" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 10, vector<string>({ "cenX", "count" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 11, vector<string>({ "cenY", "count" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 12, vector<string>({ "cenX", "cenY" }));
//    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 14, vector<string>({ "y" }));
//
//    //add Modifies relationship
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 1, vector<string>({ "count" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 2, vector<string>({ "cenX" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 3, vector<string>({ "cenY" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>({ "count" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 6, vector<string>({ "cenX" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 7, vector<string>({ "cenY" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 9, vector<string>({ "flag" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 10, vector<string>({ "cenX" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 11, vector<string>({ "cenY" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 12, vector<string>({ "normSq" }));
//    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 13, vector<string>({ "x" }));
//
//
//    //populate transitive relationship
//    PopulateNestedRelationships(pkb);
//}
//
//static bool ComparePredicates(vector<pql_table::Predicate> p1, vector<pql_table::Predicate> p2) {
//  if (p1.size() != p2.size()) {
//    return false;
//  }
//
//  for (int index = 0; index < p1.size(); index++) {
//    if (!p1[0].equal(p2[0])) {
//      return false;
//    }
//  }
//
//  return true;
//}