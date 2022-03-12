#pragma once

#include <string>
#include <vector>

#include "table.h"

// Line number will be the key and the assignment pattern will be the value
class AssignTable : public Table<int, string> {};

// Line number will be the key and the single variable being read will be the value
class ReadTable : public Table<int, string> {};

// Line number will be the key and the single variable being printed will be the value
class PrintTable : public Table<int, string> {};

// Line number will be the key and the single procedure name being called will be the value
class CallerTable : public Table<int, string> {};

// Line number will be the key and the list of constants will be the value
class ConstantTable : public Table<int, vector<int>> {};

// Line number will be the key and the list of variables in the condition expression will be the value
class IfTable : public Table<int, vector<string>> {};

// Line number will be the key and the list of variables in the condition expression will be the value
class WhileTable : public Table<int, vector<string>> {};

// Procedure name mapping to a pair of integers - the start and end numbers for that procedure
class ProcRangeTable: public Table<string, pair<int, int>> {};

class VarIndexTable: public Table<string, int>{};

class IndexVarTable: public Table<int, string>{};

class ProcIndexTable : public Table<string, int> {};

class IndexProcTable : public Table<int, string> {};