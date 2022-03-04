#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "validator.h"
#include "tokenizer.h"

const int kFirstIndex = 0;
const int kSecondIndex = 1;
const int kThirdIndex = 2;

bool validateProcedure(vector<Token> tokens) {
  int expected_size = 3;
  if (tokens.size() != expected_size) {
    return false;
  }

  bool check_variable = tokens.at(kSecondIndex).type_ == NAME || tokens.at(kSecondIndex).type_ == LETTER;
  bool check_left_curly = tokens.at(kThirdIndex).type_ == LEFT_CURLY;
  return check_variable && check_left_curly;
}

bool validateReadPrintStmt(vector<Token> tokens) {
  int expected_size = 3;
  if (tokens.size() != expected_size) {
    return false;
  }

  bool check_variable = tokens.at(kSecondIndex).type_ == NAME || tokens.at(kSecondIndex).type_ == LETTER;
  bool check_semicolon = tokens.at(kThirdIndex).type_ == SEMICOLON;
  return check_variable && check_semicolon;
}

bool validateAssignStmt(vector<Token> tokens) {
  int min_stmt_size = 4;
  if (tokens.size() < min_stmt_size) {
    return false;
  }

  bool check_lhs = tokens.at(kFirstIndex).type_ == NAME || tokens.at(kFirstIndex).type_ == LETTER;
  bool check_rhs = true;

  const int kLastIndex = tokens.size() - 1;
  bool check_semicolon = tokens.at(kLastIndex).type_ == SEMICOLON;

  vector<TokenType> expected_types = { NAME, INTEGER, LEFT_PAREN };
  vector<string> expected_operators = { "*", "/", "+", "-", "%" };

  // keep track of number of brackets
  int paren_count = 0;
  
  vector<Token>::const_iterator expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator expr_end = tokens.begin() + kLastIndex;
  vector<Token> expr(expr_start, expr_end);

  for (auto token = begin(expr); token != end(expr); ++token) {
    TokenType token_type;

    if (token->type_ == LETTER) {
      token_type = NAME;
    } else if (token->type_ == DIGIT) {
      token_type = INTEGER;
    } else {
      token_type = token->type_;
    }

    bool check_type_ = find(begin(expected_types), end(expected_types), token_type) != end(expected_types);
    bool check_operator = true;

    if (token_type == OPERATOR) {
      check_operator = find(begin(expected_operators), end(expected_operators), token->text_) != end(expected_operators);
    }
    check_rhs = check_rhs && check_type_ && check_operator;

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
  return check_lhs && check_rhs && check_semicolon && check_brackets;
}

bool validateCondExpr(vector<Token> tokens) {
  int min_expr_size = 3;
  if (tokens.size() < min_expr_size) {
    return false;
  }
  bool check_cond_expr = true;

  vector<TokenType> expected_types = { NAME, INTEGER, LEFT_PAREN, NOT_OPERATOR };
  vector<string> rel_operators = { ">", "<", "==", "!=", ">=", "<=" };

  // keep track of number of brackets
  int paren_count = 0;

  for (auto token = begin(tokens); token != end(tokens); ++token) {

    TokenType token_type;

    if (token->type_ == LETTER) {
      token_type = NAME;
    } else if (token->type_ == DIGIT) {
      token_type = INTEGER;
    } else if (token->text_ == "!") {
      token_type = NOT_OPERATOR;
    } else if (token->text_ == "||" || token->text_ == "&&") {
      token_type = COND_OPERATOR;
    } else if (find(begin(rel_operators), end(rel_operators), token->text_) != end(rel_operators)) {
      token_type = REL_OPERATOR;
    } else {
      token_type = token->type_;
    }

    bool check_type_ = find(begin(expected_types), end(expected_types), token_type) != end(expected_types);
    check_cond_expr = check_cond_expr && check_type_;

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
  int min_stmt_size = 7;
  if (tokens.size() < min_stmt_size) {
    return false;
  }

  bool check_left_paren = tokens.at(kSecondIndex).type_ == LEFT_PAREN;

  const int kLastIndex = tokens.size() - 1;
  const int kSecondLastIndex = kLastIndex - 1;

  bool check_right_paren = tokens.at(kSecondLastIndex).type_ == RIGHT_PAREN;
  bool check_left_curly = tokens.at(kLastIndex).type_ == LEFT_CURLY;

  vector<Token>::const_iterator cond_expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator cond_expr_end = tokens.begin() + kSecondLastIndex;
  vector<Token> cond_expr(cond_expr_start, cond_expr_end);
  bool check_cond_expr = validateCondExpr(cond_expr);

  return check_left_paren && check_right_paren && check_left_curly && check_cond_expr;
}

bool validateIfStmt(vector<Token> tokens) {
  int min_stmt_size = 8;
  if (tokens.size() < min_stmt_size) {
    return false;
  }

  const int kLastIndex = tokens.size() - 1;
  const int kSecondLastIndex = kLastIndex - 1;
  const int kThirdLastIndex = kSecondLastIndex - 1;

  bool check_left_paren = tokens.at(kSecondIndex).type_ == LEFT_PAREN;
  bool check_right_paren = tokens.at(kThirdLastIndex).type_ == RIGHT_PAREN;
  bool check_then_keyword = tokens.at(kSecondLastIndex).type_ == NAME && tokens.at(kSecondLastIndex).text_ == "then";
  bool check_left_curly = tokens.at(kLastIndex).type_ == LEFT_CURLY;

  vector<Token>::const_iterator first = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator last = tokens.begin() + kThirdLastIndex;
  vector<Token> cond_expr(first, last);
  bool check_cond_expr = validateCondExpr(cond_expr);

  return check_left_paren && check_right_paren && check_left_curly && check_cond_expr;
}

bool Validate(vector<Token> input) {

  // keep track of number of brackets and number of if/else
  int curly_bracket_count = 0;
  int if_stmts = 0;

  for (auto token = begin(input); token != end(input); ++token) {

    if (token->type_ == RIGHT_CURLY) {
      bool has_two_more_tokens = token != end(input) - 1 && token != end(input) - 2;
      if (has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == LEFT_CURLY) {
        if_stmts -= 1;
        token++;
        token++;
      } else {
        curly_bracket_count -= 1;
      }

    } else if (next(token, 1)->text_ == "=") {
      vector<Token> tokens;
      while (token->type_ != SEMICOLON && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateAssignStmt(tokens)) {
        return false;
      }

    } else if (token->text_ == "procedure") {
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateProcedure(tokens)) {
        return false;
      }

      curly_bracket_count += 1;

    } else if (token->text_ == "while") {
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateWhileStmt(tokens)) {
        return false;
      } else {
        curly_bracket_count += 1;
      }

    } else if (token->text_ == "if") {
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY && token != end(input) - 1) {
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

    } else if (token->text_ == "read" || token->text_ == "print") {
      vector<Token> tokens;
      while (token->type_ != SEMICOLON && token != end(input) - 1) {
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
