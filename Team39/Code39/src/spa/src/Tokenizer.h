#pragma once

#include <string>
#include <vector>

using namespace std;

enum TokenType {
	WHITESPACE,
	LETTER,
	DIGIT,
	NAME,
	INTEGER,
	LEFT_PAREN,
	RIGHT_PAREN,
	LEFT_CURLY,
	RIGHT_CURLY,
	OPERATOR,
	SEMICOLON,
	IF, 
	THEN,
	ELSE,
	WHILE
};

static const char* tokenTypeStrings[] = {
	"WHITESPACE",
	"LETTER",
	"DIGIT",
	"NAME",
	"INTEGER",
	"LEFT_PAREN",
	"RIGHT_PAREN",
	"LEFT_CURLY",
	"RIGHT_CURLY",
	"OPERATOR",
	"SEMICOLON",
	"IF",
	"THEN",
	"ELSE",
	"WHILE"
};

class Token {
public:
	enum TokenType type { WHITESPACE }; // Initialised to WHITESPACE
	string text { 0 };
	int stmtNum = 1;
		
	void print() const;

	void increaseStmtNum() {
		stmtNum += 1;
	}
};

class Tokenizer {
public:
	vector<Token> parse(const string& sourceProgram);

private:
	void endToken(Token& token, vector<Token>& tokens);
	void checkStmtType(Token& token);
};

