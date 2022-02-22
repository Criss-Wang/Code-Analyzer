#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "validator.h"
#include "tokenizer.h"

bool validateProcedure(vector<Token> tokens) {
  bool check_size = tokens.size() == 3;
  if (!check_size) {
    return false;
  }
  bool check_variable = tokens.at(1).type == NAME || tokens.at(1).type == LETTER;
  bool check_left_curly = tokens.at(2).type == LEFT_CURLY;
  return check_size && check_variable && check_left_curly;
}

bool validateReadPrintStmt(vector<Token> tokens) {
  bool check_size = tokens.size() == 3;
  if (!check_size) {
    return false;
  }
  bool check_variable = tokens.at(1).type == NAME || tokens.at(1).type == LETTER;
  bool check_semicolon = tokens.at(2).type == SEMICOLON;
  return check_size && check_variable && check_semicolon;
}

bool validateAssignStmt(vector<Token> tokens) {
  bool check_size = tokens.size() > 3;
  if (!check_size) {
    return false;
  }
  bool check_lhs = tokens.at(0).type == NAME || tokens.at(0).type == LETTER;
  bool check_rhs = true;
  bool check_semicolon = tokens.at(tokens.size() - 1).type == SEMICOLON;

  vector<TokenType> expected_types = { NAME, INTEGER, LEFT_PAREN };
  vector<string> expected_operators = { "*", "/", "+", "-", "%" };

  // keep track of number of brackets
  int paren_count = 0;
  
  for (auto token = begin(tokens) + 2; token != end(tokens) - 1; ++token) {
    TokenType token_type;

    if (token->type == LETTER) {
      token_type = NAME;
    } else if (token->type == DIGIT) {
      token_type = INTEGER;
    } else {
      token_type = token->type;
    }

    bool check_type = find(begin(expected_types), end(expected_types), token_type) != end(expected_types);
    bool check_operator = true;

    if (token_type == OPERATOR) {
      check_operator = find(begin(expected_operators), end(expected_operators), token->text) != end(expected_operators);
    }
    check_rhs = check_rhs && check_type && check_operator;

    expected_types = {};

    if (token_type == OPERATOR
      || token_type == LEFT_PAREN) { // expects variable, integer or left paren after operator/left paren
      expected_types.push_back(NAME);
      expected_types.push_back(INTEGER);
      expected_types.push_back(LEFT_PAREN);

      if (token_type == LEFT_PAREN) {
        paren_count += 1;
      }

    } else if (token_type == RIGHT_PAREN) { // expects operator after right paren
      expected_types.push_back(OPERATOR);
      expected_types.push_back(RIGHT_PAREN);

      paren_count -= 1;

    } else if (token_type == NAME || token_type == INTEGER) { // expects operator or right paren after variable or integer
      expected_types.push_back(OPERATOR);
      expected_types.push_back(RIGHT_PAREN);
    }

  }

  bool check_brackets = paren_count == 0;
  return check_size && check_lhs && check_rhs && check_semicolon && check_brackets;
}

bool validateCondExpr(vector<Token> tokens) {
  bool check_cond_expr = true;

  vector<TokenType> expected_types = { NAME, INTEGER, LEFT_PAREN, NOT_OPERATOR };
  vector<string> rel_operators = { ">", "<", "==", "!=", ">=", "<=" };

  // keep track of number of brackets
  int paren_count = 0;

  for (auto token = begin(tokens); token != end(tokens); ++token) {

    TokenType token_type;

    if (token->type == LETTER) {
      token_type = NAME;
    } else if (token->type == DIGIT) {
      token_type = INTEGER;
    } else if (token->text == "!") {
      token_type = NOT_OPERATOR;
    } else if (token->text == "||" || token->text == "&&") {
      token_type = COND_OPERATOR;
    } else if (find(begin(rel_operators), end(rel_operators), token->text) != end(rel_operators)) {
      token_type = REL_OPERATOR;
    } else {
      token_type = token->type;
    }

    bool check_type = find(begin(expected_types), end(expected_types), token_type) != end(expected_types);
    check_cond_expr = check_cond_expr && check_type;

    expected_types = {};

    if (token_type == LEFT_PAREN) { // expects variable, integer or left paren after left paren
      expected_types.push_back(NAME);
      expected_types.push_back(INTEGER);
      expected_types.push_back(LEFT_PAREN);
      expected_types.push_back(NOT_OPERATOR);

      paren_count += 1;

    } else if (token_type == RIGHT_PAREN) { // expects cond operator after right paren
      expected_types.push_back(COND_OPERATOR);
      expected_types.push_back(RIGHT_PAREN);
      expected_types.push_back(OPERATOR);

      paren_count -= 1;

    } else if (token_type == NOT_OPERATOR) { // expects left paren after not operator
      expected_types.push_back(LEFT_PAREN);

    } else if (token_type == NAME || token_type == INTEGER) { // expects rel operator or right paren after variable or integer
      expected_types.push_back(REL_OPERATOR);
      expected_types.push_back(RIGHT_PAREN);
      expected_types.push_back(OPERATOR);

    } else if (token_type == COND_OPERATOR || token_type == REL_OPERATOR || token_type == OPERATOR) { // expected name, int or left after operator
      expected_types.push_back(NAME);
      expected_types.push_back(INTEGER);
      expected_types.push_back(LEFT_PAREN);
    }
  }

  bool check_brackets = paren_count == 0;
  return check_cond_expr && check_brackets;
}

bool validateWhileStmt(vector<Token> tokens) {
  bool check_size = tokens.size() > 6;
  if (!check_size) {
    return false;
  }
  bool check_left_paren = tokens.at(1).type == LEFT_PAREN;
  bool check_right_paren = tokens.at(tokens.size() - 2).type == RIGHT_PAREN;
  bool check_left_curly = tokens.at(tokens.size() - 1).type == LEFT_CURLY;

  vector<Token>::const_iterator first = tokens.begin() + 2;
  vector<Token>::const_iterator last = tokens.end() - 2;
  vector<Token> cond_expr(first, last);
  bool check_cond_expr = validateCondExpr(cond_expr);

  return check_size && check_left_paren && check_right_paren && check_left_curly && check_cond_expr;
}

bool validateIfStmt(vector<Token> tokens) {
  bool check_size = tokens.size() > 7;
  if (!check_size) {
    return false;
  }
  bool check_left_paren = tokens.at(1).type == LEFT_PAREN;
  bool check_right_paren = tokens.at(tokens.size() - 3).type == RIGHT_PAREN;
  bool check_then_keyword = tokens.at(tokens.size() - 2).type == NAME && tokens.at(tokens.size() - 2).text == "then";
  bool check_left_curly = tokens.at(tokens.size() - 1).type == LEFT_CURLY;

  vector<Token>::const_iterator first = tokens.begin() + 2;
  vector<Token>::const_iterator last = tokens.end() - 3;
  vector<Token> cond_expr(first, last);
  bool check_cond_expr = validateCondExpr(cond_expr);

  return check_size && check_left_paren && check_right_paren && check_left_curly
    && check_cond_expr;
}

bool Validate(vector<Token> input) {

  // keep track of number of brackets and number of if/else
  int curly_bracket_count = 0;
  int if_stmts = 0;

  for (auto token = begin(input); token != end(input); ++token) {

    TokenType token_type;
    string token_text;

    if (token->type == RIGHT_CURLY) {
      bool has_two_more_tokens = token != end(input) - 1 && token != end(input) - 2;
      if (has_two_more_tokens && next(token, 1)->text == "else" && next(token, 2)->type == LEFT_CURLY) {
        if_stmts -= 1;
        token++;
        token++;
      } else {
        curly_bracket_count -= 1;
      }

    } else if (next(token, 1)->text == "=") {
      vector<Token> tokens;
      while (token->type != SEMICOLON && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateAssignStmt(tokens)) {
        return false;
      }

    } else if (token->text == "procedure") {
      vector<Token> tokens;
      while (token->type != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateProcedure(tokens)) {
        return false;
      }

      curly_bracket_count += 1;

    } else if (token->text == "while") {
      vector<Token> tokens;
      while (token->type != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateWhileStmt(tokens)) {
        return false;
      } else {
        curly_bracket_count += 1;
      }

    } else if (token->text == "if") {
      vector<Token> tokens;
      while (token->type != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateIfStmt(tokens)) {
        return false;
      } else {
        curly_bracket_count += 1;
        if_stmts += 1;
      }

    } else if (token->text == "read" || token->text == "print") {
      vector<Token> tokens;
      while (token->type != SEMICOLON && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateReadPrintStmt(tokens)) {
        return false;
      }

    } else {
      return false;
    }
  }

  if (curly_bracket_count == 0 && if_stmts == 0) {
    return true;
  } else {
    return false;
  }

}
