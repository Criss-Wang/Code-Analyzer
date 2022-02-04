#pragma once

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"

class Pkb {
  private:
    // Entity tables
    AssignTable *assign_table_ = new AssignTable();
    ReadTable *read_table_ = new ReadTable();
    PrintTable *print_table_ = new PrintTable();
    ConstantTable *constant_table_ = new ConstantTable();
    IfTable *if_table_ = new IfTable();
    WhileTable *while_table_ = new WhileTable();

    // Relation tables
    FollowsByTable *follows_by_table_ = new FollowsByTable();
    FollowsAfterTable *follows_after_table_ = new FollowsAfterTable();
    FollowsByStarTable *follows_by_star_table_ = new FollowsByStarTable();
    FollowsAfterStarTable *follows_after_star_table_ = new FollowsAfterStarTable();
    ParentTable *parent_table_ = new ParentTable();
    ChildTable *child_table_ = new ChildTable();
    ParentStarTable *parent_star_table_ = new ParentStarTable();
    ChildStarTable *child_star_table_ = new ChildStarTable();
    UsesStmtToVariablesTable *uses_stmt_to_variables_table_ = new UsesStmtToVariablesTable();
    UsesVariableToStmtsTable *uses_variable_to_stmts_table_ = new UsesVariableToStmtsTable();
    ModifiesStmtToVariablesTable *modifies_stmt_to_variables_table_ = new ModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable *modifies_variable_to_stmts_table_ = new ModifiesVariableToStmtsTable();

  public:
    // TODO(Zhenlin): https://github.com/nus-cs3203/21s2-cp-spa-team-39/issues/20
};
