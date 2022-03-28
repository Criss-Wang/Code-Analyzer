#include "../../spa/src/PKB/pkb.h"
#include "../../spa/src/SP/design_extractor.h"
#include "../../spa/src/Utility/entity.h"
#include "../../spa/src/PQL/query_evaluator/solver/predicate.h"

// This file contains the pkb that are going to be used in the test files in query_evaluator
// The SIMPLE program we will be extracting from (taken from CS3203 wiki):
/* 
  procedure main {
01      flag = 7;
02      call computeCentroid;
03      call x;
    }
    procedure readPoint {
04      read x;
05      read y;
    }
    procedure x {
06      print flag;
07      print cenX;
08      print flag;
09      print cenX;
    }
    procedure computeCentroid {
10      count = 5;
11      cenX = 20;
12      cenY = 25;
13      call readPoint;
14      while ((x != 0) && (y != 0)) {
15          count = count + 1;
16          cenX = cenX + x;
17          cenY = cenY + y;
18          call readPoint;
        }
19      if (count == 0) then {
20          flag = 1;
        } else {
21          cenX = cenX / count;
22          cenY = cenY / count;
        }
23      normSq = cenX * cenX + cenY * cenY;
24      print x;
25      print y;
    }
*/

using namespace std;

static Pkb InitializePkb() {
    Pkb pkb;

    //Add all procedures
    pkb.AddEntityToSet(EntityIdentifier::kProc, "main");
    pkb.AddEntityToSet(EntityIdentifier::kProc, "readPoint");
    pkb.AddEntityToSet(EntityIdentifier::kProc, "x");
    pkb.AddEntityToSet(EntityIdentifier::kProc, "computeCentroid");

    //Add all variables
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "count");
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "flag");
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "x");
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "y");
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenX");
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "cenY");
    pkb.AddEntityToSet(EntityIdentifier::kVariable, "normSq");

    //Add all constants
    pkb.AddEntityToSet(EntityIdentifier::kConstant, 0);
    pkb.AddEntityToSet(EntityIdentifier::kConstant, 1);
    pkb.AddEntityToSet(EntityIdentifier::kConstant, 5);
    pkb.AddEntityToSet(EntityIdentifier::kConstant, 7);
    pkb.AddEntityToSet(EntityIdentifier::kConstant, 20);
    pkb.AddEntityToSet(EntityIdentifier::kConstant, 25);

    //line 1 flag = 7
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 1);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 1);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 1, "7");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 1, "7");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 1, vector<int>({ 7 }));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 1, vector<string>{ "flag" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 1, vector<string>{});

    //line 2 call computeCentroid
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 2);
    pkb.AddEntityToSet(EntityIdentifier::kCall, 2);
    pkb.AddInfoToTable(TableIdentifier::kCaller, 2, "computeCentroid");

    //line 3 call printResults
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 3);
    pkb.AddEntityToSet(EntityIdentifier::kCall, 3);
    pkb.AddInfoToTable(TableIdentifier::kCaller, 3, "x");

    //line 4 read x
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 4);
    pkb.AddEntityToSet(EntityIdentifier::kRead, 4);
    pkb.AddInfoToTable(TableIdentifier::kRead, 4, "x");
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 4, vector<string>{"x"});

    //line 5 read y
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 5);
    pkb.AddEntityToSet(EntityIdentifier::kRead, 5);
    pkb.AddInfoToTable(TableIdentifier::kRead, 5, "y");
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 5, vector<string>{"y"});

    //line 6 print flag
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 6);
    pkb.AddEntityToSet(EntityIdentifier::kPrint, 6);
    pkb.AddInfoToTable(TableIdentifier::kPrint, 6, "flag");
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 6, vector<string>{ "flag" });

    //line 7 print cenX
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 7);
    pkb.AddEntityToSet(EntityIdentifier::kPrint, 7);
    pkb.AddInfoToTable(TableIdentifier::kPrint, 7, "cenX");
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 7, vector<string>{ "cenX" });

    //line 8 print cenY
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 8);
    pkb.AddEntityToSet(EntityIdentifier::kPrint, 8);
    pkb.AddInfoToTable(TableIdentifier::kPrint, 8, "flag");
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 8, vector<string>{ "flag" });

    //line 9 print normSq
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 9);
    pkb.AddEntityToSet(EntityIdentifier::kPrint, 9);
    pkb.AddInfoToTable(TableIdentifier::kPrint, 9, "cenX");
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 9, vector<string>{ "cenX" });

    //line 10 count = 5
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 10);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 10);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 10, "5");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 10, "5");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 10, vector<int>({ 5 }));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 10, vector<string>{});
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 10, vector<string>{});  

    //line 11 cenX = 20
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 11);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 11);    
    pkb.AddInfoToTable(TableIdentifier::kAssign, 11, "20");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 11, "20");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 11, vector<int>({ 20 }));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 11, vector<string>{});
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 11, vector<string>{});

    //line 12 cenY = 25
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 12);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 12);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 12, "25");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 12, "25");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 12, vector<int>({ 25 }));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 12, vector<string>{});
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 12, vector<string>{});

    //line 13 call readPoint
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 13);
    pkb.AddEntityToSet(EntityIdentifier::kCall, 13);
    pkb.AddInfoToTable(TableIdentifier::kCaller, 13, "readPoint");

    //line 14 while ((x!=0) && (y!=0))
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 14);
    pkb.AddEntityToSet(EntityIdentifier::kWhile, 14);
    pkb.AddInfoToTable(TableIdentifier::kWhile, 14, vector<string>({ "x","y" }));
    pkb.AddInfoToTable(TableIdentifier::kConstant, 14, vector<int>({ 0 }));
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 14, vector<string>({ "x","y" }));

    //line 15 count = count + 1
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 15);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 15);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 15, "count + 1");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 15, "count + 1");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 15, vector<int>({1}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 15, vector<string>{ "count" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 15, vector<string>{ "count" });

    //line 16 cenX = cenX + x
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 16);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 16);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 16, "cenX + x");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 16, "cenX + x");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 16, vector<int>({}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 16, vector<string>{ "cenX" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 16, vector<string>{ "cenX", "x" });

    //line 17 cenY = cenY + y
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 17);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 17);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 17, "cenY + y");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 17, "cenY + y");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 17, vector<int>({}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 17, vector<string>{ "cenY" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 17, vector<string>{ "cenY", "y" });

    //line 18 call readPoint
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 18);
    pkb.AddEntityToSet(EntityIdentifier::kCall, 18);
    pkb.AddInfoToTable(TableIdentifier::kCaller, 18, "readPoint");

    //line 19 if (count == 0)
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 19);
    pkb.AddEntityToSet(EntityIdentifier::kIf, 19);
    pkb.AddInfoToTable(TableIdentifier::kIf, 19, vector<string>{"count"});
    pkb.AddInfoToTable(TableIdentifier::kConstant, 19, vector<int>{ 0 });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 19, vector<string>{"count"});

    //line 20 flag = 1
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 20);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 20);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 20, "1");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 20, "1");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 20, vector<int>({1}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 20, vector<string>{ "flag" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 20, vector<string>{});
        
    //line 21 cenX = cenX / count
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 21);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 21);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 21, "cenX / count");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 21, "cenX / count");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 21, vector<int>({}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 21, vector<string>{ "cenX" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 21, vector<string>{ "cenX", "count" });

    //line 22 cenY = cenY / count 
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 22);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 22);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 22, "cenY / count");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 22, "cenY / count");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 22, vector<int>({}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 22, vector<string>{ "cenY" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 22, vector<string>{ "cenY", "count" });
    
    //line 23 normSq = cenX * cenX + cenY * cenY
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 23);
    pkb.AddEntityToSet(EntityIdentifier::kAssign, 23);
    pkb.AddInfoToTable(TableIdentifier::kAssign, 23, "cenX * cenX + cenY * cenY");
    pkb.AddInfoToTable(TableIdentifier::kAssignPattern, 23, "cenX * cenX + cenY * cenY");
    pkb.AddInfoToTable(TableIdentifier::kConstant, 23, vector<int>({}));
    pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, 23, vector<string>{ "normSq" });
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 23, vector<string>{ "cenX", "cenY" });
    
    //line 24 print x
    pkb.AddEntityToSet(EntityIdentifier::kStmt,24);
    pkb.AddEntityToSet(EntityIdentifier::kPrint, 24);
    pkb.AddInfoToTable(TableIdentifier::kPrint, 24, "x");
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 24, vector<string>{ "x" });
    
    //line 25 print y
    pkb.AddEntityToSet(EntityIdentifier::kStmt, 25);
    pkb.AddEntityToSet(EntityIdentifier::kPrint, 25);
    pkb.AddInfoToTable(TableIdentifier::kPrint, 25, "y");
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, 25, vector<string>{ "y" });

    //add Follows relationship
    pkb.AddInfoToTable(TableIdentifier::kFollows, 1, 2);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 2, 3);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 4, 5);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 5, 6);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 6, 7);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 7, 8);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 8, 9);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 10, 11);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 11, 12);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 12, 13);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 13, 14);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 14, 19);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 15, 16);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 16, 17);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 17, 18);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 19, 23);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 23, 24);
    pkb.AddInfoToTable(TableIdentifier::kFollows, 24, 25);

    //add Parent relationship
    pkb.AddInfoToTable(TableIdentifier::kParent, 14, vector<int>({15, 16, 17, 18}));
    pkb.AddInfoToTable(TableIdentifier::kParent, 19, vector<int>({20, 21, 22}));

    //Add procedure calls relationship
    pkb.AddInfoToTable(TableIdentifier::kCalls, "main", vector<string>({ "computeCentroid", "printResults"}));
    pkb.AddInfoToTable(TableIdentifier::kCalls, "computeCentroid", vector<string>({ "readPoint" }));

    //Add procedure modifies relationship
    pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, "main", vector<string>({ "flag" }));
    pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, "readPoint", vector<string>({ "x", "y" }));
    pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, "computeCentroid", vector<string>({ "count", "cenX", "cenY", "flag", "normSq"}));

    //Add procedure uses relationship
    pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "x", vector<string>({ "flag", "cenX" }));
    pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, "computeCentroid", vector<string>({ "count", "cenX", "x", "cenY", "y" }));

    //Add Next relationship
    pkb.AddInfoToTable(TableIdentifier::kNext, 1, vector<int>{2});
    pkb.AddInfoToTable(TableIdentifier::kNext, 2, vector<int>{3});
    pkb.AddInfoToTable(TableIdentifier::kNext, 4, vector<int>{5});
    pkb.AddInfoToTable(TableIdentifier::kNext, 6, vector<int>{7});
    pkb.AddInfoToTable(TableIdentifier::kNext, 7, vector<int>{8});
    pkb.AddInfoToTable(TableIdentifier::kNext, 8, vector<int>{9});
    pkb.AddInfoToTable(TableIdentifier::kNext, 10, vector<int>{11});
    pkb.AddInfoToTable(TableIdentifier::kNext, 11, vector<int>{12});
    pkb.AddInfoToTable(TableIdentifier::kNext, 12, vector<int>{13});
    pkb.AddInfoToTable(TableIdentifier::kNext, 13, vector<int>{14});
    pkb.AddInfoToTable(TableIdentifier::kNext, 14, vector<int>{15, 19});
    pkb.AddInfoToTable(TableIdentifier::kNext, 15, vector<int>{16});
    pkb.AddInfoToTable(TableIdentifier::kNext, 16, vector<int>{17});
    pkb.AddInfoToTable(TableIdentifier::kNext, 17, vector<int>{18});
    pkb.AddInfoToTable(TableIdentifier::kNext, 18, vector<int>{14});
    pkb.AddInfoToTable(TableIdentifier::kNext, 19, vector<int>{20, 21});
    pkb.AddInfoToTable(TableIdentifier::kNext, 20, vector<int>{23});
    pkb.AddInfoToTable(TableIdentifier::kNext, 21, vector<int>{22});
    pkb.AddInfoToTable(TableIdentifier::kNext, 22, vector<int>{23});
    pkb.AddInfoToTable(TableIdentifier::kNext, 23, vector<int>{24});
    pkb.AddInfoToTable(TableIdentifier::kNext, 24, vector<int>{25});

    //populate transitive relationship
    PopulateNestedRelationships(pkb);

    return pkb;
}

static bool ComparePredicates(vector<pql_table::Predicate> p1, vector<pql_table::Predicate> p2) {
  if (p1.size() != p2.size()) {
    return false;
  }

  for (int index = 0; index < p1.size(); index++) {
    if (!p1[0].equal(p2[0])) {
      return false;
    }
  }

  return true;
}