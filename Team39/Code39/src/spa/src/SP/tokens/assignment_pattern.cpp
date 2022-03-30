#include "assignment_pattern.h"
#include "SP/sp_exceptions.h"

AssignmentPattern::AssignmentPattern(std::vector<Token>& tokens) {

  int paren_count = 0;

  vector<TokenType> expected_types = { TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN };
  vector<string> expected_operators = { "*", "/", "+", "-", "%" };

  for (auto token = begin(tokens); token != end(tokens); ++token) {
    pattern_ += token->text_;

    TokenType token_type;

    if (token->type_ == TokenType::LETTER) {
      token_type = TokenType::NAME;
    } else if (token->type_ == TokenType::DIGIT) {
      token_type = TokenType::INTEGER;
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

    if (token_type == TokenType::OPERATOR || token_type == TokenType::LEFT_PAREN) { // expects variable, integer or left paren after operator/left paren
      expected_types.push_back(TokenType::NAME);
      expected_types.push_back(TokenType::INTEGER);
      expected_types.push_back(TokenType::LEFT_PAREN);

      if (token_type == TokenType::LEFT_PAREN) {
        paren_count += 1;
      }

    } else if (token_type == TokenType::RIGHT_PAREN) { // expects operator after right paren
      expected_types.push_back(TokenType::OPERATOR);
      expected_types.push_back(TokenType::RIGHT_PAREN);

      paren_count -= 1;

    } else if (token_type == TokenType::NAME || token_type == TokenType::INTEGER) { // expects operator or right paren after variable or integer
      expected_types.push_back(TokenType::OPERATOR);
      expected_types.push_back(TokenType::RIGHT_PAREN);

      if (token_type == TokenType::NAME && find(begin(vars_), end(vars_), token->text_) == end(vars_)) {
        vars_.push_back(token->text_);
      } else if (token_type == TokenType::INTEGER && find(begin(constants_), end(constants_), stoi(token->text_)) == end(constants_)) {
        constants_.push_back(stoi(token->text_));
      }
    }
  }

  if (paren_count != 0) {
    throw InvalidSyntaxException();
  }
}

vector<string> AssignmentPattern::GetVars() {
  return vars_;
}

void AssignmentPattern::PopulateEntities(Pkb& pkb, int stmt_num) {
  // Add pattern variables to variable_set_
  for (string s : vars_) {
    pkb.AddEntityToSet(EntityIdentifier::kVariable, s);
  }

  // Add pattern constants to constant_set_
  for (int i : constants_) {
    pkb.AddEntityToSet(EntityIdentifier::kConstant, i);
  }

  // Add stmt num and assignment pattern to Assign Table
  pkb.AddInfoToTable(TableIdentifier::kAssign, stmt_num, pattern_);
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, stmt_num, pattern_);

  if (!constants_.empty()) {
    // Add stmt num and rhs constants to Constant Table
    pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_);
  }

  if (!vars_.empty()) {
    // Add stmt num and vector of rhs variables into Uses Table
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_);
  }

}
