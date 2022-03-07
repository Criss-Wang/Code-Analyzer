#include<stdio.h>
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>

using namespace std;

#include "validator.h"
#include "tokenizer.h"

const int kFirstIndex = 0;
const int kSecondIndex = 1;
const int kThirdIndex = 2;

bool validateProcedureStmt(vector<Token> tokens) {
  int expected_size = 3;
  if (tokens.size() != expected_size) {
    return false;
  }

  bool check_variable = tokens.at(kSecondIndex).type_ == NAME || tokens.at(kSecondIndex).type_ == LETTER;
  bool check_left_curly = tokens.at(kThirdIndex).type_ == LEFT_CURLY;
  return check_variable && check_left_curly;
}

bool dfs(const string& vertex, vector<string>& vertices, vector<unordered_set<string>>& edges,
    unordered_set<string>& visited, unordered_set<string>& finished) {
  if (finished.find(vertex) != finished.end()) {
    return true;
  }

  if (visited.find(vertex) != visited.end()) {
    return false;
  }

  visited.insert(vertex);

  //find the index of vertex in vertices vector
  auto iter = find(vertices.begin(), vertices.end(), vertex);
  int idx = iter - vertices.begin();

  for (auto& neighbour : edges[idx]) {
    if (!dfs(neighbour, vertices, edges, visited, finished)) {
        return false;
    }
  }

  finished.insert(vertex);
  return true;
}

bool CheckCycle(vector<string>& vertices, vector<unordered_set<string>>& edges) { 
  unordered_set<string> visited({}); 
  unordered_set<string> finished({}); //Vertices in finished have no cyclic dependecies
  
  for (auto& vertex : vertices) {
    if (!(finished.find(vertex) != finished.end())) {
      //We only visit vertices that are not in finished
      if (!dfs(vertex, vertices, edges, visited, finished)) {
        return false;
      }
    }
  }
  
  return true;
}

bool validateProcedures(vector<string>& procedures, vector<unordered_set<string>>& calls) {
  int min_procedures = 1;
  if (procedures.size() < min_procedures) {
    return false;
  }

  //Check whether callee exist 
  for (auto& call : calls) {
    for (auto& callee : call) {
      if (find(procedures.begin(), procedures.end(), callee) == procedures.end()) {
        return false;
      }
    }
  }

  return CheckCycle(procedures, calls);
}

bool validateReadPrintCallStmt(vector<Token> tokens) {
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

    if (token_type == INTEGER && token->text_.size() > 1 && token->text_[0] == '0') {
      return false;
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

    if (token_type == INTEGER && token->text_.size() > 1 && token->text_[0] == '0') {
      return false;
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
  int proc_curly_bracket_count = 0;
  int if_curly_bracket_count = 0;
  int while_curly_bracket_count = 0;
  int if_else_stmts = 0;
  stack<string> curly_bracket_types;

  // keep track of procedures and called procedures
  vector<string> procedures = {};
  // Each set will store the procedure name called (might have multiple calls to the same procedure)
  vector<unordered_set<string>> called_procedures_set = {};
  int curr_procedure = -1;

  // check if/else/while containers for empty stmtLst
  bool contains_stmt_lst = false;

  for (auto token = begin(input); token != end(input); ++token) {

    if (token->type_ == RIGHT_CURLY) {
      if (curly_bracket_types.empty() || !contains_stmt_lst) {
        return false;
      }

      bool has_two_more_tokens = token != end(input) - 1 && token != end(input) - 2;
      if (has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == LEFT_CURLY) {
        if_else_stmts -= 1;
        token++;
        token++;
        contains_stmt_lst = false;

      } else if (curly_bracket_types.top() == "procedure") {
        proc_curly_bracket_count -= 1;
        curly_bracket_types.pop();

      } else if (curly_bracket_types.top() == "while") {
        while_curly_bracket_count -= 1;
        curly_bracket_types.pop();

      } else if (curly_bracket_types.top() == "if") {
        if_curly_bracket_count -= 1;
        curly_bracket_types.pop();

      }

    } else if (next(token, 1)->text_ == "=") {
      contains_stmt_lst = true;
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
      contains_stmt_lst = false;
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateProcedureStmt(tokens)) {
        return false;
      }

      procedures.push_back(tokens.at(kSecondIndex).text_);
      //Need to create an edge list for the new procedure
      called_procedures_set.push_back(unordered_set<string>({}));
      curr_procedure += 1;

      proc_curly_bracket_count += 1;
      curly_bracket_types.push("procedure");

    } else if (token->text_ == "while") {
      contains_stmt_lst = false;

      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateWhileStmt(tokens)) {
        return false;
      }

      while_curly_bracket_count += 1;
      curly_bracket_types.push("while");

    } else if (token->text_ == "if") {
      contains_stmt_lst = false;

      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateIfStmt(tokens)) {
        return false;
      }

      if_curly_bracket_count += 1;
      curly_bracket_types.push("if");
      if_else_stmts += 1;

    } else if (token->text_ == "read" || token->text_ == "print" || token->text_ == "call") {
      contains_stmt_lst = true;

      vector<Token> tokens;
      while (token->type_ != SEMICOLON && token != end(input) - 1) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (!validateReadPrintCallStmt(tokens)) {
        return false;
      }
      if (tokens.at(kFirstIndex).text_ == "call") {
        called_procedures_set.at(curr_procedure).insert(tokens.at(kSecondIndex).text_);
      }

    } else {
      return false;
    }
  }

  bool check_curly_brackets = proc_curly_bracket_count == 0 && if_curly_bracket_count == 0 && while_curly_bracket_count == 0;
  if (check_curly_brackets && if_else_stmts == 0 && validateProcedures(procedures, called_procedures_set)) {
    return true;
  } else {
    return false;
  }

}
