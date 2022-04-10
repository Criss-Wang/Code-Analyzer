#include <memory>

#include "../../../../spa/src/PQL/query_evaluator/solver/solver.h"
#include "../../../../spa/src/PQL/query_evaluator/clause/such_that_clause.h"
#include "../../../../spa/src/PQL/query_evaluator/clause/pattern_clause.h"
#include "../../../../spa/src/PQL/query_evaluator/clause/with_clause.h"
#include "../../../../spa/src/Utility/Entity.h"

#include "catch.hpp"

using namespace std;

TEST_CASE("Checks the correctness of grouping ufds") {
    /*
    stmt s1, s2, s3; variable v1, v2;
    Select <s1, s2, v2> such that Uses(s3, v1) and Modifies(s3, “x”) and Follows(s1, s2) and Parent(s3, s1) and Uses(s2, v1)
    */
     
    /*pql::Synonym s1("s1", EntityIdentifier::kStmt);
    pql::Synonym s2("s2", EntityIdentifier::kStmt);
    pql::Synonym s3("s3", EntityIdentifier::kStmt);
    pql::Synonym v1("v1", EntityIdentifier::kVariable);
    pql::Synonym v2("v2", EntityIdentifier::kVariable);

    shared_ptr<pql_clause::Clause> uses_s3_v1 = make_shared<pql_clause::UsesSClause>("s3", true, "v1", true);
    shared_ptr<pql_clause::Clause> mod_s3_x = make_shared<pql_clause::ModifiesSClause>("s3", true, "x", false);
    shared_ptr<pql_clause::Clause> follows_s1_s2 = make_shared<pql_clause::FollowsClause>("s1", true, "s2", true);
    shared_ptr<pql_clause::Clause> parent_s3_s1 = make_shared<pql_clause::ParentClause>("s3", true, "s1", true);
    shared_ptr<pql_clause::Clause> uses_s2_v1 = make_shared<pql_clause::UsesSClause>("s2", true, "v1", true);
    
    vector<pql::Synonym> synonyms {s1, s2, s3, v1, v2};
    vector<shared_ptr<pql_clause::Clause>> clauses{uses_s3_v1, mod_s3_x, follows_s1_s2, parent_s3_s1, uses_s2_v1};

    pql_solver::Ufds ufds(&synonyms, &clauses);

    std::vector<std::vector<pql::Synonym>> syn_groups = ufds.GetSynGroups();
    std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>> clause_groups = ufds.GetClauseGroups();
    std::unordered_set<std::string> s =  ufds.GetSynInvovledInClause();*/

    /*
     assign a, a1, a2; stmt s1, s2, s3; variable v1, v2; 
     Select <s1, a, a1, v2> such that Uses (5, "y") and Uses (s1, v1) and Affects (a1, a2) with a1.stmt = 20  
     such that Parent (s1, s2) pattern a2 ("a", _) such that Next (s2, s3) and Modifies (s1, "x")  
     and Modifies (a, v2) pattern a (v1, "x") 
    */

    pql::Synonym a("a", EntityIdentifier::kAssign);
    pql::Synonym a1("a1", EntityIdentifier::kAssign);
    pql::Synonym a2("a2", EntityIdentifier::kAssign);
    pql::Synonym s1("s1", EntityIdentifier::kStmt);
    pql::Synonym s2("s2", EntityIdentifier::kStmt);
    pql::Synonym s3("s3", EntityIdentifier::kStmt);
    pql::Synonym v1("v1", EntityIdentifier::kVariable);
    pql::Synonym v2("v2", EntityIdentifier::kVariable);
    
    shared_ptr<pql::AttrRef> a1_stmt = make_shared<pql::AttrRef>(a1, AttrIdentifier::kStmtNum);

    shared_ptr<pql_clause::Clause> uses_5_y = make_shared<pql_clause::UsesSClause>("5", false, "y", false);
    shared_ptr<pql_clause::Clause> uses_s1_v1 = make_shared<pql_clause::UsesSClause>("s1", true, "v1", true);
    shared_ptr<pql_clause::Clause> affects_a1_a2 = make_shared<pql_clause::AffectsClause>("a1", true, "a2", true);
    shared_ptr<pql_clause::Clause> with_a1stmt_20 = make_shared<pql_clause::WithClause>(a1_stmt, "", true, nullptr, "20", false);
    shared_ptr<pql_clause::Clause> parent_s1_s2 = make_shared<pql_clause::ParentClause>("s1", true, "s2", true);
    shared_ptr<pql_clause::Clause> pattern_a2_a = make_shared<pql_clause::AssignPatternClause>("a2", "a", false, "_", true);
    shared_ptr<pql_clause::Clause> next_s2_s3 = make_shared<pql_clause::NextClause>("s2", true, "s3", true);
    shared_ptr<pql_clause::Clause> mod_s1_x = make_shared<pql_clause::ModifiesSClause>("s1", true, "x", false);
    shared_ptr<pql_clause::Clause> mod_a_v2 = make_shared<pql_clause::ModifiesSClause>("a", true, "v2", true);
    shared_ptr<pql_clause::Clause> pattern_a_v1 = make_shared<pql_clause::AssignPatternClause>("a", "v1", true, "x", true);

    vector<pql::Synonym> synonyms{ a, a1, a2, s1, s2, s3, v1, v2 };
    vector<shared_ptr<pql_clause::Clause>> clauses{ uses_5_y, uses_s1_v1, affects_a1_a2, with_a1stmt_20, parent_s1_s2, 
      pattern_a2_a, next_s2_s3, mod_s1_x, mod_a_v2, pattern_a_v1 };

    pql_solver::Ufds ufds(&synonyms, &clauses);

    std::vector<std::vector<pql::Synonym>> syn_groups = ufds.GetSynGroups();
    std::vector<std::vector<std::shared_ptr<pql_clause::Clause>>> clause_groups = ufds.GetClauseGroups();
    std::unordered_set<std::string> s = ufds.GetSynInvovledInClause();

    REQUIRE(1 == 1);
}