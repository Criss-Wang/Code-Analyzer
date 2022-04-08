#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>

#include "tokenizer.h"
#include "sp_exceptions.h"

using namespace std; 

// Constructor for Tokenizer
Tokenizer::Tokenizer(const std::string& source_program) {
  vector<Token> tokens_list;
  Token current_token;

  for (char curr_char : source_program) {
    if (isdigit(curr_char)) { // Digits 0-9
      AddDigitTokenToList(curr_char, current_token, tokens_list);

    } else if (IsLeftCurlyBracket(curr_char)) {
      AddCharTokenToList(curr_char, TokenType::LEFT_CURLY, current_token, tokens_list);

    } else if (IsRightCurlyBracket(curr_char)) {
      AddCharTokenToList(curr_char, TokenType::RIGHT_CURLY, current_token, tokens_list);

    } else if (IsLeftParen(curr_char)) {
      AddCharTokenToList(curr_char, TokenType::LEFT_PAREN, current_token, tokens_list);

    } else if (IsRightParen(curr_char)) {
      AddCharTokenToList(curr_char, TokenType::RIGHT_PAREN, current_token, tokens_list);

    } else if (IsWhitespace(curr_char)) {
      EndToken(current_token, tokens_list);

    } else if (IsOperator(curr_char)) {
      AddOperatorTokenToList(curr_char, current_token, tokens_list);

    } else if (IsSemicolon(curr_char)) {
      AddCharTokenToList(curr_char, TokenType::SEMICOLON, current_token, tokens_list);

    } else if (isalpha(curr_char)) { // Letters 
      AddLetterTokenToList(curr_char, current_token, tokens_list);

    } else {
      throw InvalidSyntaxException();
    }
  }
  
  EndToken(current_token, tokens_list);
  tokens_list_ = tokens_list;
}

void Tokenizer::AddCharTokenToList(char curr_char, TokenType token_type,
                                    Token& current_token, vector<Token>& tokens_list) {
  EndToken(current_token, tokens_list);
  current_token.type_ = token_type;
  current_token.text_.append(1, curr_char);
  EndToken(current_token, tokens_list);
}

void Tokenizer::AddDigitTokenToList(char curr_char, Token& current_token, vector<Token>& tokens_list) {
  if (current_token.type_ == TokenType::OPERATOR) {
    EndToken(current_token, tokens_list);
  }
  if (current_token.type_ == TokenType::WHITESPACE) {
    current_token.type_ = TokenType::DIGIT;
  } else if (current_token.type_ == TokenType::DIGIT) {
    current_token.type_ = TokenType::INTEGER;
  }
  current_token.text_.append(1, curr_char);
}

void Tokenizer::AddLetterTokenToList(char curr_char, Token& current_token, vector<Token>& tokens_list) {
  if (current_token.type_ == TokenType::OPERATOR) {
    EndToken(current_token, tokens_list);
  }

  if (current_token.type_ == TokenType::DIGIT || current_token.type_ == TokenType::INTEGER) {
    throw InvalidSyntaxException();
  }

  if (current_token.type_ == TokenType::WHITESPACE) {
    current_token.type_ = TokenType::LETTER;
  } else if (current_token.type_ == TokenType::LETTER) {
    current_token.type_ = TokenType::NAME;
  }
  current_token.text_.append(1, curr_char);
}

void Tokenizer::AddOperatorTokenToList(char curr_char, Token& current_token, vector<Token>& tokens_list) {
  if (current_token.type_ != TokenType::OPERATOR) {
    EndToken(current_token, tokens_list);
  }
  current_token.type_ = TokenType::OPERATOR;
  current_token.text_.append(1, curr_char);
}

// Resets current token to WHITESPACE
void Tokenizer::EndToken(Token& token, vector<Token>& tokens_list) {
  if (token.type_ != TokenType::WHITESPACE) {
    tokens_list.push_back(token);
  }

  token.type_ = TokenType::WHITESPACE;
  token.text_.erase();
}