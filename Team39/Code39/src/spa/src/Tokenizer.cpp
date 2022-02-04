#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include "PKB.h"
#include "TNode.h"
#include "Tokenizer.h"

using namespace std; 
bool isSyntaxError = false;

// Tokenizes a given source program  
vector<Token> Tokenizer::parse(const string& sourceProgram) {
  int curlyBracketsCount = 0; // 0 for syntactically correct program
  int parenCount = 0;

  vector<Token> tokensList;
  Token currentToken;
  Token prevToken;

  for (char currChar : sourceProgram) {
    if (isSyntaxError) {
      return tokensList;
    }

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
        if (currentToken.type == WHITESPACE) {
          currentToken.type = DIGIT;
        } else if (currentToken.type == DIGIT || currentToken.type == INTEGER) {
          currentToken.type = INTEGER;
        }
        currentToken.text.append(1, currChar);
        break;

      // Brackets
      case '{':
        if (prevToken.type == INTEGER || prevToken.type == DIGIT
          || prevToken.type == OPERATOR || prevToken.type == SEMICOLON) {
          isSyntaxError = true;
          continue;
        }

        if (currentToken.type != WHITESPACE) {
          endToken(currentToken, tokensList);
        }

        currentToken.type = LEFT_CURLY;
        currentToken.text.append(1, currChar);
        endToken(currentToken, tokensList);
        curlyBracketsCount++;
        break;

      case '}':
        if (currentToken.type != WHITESPACE) {
          endToken(currentToken, tokensList);
        }

        if (curlyBracketsCount <= 0) {
          isSyntaxError = true;
          continue;
        }

        currentToken.type = RIGHT_CURLY;
        currentToken.text.append(1, currChar);
        endToken(currentToken, tokensList);
        curlyBracketsCount--;

        if (curlyBracketsCount != 0 || parenCount != 0) {
          isSyntaxError = true;
          continue;
        }
        break;

      case '(':
        if (currentToken.type != WHITESPACE) {
          endToken(currentToken, tokensList);			
        }
        currentToken.type = LEFT_PAREN;
        currentToken.text.append(1, currChar);
        endToken(currentToken, tokensList);
        parenCount++;
        break;

      case ')':
        if (currentToken.type != WHITESPACE) {
          endToken(currentToken, tokensList);
        }

        if (parenCount <= 0) {
          isSyntaxError = true;
          continue;
        }

        currentToken.type = RIGHT_PAREN;
        currentToken.text.append(1, currChar);
        endToken(currentToken, tokensList);
        parenCount--;
        break;

      // Space and tab
      case ' ':
      case '	': 
        prevToken = currentToken;
        endToken(currentToken, tokensList);
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
        if (prevToken.type != LETTER && prevToken.type != NAME
          && prevToken.type != DIGIT && prevToken.type != INTEGER
          && currentToken.type != RIGHT_PAREN) {
          isSyntaxError = true;
          continue;
        }

        endToken(currentToken, tokensList);
        currentToken.type = OPERATOR;
        currentToken.text.append(1, currChar);
        break;

      // Semicolon
      case ';':
        if (currentToken.type == OPERATOR || currentToken.type == LEFT_CURLY
          || currentToken.type == LEFT_PAREN) {
          isSyntaxError = true;
          continue;
        } 

        if (currentToken.type == WHITESPACE) {
          if (prevToken.type == OPERATOR || prevToken.type == LEFT_CURLY
            || prevToken.type == LEFT_PAREN) {
            isSyntaxError = true;
            continue;
          }
        }

        if (currentToken.type != WHITESPACE) {
          endToken(currentToken, tokensList);
        }

        currentToken.type = SEMICOLON;
        currentToken.text.append(1, currChar);
        currentToken.increaseStmtNum();
        break;

      // Letters
      default:
        if (currentToken.type == WHITESPACE) {
          currentToken.type = LETTER;
          currentToken.text.append(1, currChar);
        } else if (currentToken.type == LETTER || currentToken.type == NAME) {
          currentToken.type = NAME;
          currentToken.text.append(1, currChar);
        } else if (currentToken.type == DIGIT || currentToken.type == INTEGER) {
          isSyntaxError = true;
          continue;
        } 
        break;
    }
  }

  endToken(currentToken, tokensList);
  return tokensList;
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
  if (!isSyntaxError) {
    string output = tokenTypeStrings[type] + ", \"" + text + "\", " + to_string(stmtNum);
    cout << output << endl;
    return output;
  } else {
    return "Syntax Error";
  }
}
