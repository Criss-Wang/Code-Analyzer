#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "validator.h"

bool Validate(vector<Token> input) {

  string symbols[] = { "=", "<", ">", "(", ")", "+", "-", "*", "/", ";" };
  string statements[] = { "read", "print", "call" };
  string wildcard = "**";

  TokenType expected_type = NAME;
  string expected_text = "procedure";
  
  bool is_procedure = false;
  bool expect_close_curly_bracket = false;
  bool expect_open_bracket = false;

  for (auto token = begin(input); token != end(input); ++token) {

    string token_text;
    TokenType token_type;
    token->print();
    if (find(begin(symbols), end(symbols), token->text) != end(symbols)) {
      token_text = "symbol";
    } else if (find(begin(statements), end(statements), token->text) != end(statements)) {
      token_text = "statement";
    } else {
      token_text = token->text;
    }

    if (token->type == LETTER) {
      token_type = NAME;
    } else {
      token_type = token->type;
    }

    // check if current token type and text matches the expected ones
    if (expect_close_curly_bracket) {
      if (token_type != RIGHT_CURLY && token_type != NAME) {
        return false;
      } else {
        expect_close_curly_bracket = false;
      }

    } else if (expect_open_bracket) {
      if (token_type != RIGHT_PAREN && token_type != NAME) {
        return false;
      } else {
        expect_close_curly_bracket = false;
      }

    } else if ((token_type != expected_type && expected_type != WILDCARD)
      || (token_text != expected_text && expected_text != wildcard)) {
      return false;
    }

    // find expected type and text of next token
    if (token_type == OPERATOR) { // expects variable or bracket after operator
      expected_type = NAME;
      expected_text = "**";
      expect_open_bracket = true;

    } else if (token_type == SEMICOLON) { // expects statement or close curly bracket after semicolon
      expected_type = NAME;
      expected_text = "statement";
      expect_close_curly_bracket = true;

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

    } else if (token_type == NAME) {
      if (is_procedure) { // expects "{" after procedure_name
        expected_type = LEFT_CURLY;
        expected_text = "{";
        is_procedure = false;

      } else { // expects symbol after variable
        expected_type = WILDCARD;
        expected_text = "symbol";
        cout << "here";
      }

    } else {
      cout << "Token ";
      token->print();
      cout <<" not recognized" << endl;
    }
  }
  return true;
}
