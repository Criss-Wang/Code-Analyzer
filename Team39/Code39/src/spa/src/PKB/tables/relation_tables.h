#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "table.h"

/**
 * Relationship tables between same type of entities
 * - Follows
 * - Parent
 * - Call
 * - Next
 */

// Line number will be the key and the line number that follows after will be the value i.e. follows(key, value)
class FollowsTable : public Table<int, int> {};

// Line number will be the key and the other line number that appears before it will be the value i.e. follows(value, key)
class FollowsBeforeTable : public Table<int, int> {};

// Line number will be the key and the statements after such that follows* holds will be the value i.e. follows*(key, [values])
class FollowsStarTable : public Table<int, vector<int>> {};

// Line number will be the key and the statements before it such that follows* holds will be the value i.e. follows*([values], key)
class FollowsBeforeStarTable : public Table<int, vector<int>> {};

// Line number will be the key and the statements nested within it that parent holds will be the value i.e. parent(key, [value])
class ParentTable : public Table<int, vector<int>> {};

// Nested line number will be the key and the parent line number such that parent([value], key) holds will be the value
class ChildTable : public Table<int, vector<int>> {};

// Line number will be the key and the statements nested within it that parent* holds will be the value i.e. parent*(key, [value])
class ParentStarTable : public Table<int, vector<int>> {};

// Nested line number will be the key and the parent line number such that parent*([value], key) holds will be the value
class ChildStarTable : public Table<int, vector<int>> {};

// Procedure name will be the key mapping to list of procedure names
class CallsTable : public Table<string, vector<string>> {};

class CalledByTable : public Table<string, vector<string>> {};

class CallsStarTable : public Table<string, vector<string>> {};

class CalledByStarTable : public Table<string, vector<string>> {};

// Line number will be the key mapping to another line number which is next in the execution flow
class NextTable: public Table<int, int> {};

class BeforeTable: public Table<int, int> {};


/**
 * Relationship tables between different types of entities
 * - Modifies
 * - Uses
 * - Affects (unique)
 * - Pattern
 */

// Line number will be the key and the list of valid substring patterns will be the value
class StmtToPatternsTable : public Table<int, unordered_set<string>> {};

// Valid substring patterns will be the key and the list of statement having these patterns will be the value
class PatternToStmtsTable : public Table<string, unordered_set<int>> {};

class ExactPatternToStmtTable : public Table<string, unordered_set<int>> {};

// Line number will be the key and the list of variables used will be the value
class UsesStmtToVariablesTable : public Table<int, vector<string>> {};

// Variables will be the key and the corresponding line number will be the value
class UsesVariableToStmtsTable : public Table<string, vector<int>> {
  public:
    bool UpdateKeyValuePair(int value_to_update, const vector<string>& keys);
};

class UsesProcToVariablesTable : public Table<string, vector<string>> {};

class UsesVariableToProcsTable : public Table<string, vector<string>> {
  public:
    bool UpdateKeyValuePair(const string& value_to_update, const vector<string>& keys);
};

// Line number will be the key and the list of variables modified will be the value
class ModifiesStmtToVariablesTable : public Table<int, vector<string>> {};

// Variables will be the key and the corresponding line number will be the value
class ModifiesVariableToStmtsTable : public Table<string, vector<int>> {
  public:
    bool UpdateKeyValuePair(int value_to_update, const vector<string>& keys);
};

class ModifiesProcToVariablesTable : public Table<string, vector<string>> {};

class ModifiesVariableToProcsTable : public Table<string, vector<string>> {
  public:
    bool UpdateKeyValuePair(const string& value_to_update, const vector<string>& keys);
};
