#pragma once

#include <set>
#include <unordered_set>

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"
#include "pkb_exceptions.h"

// Custom hash function from https://www.geeksforgeeks.org/unordered-set-of-vectors-in-c-with-examples/
struct HashFunction {
  size_t operator()(const set<int>
    & my_vector) const {
    std::hash<int> hasher;
    size_t answer = 0;

    for (int i : my_vector) {
      answer ^= hasher(i) + 0x9e3779b9 +
        (answer << 6) + (answer >> 2);
    }
    return answer;
  }
};

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
    unordered_set<string> variable_set_;
    unordered_set<string> procedure_set_;
    unordered_set<set<int>, HashFunction> stmt_list_set_;

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
     // TODO(Zhenlin): [Implementation] If we don't want empty value, we should add a checker and throw exception when T2 Value is empty (invalid)
    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<int>& value) {
      try {
        if (value.empty()) throw InvalidValueException();
        switch (table_identifier) {
          case TableIdentifier::kConstant: return constant_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kParent: {
            bool add_success = parent_table_->AddKeyValuePair(key, value);
            for (const auto child: value) {
              add_success = child_table_->AddKeyValuePair(child, { key }) && add_success;
            } 
            return add_success;
          }
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const vector<string>& value) {
      try {
        if (value.empty()) throw InvalidValueException();
        switch (table_identifier) {
          case TableIdentifier::kIf: return if_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kWhile: return while_table_->AddKeyValuePair(key, value);
          case TableIdentifier::kModifiesStmtToVar: {
            bool add_success = modifies_stmt_to_variables_table_->AddKeyValuePair(key, value);
            add_success = add_success && modifies_variable_to_stmts_table_->UpdateKeyValuePair(key, value);
            return add_success;
          }
          default:
            throw InvalidIdentifierException();
        }
      } catch (exception& e) {
        return false;
      }
    }

    bool AddInfoToTable(const TableIdentifier table_identifier, const int key, const int value) {
      try {
        if (value < 1) throw InvalidValueException();
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
        if (value.empty()) throw InvalidValueException();
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
    bool AddEntityToSet(const EntityIdentifier table_identifier, int entity_val) {
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

    [[nodiscard]] bool IsParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsTransitiveParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] int GetParent(int stmt) const;
    [[nodiscard]] vector<int> GetAllParents(int stmt) const;
    [[nodiscard]] vector<int> GetChild(int stmt) const;
    [[nodiscard]] vector<int> GetAllChildren(int stmt) const;

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

    unordered_set<set<int>, HashFunction> GetAllEntityStmtLst(const EntityIdentifier table_identifier) {
      switch (table_identifier) {
        case EntityIdentifier::kStmtLst: return stmt_list_set_;
        default:
          throw InvalidIdentifierException();
      }
    }

    
};
