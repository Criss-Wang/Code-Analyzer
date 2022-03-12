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
    CallerTable* caller_table_ = new CallerTable();
    IfTable *if_table_ = new IfTable();
    WhileTable *while_table_ = new WhileTable();
    ProcRangeTable *proc_range_table_ = new ProcRangeTable();
    VarIndexTable* var_index_table_ = new VarIndexTable();
    IndexVarTable* index_var_table_ = new IndexVarTable();
    ProcIndexTable* proc_index_table_ = new ProcIndexTable();
    IndexProcTable* index_proc_table_ = new IndexProcTable();

    // Relation tables
    FollowsTable *follows_table_ = new FollowsTable();
    FollowsBeforeTable *follows_before_table_ = new FollowsBeforeTable();
    FollowsStarTable *follows_star_table_ = new FollowsStarTable();
    FollowsBeforeStarTable *follows_before_star_table_ = new FollowsBeforeStarTable();
    ParentTable *parent_table_ = new ParentTable();
    ChildTable *child_table_ = new ChildTable();
    ParentStarTable *parent_star_table_ = new ParentStarTable();
    ChildStarTable *child_star_table_ = new ChildStarTable();
    CallsTable *calls_table_ = new CallsTable();
    CalledByTable *called_by_table_ = new CalledByTable();
    CallsStarTable *calls_star_table_ = new CallsStarTable();
    CalledByStarTable *called_by_star_table_ = new CalledByStarTable();
    NextTable *next_table_ = new NextTable();
    BeforeTable *before_table_ = new BeforeTable();

    UsesStmtToVariablesTable *uses_stmt_to_variables_table_ = new UsesStmtToVariablesTable();
    UsesVariableToStmtsTable *uses_variable_to_stmts_table_ = new UsesVariableToStmtsTable();
    ModifiesStmtToVariablesTable *modifies_stmt_to_variables_table_ = new ModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable *modifies_variable_to_stmts_table_ = new ModifiesVariableToStmtsTable();
    StmtToPatternsTable *stmt_to_patterns_table_ = new StmtToPatternsTable();
    PatternToStmtsTable *pattern_to_stmts_table_ = new PatternToStmtsTable();
    ExactPatternToStmtTable* exact_pattern_to_stmt_table_ = new ExactPatternToStmtTable();

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
    bool AddCalls(const string& key, const vector<string>& value);
    bool AddNext(int key, int value);
    bool AddModifies(int key, const vector<string>& value);
    bool AddUses(int key, const vector<string>& value);
    bool AddPattern(bool& add_success, unordered_set<string> pattern_set, Table<string, unordered_set<int>>* table_to_update, int line_num);
    bool UpdateIndexTable(Table<int, string>* index_to_string_table, Table<string, int>* string_to_int_table, const string& entity_value);

  public:
    /**
     * In the following, we implement 5 overloaded APIs to add key value information pairs to pkb tables.
     * Generic function which passes generic variable types into a function of a fixed-typed class object is not accepted.
     */
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const vector<int>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const vector<string>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, int value);
    bool AddInfoToTable(TableIdentifier table_identifier, int key, const string& value);
    bool AddInfoToTable(TableIdentifier table_identifier, const string& key, const vector<string>& value);
    bool AddInfoToTable(TableIdentifier table_identifier, const string& key, const pair<int, int>& value);

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
    CallsTable* GetCallsTable();
    CallsStarTable* GetCallsStarTable();
    CalledByTable* GetCalledByTable();
    CalledByStarTable* GetCalledByStarTable();
    ModifiesStmtToVariablesTable* GetModifiesStmtToVariablesTable();
    ModifiesVariableToStmtsTable* GetModifiesVariableToStmtsTable();
    UsesStmtToVariablesTable* GetUsesStmtToVariablesTable();
    UsesVariableToStmtsTable* GetUsesVariableToStmtsTable();

    // Relationship utility APIs for PQL
    [[nodiscard]] bool IsParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsParentExists() const;
    [[nodiscard]] bool IsTransitiveParent(int stmt_1, int stmt_2) const;
    [[nodiscard]] vector<int> GetParent(int stmt) const;
    [[nodiscard]] vector<int> GetAllParents(int stmt) const;
    [[nodiscard]] vector<pair<int,int>> GetAllParentPairs() const;
    [[nodiscard]] vector<int> GetChild(int stmt) const;
    [[nodiscard]] vector<int> GetAllChildren(int stmt) const;
    [[nodiscard]] vector<pair<int, int>> GetAllTransitiveParentPairs() const;

    [[nodiscard]] bool IsCalls(const int proc_1, const int proc_2) const;
    [[nodiscard]] bool IsTransitiveCalls(const int proc_1, const int proc_2) const;
    [[nodiscard]] vector<int> GetCallers(const int proc) const;
    [[nodiscard]] vector<int> GetAllCallers(const int proc) const;
    [[nodiscard]] vector<int> GetCallees(const int proc) const;
    [[nodiscard]] vector<int> GetAllCallees(const int proc) const;
    [[nodiscard]] vector<pair<int, int>> GetAllCallsPairs() const;
    [[nodiscard]] vector<pair<int, int>> GetAllTransitiveCallsPairs() const;

    [[nodiscard]] bool IsFollows(int stmt_1, int stmt_2) const;
    [[nodiscard]] bool IsFollowsExists() const;
    [[nodiscard]] bool IsTransitiveFollows(int stmt_1, int stmt_2) const;
    [[nodiscard]] vector<int> GetStmtRightBefore(int stmt) const;
    [[nodiscard]] vector<pair<int,int>> GetAllFollowsPairs() const;
    [[nodiscard]] vector<int> GetStmtsBefore(int stmt) const;
    [[nodiscard]] vector<int> GetStmtRightAfter(int stmt) const;
    [[nodiscard]] vector<int> GetStmtsAfter(int stmt) const;
    [[nodiscard]] vector<pair<int, int>> GetAllTransitiveFollowsPairs() const;

    [[nodiscard]] bool IsUsesStmt(int stmt, int var_idx) const;
    [[nodiscard]] bool IsUsesStmtExists() const;
    [[nodiscard]] vector<int> GetUsesStmtsByVar(int var_idx) const;
    [[nodiscard]] vector<int> GetUsesVarByStmt(int stmt) const;
    [[nodiscard]] vector<pair<int, int>> GetAllUsesStmtVarPairs() const;

    [[nodiscard]] bool IsModifiesStmt(int stmt, int var_idx) const;
    [[nodiscard]] bool IsModifiesStmtExists() const;
    [[nodiscard]] vector<int> GetModifiesStmtsByVar(int var_idx) const;
    [[nodiscard]] vector<int> GetModifiesVarByStmt(int stmt) const;
    [[nodiscard]] vector<pair<int, int>> GetAllModifiesStmtVarPairs() const;

    [[nodiscard]] unordered_set<int> GetAllStmtsWithPattern(const string& pattern) const;
    [[nodiscard]] unordered_set<int> GetStmtsWithExactPattern(const string& pattern) const;

    // Get all the items of a certain entity type
    unordered_set<int> GetAllEntityInt(const EntityIdentifier entity_identifier);
    unordered_set<string> GetAllEntityString(const EntityIdentifier entity_identifier);
    unordered_set<set<int>, HashFunction> GetAllEntityStmtLst(const EntityIdentifier entity_identifier);

    // Get all the index-string relationships
    [[nodiscard]] vector<pair<int, string>> GetAllIndexVarPairs() const;
    [[nodiscard]] vector<pair<string, int>> GetAllVarIndexPairs() const;
    [[nodiscard]] string GetVarByIndex(int idx) const;
    [[nodiscard]] int GetIndexByVar(const string& var_name) const;

    [[nodiscard]] vector<pair<int, string>> GetAllIndexProcPairs() const;
    [[nodiscard]] vector<pair<string, int>> GetAllProcIndexPairs() const;
    [[nodiscard]] string GetProcByIndex(int idx) const;
    [[nodiscard]] int GetIndexByProc(const string& proc_name) const;

    // Get all the attribute
    [[nodiscard]] bool IsVar(int var_idx) const;

    [[nodiscard]] bool IsRead(int stmt_no) const;
    [[nodiscard]] int GetVarFromRead(int stmt_no) const;
    [[nodiscard]] vector<int> GetReadByVar(int var_idx) const;
    
    [[nodiscard]] bool IsPrint(int stmt_no) const;
    [[nodiscard]] int GetVarFromPrint(int stmt_no) const;
    [[nodiscard]] vector<int> GetPrintByVar(int var_idx) const;

    [[nodiscard]] bool IsAssign(int stmt_no) const;
    [[nodiscard]] int GetVarFromAssign(int stmt_no) const;
    [[nodiscard]] vector<int> GetAssignByVar(int var_idx) const;

    [[nodiscard]] bool IsCall(int stmt_no) const;
    [[nodiscard]] int GetProcFromCall(int stmt_no) const;
    [[nodiscard]] vector<int> GetCallFromProc(int proc_idx) const;

    [[nodiscard]] bool IsIf(int stmt_no) const;
    [[nodiscard]] bool IsWhile(int stmt_no) const;
    [[nodiscard]] bool IsProcedure(int proc_idx) const;
    [[nodiscard]] bool IsStmt(int stmt_no) const;
    [[nodiscard]] bool IsConstant(int stmt_no) const;
};
