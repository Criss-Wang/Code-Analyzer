#pragma once

#include <string>
#include <vector>
#include <unordered_set>

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

    bool operator==(const Token& t) const {
      return t.type_ == this->type_ && t.text_ == this->text_;
    }
};

class Tokenizer {
  public:
    Tokenizer() = default;
    Tokenizer(const std::string& sourceProgram);
    vector<Token> tokens_list_;

  private:
    void EndToken(Token& token, vector<Token>& tokens);

    unordered_set<char> operators = { '+', '-', '*', '/', '%', '=', '<', '>', '!', '&', '|' };

    bool IsOperator(char curr_char) {
      return (operators.find(curr_char) != operators.end());
    }

    bool IsWhitespace(char curr_char) {
      return curr_char == ' '
        || curr_char == '	'
        || curr_char == '\n';
    }

    bool IsLeftParen(char curr_char) {
      return curr_char == '(';
    }

    bool IsRightParen(char curr_char) {
      return curr_char == ')';
    }

    bool IsLeftCurlyBracket(char curr_char) {
      return curr_char == '{';
    }

    bool IsRightCurlyBracket(char curr_char) {
      return curr_char == '}';
    }

    bool IsSemicolon(char curr_char) {
      return curr_char == ';';
    }

    void AddCharTokenToList(char curr_char, TokenType token_type, Token& current_token, vector<Token>& tokens_list);

    void AddDigitTokenToList(char curr_char, Token& current_token, vector<Token>& tokens_list);

    void AddLetterTokenToList(char curr_char, Token& current_token, vector<Token>& tokens_list);

    void AddOperatorTokenToList(char curr_char, Token& current_token, vector<Token>& tokens_list);
};
