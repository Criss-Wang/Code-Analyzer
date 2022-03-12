#pragma once

#include<iostream>

#include "cfg_token.h"

using namespace std;

CFGToken::CFGToken() {
	this->type_ = CFGTokenType::kStart;
	this->stmt_num_ = 0;
}

CFGToken::CFGToken(CFGTokenType type, int stmt_num) {
	this->type_ = type;
	this->stmt_num_ = stmt_num;
}

void CFGToken::print() {
	string type = "";
	switch (this->type_) {
		case CFGTokenType::kStart:
			type = "start";
			break;
		case CFGTokenType::kAssign:
			type = "assign";
			break;
		case CFGTokenType::kRead:
			type = "read";
			break;
		case CFGTokenType::kPrint:
			type = "print";
			break;
		case CFGTokenType::kCall:
			type = "call";
			break;
		case CFGTokenType::kIfStart:
			type = "ifStart";
			break;
		case CFGTokenType::kIfElse:
			type = "ifElse";
			break;
		case CFGTokenType::kIfEnd:
			type = "ifEnd";
			break;
		case CFGTokenType::kWhileStart:
			type = "whileStart";
			break;
		case CFGTokenType::kWhileEnd:
			type = "whileEnd";
			break;
		case CFGTokenType::kEnd:
			type = "end";
			break;
		default:
			break;
	}

	cout << "Token type: " << type << " Line: " << this->stmt_num_ << endl;
}
