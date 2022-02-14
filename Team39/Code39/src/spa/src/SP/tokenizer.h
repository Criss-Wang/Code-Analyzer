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
  WILDCARD
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
  "WILDCARD"
};

class Token {
  public:
    enum TokenType type { WHITESPACE }; // Initialised to WHITESPACE
    string text = "";
    int stmt_num_ = 0;
    int stored_stmt_num_ = 0;
    
    string print();

    void IncreaseStmtNum() {
      stored_stmt_num_ += 1;
      stmt_num_ = stored_stmt_num_;
    }

    void StopStmtNum() {
      stored_stmt_num_ -= 1;
      stmt_num_ = 0;
    }

    bool operator==(const Token& t) const {
      return t.type == type && t.text == text && t.stmt_num_ == stmt_num_;
    }
};

class Tokenizer {
  public:
    vector<Token> parse(const string& sourceProgram);

  private:
    void EndToken(Token& token, vector<Token>& tokens);
    void CheckStmtNum(Token& token);
};

