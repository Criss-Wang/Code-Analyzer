#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "validator.h"

bool Validate(vector<Token> input) {

  string symbols[] = { "=", "<", ">", "(", ")", "+", "-", "*", "/", ";" };
  string statements[] = { "read", "print", "if", "while", "call" };
  string wildcard = "**";

  TokenType expected_type = NAME;
  string expected_text = "procedure";
  
  bool is_procedure = false;
  bool expect_right_curly_bracket = false;
  bool expect_left_paren_or_integer = false;
  bool expect_semicolon = false;

  for (auto token = begin(input); token != end(input); ++token) {

    string token_text;
    TokenType token_type;

    if (find(begin(symbols), end(symbols), token->text) != end(symbols)) {
      token_text = "symbol";
    } else if (find(begin(statements), end(statements), token->text) != end(statements)) {
      token_text = "statement";
    } else {
      token_text = token->text;
    }

    if (token->type == LETTER) {
      token_type = NAME;
    } else if (token->type == DIGIT) {
      token_type = INTEGER;
    } else {
      token_type = token->type;
    }

    // check if token type and text matches expected
    bool is_match = (token_type == expected_type || expected_type == WILDCARD)
      && (token_text == expected_text || expected_text == wildcard);

    // first check for other possible tokens which are not expected but still valid
    if (expect_right_curly_bracket) {
      if (!(token_type == RIGHT_CURLY || token_type == NAME)) {
        return false;
      } else {
        expect_right_curly_bracket = false;
      }
    } else if (expect_left_paren_or_integer) {
      if (!(token_type == LEFT_PAREN || token_type == INTEGER || token_type == NAME)) {
        return false;
      } else {
        expect_left_paren_or_integer = false;
      }
    } else if (expect_semicolon) {
      if (!(token_type == SEMICOLON || token_type == OPERATOR)) {
        return false;
      } else {
        expect_semicolon = false;
      }
    } else if (!is_match) {
      return false;
    }

    // find expected type and text of next token
    if (token_type == OPERATOR 
      || token_type == LEFT_PAREN) { // expects variable, integer or left paren after operator/left paren
      expected_type = NAME;
      expected_text = "**";
      expect_left_paren_or_integer = true;

    } else if (token_type == RIGHT_PAREN) { // expects operator or semicolon after right paren
      expected_type = OPERATOR;
      expected_text = "**";
      expect_semicolon = true;

    } else if (token_type == SEMICOLON 
      || token_type == RIGHT_CURLY) { // expects statement or right curly after semicolon/right curly
      expected_type = NAME;
      expected_text = "statement";
      expect_right_curly_bracket = true;

    } else if (token_type == NAME && token_text == "procedure") { // expects procedure_name after 'procedure'
      expected_type = NAME;
      expected_text = "**";
      is_procedure = true;

    } else if (token_type == LEFT_CURLY) { // expects variable or stmt after "{"
      if (next(token, 2)->text == "=") {
        expected_type = NAME;
        expected_text = "**";
      } else {
        expected_type = NAME;
        expected_text = "statement";
      }

    } else if (token_text == "statement") { // expects variable after stmt
      expected_type = NAME;
      expected_text = "**";

    } else if (token_type == NAME && is_procedure) { // expects "{" after procedure_name
      expected_type = LEFT_CURLY;
      expected_text = "{";
      is_procedure = false;

    } else if (token_type == NAME || token_type == INTEGER) { // expects symbol after variable or integer
      expected_type = WILDCARD;
      expected_text = "symbol";

    } else {
      cout << "Token ";
      token->print();
      cout <<" not recognized" << endl;
    }
  }
  return true;
}
