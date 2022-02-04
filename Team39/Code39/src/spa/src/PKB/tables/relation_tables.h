#pragma once

#include <string>

#include "table.h"

// Line number will be the key and the line number that follows after will be the value
class FollowsByTable : public Table<int, int> {};
