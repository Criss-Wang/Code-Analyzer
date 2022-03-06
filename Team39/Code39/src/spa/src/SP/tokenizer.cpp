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
        if (current_token.type_ == OPERATOR) {
          EndToken(current_token, tokens_list);
        }

        if (current_token.type_ == WHITESPACE) {
          current_token.type_ = DIGIT;
        } else if (current_token.type_ == DIGIT || current_token.type_ == INTEGER) {
          current_token.type_ = INTEGER;
        }
        current_token.text_.append(1, curr_char);
        break;

      // Brackets
      case '{':
        if (current_token.type_ != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = LEFT_CURLY;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        current_token.IncreaseStmtNum();
        break;

      case '}':
        if (current_token.type_ != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = RIGHT_CURLY;
        current_token.text_.append(1, curr_char);
        current_token.StopStmtNum();
        EndToken(current_token, tokens_list);
        current_token.IncreaseStmtNum();
        break;

      case '(':
        if (current_token.type_ != WHITESPACE) {
          EndToken(current_token, tokens_list);			
        }

        current_token.type_ = LEFT_PAREN;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      case ')':
        if (current_token.type_ != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = RIGHT_PAREN;
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
        if (current_token.type_ != OPERATOR) {
          EndToken(current_token, tokens_list);
        }
        current_token.type_ = OPERATOR;
        current_token.text_.append(1, curr_char);
        break;

      // Semicolon
      case ';':
        if (current_token.type_ != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type_ = SEMICOLON;
        current_token.text_.append(1, curr_char);
        EndToken(current_token, tokens_list);
        current_token.IncreaseStmtNum();
        break;

      // Letters
      default:
        if (current_token.type_ == OPERATOR) {
          EndToken(current_token, tokens_list);
        }

        if (current_token.type_ == WHITESPACE) {
          current_token.type_ = LETTER;
          current_token.text_.append(1, curr_char);
        } else if (current_token.type_ == LETTER || current_token.type_ == NAME) {
          current_token.type_ = NAME;
          current_token.text_.append(1, curr_char);
        } else if (current_token.type_ == DIGIT || current_token.type_ == INTEGER) {
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
  if (token.type_ == LEFT_CURLY && !tokens_list.empty()) {
    Token prev_token = tokens_list.back();

    if (prev_token.text_ == "else") {
      tokens_list.pop_back();
      tokens_list.push_back({ NAME, "else", 0 });
      token.StopStmtNum();
    }

  } else if (token.type_ == NAME && !tokens_list.empty()) {
    Token prev_token = tokens_list.back();

    if (prev_token.text_ == "procedure") {
      tokens_list.pop_back();
      tokens_list.push_back({ NAME, "procedure", 0 });
      token.StopStmtNum();
    }
  }

  if (token.type_ != WHITESPACE) {
    tokens_list.push_back(token);
  }

  token.type_ = WHITESPACE;
  token.text_.erase();
}

string Token::print() {
  string output = tokenTypeStrings[type_] + ", \"" + text_ + "\", " + to_string(stmt_num_);
  cout << output << endl;
  return output;
}
