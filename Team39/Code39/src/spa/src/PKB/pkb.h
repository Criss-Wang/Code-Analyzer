#pragma once

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"
#include "pkb_exceptions.h"

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

    // A table to store pointers to all tables
    //IndexTable* index_table_ = new IndexTable();

  public:
    enum class Identifier {
      kAssign, kRead, kPrint, kConstant, kIf, kWhile,
      kFollowsBy,kFollowsAfter, kFollowsByStar, kFollowsAfterStar,
      kParent, kChild, kParentStar, kChildStar,
      kUsesVarToStmt, kUsesStmtToVar, kModifiesVarToStmt, kModifiesStmtToVar
    };
    
    template<typename T1, typename T2>
    bool AddInfoToTable(const Identifier table_identifier, T1 key, T2 value) {
      try {
        switch (table_identifier) {
          case Identifier::kAssign: return assign_table_->AddKeyValuePair(key, value);
          case Identifier::kRead: return read_table_->AddKeyValuePair(key, value);
          case Identifier::kPrint: return print_table_->AddKeyValuePair(key, value);
          case Identifier::kConstant: return constant_table_->AddKeyValuePair(key, value);
          case Identifier::kIf: return if_table_->AddKeyValuePair(key, value);
          case Identifier::kWhile: return while_table_->AddKeyValuePair(key, value);
          case Identifier::kFollowsBy: return follows_by_table_->AddKeyValuePair(key, value);
          case Identifier::kFollowsAfter: return follows_after_table_->AddKeyValuePair(key, value);
          case Identifier::kFollowsByStar: return follows_by_star_table_->AddKeyValuePair(key, value);
          case Identifier::kFollowsAfterStar: return follows_after_star_table_->AddKeyValuePair(key, value);
          case Identifier::kParent: return parent_table_->AddKeyValuePair(key, value);
          case Identifier::kChild: return child_table_->AddKeyValuePair(key, value);
          case Identifier::kParentStar: return parent_star_table_->AddKeyValuePair(key, value);
          case Identifier::kChildStar: return child_star_table_->AddKeyValuePair(key, value);
          case Identifier::kUsesVarToStmt: return uses_variable_to_stmts_table_->AddKeyValuePair(key, value);
          case Identifier::kUsesStmtToVar: return uses_stmt_to_variables_table_->AddKeyValuePair(key, value);
          case Identifier::kModifiesStmtToVar: return modifies_stmt_to_variables_table_->AddKeyValuePair(key, value);
          case Identifier::kModifiesVarToStmt: return modifies_variable_to_stmts_table_->AddKeyValuePair(key, value);
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    int PopulateNestedRelationship();
};
