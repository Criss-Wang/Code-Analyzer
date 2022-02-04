#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "Parser.h"
#include "SP/validator.h"

<<<<<<< HEAD
void Parse(vector<Token> input) {

  /*
  * Tokenizer tokenizer;
  *
  * vector<Token> inputTokens = tokenizer.parse(sourceProgramInput);
  * if (validator.validate(inputTokens)) {
  *	populate PKB;
  * } else {
  *	cout << "Syntax Error";
  * }
  */


  // TODO (Yuxuan): [Integration] Use PKB declared globally
  Pkb pkb;

  if (Validate(input)) {

    // Stores parent/previous stmt's line number for Parent/Follows relationship
    int parent = -1;
    int previous = -1;
    bool is_rhs_of_assignment = false;

    for (auto token = begin(input); token != end(input); ++token) {
      if (token->type == "procedure") {
        string proc_name = next(token, 1)->text;
        pkb.proc_table_.AddEntityByName(proc_name);
        parent = token->line_num;
        cout << "add procedure: " << proc_name << endl;

      } else if (token->type == "variable" || token->type == "name") {
        pkb.var_table_.AddEntityByName(token->text);
        cout << "add variable: " << token->text << endl;

        bool is_assignment = next(token, 1)->text == "=";
        if (is_assignment) {
          // TODO (Yuxuan): populate modifies relationship using API given by PKB
          cout << "add modifies: " << token->line_num << ", " << token->text << endl;

        } else if (is_rhs_of_assignment) {
          // TODO (Yuxuan): populate uses relationship using API given by PKB
          cout << "add uses: " << token->line_num << ", " << token->text << endl;
        }
      } else if (token->type == "constant") {
        pkb.const_table_.AddEntityByName(token->text);
        cout << "add constant: " << token->text << endl;

      } else if (token->type == "read") {
        string read_var = next(token, 1)->text;
        pkb.stmt_table_.AddEntity(token->line_num, StmtTable::read_idx_);
        cout << "add read stmt: line " << token->line_num << endl;

        // TODO (Yuxuan): populate modifies relationship using API given by PKB
        cout << "add modifies: " << token->line_num << ", " << read_var << endl;

        if (previous != -1) {
          // TODO (Yuxuan): populate follows relationship using API given by PKB
          cout << "add follows: " << previous << ", " << token->line_num << endl;
        }
        if (parent != -1) {
          // TODO (Yuxuan): populate parent relationship using API given by PKB
          cout << "add parent: " << parent << ", " << token->line_num << endl;
        }

      } else if (token->type == "print") {
        string print_var = next(token, 1)->text;
        pkb.stmt_table_.AddEntity(token->line_num, StmtTable::print_idx_);
        cout << "add print stmt: line " << token->line_num << endl;

        // TODO (Yuxuan): populate modifies relationship using API given by PKB
        cout << "add uses: " << token->line_num << ", " << print_var << endl;

        if (previous != -1) {
          // TODO (Yuxuan): populate follows relationship using API given by PKB
          cout << "add follows: " << previous << ", " << token->line_num << endl;
        }
        if (parent != -1) {
          // TODO (Yuxuan): populate parent relationship using API given by PKB
          cout << "add parent: " << parent << ", " << token->line_num << endl;
        }

      } else if (token->type == "assign") { // kiv, tokenType no assign var?
        pkb.stmt_table_.AddEntity(token->line_num, StmtTable::assign_idx_);

      } else if (token->type == "call") {
        pkb.stmt_table_.AddEntity(token->line_num, StmtTable::call_idx_);

      } else if (token->type == "while") {
        pkb.stmt_table_.AddEntity(token->line_num, StmtTable::while_idx_);

      } else if (token->type == "if") {
        pkb.stmt_table_.AddEntity(token->line_num, StmtTable::if_idx_);

      } else if (token->type == "{") {
        parent = token->line_num;

      } else if (token->type == "}") {
        parent = -1;

      } else if (token->type == ";") {
        previous = token->line_num;
        is_rhs_of_assignment = false;

      } else if (token->type == "operator") {
        if (token->text == "=") {
          is_rhs_of_assignment = true;
          cout << "add assign stmt: line " << token->line_num << endl;

          if (previous != -1) {
            // TODO (Yuxuan): populate follows relationship using API given by PKB
            cout << "add follows: " << previous << ", " << token->line_num << endl;
          }
          if (parent != -1) {
            // TODO (Yuxuan): populate parent relationship using API given by PKB
            cout << "add parent: " << parent << ", " << token->line_num << endl;
          }
        }
      } else {
        cout << "Error: Token type not recognized" << endl;
      }
    }
  }
}
=======
int Parse() {

  return 0;
}
>>>>>>> 91459c69eecf45cdca5b3c470bb29ccddf960d2e
