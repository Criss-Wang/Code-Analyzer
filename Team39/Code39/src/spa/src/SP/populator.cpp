#include <stdio.h>
#include <iostream>
#include <stack>

using namespace std;

#include "populator.h"
#include "SP/design_extractor.h"

void populateProcedure(vector<Token> tokens, Pkb& pkb) {
  string proc_name = tokens.at(1).text;

  // Add procedure name to procedure_set_
  pkb.AddEntityToSet(EntityIdentifier::kProc, proc_name);
}

void populateReadStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(0).stmt_num_;
  string read_var = tokens.at(1).text;

  // Add stmt num to stmt_set_ and read_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kRead, stmt_num);

  // Add stmt num and read_var to ReadTable
  pkb.AddInfoToTable(TableIdentifier::kRead, stmt_num, read_var);

  // Add stmt num and read_var to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num, vector<string>{read_var});

  // Add read_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, read_var);
}

void populatePrintStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(0).stmt_num_;
  string print_var = tokens.at(1).text;

  // Add stmt num to stmt_set_ and print_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kPrint, stmt_num);

  // Add stmt num and print_var to PrintTable
  pkb.AddInfoToTable(TableIdentifier::kPrint, stmt_num, print_var);

  // Add stmt num and print_var to Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vector<string>{ print_var });

  // Add read_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, print_var);
}

void populateAssignStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(0).stmt_num_;
  string lhs_var = tokens.at(0).text;
  vector<string> rhs_vars;
  vector<int> rhs_constants;
  string assignment_pattern = "";

  // Add stmt num to stmt_set_ and assign_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kAssign, stmt_num);

  // Add lhs_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, lhs_var);

  // Build assignment pattern
  for (auto token = begin(tokens) + 2; token != end(tokens) - 1; ++token) {
    assignment_pattern += token->text;

    if (token->type == LETTER || token->type == NAME) {
      // Add RHS variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text);
      if (find(begin(rhs_vars), end(rhs_vars), token->text) == end(rhs_vars)) {
        rhs_vars.push_back(token->text);
      }

    } else if (token->type == DIGIT || token->type == INTEGER) {
      // Add RHS constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text));
      if (find(begin(rhs_constants), end(rhs_constants), stoi(token->text)) == end(rhs_constants)) {
        rhs_constants.push_back(stoi(token->text));
      }
    }
  }

  // Add stmt num and assignment pattern to Assign Table
  pkb.AddInfoToTable(TableIdentifier::kAssign, stmt_num, assignment_pattern);

  // Add stmt num and rhs constants to Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, rhs_constants);

  // Add stmt num and LHS variable to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num, vector<string>{ lhs_var });

  // Add stmt num and vector of variables into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, rhs_vars);
}

void populateIfStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(0).stmt_num_;
  vector<string> vars_in_cond_expr;
  vector<int> constants_in_cond_expr;

  // Add stmt num to stmt_set_ and if_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kIf, stmt_num);

  for (auto token = begin(tokens) + 2; token != end(tokens) - 2; ++token) {

    if (token->type == LETTER || token->type == NAME) {
      // Add variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text);
      if (find(begin(vars_in_cond_expr), end(vars_in_cond_expr), token->text) == end(vars_in_cond_expr)) {
        vars_in_cond_expr.push_back(token->text);
      }

    } else if (token->type == DIGIT || token->type == INTEGER) {
      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text));
      if (find(begin(constants_in_cond_expr), end(constants_in_cond_expr), stoi(token->text)) == end(constants_in_cond_expr)) {
        constants_in_cond_expr.push_back(stoi(token->text));
      }
    }
  }

  // Add stmt num and variables in cond expr into If Table
  pkb.AddInfoToTable(TableIdentifier::kIf, stmt_num, vars_in_cond_expr);

  // Add stmt num and constants in cond expr into Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_in_cond_expr);

  // Add stmt num and variables in cond expr into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_in_cond_expr);
}

void populateWhileStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(0).stmt_num_;
  vector<string> vars_in_cond_expr;
  vector<int> constants_in_cond_expr;

  // Add stmt num to stmt_set_ and while_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kWhile, stmt_num);

  for (auto token = begin(tokens) + 2; token != end(tokens) - 2; ++token) {
    if (token->type == LETTER || token->type == NAME) {
      // Add variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text);
      if (find(begin(vars_in_cond_expr), end(vars_in_cond_expr), token->text) == end(vars_in_cond_expr)) {
        vars_in_cond_expr.push_back(token->text);
      }

    } else if (token->type == DIGIT || token->type == INTEGER) {
      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text));
      if (find(begin(constants_in_cond_expr), end(constants_in_cond_expr), stoi(token->text)) == end(constants_in_cond_expr)) {
        constants_in_cond_expr.push_back(stoi(token->text));
      }
    }
  }

  // Add stmt num and variables in cond expr into While Table
  pkb.AddInfoToTable(TableIdentifier::kWhile, stmt_num, vars_in_cond_expr);

  // Add stmt num and constants in cond expr into Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_in_cond_expr);

  // Add stmt num and variables in cond expr into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_in_cond_expr);
}

stack<int> populateFollowsRelationship(stack<int> previous, Pkb& pkb, int stmt_num) {
  if (!previous.empty()) {
    if (stmt_num == previous.top()) {
      return previous;
    } else {
      // Add previous stmt num and current stmt num to FollowsTable
      pkb.AddInfoToTable(TableIdentifier::kFollows, previous.top(), stmt_num);
      previous.pop();
    }
  }

  previous.push(stmt_num);
  return previous;
}

stack<vector<int>> populateParentRelationship(stack<int> parent, stack<vector<int>> children, int stmt_num) {
  if (!parent.empty()) {
    children.top().push_back(stmt_num);
  }

  return children;
}

void populate(vector<Token> input_tokens, Pkb& pkb) {

  // Stores parent/previous stmt's line number for Parent/Follows relationship
  stack<int> parent;
  stack<vector<int>> children;
  children.push({});
  stack<int> previous;
  
  set<int> stmt_lst;

  for (auto token = begin(input_tokens); token != end(input_tokens); ++token) {
    if (token->text == "procedure") {
      vector<Token> tokens;
      while (token->type != LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      populateProcedure(tokens, pkb);

    } else if (token->type == RIGHT_CURLY) {
      
      int previous_stmt_num = 0;
      if (!previous.empty()) {
        previous_stmt_num = previous.top();
        previous.pop();
      }

      bool has_two_more_tokens = token != end(input_tokens) - 1 && token != end(input_tokens) - 2;
      bool is_else_stmt = has_two_more_tokens && next(token, 1)->text == "else" && next(token, 2)->type == LEFT_CURLY;

      if (is_else_stmt) {
        previous.push(previous_stmt_num + 1);
      } else {
        if (!parent.empty()) {
          // Add parent stmt num and current stmt num to ParentTable
          if (!children.top().empty()) {
            pkb.AddInfoToTable(TableIdentifier::kParent, parent.top(), children.top());
          }
          parent.pop();
          children.pop();
        }
      }

    } else if (next(token, 1)->text == "=") {
      vector<Token> tokens;
      while (token->type != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      populateAssignStmt(tokens, pkb);
      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      stmt_lst.insert(token->stmt_num_);

    } else if (token->text == "read") {
      vector<Token> tokens;
      while (token->type != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      populateReadStmt(tokens, pkb);
      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      stmt_lst.insert(token->stmt_num_);

    } else if (token->text == "print") {
      vector<Token> tokens;
      while (token->type != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      populatePrintStmt(tokens, pkb);
      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      stmt_lst.insert(token->stmt_num_);

    } else if (token->text == "if") {
      vector<Token> tokens;
      while (token->type != LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      populateIfStmt(tokens, pkb);

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);
      parent.push(token->stmt_num_);
      children.push({});
      previous.push(token->stmt_num_ + 1);

      stmt_lst.insert(token->stmt_num_);

    } else if (token->text == "while") {
      vector<Token> tokens;
      while (token->type != LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      
      populateWhileStmt(tokens, pkb);

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);
      parent.push(token->stmt_num_);
      children.push({});
      previous.push(token->stmt_num_ + 1);

      stmt_lst.insert(token->stmt_num_);

    } else if (token->text == "call") {
      // TODO (Yuxuan): Add implementation in future iterations

    }
  }

  // Add stmt_lst to stmt_list_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmtLst, stmt_lst);

  if (PopulateNestedRelationships(pkb) == 0) {
    throw invalid_argument("PKB Population failed");
  }
}
