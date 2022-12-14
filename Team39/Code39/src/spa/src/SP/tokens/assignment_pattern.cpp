#include <unordered_map>

#include "assignment_pattern.h"
#include "SP/sp_exceptions.h"

#define INDEX_OF_FIRST_DIGIT 0
#define EXPECTED_PAREN_COUNT 0

const unordered_map<TokenType, vector<TokenType>> ExpectedNextTokenTypeMap = {
  { TokenType::NAME, {TokenType::RIGHT_PAREN, TokenType::OPERATOR} },
  { TokenType::INTEGER, {TokenType::RIGHT_PAREN, TokenType::OPERATOR} },
  { TokenType::LEFT_PAREN, {TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN} },
  { TokenType::RIGHT_PAREN, {TokenType::RIGHT_PAREN, TokenType::OPERATOR} },
  { TokenType::OPERATOR, {TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN } }
};

TokenType GetTypeForAssigPattern(Token& token) {
  if (token.type_ == TokenType::LETTER) {
    return TokenType::NAME;
  } else if (token.type_ == TokenType::DIGIT) {
    return TokenType::INTEGER;
  } else {
    return token.type_;
  }
}

AssignmentPattern::AssignmentPattern(std::vector<Token>& tokens) {

  int paren_count = 0;

  vector<TokenType> expected_types = { TokenType::NAME, TokenType::INTEGER, TokenType::LEFT_PAREN };
  vector<string> expected_operators = { "*", "/", "+", "-", "%" };

  for (auto token = begin(tokens); token != end(tokens); ++token) {
    pattern_ += token->text_;

    TokenType token_type = GetTypeForAssigPattern(*token);

    if (token->type_ == TokenType::INTEGER && token->text_[INDEX_OF_FIRST_DIGIT] == '0') {
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

    } else {
      continue;
    }
  }

  if (paren_count != EXPECTED_PAREN_COUNT) {
    throw InvalidSyntaxException();
  }
}

unordered_set<string> AssignmentPattern::GetVars() {
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
    vector<int> constants;
    for (int c : constants_) {
      constants.push_back(c);
    }
    pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants);
  }

  if (!vars_.empty()) {
    // Add stmt num and vector of rhs variables into Uses Table
    vector<string> vars;
    for (string v : vars_) {
      vars.push_back(v);
    }
    pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars);
  }

}
