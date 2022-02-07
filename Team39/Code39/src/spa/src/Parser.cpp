#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "Parser.h"
#include "SP/validator.h"
#include "PKB/tables/table.h"

void Parse(string input) {

  Pkb pkb; // TODO: Need to integrate with globally declared pkb for autotester
  Tokenizer tokenizer;
  vector<Token> inputTokens = tokenizer.parse(input);

  if (Validate(inputTokens)) {

    // Stores parent/previous stmt's line number for Parent/Follows relationship
    int parent = -1;
    int previous = -1;
    bool is_rhs_of_assignment = false;
    bool is_procedure_name = false;
    bool is_variable_name = false;

    for (auto token = begin(inputTokens); token != end(inputTokens); ++token) {
      
      if (token->type == NAME && token->text == "procedure") {
        parent = token->stmt_num_;
        is_procedure_name = true;

      } else if (token->text == "read") {

        string read_var = next(token, 1)->text;

        // Add stmt num to stmt_set_ and read_set_
        pkb.AddEntityToSet(EntityIdentifier::kStmt, token->stmt_num_);
        pkb.AddEntityToSet(EntityIdentifier::kRead, token->stmt_num_);

        pkb.AddInfoToTable(TableIdentifier::kRead, token->stmt_num_, read_var);

        // Add modifies to modify stmt to var
        pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, token->stmt_num_, { token->text });


        // TODO (Yuxuan): Add helper function to avoid repeating same code
        if (previous != -1) {
          // Add followsBy and followsAfter
          pkb.AddInfoToTable(TableIdentifier::kFollows, previous, token->stmt_num_);
        }
        if (parent != -1) {
          // Add parent and child
          pkb.AddInfoToTable(TableIdentifier::kParent, parent, token->stmt_num_);
        }

      } else if (token->text == "print") {

        string print_var = next(token, 1)->text;

        // Add stmt num to stmt_set_ and print_set_
        pkb.AddEntityToSet(EntityIdentifier::kStmt, token->stmt_num_);
        pkb.AddEntityToSet(EntityIdentifier::kPrint, token->stmt_num_);

        pkb.AddInfoToTable(TableIdentifier::kPrint, token->stmt_num_, print_var);

        // add uses
        pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, token->stmt_num_, { token->text });

        if (previous != -1) {
          // Add follows
          pkb.AddInfoToTable(TableIdentifier::kFollows, previous, token->stmt_num_);
        }
        if (parent != -1) {
          // Add parent
          pkb.AddInfoToTable(TableIdentifier::kParent, parent, token->stmt_num_);
        }

      } else if (token->text == "call") {
        // TODO (Yuxuan): Add implementation in future iterations

      } else if (token->type == WHILE) {
        // TODO (Yuxuan): Add implementation in future iterations

      } else if (token->type == IF) {
        // TODO (Yuxuan): Add implementation in future iterations

      } else if (token->type == LETTER || token->type == NAME) {
        if (is_procedure_name) {
          // Add procedure name to procedure_set_
          pkb.AddEntityToSet(EntityIdentifier::kProc, token->text);
          is_procedure_name = false;
        } else {
          pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text);
        }

        bool is_assignment = next(token, 1)->text == "=";
        if (is_assignment) {

          // Add stmt num to stmt_set_ and assign_set_
          pkb.AddEntityToSet(EntityIdentifier::kStmt, token->stmt_num_);
          pkb.AddEntityToSet(EntityIdentifier::kAssign, token->stmt_num_);

          // Build assignments for pattern matching
          string assignment = "";
          int i = 2;
          while (next(token, i)->type != SEMICOLON) {
            assignment += next(token, i)->text;
            i++;
          }

          // add brackets for '*' or '/' first
          for (int i = 0; i < assignment.length(); i++) {
            if (assignment[i] == '*' || assignment[i] == '/') {
              int right_offset = 1;
              if (assignment.substr(i).find(")") != string::npos) {
                right_offset = assignment.substr(i).find(")") + 1;
              }
              assignment.insert(i + right_offset + 1, ")");

              int pos = i - 1;
              int start = 0;
              while (assignment.substr(start, i).find("(") != string::npos) {
                pos = assignment.substr(start, i).find("(");
                start++;
              }
              assignment.insert(pos, "(");

              i += 2;
            }
          }
          
          // add brackets for '+' and '-'
          for (int i = 0; i < assignment.length(); i++) {
            if (assignment[i] == '+' || assignment[i] == '-') {
              int right_offset = 1;
              if (assignment.substr(i).find(")") != string::npos) {
                right_offset = assignment.substr(i).find(")") + 1;
              }
              assignment.insert(i + right_offset + 1, ")");

              int pos = i - 1;
              int start = 0;
              while (assignment.substr(start, i).find("(") != string::npos) {
                pos = assignment.substr(start, i).find("(");
                start++;
              }
              assignment.insert(pos, "(");

              i += 2;
            }
          }

          pkb.AddInfoToTable(TableIdentifier::kAssign, token->stmt_num_, assignment);

          // Add modifies to modify stmt to var
          pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, token->stmt_num_, { token->text });

          // build a vector containing all variables on the RHS and put them into UsesStmtToVar table
          string uses_var = "";
          vector<string> all_uses_var;
          for (int i = 0; i < assignment.length(); i++) {
            if (isalpha(assignment[i]) || isdigit(assignment[i])) {
              uses_var += assignment[i];
            } else {
              if (uses_var != "") {
                all_uses_var.push_back(uses_var);
                uses_var = "";
              }
            }
          }
          pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, token->stmt_num_, all_uses_var);
        }

      } else if (token->type == DIGIT || token->type == INTEGER) {
        // Add constant to constant_set_
        pkb.AddEntityToSet(EntityIdentifier::kConstant, token->text);

      } else if (token->type == LEFT_CURLY) {
        parent = token->stmt_num_;

      } else if (token->type == RIGHT_CURLY) {
        parent = -1;

      } else if (token->type == SEMICOLON) {
        previous = token->stmt_num_;
        is_rhs_of_assignment = false;

      } else if (token->type == OPERATOR) {
        if (token->text == "=") {
          is_rhs_of_assignment = true;

          if (previous != -1) {
            // Add followsBy
            pkb.AddInfoToTable(TableIdentifier::kFollows, previous, token->stmt_num_);
          }
          if (parent != -1) {
            // Add parent
            pkb.AddInfoToTable(TableIdentifier::kParent, parent, token->stmt_num_);
          }
        }
      } else {
        cout << "Error: Token type not recognized" << endl;
      }
    }

    /* 
    // Print tables for testing purposes 
    cout << "Procedure Names: " << endl;
    for (auto const& i : pkb.GetAllEntityString(Pkb::EntityIdentifier::kProc)) {
      std::cout << i << " ";
    }
    cout << endl;

    cout << "Variable Names: " << endl;
    for (auto const& i : pkb.GetAllEntityString(Pkb::EntityIdentifier::kVariable)) {
      std::cout << i << " ";
    }
    cout << endl;
    */

    if (pkb.PopulateNestedRelationship() == 0) {
      throw invalid_argument("PKB Population failed");
    }    
  }
}
