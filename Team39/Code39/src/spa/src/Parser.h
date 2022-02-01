#pragma once

#include <optional>
#include <string>

#include "Tokenizer.h"

using namespace std;

class Parser {
public: 
	void parse(vector<Token>& tokens);
	void parseStmtLst(vector<Token>& tokens);

private:
	// Default identifier is empty string
	optional<Token> expectIdentifier(const string& name = "");

	optional<Token> expectBracket(const string& name = "");

	optional<Token> expectSymbol(const string& name = "");

	vector<Token>::iterator currToken;
	vector<Token>::iterator endToken;
};

