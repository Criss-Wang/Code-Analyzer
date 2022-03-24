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
    shared_ptr<AssignTable> assign_table_ = make_shared<AssignTable>();
    shared_ptr<ReadTable> read_table_ = make_shared<ReadTable>();
    shared_ptr<PrintTable> print_table_ = make_shared<PrintTable>();
    shared_ptr<ConstantTable> constant_table_ = make_shared<ConstantTable>();
    shared_ptr<CallerTable> caller_table_ = make_shared<CallerTable>();
    shared_ptr<IfTable> if_table_ = make_shared<IfTable>();
    shared_ptr<WhileTable> while_table_ = make_shared<WhileTable>();
    // Procedure name mapping to a pair of integers - the start and end numbers for that procedure
    Table<string, pair<int, int>> *proc_range_table_ = new Table<string, pair<int, int>>();

    // Index tables
    shared_ptr<EntityToIndexTable> var_index_table_ = make_shared<EntityToIndexTable>();
    shared_ptr<IndexToEntityTable> index_var_table_ = make_shared<IndexToEntityTable>();
    shared_ptr<EntityToIndexTable> proc_index_table_ = make_shared<EntityToIndexTable>();
    shared_ptr<IndexToEntityTable> index_proc_table_ = make_shared<IndexToEntityTable>();

    // Relation tables
    shared_ptr<FollowsTable> follows_table_ = make_shared<FollowsTable>();
    shared_ptr<FollowsBeforeTable> follows_before_table_ = make_shared<FollowsBeforeTable>();
    shared_ptr<FollowsStarTable> follows_star_table_ = make_shared<FollowsStarTable>();
    shared_ptr<FollowsBeforeStarTable> follows_before_star_table_ = make_shared<FollowsBeforeStarTable>();
    shared_ptr<ParentTable> parent_table_ = make_shared<ParentTable>();
    shared_ptr<ChildTable> child_table_ = make_shared<ChildTable>();
    shared_ptr<ParentStarTable> parent_star_table_ = make_shared<ParentStarTable>();
    shared_ptr<ChildStarTable> child_star_table_ = make_shared<ChildStarTable>();
    shared_ptr<CallsTable> calls_table_ = make_shared<CallsTable>();
    shared_ptr<CalledByTable> called_by_table_ = make_shared<CalledByTable>();
    shared_ptr<CallsStarTable> calls_star_table_ = make_shared<CallsStarTable>();
    shared_ptr<CalledByStarTable> called_by_star_table_ = make_shared<CalledByStarTable>();
    shared_ptr<NextTable> next_table_ = make_shared<NextTable>();
    shared_ptr<BeforeTable> before_table_ = make_shared<BeforeTable>();

    shared_ptr<UsesStmtToVariablesTable> uses_stmt_to_variables_table_ = make_shared<UsesStmtToVariablesTable>();
    shared_ptr<UsesVariableToStmtsTable> uses_variable_to_stmts_table_ = make_shared<UsesVariableToStmtsTable>();
    shared_ptr<UsesProcToVariablesTable> uses_proc_to_variables_table_ = make_shared<UsesProcToVariablesTable>();
    shared_ptr<UsesVariableToProcsTable> uses_variable_to_procs_table_ = make_shared<UsesVariableToProcsTable>();
    shared_ptr<ModifiesStmtToVariablesTable> modifies_stmt_to_variables_table_ = make_shared<ModifiesStmtToVariablesTable>();
    shared_ptr<ModifiesVariableToStmtsTable> modifies_variable_to_stmts_table_ = make_shared<ModifiesVariableToStmtsTable>();
    shared_ptr<ModifiesProcToVariablesTable> modifies_proc_to_variables_table_ = make_shared<ModifiesProcToVariablesTable>();
    shared_ptr<ModifiesVariableToProcsTable> modifies_variable_to_procs_table_ = make_shared<ModifiesVariableToProcsTable>();
    
    // Pattern tables
    shared_ptr<Table<int, unordered_set<string>>> assign_stmt_to_patterns_table_ = make_shared<Table<int, unordered_set<string>>>();
    shared_ptr<Table<string, unordered_set<int>>> assign_pattern_to_stmts_table_ = make_shared<Table<string, unordered_set<int>>>();
    shared_ptr<Table<string, unordered_set<int>>> exact_pattern_to_stmt_table_ = make_shared<Table<string, unordered_set<int>>>();
    shared_ptr<Table<string, unordered_set<int>>> if_pattern_to_stmt_table_ = make_shared<Table<string, unordered_set<int>>>();
    shared_ptr<Table<int, unordered_set<string>>> if_stmt_to_pattern_table_ = make_shared<Table<int, unordered_set<string>>>();
    shared_ptr<Table<string, unordered_set<int>>> while_pattern_to_stmt_table_ = make_shared<Table<string, unordered_set<int>>>();
    shared_ptr<Table<int, unordered_set<string>>> while_stmt_to_pattern_table_ = make_shared<Table<int, unordered_set<string>>>();

    // Entity sets - statement numbers
    unordered_set<int> stmt_set_;
    unordered_set<int> assign_set_;
    unordered_set<int> read_set_;
    unordered_set<int> print_set_;
    unordered_set<int> call_set_;
    unordered_set<int> if_set_;
    unordered_set<int> while_set_;
    unordered_set<int> constant_set_;

    // Entity sets - names and lists
    unordered_set<int> variable_set_;
    unordered_set<int> procedure_set_;
    unordered_set<set<int>, HashFunction> stmt_list_set_;

    // Insert all possible expression patterns for a statement
    bool AddPattern(int line_num, const string& input, TableIdentifier table_identifier);
    bool AddParent(int key, const vector<int>& value);
    bool AddFollows(int key, int value);
    bool AddCalls(const string& key, const vector<string>& value);
    bool AddNext(int key, int value);
    bool AddModifies(int key, const vector<string>& value);
    bool AddModifiesP(const string& key, const vector<string>& value);
    bool AddUses(int key, const vector<string>& value);
    bool AddUsesP(const string& key, const vector<string>& value);
    bool AddPatternToTable(bool& add_success, unordered_set<string> pattern_set, shared_ptr<Table<string, unordered_set<int>>> table_to_update, int line_num);
    bool UpdateIndexTable(shared_ptr<Table<int, string>> index_to_string_table, shared_ptr<Table<string, int>> string_to_int_table, const string& entity_value);

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
    shared_ptr<FollowsTable> GetFollowsTable();
    shared_ptr<FollowsStarTable> GetFollowsStarTable();
    shared_ptr<FollowsBeforeTable> GetFollowsBeforeTable();
    shared_ptr<FollowsBeforeStarTable> GetFollowsBeforeStarTable();
    shared_ptr<ParentTable> GetParentTable();
    shared_ptr<ChildTable> GetChildTable();
    shared_ptr<ParentStarTable> GetParentStarTable();
    shared_ptr<ChildStarTable> GetChildStarTable();
    shared_ptr<CallsTable> GetCallsTable();
    shared_ptr<CallsStarTable> GetCallsStarTable();
    shared_ptr<CalledByTable> GetCalledByTable();
    shared_ptr<CalledByStarTable> GetCalledByStarTable();
    shared_ptr<ModifiesStmtToVariablesTable> GetModifiesStmtToVariablesTable();
    shared_ptr<ModifiesVariableToStmtsTable> GetModifiesVariableToStmtsTable();
    shared_ptr<ModifiesProcToVariablesTable> GetModifiesProcToVariablesTable();
    shared_ptr<ModifiesVariableToProcsTable> GetModifiesVariableToProcsTable();
    shared_ptr<UsesStmtToVariablesTable> GetUsesStmtToVariablesTable();
    shared_ptr<UsesVariableToStmtsTable> GetUsesVariableToStmtsTable();
    shared_ptr<UsesProcToVariablesTable> GetUsesProcToVariablesTable();
    shared_ptr<UsesVariableToProcsTable> GetUsesVariableToProcsTable();
    shared_ptr<CallerTable> GetCallerTable();

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

    [[nodiscard]] bool IsCalls(const int proc_1_idx, const int proc_2_idx) const;
    [[nodiscard]] bool IsCallsExists() const;
    [[nodiscard]] bool IsTransitiveCalls(const int proc_1_idx, const int proc_2_idx) const;
    [[nodiscard]] vector<int> GetCallers(const int proc_idx) const;
    [[nodiscard]] vector<int> GetAllCallers(const int proc_idx) const;
    [[nodiscard]] vector<int> GetCallees(const int proc_idx) const;
    [[nodiscard]] vector<int> GetAllCallees(const int proc_idx) const;
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

    [[nodiscard]] bool IsProcModifiesVar(int proc_idx, int var_idx) const;
    [[nodiscard]] vector<int> GetModifiesProcsByVar(int var_idx) const;
    [[nodiscard]] vector<int> GetModifiesVarsByProc(int proc_idx) const;
    [[nodiscard]] vector<pair<int, int>> GetAllModifiesProcVarPairs() const;

    [[nodiscard]] bool IsProcUsesVar(int proc_idx, int var_idx) const;
    [[nodiscard]] vector<int> GetUsesProcsByVar(int var_idx) const;
    [[nodiscard]] vector<int> GetUsesVarsByProc(int proc_idx) const;
    [[nodiscard]] vector<pair<int, int>> GetAllUsesProcVarPairs() const;

    [[nodiscard]] unordered_set<int> GetAllStmtsWithPattern(const string& pattern) const;
    [[nodiscard]] unordered_set<int> GetStmtsWithExactPattern(const string& pattern) const;
    [[nodiscard]] unordered_set<string> GetAllPatternVariablesInStmt(const int stmt_no, TableIdentifier table_identifier) const;
    [[nodiscard]] unordered_set<int> GetAllStmtsWithPatternVariable(const string& pattern_var_string, TableIdentifier table_identifier) const;
    [[nodiscard]] vector<pair<int, string>> GetContainerStmtVarPair(TableIdentifier table_identifier) const;

    // Get all the items of a certain entity type
    unordered_set<int> GetAllEntity(const EntityIdentifier entity_identifier);
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
