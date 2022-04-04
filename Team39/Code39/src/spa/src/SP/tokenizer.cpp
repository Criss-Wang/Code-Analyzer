#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>

#include "tokenizer.h"
#include "sp_exceptions.h"

using namespace std; 

// Tokenizes a given source program  
vector<Token> Tokenizer::parse(const string& sourceProgram) {
  vector<Token> tokens_list;
  Token current_token;

  for (char curr_char : sourceProgram) {
    if (isdigit(curr_char)) { // Digits 0-9
      if (current_token.type_ == TokenType::OPERATOR) {
        EndToken(current_token, tokens_list);
      }
      if (current_token.type_ == TokenType::WHITESPACE) {
        current_token.type_ = TokenType::DIGIT;
      } else if (current_token.type_ == TokenType::DIGIT || current_token.type_ == TokenType::INTEGER) {
        current_token.type_ = TokenType::INTEGER;
      }
      current_token.text_.append(1, curr_char);

    } else if (IsLeftCurlyBracket(curr_char)) {
      EndToken(current_token, tokens_list);
      current_token.type_ = TokenType::LEFT_CURLY;
      current_token.text_.append(1, curr_char);
      EndToken(current_token, tokens_list);

    } else if (IsRightCurlyBracket(curr_char)) {
      EndToken(current_token, tokens_list);
      current_token.type_ = TokenType::RIGHT_CURLY;
      current_token.text_.append(1, curr_char);
      EndToken(current_token, tokens_list);

    } else if (IsLeftParen(curr_char)) {
      EndToken(current_token, tokens_list);
      current_token.type_ = TokenType::LEFT_PAREN;
      current_token.text_.append(1, curr_char);
      EndToken(current_token, tokens_list);

    } else if (IsRightParen(curr_char)) {
      EndToken(current_token, tokens_list);
      current_token.type_ = TokenType::RIGHT_PAREN;
      current_token.text_.append(1, curr_char);
      EndToken(current_token, tokens_list);

    } else if (IsWhitespace(curr_char)) {
      EndToken(current_token, tokens_list);

    } else if (IsOperator(curr_char)) {
      if (current_token.type_ != TokenType::OPERATOR) {
        EndToken(current_token, tokens_list);
      }
      current_token.type_ = TokenType::OPERATOR;
      current_token.text_.append(1, curr_char);

    } else if (IsSemicolon(curr_char)) {
      EndToken(current_token, tokens_list);
      current_token.type_ = TokenType::SEMICOLON;
      current_token.text_.append(1, curr_char);
      EndToken(current_token, tokens_list);

    } else if (isalpha(curr_char)) { // Letters 
      if (current_token.type_ == TokenType::OPERATOR) {
        EndToken(current_token, tokens_list);
      }

      if (current_token.type_ == TokenType::WHITESPACE) {
        current_token.type_ = TokenType::LETTER;
        current_token.text_.append(1, curr_char);
      } else if (current_token.type_ == TokenType::LETTER || current_token.type_ == TokenType::NAME) {
        current_token.type_ = TokenType::NAME;
        current_token.text_.append(1, curr_char);
      } else if (current_token.type_ == TokenType::DIGIT || current_token.type_ == TokenType::INTEGER) {
        throw InvalidSyntaxException();
      }
    } else {
      throw InvalidSyntaxException();
    }
  }
  
  EndToken(current_token, tokens_list);
  return tokens_list;
}

// Resets current token to WHITESPACE
void Tokenizer::EndToken(Token &token, vector<Token> &tokens_list) {
  if (token.type_ != TokenType::WHITESPACE) {
    tokens_list.push_back(token);
  }

  token.type_ = TokenType::WHITESPACE;
  token.text_.erase();
}

void Token::print() {
  string text = ", \"" + text_;
  switch (this->type_) {
    case TokenType::WHITESPACE:
      cout << "Whitespace" << text << endl;
      break;
    case TokenType::LETTER:
      cout << "Letter" << text << endl;
      break;
    case TokenType::DIGIT:
      cout << "Digit" << text << endl;
      break;
    case TokenType::NAME:
      cout << "Name" << text << endl;
      break;
    case TokenType::INTEGER:
      cout << "Integer" << text << endl;
      break;
    case TokenType::LEFT_PAREN:
      cout << "Left paren" << text << endl;
      break;
    case TokenType::RIGHT_PAREN:
      cout << "Right paren" << text << endl;
      break;
    case TokenType::LEFT_CURLY:
      cout << "Left curly" << text << endl;
      break;
    case TokenType::RIGHT_CURLY:
      cout << "Right curly" << text << endl;
      break;
    case TokenType::OPERATOR:
      cout << "Operator" << text << endl;
      break;
    case TokenType::SEMICOLON:
      cout << "Semicolon" << text << endl;
      break;
    case TokenType::NOT_OPERATOR:
      cout << "Not operator" << text << endl;
      break;
    case TokenType::REL_OPERATOR:
      cout << "Rel operator" << text << endl;
      break;
    case TokenType::COND_OPERATOR:
      cout << "Cond operator" << text << endl;
      break;
    default:
      break;
  }
}
