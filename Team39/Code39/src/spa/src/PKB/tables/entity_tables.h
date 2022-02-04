#pragma once

#include <string>

#include "table.h"

// Line number will be the key and the assignment pattern will be the value
class AssignTable : public Table<int, string> {};

// Line number will be the key and the single variable being read will be the value
class ReadTable : public Table<int, string> {};
