#pragma once

#include <set>
#include <unordered_set>

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
    unordered_set<int> stmt_set_;
    unordered_set<int> assign_set_;
    unordered_set<int> read_set_;
    unordered_set<int> print_set_;
    unordered_set<int> call_set_;
    unordered_set<int> if_set_;
    unordered_set<int> while_set_;
    unordered_set<int> constant_set_;
    unordered_set<set<int>> stmt_list_set_;
    unordered_set<string> variable_set_;
    unordered_set<string> procedure_set_;

  public:
    enum class TableIdentifier {
      kAssign, kRead, kPrint, kConstant, kIf, kWhile,
      kFollowsBy,kFollowsAfter, kFollowsByStar, kFollowsAfterStar,
      kParent, kChild, kParentStar, kChildStar,
      kUsesVarToStmt, kUsesStmtToVar, kModifiesVarToStmt, kModifiesStmtToVar
    };

    enum class EntityIdentifier {
      kProc, kStmtLst, kStmt, kRead, kPrint, kAssign, kWhile, kCall, kIf, kVariable, kConstant
    };

    /**
     * In the following, we implement 5 overloaded APIs to add key value information pairs to pkb tables.
     * Generic function which passes generic variable types into a function of a fixed-typed class object is not accepted.
     */
    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<int>& value) {
      try {
        switch (table_identifier) {
          case TableIdentifier::kConstant: return constant_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kParent: return parent_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kChild: return child_table_->AddKeyValuePair(key, value);
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<string>& value) {
      try {
        switch (table_identifier) {
          case TableIdentifier::kIf: return if_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kWhile: return while_table_->AddKeyValuePair(key, value);
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const int value) {
      try {
        switch (table_identifier) {
          case TableIdentifier::kFollowsBy: return follows_by_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kFollowsAfter: return follows_after_table_->AddKeyValuePair(key, value);
        default:
          throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const string& value) {
      try {
        switch (table_identifier) {
          case TableIdentifier::kAssign: return assign_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kRead: return read_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kPrint: return print_table_->AddKeyValuePair(key, value);
        default:
          throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    // Add entities to individual sets (Again very bad practice, not sure how to optimize the code)
    // Will fill in the test cases after optimization
    bool AddEntityToSet(const EntityIdentifier table_identifier, const int entity_val) {
      try {
        switch (table_identifier) {
          case EntityIdentifier::kStmt: {
            stmt_set_.insert(entity_val);
            return true;
          }
          case EntityIdentifier::kAssign: {
            assign_set_.insert(entity_val);
            return true;
          }
          case EntityIdentifier::kRead: {
            read_set_.insert(entity_val);
            return true;
          }
          case EntityIdentifier::kPrint: {
            print_set_.insert(entity_val);
            return true;
          }
          case EntityIdentifier::kCall: {
            call_set_.insert(entity_val);
            return true;
          }
          case EntityIdentifier::kConstant: {
            constant_set_.insert(entity_val);
            return true;
          }
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddEntityToSet(const EntityIdentifier table_identifier, const string& entity_val) {
      try {
        switch (table_identifier) {
          case EntityIdentifier::kVariable: {
            variable_set_.insert(entity_val);
            return true;
          }
          case EntityIdentifier::kProc: {
            procedure_set_.insert(entity_val);
            return true;
          }
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddEntityToSet(const EntityIdentifier table_identifier, const set<int>& entity_val) {
      try {
        switch (table_identifier) {
          case EntityIdentifier::kStmtLst: {
            stmt_list_set_.insert(entity_val);
            return true;
          }
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    int PopulateNestedRelationship();

    void PopulateNestedFollows();
    void PopulateNestedParents();
    void PopulateUses();
    void PopulateModifies();

    bool IsParent(int stmt_1, int stmt_2);
    bool IsTransitiveParent(int stmt_1, int stmt_2);
    int GetParent(int stmt);
    vector<int> GetAllParents(int stmt);
    vector<int> GetChild(int stmt);
    vector<int> GetAllChildren(int stmt);

    unordered_set<int> GetAllEntityInt(const EntityIdentifier table_identifier) {
      switch (table_identifier) {
        case EntityIdentifier::kStmt: return stmt_set_;
        case EntityIdentifier::kAssign: return assign_set_;
        case EntityIdentifier::kRead: return read_set_;
        case EntityIdentifier::kPrint: return print_set_;
        case EntityIdentifier::kCall: return call_set_;
        case EntityIdentifier::kConstant: return constant_set_;
        default:
          throw InvalidIdentifierException();
      }
    }

    unordered_set<string> GetAllEntityString(const EntityIdentifier table_identifier) {
      switch (table_identifier) {
        case EntityIdentifier::kVariable: return variable_set_;
        case EntityIdentifier::kProc: return procedure_set_;
        default:
          throw InvalidIdentifierException();
      }
    }

    unordered_set<set<int>> GetAllEntityStmtLst(const EntityIdentifier table_identifier) {
      switch (table_identifier) {
        case EntityIdentifier::kStmtLst: return stmt_list_set_;
        default:
          throw InvalidIdentifierException();
      }
    }

    
};
