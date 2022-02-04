#pragma once

#include <string>
#include <vector>

#include "table.h"

// Line number will be the key and the line number that follows after will be the value i.e. follows(key, value)
class FollowsByTable : public Table<int, int> {};

// Line number will be the key and the other line number that appears before it will be the value i.e. follows(value, key)
class FollowsAfterTable : public Table<int, int> {};

// Line number will be the key and the statements after such that follows* holds will be the value i.e. follows*(key, [values])
class FollowsByStarTable : public Table<int, vector<int>> {};

// Line number will be the key and the statements before it such that follows* holds will be the value i.e. follows*([values], key)
class FollowsAfterStarTable : public Table<int, vector<int>> {};

// Line number will be the key and the statements nested within it that parent holds will be the value i.e. parent(key, [value])
class ParentTable : public Table<int, vector<int>> {};

// Nested line number will be the key and the parent line number such that parent([value], key) holds will be the value
class ChildTable : public Table<int, vector<int>> {};

// Line number will be the key and the statements nested within it that parent* holds will be the value i.e. parent*(key, [value])
class ParentStarTable : public Table<int, vector<int>> {};

// Nested line number will be the key and the parent line number such that parent*([value], key) holds will be the value
class ChildStarTable : public Table<int, vector<int>> {};

// Line number will be the key and the list of variables used will be the value
class UsesStmtToVariablesTable : public Table<int, vector<string>> {};

// Variables will be the key and the corresponding line number will be the value
class UsesVariableToStmtsTable : public Table<string, vector<int>> {};

// Line number will be the key and the list of variables modified will be the value
class ModifiesStmtToVariablesTable : public Table<int, vector<string>> {};

// Variables will be the key and the corresponding line number will be the value
class ModifiesVariableToStmtsTable : public Table<string, vector<int>> {};
