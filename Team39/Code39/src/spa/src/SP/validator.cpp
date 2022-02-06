#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "validator.h"

bool Validate(vector<Token> input) {

  string symbols[] = { "=", "<", ">", "(", ")", "+", "-", "*", "/", ";" };
  string statements[] = { "read", "print" };
  string wildcard = "**";

  TokenType expected_type = NAME;
  string expected_text = "procedure";
  string variable_type = ""; // can be "procedure_name", "variable_name"

  bool expect_close_curly_bracket = false;
  bool expect_open_bracket = false;
  bool expect_letter = false; // TODO (lauren): change letter types to names for tokenizer

  for (auto token = begin(input); token != end(input); ++token) {

    string token_text;
    if (find(begin(symbols), end(symbols), token->text) != end(symbols)) {
      token_text = "symbol";
    } else if (find(begin(statements), end(statements), token->text) != end(statements)) {
      token_text = "statement";
    } else {
      token_text = token->text;
    }

    //cout << "token: " << token->print() << endl;
    //cout << "expect: " << tokenTypeStrings[expected_type] << " " << expected_text << endl;
    //cout << "actual: " << tokenTypeStrings[token->type] << ", " << token_text<< endl;

    if (expect_close_curly_bracket) {
      if (token->type != RIGHT_CURLY) {
        expect_close_curly_bracket = false;
      }
    } else if (expect_open_bracket) {
      if (token->type != RIGHT_PAREN) {
        expect_close_curly_bracket = false;
      }
    } else if (expect_letter) {
      if (token->type != LETTER) {
        expect_letter = false;
      }
    } else if ((token->type != expected_type && expected_type != WILDCARD)
      || (token_text != expected_text && expected_text != wildcard)) {
      return false;
    }

    if (token->type == OPERATOR) { // expects variable or bracket after operator
      expected_type = NAME;
      expected_text = "**";
      expect_open_bracket = true;
    } else if (token->type == SEMICOLON) { // expects statement or close curly bracket after semicolon
      expected_type = NAME;
      expected_text = "statement";
      expect_close_curly_bracket = true;
    } else if (expected_type == NAME && expected_text == "procedure") { // expects procedure_name after 'procedure'
      expected_type = NAME;
      expected_text = "**";
      variable_type = "procedure_name";
    } else if (variable_type == "procedure_name") { // expects "{" after procedure_name
      expected_type = LEFT_CURLY;
      expected_text = "{";
      variable_type = "";
    } else if (expected_type == LEFT_CURLY) { // expects stmt or assignment after "{"
      if (next(token, 2)->text == "=") {
        expected_type = NAME;
        expected_text = "**";
        expect_letter = true;
      } else {
        expected_type = NAME;
        expected_text = "statement";
      }
    } else if (expected_text == "statement") { // expects variable or letter after stmt
      expected_type = NAME;
      expected_text = "**";
      expect_letter = true;
    } else if (expected_type == NAME) { // expects symbol after variable or letter name
      expected_type = WILDCARD;
      expected_text = "symbol";
    }
  }
  return true;
}
