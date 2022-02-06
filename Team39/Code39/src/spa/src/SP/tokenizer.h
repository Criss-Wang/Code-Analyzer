#pragma once

#include <string>
#include <vector>

using namespace std;

enum TokenType {
  WHITESPACE,
  LETTER,
  DIGIT,
  NAME,
  INTEGER,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_CURLY,
  RIGHT_CURLY,
  OPERATOR,
  SEMICOLON,
  IF, 
  THEN,
  ELSE,
  WHILE
};

static const string tokenTypeStrings[] = {
  "WHITESPACE",
  "LETTER",
  "DIGIT",
  "NAME",
  "INTEGER",
  "LEFT_PAREN",
  "RIGHT_PAREN",
  "LEFT_CURLY",
  "RIGHT_CURLY",
  "OPERATOR",
  "SEMICOLON",
  "IF",
  "THEN",
  "ELSE",
  "WHILE"
};

class Token {
  public:
    enum TokenType type { WHITESPACE }; // Initialised to WHITESPACE
    string text = "";
    int stmt_num_ = 1;
    
    string print();

    void IncreaseStmtNum() {
      stmt_num_ += 1;
    }

    bool operator==(const Token& t) const {
      return t.type == type && t.text == text;
    }
};

class Tokenizer {
  public:
    vector<Token> parse(const string& sourceProgram);

  private:
    void EndToken(Token& token, vector<Token>& tokens);
    void CheckStmtType(Token& token);
};

