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
  NOT_OPERATOR,
  REL_OPERATOR,
  COND_OPERATOR
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
  "NOT_OPERATOR",
  "REL_OPERATOR",
  "COND_OPERATOR"
};

class Token {
  public:
    enum TokenType type_ { WHITESPACE }; // Initialised to WHITESPACE
    string text_ = "";
    int stmt_num_ = 1;
    int stored_stmt_num_ = 1;
    
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
      return t.type_ == this->type_ && t.text_ == this->text_ && t.stmt_num_ == this->stmt_num_;
    }
};

class Tokenizer {
  public:
    vector<Token> parse(const string& sourceProgram);

  private:
    void EndToken(Token& token, vector<Token>& tokens);
};

