#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "Tokenizer.h"

using namespace std; 

// Tokenizes a given source program  
vector<Token> Tokenizer::parse(const string& sourceProgram) {
  int curlyBracketsCount = 0; // 0 for syntactically correct program
  int parenCount = 0;

  vector<Token> tokens_list;
  Token current_token;
  Token prev_token;

  for (char currChar : sourceProgram) {

    switch (currChar) {
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
        current_token.text.append(1, currChar);
        break;

      // Brackets
      case '{':
        if (prev_token.type == INTEGER || prev_token.type == DIGIT
          || prev_token.type == OPERATOR || prev_token.type == SEMICOLON) {
          throw invalid_argument("Syntax error");
        }

        if (current_token.type != WHITESPACE) {
          endToken(current_token, tokens_list);
        }

        current_token.type = LEFT_CURLY;
        current_token.text.append(1, currChar);
        endToken(current_token, tokens_list);
        curlyBracketsCount++;
        break;

      case '}':
        if (current_token.type != WHITESPACE) {
          endToken(current_token, tokens_list);
        }

        if (curlyBracketsCount <= 0) {
          throw invalid_argument("Syntax error");
        }

        current_token.type = RIGHT_CURLY;
        current_token.text.append(1, currChar);
        endToken(current_token, tokens_list);
        curlyBracketsCount--;

        if (curlyBracketsCount != 0 || parenCount != 0) {
          throw invalid_argument("Syntax error");
        }
        break;

      case '(':
        if (current_token.type != WHITESPACE) {
          endToken(current_token, tokens_list);			
        }
        current_token.type = LEFT_PAREN;
        current_token.text.append(1, currChar);
        endToken(current_token, tokens_list);
        parenCount++;
        break;

      case ')':
        if (current_token.type != WHITESPACE) {
          endToken(current_token, tokens_list);
        }

        if (parenCount <= 0) {
          throw invalid_argument("Syntax error");
        }

        current_token.type = RIGHT_PAREN;
        current_token.text.append(1, currChar);
        endToken(current_token, tokens_list);
        parenCount--;
        break;

      // Space and tab
      case ' ':
      case '	': 
        prev_token = current_token;
        endToken(current_token, tokens_list);
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
        if (prev_token.type != LETTER && prev_token.type != NAME
          && prev_token.type != DIGIT && prev_token.type != INTEGER
          && current_token.type != RIGHT_PAREN) {
          throw invalid_argument("Syntax error");
        }

        endToken(current_token, tokens_list);
        current_token.type = OPERATOR;
        current_token.text.append(1, currChar);
        break;

      // Semicolon
      case ';':
        if (current_token.type == OPERATOR || current_token.type == SEMICOLON
          || current_token.type == LEFT_CURLY || current_token.type == LEFT_PAREN) {
          throw invalid_argument("Syntax error");
        } 

        if (current_token.type == WHITESPACE) {
          if (prev_token.type == OPERATOR || prev_token.type == LEFT_CURLY
            || prev_token.type == LEFT_PAREN) {
            throw invalid_argument("Syntax error");
          }
        }

        if (current_token.type != WHITESPACE) {
          endToken(current_token, tokens_list);
        }

        current_token.type = SEMICOLON;
        current_token.text.append(1, currChar);
        current_token.increaseStmtNum();
        break;

      // Letters
      default:
        if (current_token.type == WHITESPACE) {
          current_token.type = LETTER;
          current_token.text.append(1, currChar);
        } else if (current_token.type == LETTER || current_token.type == NAME) {
          current_token.type = NAME;
          current_token.text.append(1, currChar);
        } else if (current_token.type == DIGIT || current_token.type == INTEGER) {
          throw invalid_argument("Syntax error");
        } 
        break;
    }
  }

  endToken(current_token, tokens_list);
  return tokens_list;
}

// Resets current token to WHITESPACE
void Tokenizer::endToken(Token &token, vector<Token> &tokensList) {
  if (token.type == NAME) {
    checkStmtType(token);
  }

  if (token.type != WHITESPACE) {	
    tokensList.push_back(token);
  }

  token.type = WHITESPACE;
  token.text.erase();
}

void Tokenizer::checkStmtType(Token &token) {
  if (token.text == "if") {
    token.type = IF;
  } else if (token.text == "then") {
    token.type = THEN;
  } else if (token.text == "else") {
    token.type = ELSE;
  } else if (token.text == "while") {
    token.type = WHILE;
  }
}

string Token::print() {
    string output = tokenTypeStrings[type] + ", \"" + text + "\", " + to_string(stmt_num_);
    cout << output << endl;
    return output;
}
