#pragma once
#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "entity_table.h"

/*
 exception class for pkb, not in use right now
 todo: discuss with zheng wei and the team about how to handle exceptions
 */
class invalidargumentexception : public exception
{
private:
	static const string invalidstmtpropmsg = "invalid property index, expected values between " + to_string(StmtTable.assign_idx_) + " to " + to_string(StmtTable.while_idx_) + ".";
public:
	//invalidargumentexception
};