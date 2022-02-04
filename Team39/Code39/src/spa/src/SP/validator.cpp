#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

#include "validator.h"

bool Validate(vector<TokenTemp> input) {

  string symbols[] = { "=", "<", ">", "(", ")", "+", "-", "*", "/", ";" };
  string statements[] = { "read", "print" };
  string wildcard = "**";

  string expected_type = "name";
  string expected_text = "procedure";
  string variable_type = ""; // can be "procedure_name", "variable_name"

  bool expect_close_curly_bracket = false;
  bool expect_open_bracket = false;

  for (auto token = begin(input); token != end(input); ++token) {

    string token_type;
    if (find(begin(symbols), end(symbols), token->text) != end(symbols)) {
      token_type = "symbol";
    } else if (find(begin(statements), end(statements), token->text) != end(statements)) {
      token_type = "statement";
    } else {
      token_type = token->type;
    }

    cout << "expect: " << expected_type << " " << expected_text << endl;
    cout << "actual: " << token->type << " " << token->text << endl;
    if (expect_close_curly_bracket) {
      if (token->type != "}") {
        expect_close_curly_bracket = false;
      }
    } else if (expect_open_bracket) {
      if (token->type != ")") {
        expect_close_curly_bracket = false;
      }
    } else if ((token_type != expected_type && expected_type != wildcard)
      || (token->text != expected_text && expected_text != wildcard)) {
      return false;
    }

    if (expected_type == "name" && expected_text == "procedure") { // expects procedure_name after 'procedure'
      expected_type = "name";
      expected_text = "**";
      variable_type = "procedure_name";
    } else if (variable_type == "procedure_name") { // expects "{" after procedure_name
      expected_type = "{";
      expected_text = "{";
      variable_type = "";
    } else if (expected_type == "{") { // expects stmt or assignment after "{"
      if (next(token, 2)->text == "=") {
        expected_type = "variable";
        expected_text = "**";
      } else {
        expected_type = "statement";
        expected_text = "**";
      }
    } else if (expected_type == "statement") { // expects variable after stmt
      expected_type = "variable";
      expected_text = "**";
    } else if (expected_type == "variable") { // expects symbol after variable name
      expected_type = "symbol";
      expected_text = "**";
    } else if (token->type == "operator") { // expects variable or bracket after operator
      expected_type = "variable";
      expected_text = "**";
      expect_open_bracket = true;
    } else if (token->type == ";") { // expects statement or close curly bracket after semicolon
      expected_type = "statement";
      expected_text = "**";
      expect_close_curly_bracket = true;
    }
  }
  return true;
}
