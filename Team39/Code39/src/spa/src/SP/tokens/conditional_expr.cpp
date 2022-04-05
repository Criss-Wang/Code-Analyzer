#include "conditional_expr.h"
#include "SP/sp_exceptions.h"

ConditionalExpression::ConditionalExpression(std::vector<Token>& tokens) {

  vector<TokenType> expected_types = { TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN, TokenType::NOT_OPERATOR };
  vector<string> rel_operators = { ">", "<", "==", "!=", ">=", "<=" };
  vector<string> expected_operators = { "*", "/", "+", "-", "%" };

  // keep track of number of brackets
  int paren_count = 0;

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

    expected_types = {};

    if (token_type == TokenType::LEFT_PAREN) { // expects variable, integer or left paren after left paren
      expected_types.push_back(TokenType::NAME);
      expected_types.push_back(TokenType::INTEGER);
      expected_types.push_back(TokenType::LEFT_PAREN);
      expected_types.push_back(TokenType::NOT_OPERATOR);

      paren_count += 1;

    } else if (token_type == TokenType::RIGHT_PAREN) { // expects cond/rel/normal operator or right paren after right paren
      expected_types.push_back(TokenType::COND_OPERATOR);
      expected_types.push_back(TokenType::RIGHT_PAREN);
      expected_types.push_back(TokenType::OPERATOR);
      expected_types.push_back(TokenType::REL_OPERATOR);

      paren_count -= 1;

    } else if (token_type == TokenType::NOT_OPERATOR) { // expects left paren after not operator
      expected_types.push_back(TokenType::LEFT_PAREN);

    } else if (token_type == TokenType::NAME || token_type == TokenType::INTEGER) { // expects rel operator or right paren after variable or integer
      expected_types.push_back(TokenType::REL_OPERATOR);
      expected_types.push_back(TokenType::RIGHT_PAREN);
      expected_types.push_back(TokenType::OPERATOR);

      if (token_type == TokenType::NAME && find(begin(vars_), end(vars_), token->text_) == end(vars_)) {
        vars_.push_back(token->text_);
      } else if (token_type == TokenType::INTEGER && find(begin(constants_), end(constants_), stoi(token->text_)) == end(constants_)) {
        constants_.push_back(stoi(token->text_));
      }

    } else if (token_type == TokenType::COND_OPERATOR || token_type == TokenType::REL_OPERATOR || token_type == TokenType::OPERATOR) { // expected name, int or left after operator
      expected_types.push_back(TokenType::NAME);
      expected_types.push_back(TokenType::INTEGER);
      expected_types.push_back(TokenType::LEFT_PAREN);
    }
  }

  if (paren_count != 0) {
    throw InvalidSyntaxException();
  }
}

vector<string> ConditionalExpression::GetVars() {
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
    pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_);
  }

  if (!vars_.empty()) {
    // Add stmt num and variables in cond expr into Uses Table
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_);
  }

}
