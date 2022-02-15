#pragma once

#include <set>
#include <unordered_set>

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"
#include "../Utility/entity.h"

// Custom hash function from https://www.geeksforgeeks.org/unordered-set-of-vectors-in-c-with-examples/
struct HashFunction {
  size_t operator()(const set<int> & my_vector) const {
    std::hash<int> hasher;
    size_t answer = 0;

    for (int i : my_vector) {
      answer ^= hasher(i) + 0x9e3779b9 + (answer << 6) + (answer >> 2);
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
    FollowsTable *follows_table_ = new FollowsTable();
    FollowsBeforeTable *follows_before_table_ = new FollowsBeforeTable();
    FollowsStarTable *follows_star_table_ = new FollowsStarTable();
    FollowsBeforeStarTable *follows_before_star_table_ = new FollowsBeforeStarTable();
    ParentTable *parent_table_ = new ParentTable();
    ChildTable *child_table_ = new ChildTable();
    ParentStarTable *parent_star_table_ = new ParentStarTable();
    ChildStarTable *child_star_table_ = new ChildStarTable();
    UsesStmtToVariablesTable *uses_stmt_to_variables_table_ = new UsesStmtToVariablesTable();
    UsesVariableToStmtsTable *uses_variable_to_stmts_table_ = new UsesVariableToStmtsTable();
    ModifiesStmtToVariablesTable *modifies_stmt_to_variables_table_ = new ModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable *modifies_variable_to_stmts_table_ = new ModifiesVariableToStmtsTable();
    StmtToPatternsTable *stmt_to_patterns_table_ = new StmtToPatternsTable();
    PatternToStmtsTable* pattern_to_stmts_table_ = new PatternToStmtsTable();

    // Stores the line numbers into a set
    unordered_set<int> stmt_set_;
    unordered_set<int> assign_set_;
    unordered_set<int> read_set_;
    unordered_set<int> print_set_;
    unordered_set<int> call_set_;
    unordered_set<int> if_set_;
    unordered_set<int> while_set_;
    unordered_set<int> constant_set_;
    // Stores the variable or procedure name into the set
    unordered_set<string> variable_set_;
    unordered_set<string> procedure_set_;
    unordered_set<set<int>, HashFunction> stmt_list_set_;

    // Insert all possible expression patterns for a statement
    bool AddPattern(int line_num, const string& input);
    bool AddParent(int key, const vector<int>& value);
    bool AddFollows(int key, int value);
    bool AddModifies(int key, const vector<string>& value);
    bool AddUses(int key, const vector<string>& value);

  public:
    /**
     * In the following, we implement 5 overloaded APIs to add key value information pairs to pkb tables.
     * Generic function which passes generic variable types into a function of a fixed-typed class object is not accepted.
     */
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const vector<int>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const vector<string>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, int value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const string& value);

    // Add entities to individual sets (Again very bad practice, not sure how to optimize the code)
    bool AddEntityToSet(EntityIdentifier entity_identifier, int entity_val);
    bool AddEntityToSet(EntityIdentifier entity_identifier, const string& entity_val);
    bool AddEntityToSet(EntityIdentifier entity_identifier, const set<int>& entity_val);

    // Get tables
    FollowsTable* GetFollowsTable();
    FollowsStarTable* GetFollowsStarTable();
    FollowsBeforeTable* GetFollowsBeforeTable();
    FollowsBeforeStarTable* GetFollowsBeforeStarTable();
    ParentTable* GetParentTable();
    ChildTable* GetChildTable();
    ParentStarTable* GetParentStarTable();
    ChildStarTable* GetChildStarTable();
    ModifiesStmtToVariablesTable* GetModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable* GetModifiesVariableToStmtsTable();
    UsesStmtToVariablesTable* GetUsesStmtToVariablesTable();
    UsesVariableToStmtsTable* GetUsesVariableToStmtsTable();

    // Relationship utility APIs for PQL
    [[nodiscard]] bool IsParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsTransitiveParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] vector<int> GetParent(int stmt) const;
    [[nodiscard]] vector<int> GetAllParents(int stmt) const;
    [[nodiscard]] vector<pair<int,int>> GetAllParentPairs() const;
    [[nodiscard]] vector<int> GetChild(int stmt) const;
    [[nodiscard]] vector<int> GetAllChildren(int stmt) const;
    [[nodiscard]] vector<pair<int, int>> GetAllTransitiveParentPairs() const;

    [[nodiscard]] bool IsFollows(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsTransitiveFollows(int stmt_1, int stmt_2) const;
    [[nodiscard]] vector<int> GetStmtRightBefore(int stmt) const;
    [[nodiscard]] vector<pair<int,int>> GetFollowsPair(int stmt) const;
    [[nodiscard]] vector<int> GetStmtsBefore(int stmt) const;
    [[nodiscard]] vector<int> GetStmtRightAfter(int stmt) const;
    [[nodiscard]] vector<int> GetStmtsAfter(int stmt) const;
    [[nodiscard]] vector<pair<int, int>> GetAllTransitiveFollowsPairs() const;

    [[nodiscard]] unordered_set<int> GetAllStmtsWithPattern(const string& pattern) const;

    // Get all the items of a certain entity type
    unordered_set<int> GetAllEntityInt(const EntityIdentifier entity_identifier);
    unordered_set<string> GetAllEntityString(const EntityIdentifier entity_identifier);
    unordered_set<set<int>, HashFunction> GetAllEntityStmtLst(const EntityIdentifier entity_identifier);
};
