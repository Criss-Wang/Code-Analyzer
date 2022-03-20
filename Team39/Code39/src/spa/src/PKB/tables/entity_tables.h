#pragma once

#include "table.h"

// Line number will be the key and the assignment pattern will be the value
class AssignTable : public EntityVarsTable {};

// Line number will be the key and the single variable being read will be the value
class ReadTable : public EntityVarsTable {};

// Line number will be the key and the single variable being printed will be the value
class PrintTable : public EntityVarsTable {};

// Line number will be the key and the single procedure name being called will be the value
class CallerTable : public EntityVarsTable {};

// Line number will be the key and the list of constants will be the value
class ConstantTable : public EntityIntListTable {};

// Line number will be the key and the list of variables in the condition expression will be the value
class IfTable : public EntityVarsListTable {};

// Line number will be the key and the list of variables in the condition expression will be the value
class WhileTable : public EntityVarsListTable {};