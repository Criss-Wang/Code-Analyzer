#include <stdio.h>
#include <iostream>
#include <stack>

using namespace std;

#include "populator.h"

vector<string> findUsedVaribales(string assignment) {
  string uses_var = "";
  vector<string> all_used_var;
  for (int i = 0; i < assignment.length(); i++) {
    if (isalpha(assignment[i]) || (isdigit(assignment[i]) && uses_var != "")) {
      uses_var += assignment[i];
    } else {
      if (uses_var != "") {
        all_used_var.push_back(uses_var);
        uses_var = "";
      }
    }
  }
  return all_used_var;
}

void populate(vector<Token> inputTokens, Pkb& pkb) {

  // Stores parent/previous stmt's line number for Parent/Follows relationship
  stack<int> parent;
  stack<int> previous;
  bool is_procedure = false;
  bool is_new_nesting = false;

  for (auto token = begin(inputTokens); token != end(inputTokens); ++token) {

    if (token->type == NAME && token->text == "procedure") {

      is_procedure = true;

    } else if (token->text == "read") {

      string read_var = next(token, 1)->text;

      // Add stmt num to stmt_set_ and read_set_
      pkb.AddEntityToSet(EntityIdentifier::kStmt, token->stmt_num_);
      pkb.AddEntityToSet(EntityIdentifier::kRead, token->stmt_num_);

      // Add stmt num and read_var to ReadTable
      pkb.AddInfoToTable(TableIdentifier::kRead, token->stmt_num_, read_var);

      // Add stmt num and read_var to Modifies Table
      pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, token->stmt_num_, { token->text });

    } else if (token->text == "print") {

      string print_var = next(token, 1)->text;

      // Add stmt num to stmt_set_ and print_set_
      pkb.AddEntityToSet(EntityIdentifier::kStmt, token->stmt_num_);
      pkb.AddEntityToSet(EntityIdentifier::kPrint, token->stmt_num_);

      // Add stmt num and print_var to PrintTable
      pkb.AddInfoToTable(TableIdentifier::kPrint, token->stmt_num_, print_var);

      // Add stmt num and print_var to Uses Table
      pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, token->stmt_num_, { token->text });

    } else if (token->text == "call") {
      // TODO (Yuxuan): Add implementation in future iterations

    } else if (token->type == WHILE) {
      // TODO (Yuxuan): Add implementation in future iterations

    } else if (token->type == IF) {
      // TODO (Yuxuan): Add implementation in future iterations

    } else if (token->type == LETTER || token->type == NAME) {

      if (is_procedure) {
        // Add procedure name to procedure_set_
        pkb.AddEntityToSet(EntityIdentifier::kProc, token->text);

      } else {
        // Add variable name to variable_set_
        pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text);
      }

      bool is_assignment = next(token, 1)->text == "=";

      if (is_assignment) {

        // Add stmt num to stmt_set_ and assign_set_
        pkb.AddEntityToSet(EntityIdentifier::kStmt, token->stmt_num_);
        pkb.AddEntityToSet(EntityIdentifier::kAssign, token->stmt_num_);

        // Build assignment pattern
        string assignment = "";
        int i = 2;
        while (next(token, i)->type != SEMICOLON) {
          assignment += next(token, i)->text;
          i++;
        }

        // Add stmt num and assignment pattern to AssignTable
        pkb.AddInfoToTable(TableIdentifier::kAssign, token->stmt_num_, assignment);

        // Add stmt num and LHS variable to Modifies Table
        pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, token->stmt_num_, { token->text });

        // Build a vector containing all variables in the assignment pattern
        vector<string> used_varibles = findUsedVaribales(assignment);

        // Add stmt num and vector of variables into Uses Table
        pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, token->stmt_num_, used_varibles);
      }

    } else if (token->type == DIGIT || token->type == INTEGER) {

      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, token->text);

    } else if (token->type == LEFT_CURLY) {

      if (!is_procedure) {
        parent.push(token->stmt_num_);
        is_new_nesting = true;
      }

    } else if (token->type == RIGHT_CURLY) {

      if (is_procedure) {
        is_procedure = false;
      } else {
        parent.pop();
        previous.pop();
        is_new_nesting = false;
      }

    } else if (token->type == SEMICOLON) {

      if (!previous.empty() && !is_new_nesting) {
        // Add previous stmt num and current stmt num to FollowsTable
        pkb.AddInfoToTable(TableIdentifier::kFollows, previous.top(), token->stmt_num_);
        previous.pop();
      }

      if (!parent.empty()) {
        // Add parent stmt num and current stmt num to ParentTable
        pkb.AddInfoToTable(TableIdentifier::kParent, parent.top(), token->stmt_num_);
      }

      previous.push(token->stmt_num_);

    }
  }

  if (pkb.PopulateNestedRelationship() == 0) {
    throw invalid_argument("PKB Population failed");
  }
}
