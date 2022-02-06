#include "SP/tokenizer.h"
#include "SP/validator.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

void RequireTokenizer(bool b) {
    REQUIRE(b);
}

TEST_CASE("Valid tokenizer output") {
  Tokenizer tokenizer;
  string source_prog;
  vector<Token> actual_tokens;
  vector<Token> expected_tokens;

  SECTION("Digits and integer") {
    source_prog = "1 23 045";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ DIGIT, "1", 1 };
    Token token2{ INTEGER, "23", 1 };
    Token token3{ INTEGER, "045", 1 };
    expected_tokens = { token1, token2, token3 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Letters and names") {
    source_prog = "x procedure myInt123";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ LETTER, "x", 1 };
    Token token2{ NAME, "procedure", 1 };
    Token token3{ NAME, "myInt123", 1 };
    expected_tokens = { token1, token2, token3 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Brackets") {
    source_prog = "( ) { }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ LEFT_PAREN, "(", 1 };
    Token token2{ RIGHT_PAREN, ")", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ RIGHT_CURLY, "}", 1 };
    expected_tokens = { token1, token2, token3, token4 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Operators and semicolon") {
    source_prog = "A + B - C * D / E % F = G < H > I ;"; // To avoid syntax error exception
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ LETTER, "A", 1 };
    Token token2{ OPERATOR, "+", 1 };
    Token token3{ LETTER, "B", 1 };
    Token token4{ OPERATOR, "-", 1 };
    Token token5{ LETTER, "C", 1 };
    Token token6{ OPERATOR, "*", 1 };
    Token token7{ LETTER, "D", 1 };
    Token token8{ OPERATOR, "/", 1 };
    Token token9{ LETTER, "E", 1 };
    Token token10{ OPERATOR, "%", 1 };
    Token token11{ LETTER, "F", 1 };
    Token token12{ OPERATOR, "=", 1 };
    Token token13{ LETTER, "G", 1 };
    Token token14{ OPERATOR, "<", 1 };
    Token token15{ LETTER, "H", 1 };
    Token token16{ OPERATOR, ">", 1 };
    Token token17{ LETTER, "I", 1 };
    Token token18{ SEMICOLON, ";", 1 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9,
                            token10, token11, token12, token13, token14, token15, token16, token17, token18 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with read and print statements") {
    source_prog = "procedure procName { read x; print y; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "procName", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ NAME, "read", 1 };
    Token token5{ LETTER, "x", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ NAME, "print", 2 };
    Token token8{ LETTER, "y", 2 };
    Token token9{ SEMICOLON, ";", 2 };
    Token token10{ RIGHT_CURLY, "}", 3 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with assign statement") {
    source_prog = "procedure procName { x = y + 2; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "procName", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ LETTER, "x", 1 };
    Token token5{ OPERATOR, "=", 1 };
    Token token6{ LETTER, "y", 1 };
    Token token7{ OPERATOR, "+", 1 };
    Token token8{ DIGIT, "2", 1 };
    Token token9{ SEMICOLON, ";", 1 };
    Token token10{ RIGHT_CURLY, "}", 2 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Same procedure name, variable name and terminal") {
    source_prog = "procedure read { read read; read = read + 1; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "read", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ NAME, "read", 1 };
    Token token5{ NAME, "read", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ NAME, "read", 2 };
    Token token8{ OPERATOR, "=", 2 };
    Token token9{ NAME, "read", 2 };
    Token token10{ OPERATOR, "+", 2 };
    Token token11{ DIGIT, "1", 2 };
    Token token12{ SEMICOLON, ";", 2 };
    Token token13{ RIGHT_CURLY, "}", 3 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10, 
                            token11, token12, token13 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with whitespaces") {
    source_prog = "procedure     procName   {    read       x    ;      }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "procName", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ NAME, "read", 1 };
    Token token5{ LETTER, "x", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ RIGHT_CURLY, "}", 2 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }
  
  SECTION("Procedure with newline") {
    source_prog = "procedure procName \n { read x;\n}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "procName", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ NAME, "read", 1 };
    Token token5{ LETTER, "x", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ RIGHT_CURLY, "}", 2 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Stmt with brackets") {
    source_prog = "procedure procName {x = y + (5 + z);}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "procName", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ LETTER, "x", 1 };
    Token token5{ OPERATOR, "=", 1 };
    Token token6{ LETTER, "y", 1 };
    Token token7{ OPERATOR, "+", 1 };
    Token token8{ LEFT_PAREN, "(", 1 };
    Token token9{ DIGIT, "5", 1 };
    Token token10{ OPERATOR, "+", 1 };
    Token token11{ LETTER, "z", 1 };
    Token token12{ RIGHT_PAREN, ")", 1 };
    Token token13{ SEMICOLON, ";", 1 };
    Token token14{ RIGHT_CURLY, "}", 2 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Stmt with brackets") {
    source_prog = "procedure procName {x = y + (5 + (z + 5));}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 1 };
    Token token2{ NAME, "procName", 1 };
    Token token3{ LEFT_CURLY, "{", 1 };
    Token token4{ LETTER, "x", 1 };
    Token token5{ OPERATOR, "=", 1 };
    Token token6{ LETTER, "y", 1 };
    Token token7{ OPERATOR, "+", 1 };
    Token token8{ LEFT_PAREN, "(", 1 };
    Token token9{ DIGIT, "5", 1 };
    Token token10{ OPERATOR, "+", 1 };
    Token token11{ LEFT_PAREN, "(", 1 };
    Token token12{ LETTER, "z", 1 };
    Token token13{ OPERATOR, "+", 1 };
    Token token14{ DIGIT, "5", 1 };
    Token token15{ RIGHT_PAREN, ")", 1 };
    Token token16{ RIGHT_PAREN, ")", 1 };
    Token token17{ SEMICOLON, ";", 1 };
    Token token18{ RIGHT_CURLY, "}", 2 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }
}

TEST_CASE("Invalid tokenizer output") {
  Tokenizer tokenizer;
  string source_prog;
  vector<Token> actual_tokens;

  SECTION("Procedure with no semicolon") {
    source_prog = "procedure procName { x = y + 2 }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid assign") {
    source_prog = "procedure procName { = x + 2; }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid read") {
    source_prog = "procedure procName { read +; }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid empty stmtLst") {
    source_prog = "procedure procName { }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid stmtLst") {
    source_prog = "procedure procName { ; }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid name of digits followed by letters") {
    source_prog = "procedure procName { print 123myInt; }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid lack of close parenthesis") {
    source_prog = "procedure procName { y = (x + 2; }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid order of parenthesis") {
    source_prog = "procedure procName { y = )x + 2(; }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }

  SECTION("Invalid number of parenthesis") {
    source_prog = "procedure procName { x = x + (2 + y)); }";

    try {
      actual_tokens = tokenizer.parse(source_prog);
    } catch (const std::invalid_argument& e) {}
  }
}

/*
TEST_CASE("Run tokenizer with file input") {
  ifstream myFile("SourceProgramInput.txt"); // Put input file in same folder as unit_testing.exe
  Tokenizer tokenizer;
  string source_prog;

  if (myFile) {
    try {
      ostringstream ss;
      ss << myFile.rdbuf();
      source_prog = ss.str();

      vector<Token> actual_tokens = tokenizer.parse(source_prog);

      for (Token curr_token : actual_tokens) {
        curr_token.print();
      }

      if (actual_tokens.size() != 0) {
        //
      } else {
        cout << "No tokens";
      }
    } catch (const std::invalid_argument& e) {
      cout << "Syntax error" << endl;
    }
    myFile.close();
  } else {
    cout << "Unable to open file";
  }
  RequireTokenizer(1 == 1);
}
*/


