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
        if (current_token.type == WHITESPACE) {
          current_token.type = DIGIT;
        } else if (current_token.type == DIGIT || current_token.type == INTEGER) {
          current_token.type = INTEGER;
        }
        current_token.text.append(1, curr_char);
        break;

      // Brackets
      case '{':

        if (current_token.type != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type = LEFT_CURLY;
        current_token.text.append(1, curr_char);
        EndToken(current_token, tokens_list);
        current_token.IncreaseStmtNum();
        break;

      case '}':
        if (current_token.type != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type = RIGHT_CURLY;
        current_token.text.append(1, curr_char);
        current_token.StopStmtNum();
        EndToken(current_token, tokens_list);
        current_token.IncreaseStmtNum();
        break;

      case '(':
        if (current_token.type != WHITESPACE) {
          EndToken(current_token, tokens_list);			
        }
        current_token.type = LEFT_PAREN;
        current_token.text.append(1, curr_char);
        EndToken(current_token, tokens_list);
        break;

      case ')':
        if (current_token.type != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type = RIGHT_PAREN;
        current_token.text.append(1, curr_char);
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
        if (current_token.type != OPERATOR) {
          EndToken(current_token, tokens_list);
        }
        current_token.type = OPERATOR;
        current_token.text.append(1, curr_char);
        break;

      // Semicolon
      case ';':
        if (current_token.type != WHITESPACE) {
          EndToken(current_token, tokens_list);
        }

        current_token.type = SEMICOLON;
        current_token.text.append(1, curr_char);
        EndToken(current_token, tokens_list);
        current_token.IncreaseStmtNum();
        break;

      // Letters
      default:
        if (current_token.type == WHITESPACE) {
          current_token.type = LETTER;
          current_token.text.append(1, curr_char);
        } else if (current_token.type == LETTER || current_token.type == NAME) {
          current_token.type = NAME;
          current_token.text.append(1, curr_char);
        } else if (current_token.type == DIGIT || current_token.type == INTEGER) {
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
  if (token.type == NAME) {
      CheckStmtNum(token);
  }

  if (token.type != WHITESPACE) {	
    tokens_list.push_back(token);
  }

  token.type = WHITESPACE;
  token.text.erase();
}

void Tokenizer::CheckStmtNum(Token &token) {
  if (token.text == "else") { // To correct stmt num for "} else {"
    token.StopStmtNum();
  }
}

string Token::print() {
    string output = tokenTypeStrings[type] + ", \"" + text + "\", " + to_string(stmt_num_);
    cout << output << endl;
    return output;
}
