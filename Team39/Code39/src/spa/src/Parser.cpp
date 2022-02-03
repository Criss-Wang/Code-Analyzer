#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

<<<<<<< HEAD
#include "Parser.h"
#include "PKB.h"
=======
#include "PKB/pkb.h"
#include "Utility/TNode.h"
>>>>>>> 9b6b81faab7bc66b1369f9387349897ccd4bc3d4

void parse (string sourceProgramInput) {

	/* 
	* Tokenizer tokenizer;
	* Validator validator;
	* 
	* vector<Token> inputTokens = tokenizer.parse(sourceProgramInput);
	* if (validator.validate(inputTokens)) {
	*	populate PKB;
	* } else {
	*	cout << "Syntax Error";
	* }
	*/

	// TODO (Yuxuan): [Integration] Use actual vector<Token> returned by Tokenizer to populate
	struct Token {
		string type;
		string text;
		int line_num;
	};

	Token token1{ "procedure", "procName", 1 };
	Token token2{ "read", "x", 1 };
	Token token3{ "print", "y", 1 };
	vector<Token> input = { token1, token2, token3 };

	// TODO (Yuxuan): [Integration] Use PKB declared globally
	PKB pkb;

	// Stores parent/previous stmt's line number for Parent/Follows relationship
	int parent = -1;
	int previous = -1;
	bool is_rhs_of_assignment = false;

	for (auto token = begin(input); token != end(input); ++token) {
		if (token->type == "procedure") {
			pkb->proc_table_.AddEntityByName(token->text);
			parent = token->line_num;

		} else if (token->type == "variable") {
			pkb->var_table_.AddEntityByName(token->text);
			bool is_assignment = next(token, 1)->text == "=";
			if (is_assignment) {
				// TODO (Yuxuan): populate modifies relationship using API given by PKB
				pkb->modifies_table_.AddEntity(token->line_num, token->text);
			} else if (is_rhs_of_assignment) {
				// TODO (Yuxuan): populate uses relationship using API given by PKB
				pkb->uses_table_.AddEntity(token->line_num, token->text);
			}
		} else if (token->type == "constant") {
			pkb->const_table_.AddEntityByName(token->text);

		} else if (token->type == "read") {
			pkb->stmt_table.AddEntity(token->line_num, StmtTable::read_idx_);
			if (previous != -1) {
				// TODO (Yuxuan): populate follows relationship using API given by PKB
				pkb->follows_table_.AddEntity(previous, token->line_num);
			}
			if (parent != -1) {
				// TODO (Yuxuan): populate parent relationship using API given by PKB
				pkb->parent_table_.AddEntity(parent, token->line_num);
			}
			previous = token->line_num;

		} else if (token->type == "write") {
			pkb->stmt_table.AddEntity(token->line_num, StmtTable::print_idx_);

		} else if (token->type == "assign") {
			pkb->stmt_table.AddEntity(token->line_num, StmtTable::assign_idx_);

		} else if (token->type == "call") {
			pkb->stmt_table.AddEntity(token->line_num, StmtTable::call_idx_);

		} else if (token->type == "while") {
			pkb->stmt_table.AddEntity(token->line_num, StmtTable::while_idx_);

		} else if (token->type == "if") {
			pkb->stmt_table.AddEntity(token->line_num, StmtTable::if_idx_);

		} else {
			cout << "Error: Token type not recognized";
		}
	}

	return 0;
}
