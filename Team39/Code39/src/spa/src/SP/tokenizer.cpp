#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "tokenizer.h"

using namespace std; 

// Tokenizes a given source program  
vector<Token> Tokenizer::parse(const string& sourceProgram) {
  bool is_syntax_error = false;
  vector<Token> tokens_list;
  Token current_token;

  for (char curr_char : sourceProgram) {

    switch (curr_char) {
      // Digits
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (current_token.type_ == TokenType::OPERATOR) {
          EndToken(current_token, tokens_list);
        }

        if (current_token.type_ == TokenType::WHITESPACE) {
          current_token.type_ = TokenType::DIGIT;
        } else if (current_token.type_ == TokenType::DIGIT || current_token.type_ == TokenType::INTEGER) {
          current_token.type_ = TokenType::INTEGER;
        }
        current_token.text_.append(1, curr_char);
        break;

      // Brackets
      case '{':
        if (current_token.type_ != TokenType::WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = TokenType::LEFT_CURLY;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      case '}':
        if (current_token.type_ != TokenType::WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = TokenType::RIGHT_CURLY;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      case '(':
        if (current_token.type_ != TokenType::WHITESPACE) {
          EndToken(current_token, tokens_list);			
        }

        current_token.type_ = TokenType::LEFT_PAREN;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      case ')':
        if (current_token.type_ != TokenType::WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = TokenType::RIGHT_PAREN;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      // Space and tab
      case ' ':
      case '	': 
      case '\n':
        EndToken(current_token, tokens_list);
        break;

      // Operators
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
      case '=':
      case '<':
      case '>':
      case '!':
      case '&':
      case '|':
        if (current_token.type_ != TokenType::OPERATOR) {
          EndToken(current_token, tokens_list);
        }
        current_token.type_ = TokenType::OPERATOR;
        current_token.text_.append(1, curr_char);
        break;

      // Semicolon
      case ';':
        if (current_token.type_ != TokenType::WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = TokenType::SEMICOLON;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      // Letters
      default:
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
          is_syntax_error = true;
        } 
        break;
    }
  }

  if (is_syntax_error) {
    return {};
  } else {
    EndToken(current_token, tokens_list);
    return tokens_list;
  }
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
