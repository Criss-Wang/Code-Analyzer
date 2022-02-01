#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "Parser.h"
#include "PKB.h"
#include "TNode.h"

using namespace std;

// Checks for procedure, procedure name, and open curly bracket
void Parser::parse(vector<Token>& tokens) {
	currToken = tokens.begin(); 
	endToken = tokens.end();

	while (currToken != endToken) {	
		optional<Token> possibleType = expectIdentifier();
			
		if (possibleType.has_value()) {
			optional<Token> possibleName = expectIdentifier();
				
			if (possibleName.has_value()) { // Name found
				optional<Token> possibleOpenBracket = expectBracket("{");

				if (possibleOpenBracket.has_value()) { // Procedure found
					cout << "Procedure: " << possibleName->text << endl;

					while (currToken != endToken) {
						parseStmtLst(tokens);
					}
				}
			}
		}
	}
	return;
}

// Checks for read and print statements, and semicolon
void Parser::parseStmtLst(vector<Token>& tokens) {
	optional<Token> possibleIdentifier = expectIdentifier();
	optional<Token> possibleCloseBracket = expectBracket("}");

	if (!possibleIdentifier.has_value()) {
		return;
	}

	if (possibleCloseBracket.has_value()) {
		cout << "Done";
	}

	if (possibleIdentifier->text == "read" || possibleIdentifier->text == "print") {
		optional<Token> possibleVar = expectIdentifier();

		if (possibleVar.has_value()) {
			optional<Token> possibleSemicolon = expectSymbol(";");

			if (possibleSemicolon.has_value()) {
				cout << "Statement " << possibleSemicolon->stmtNum << ": " 
					<< possibleIdentifier->text << " " << possibleVar->text << endl;
			}
		}
	}
}

// Checks for possible identifiers (Names)
optional<Token> Parser::expectIdentifier(const string& name) {
	if (currToken->type != NAME
		&& currToken->type != LETTER) {
		return nullopt;
	}

	if (!name.empty() && currToken->text != name) {
		return nullopt;
	}

	Token returnToken = *currToken;
	currToken++; // Increment current token
	return returnToken;
}

// Checks for possible brackets 
optional<Token> Parser::expectBracket(const string& name) {
	if (currToken->type != LEFT_CURLY
		&& currToken->type != RIGHT_CURLY
		&& currToken->type != LEFT_PAREN
		&& currToken->type != RIGHT_PAREN) {
		return nullopt;
	}

	if (!name.empty() && currToken->text != name) {
		return nullopt;
	}

	Token returnToken = *currToken;
	currToken++; // Increment current token
	return returnToken;
}

// Checks for possible semicolon
optional<Token> Parser::expectSymbol(const string& name) {
	if (currToken->type != SEMICOLON) {
		return nullopt;
	}

	if (!name.empty() && currToken->text != name) {
		return nullopt;
	}

	Token returnToken = *currToken;
	currToken++; // Increment current token
	return returnToken;
}
