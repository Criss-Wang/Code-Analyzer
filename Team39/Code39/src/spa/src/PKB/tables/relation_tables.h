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
class FollowsTable : public RelTable {};

// Line number will be the key and the other line number that appears before it will be the value i.e. follows(value, key)
class FollowsBeforeTable : public RelTable {};

// Line number will be the key and the statements after such that follows* holds will be the value i.e. follows*(key, [values])
class FollowsStarTable : public RelListTable {};

// Line number will be the key and the statements before it such that follows* holds will be the value i.e. follows*([values], key)
class FollowsBeforeStarTable : public RelListTable {};

// Line number will be the key and the statements nested within it that parent holds will be the value i.e. parent(key, [value])
class ParentTable : public RelListTable {};

// Nested line number will be the key and the parent line number such that parent([value], key) holds will be the value
class ChildTable : public RelListTable {};

// Line number will be the key and the statements nested within it that parent* holds will be the value i.e. parent*(key, [value])
class ParentStarTable : public RelListTable {};

// Nested line number will be the key and the parent line number such that parent*([value], key) holds will be the value
class ChildStarTable : public RelListTable {};

// Procedure name will be the key mapping to list of procedure names
class CallsTable : public RelListTable {};

class CalledByTable : public RelListTable {};

class CallsStarTable : public RelListTable {};

class CalledByStarTable : public RelListTable {};

// Line number will be the key mapping to another line number which is next in the execution flow
// In certain cases such as container statements, it can map to two numbers
class NextTable: public RelListTable {};

class BeforeTable: public RelTable {};

// Line number will be the key and the list of variables used will be the value
class UsesStmtToVariablesTable : public RelListTable {};

// Variables will be the key and the corresponding line number will be the value
class UsesVariableToStmtsTable : public RelListReverseTable {};

class UsesProcToVariablesTable : public RelListTable {};

class UsesVariableToProcsTable : public RelListReverseTable {};

// Line number will be the key and the list of variables modified will be the value
class ModifiesStmtToVariablesTable : public RelListTable {};

// Variables will be the key and the corresponding line number will be the value
class ModifiesVariableToStmtsTable : public RelListReverseTable {};

class ModifiesProcToVariablesTable : public RelListTable {};

class ModifiesVariableToProcsTable : public RelListReverseTable {};

// Line number will be the key and the list of valid substring patterns will be the value
class StmtToPatternsTable : public Table<int, unordered_set<string>> {};

// Valid substring patterns will be the key and the list of statement having these patterns will be the value
class PatternToStmtsTable : public Table<string, unordered_set<int>> {};

class ExactPatternToStmtTable : public Table<string, unordered_set<int>> {};
