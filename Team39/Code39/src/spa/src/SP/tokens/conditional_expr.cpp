#include <unordered_map>

#include "conditional_expr.h"
#include "SP/sp_exceptions.h"

const unordered_map<TokenType, vector<TokenType>> ExpectedNextTokenTypeMap = {
  { TokenType::NAME, {TokenType::REL_OPERATOR, TokenType::RIGHT_PAREN, TokenType::OPERATOR} },
  { TokenType::INTEGER, {TokenType::REL_OPERATOR, TokenType::RIGHT_PAREN, TokenType::OPERATOR} },
  { TokenType::LEFT_PAREN, {TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN, TokenType::NOT_OPERATOR} },
  { TokenType::RIGHT_PAREN, {TokenType::COND_OPERATOR, TokenType::RIGHT_PAREN, TokenType::OPERATOR, TokenType::REL_OPERATOR} },
  { TokenType::NOT_OPERATOR, {TokenType::LEFT_PAREN} },
  { TokenType::COND_OPERATOR, {TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN } },
  { TokenType::REL_OPERATOR, {TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN } },
  { TokenType::OPERATOR, {TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN } }
};

ConditionalExpression::ConditionalExpression(std::vector<Token>& tokens) {

  vector<TokenType> expected_types = { TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN, TokenType::NOT_OPERATOR };
  vector<string> rel_operators = { ">", "<", "==", "!=", ">=", "<=" };
  vector<string> expected_operators = { "*", "/", "+", "-", "%" };

  // keep track of number of brackets, rel_op and cond_op
  int paren_count = 0;
  int rel_op_count = 0;
  int cond_op_count = 0;

  for (auto token = begin(tokens); token != end(tokens); ++token) {

    TokenType token_type;

    if (token->type_ == TokenType::LETTER) {
      token_type = TokenType::NAME;
    } else if (token->type_ == TokenType::DIGIT) {
      token_type = TokenType::INTEGER;
    } else if (token->text_ == "!") {
      token_type = TokenType::NOT_OPERATOR;
    } else if (token->text_ == "||" || token->text_ == "&&") {
      token_type = TokenType::COND_OPERATOR;
    } else if (find(begin(rel_operators), end(rel_operators), token->text_) != end(rel_operators)) {
      token_type = TokenType::REL_OPERATOR;
    } else {
      token_type = token->type_;
    }

    if (token_type == TokenType::INTEGER && token->text_.size() > 1 && token->text_[0] == '0') {
      throw InvalidSyntaxException();
    }

    bool check_type_ = find(begin(expected_types), end(expected_types), token_type) != end(expected_types);
    bool check_operator = true;

    if (token_type == TokenType::OPERATOR) {
      check_operator = find(begin(expected_operators), end(expected_operators), token->text_) != end(expected_operators);
    }

    if (!check_type_ || !check_operator) {
      throw InvalidSyntaxException();
    }

    expected_types = ExpectedNextTokenTypeMap.at(token_type);

    if (token_type == TokenType::LEFT_PAREN) {

      paren_count += 1;

    } else if (token_type == TokenType::RIGHT_PAREN) {
      paren_count -= 1;

    } else if (token_type == TokenType::NAME) {
      vars_.insert(token->text_);

    } else if (token_type == TokenType::INTEGER) {
      constants_.insert(stoi(token->text_));

    } else if (token_type == TokenType::REL_OPERATOR) {
      rel_op_count += 1;

    } else if (token_type == TokenType::COND_OPERATOR) {
      cond_op_count += 1;

    }
  }

  if (paren_count != 0 || (rel_op_count -1) != cond_op_count) {
    throw InvalidSyntaxException();
  }
}

unordered_set<string> ConditionalExpression::GetVars() {
  return vars_;
}

void ConditionalExpression::PopulateEntities(Pkb& pkb, int stmt_num) {

  // Add variables to variable_set_
  for (string s : vars_) {
    pkb.AddEntityToSet(EntityIdentifier::kVariable, s);
  }

  // Add constants to constant_set_
  for (int i : constants_) {
    pkb.AddEntityToSet(EntityIdentifier::kConstant, i);
  }

  if (!constants_.empty()) {
    // Add stmt num and constants in cond expr into Constant Table
    vector<int> constants;
    for (int c : constants_) {
      constants.push_back(c);
    }
    pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants);
  }

  if (!vars_.empty()) {
    // Add stmt num and variables in cond expr into Uses Table
    vector<string> vars;
    for (string v : vars_) {
      vars.push_back(v);
    }
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars);
  }

}
