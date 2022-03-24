#pragma once

#include <set>
#include <unordered_set>

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"
#include "../Utility/entity.h"
#include "PQL/query_parser/token.h"

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
    //unordered_set<set<int>, HashFunction> stmt_list_set_;

    // Insert all possible expression patterns for a statement
    bool AddPattern(int line_num, const vector<string>& input_set, const string& input, TableIdentifier table_identifier);
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

    bool IsEntity(EntityIdentifier entity_identifier, int entity_idx);

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
    //bool AddEntityToSet(EntityIdentifier entity_identifier, const set<int>& entity_val);

    // Get tables
    shared_ptr<RelTable> GetFollowsTable();
    shared_ptr<RelListTable> GetFollowsStarTable();
    shared_ptr<RelTable> GetFollowsBeforeTable();
    shared_ptr<RelListTable> GetFollowsBeforeStarTable();
    shared_ptr<RelListTable> GetParentTable();
    shared_ptr<RelListTable> GetChildTable();
    shared_ptr<RelListTable> GetParentStarTable();
    shared_ptr<RelListTable> GetChildStarTable();
    shared_ptr<RelListTable> GetCallsTable();
    shared_ptr<RelListTable> GetCallsStarTable();
    shared_ptr<RelListTable> GetCalledByTable();
    shared_ptr<RelListTable> GetCalledByStarTable();
    shared_ptr<RelListTable> GetModifiesStmtToVariablesTable();
    shared_ptr<RelListReverseTable> GetModifiesVariableToStmtsTable();
    shared_ptr<RelListTable> GetModifiesProcToVariablesTable();
    shared_ptr<RelListReverseTable> GetModifiesVariableToProcsTable();
    shared_ptr<RelListTable> GetUsesStmtToVariablesTable();
    shared_ptr<RelListReverseTable> GetUsesVariableToStmtsTable();
    shared_ptr<RelListTable> GetUsesProcToVariablesTable();
    shared_ptr<RelListReverseTable> GetUsesVariableToProcsTable();

    // Getter for all entity attribute tables
    shared_ptr<EntityVarsTable> GetCallerTable();
    shared_ptr<EntityVarsTable> GetReadTable();
    shared_ptr<EntityVarsTable> GetPrintTable();

    // Getter for all entity sets
    unordered_set<int> GetStmtSet();
    unordered_set<int> GetAssignSet();
    unordered_set<int> GetReadSet();
    unordered_set<int> GetPrintSet();
    unordered_set<int> GetCallSet();
    unordered_set<int> GetIfSet();
    unordered_set<int> GetWhileSet();
    unordered_set<int> GetVarSet();
    unordered_set<int> GetProcSet();
    unordered_set<int> GetConstantSet();
    

    // Relationship utility APIs for PQL
    bool IsRelationshipHolds(pql::RelationshipTypes rel_types, int key, int value);
    bool IsRelationshipExists(pql::RelationshipTypes rel_types);
    vector<int> GetRelFirstArgument(pql::RelationshipTypes rel_types, int second_arg_idx);
    vector<int> GetRelSecondArgument(const pql::RelationshipTypes rel_types, const int first_arg_idx);
    vector<pair<int, int>> GetRelArgumentPairs(const pql::RelationshipTypes rel_types);

    [[nodiscard]] unordered_set<int> GetAllStmtsWithPattern(const string& pattern, bool is_exact) const;
    [[nodiscard]] unordered_set<string> GetAllPatternVariablesInStmt(const int stmt_no, TableIdentifier table_identifier) const;
    [[nodiscard]] unordered_set<int> GetAllStmtsWithPatternVariable(const string& pattern_var_string, TableIdentifier table_identifier) const;
    [[nodiscard]] vector<pair<int, string>> GetContainerStmtVarPair(TableIdentifier table_identifier) const;

    // Get all the items of a certain entity type
    unordered_set<int> GetAllEntity(const EntityIdentifier entity_identifier);
    //unordered_set<set<int>, HashFunction> GetAllEntityStmtLst(const EntityIdentifier entity_identifier);

    // Get all the index-string relationships
    [[nodiscard]] vector<pair<int, string>> GetAllIndexStringPairs(IndexTableType index_table_type) const;
    [[nodiscard]] vector<pair<string, int>> GetAllStringIndexPairs(IndexTableType index_table_type) const;
    [[nodiscard]] string GetStringByIndex(IndexTableType index_table_type, int idx) const;
    [[nodiscard]] int GetIndexByString(IndexTableType index_table_type, const string& entity_name) const;

    // Get all the attribute
    int GetStringAttribute(EntityIdentifier entity_identifier, const int stmt_no);
    vector<int> GetStmtNumByStringAttribute(EntityIdentifier entity_identifier, const int string_idx);

    //[[nodiscard]] bool IsAssign(int stmt_no) const;
    //[[nodiscard]] int GetVarFromAssign(int stmt_no) const;
    //[[nodiscard]] vector<int> GetAssignByVar(int var_idx) const;
       
};
