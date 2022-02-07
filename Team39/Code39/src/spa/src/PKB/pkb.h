#pragma once

#include <set>
#include <unordered_set>

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"
#include "pkb_exceptions.h"
#include "../Utility/Entity.h"

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

    void PopulateNestedFollows();
    void PopulateNestedParents();
    void PopulateUses();
    void PopulateModifies();

  public:
    /**
     * In the following, we implement 5 overloaded APIs to add key value information pairs to pkb tables.
     * Generic function which passes generic variable types into a function of a fixed-typed class object is not accepted.
     */
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const vector<int>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const vector<string>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, int value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const string& value);

    // Populate the nested relationships using the tables storing basic information/relationships about entities
    int PopulateNestedRelationship();

    // Relationship utility APIs for PQL
    [[nodiscard]] bool IsParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsTransitiveParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] int GetParent(int stmt) const;
    [[nodiscard]] vector<int> GetAllParents(int stmt) const;
    [[nodiscard]] vector<int> GetChild(int stmt) const;
    [[nodiscard]] vector<int> GetAllChildren(int stmt) const;

    [[nodiscard]] bool IsFollows(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsTransitiveFollows(int stmt_1, int stmt_2) const;
    [[nodiscard]] int GetStmtRightBefore(int stmt) const;
    [[nodiscard]] vector<int> GetStmtsBefore(int stmt) const;
    [[nodiscard]] int GetStmtRightAfter(int stmt) const;
    [[nodiscard]] vector<int> GetStmtsAfter(int stmt) const;

    // Add entities to individual sets (Again very bad practice, not sure how to optimize the code)
    bool AddEntityToSet(const EntityIdentifier entity_identifier, int entity_val);
    bool AddEntityToSet(const EntityIdentifier entity_identifier, const string& entity_val);
    bool AddEntityToSet(const EntityIdentifier entity_identifier, const set<int>& entity_val);

    // Get all the items of a certain entity type
    unordered_set<int> GetAllEntityInt(const EntityIdentifier entity_identifier);
    unordered_set<string> GetAllEntityString(const EntityIdentifier entity_identifier);
    unordered_set<set<int>, HashFunction> GetAllEntityStmtLst(const EntityIdentifier entity_identifier);
};
