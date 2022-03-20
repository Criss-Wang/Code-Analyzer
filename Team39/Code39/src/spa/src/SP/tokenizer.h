#pragma once

#include <string>
#include <vector>

using namespace std;

enum class TokenType {
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

class Token {
  public:
    TokenType type_ { TokenType::WHITESPACE }; // Initialised to WHITESPACE
    string text_ = "";
    
    void print();

    bool operator==(const Token& t) const {
      return t.type_ == this->type_ && t.text_ == this->text_;
    }
};

class Tokenizer {
  public:
    vector<Token> parse(const string& sourceProgram);

  private:
    void EndToken(Token& token, vector<Token>& tokens);
};
