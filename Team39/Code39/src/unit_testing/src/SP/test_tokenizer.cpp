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

    Token token1{ DIGIT, "1", 0 };
    Token token2{ INTEGER, "23", 0 };
    Token token3{ INTEGER, "045", 0 };
    expected_tokens = { token1, token2, token3 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Letters and names") {
    source_prog = "x procedure myInt123";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ LETTER, "x", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ NAME, "myInt123", 0 };
    expected_tokens = { token1, token2, token3 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Brackets") {
    source_prog = "( ) { }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ LEFT_PAREN, "(", 0 };
    Token token2{ RIGHT_PAREN, ")", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Operators and semicolon") {
    source_prog = "A + B - C * D / E % F = G < H > I ;"; // To avoid syntax error exception
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ LETTER, "A", 0 };
    Token token2{ OPERATOR, "+", 0 };
    Token token3{ LETTER, "B", 0 };
    Token token4{ OPERATOR, "-", 0 };
    Token token5{ LETTER, "C", 0 };
    Token token6{ OPERATOR, "*", 0 };
    Token token7{ LETTER, "D", 0 };
    Token token8{ OPERATOR, "/", 0 };
    Token token9{ LETTER, "E", 0 };
    Token token10{ OPERATOR, "%", 0 };
    Token token11{ LETTER, "F", 0 };
    Token token12{ OPERATOR, "=", 0 };
    Token token13{ LETTER, "G", 0 };
    Token token14{ OPERATOR, "<", 0 };
    Token token15{ LETTER, "H", 0 };
    Token token16{ OPERATOR, ">", 0 };
    Token token17{ LETTER, "I", 0 };
    Token token18{ SEMICOLON, ";", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9,
                            token10, token11, token12, token13, token14, token15, token16, token17, token18 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with read and print statements") {
    source_prog = "procedure procName { read x; print print; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procName", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "read", 1 };
    Token token5{ LETTER, "x", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ NAME, "print", 2 };
    Token token8{ NAME, "print", 2 };
    Token token9{ SEMICOLON, ";", 2 };
    Token token10{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with assign statement") {
    source_prog = "procedure procName { x = y + 2; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procName", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ LETTER, "x", 1 };
    Token token5{ OPERATOR, "=", 1 };
    Token token6{ LETTER, "y", 1 };
    Token token7{ OPERATOR, "+", 1 };
    Token token8{ DIGIT, "2", 1 };
    Token token9{ SEMICOLON, ";", 1 };
    Token token10{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Same procedure name, variable name and terminal") {
    source_prog = "procedure read { read read; read = read + 1; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "read", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "read", 1 };
    Token token5{ NAME, "read", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ NAME, "read", 2 };
    Token token8{ OPERATOR, "=", 2 };
    Token token9{ NAME, "read", 2 };
    Token token10{ OPERATOR, "+", 2 };
    Token token11{ DIGIT, "1", 2 };
    Token token12{ SEMICOLON, ";", 2 };
    Token token13{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10, 
                            token11, token12, token13 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with whitespaces") {
    source_prog = "procedure     procName   {    read       x    ;      }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procName", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "read", 1 };
    Token token5{ LETTER, "x", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }
  
  SECTION("Procedure with newline") {
    source_prog = "procedure procName \n { read read;\n}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procName", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "read", 1 };
    Token token5{ NAME, "read", 1 };
    Token token6{ SEMICOLON, ";", 1 };
    Token token7{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Stmt with brackets") {
    source_prog = "procedure procName {x = y + (5 + z);}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procName", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
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
    Token token14{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Stmt with brackets") {
    source_prog = "procedure procedure {x = y + (5 + (z + 5));}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
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
    Token token18{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with ifs") {
    source_prog = "procedure procedure { if (x = 1) then { read read; } else { print print; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "if", 1 };
    Token token5{ LEFT_PAREN, "(", 1 };
    Token token6{ LETTER, "x", 1 };
    Token token7{ OPERATOR, "=", 1 };
    Token token8{ DIGIT, "1", 1 };
    Token token9{ RIGHT_PAREN, ")", 1 };
    Token token10{ NAME, "then", 1 };
    Token token11{ LEFT_CURLY, "{", 1 };
    Token token12{ NAME, "read", 2 };
    Token token13{ NAME, "read", 2 };
    Token token14{ SEMICOLON, ";", 2 };
    Token token15{ RIGHT_CURLY, "}", 0 };
    Token token16{ NAME, "else", 0 };
    Token token17{ LEFT_CURLY, "{", 0 };
    Token token18{ NAME, "print", 3 };
    Token token19{ NAME, "print", 3 };
    Token token20{ SEMICOLON, ";", 3 };
    Token token21{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20, token21 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with while") {
    source_prog = "procedure procName { while (x = 1) { read x; x = x + 1; } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procName", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "while", 1 };
    Token token5{ LEFT_PAREN, "(", 1 };
    Token token6{ LETTER, "x", 1 };
    Token token7{ OPERATOR, "=", 1 };
    Token token8{ DIGIT, "1", 1 };
    Token token9{ RIGHT_PAREN, ")", 1 };
    Token token10{ LEFT_CURLY, "{", 1 };
    Token token11{ NAME, "read", 2 };
    Token token12{ LETTER, "x", 2 };
    Token token13{ SEMICOLON, ";", 2 };
    Token token14{ LETTER, "x", 3 };
    Token token15{ OPERATOR, "=", 3 };
    Token token16{ LETTER, "x", 3 };
    Token token17{ OPERATOR, "+", 3 };
    Token token18{ DIGIT, "1", 3 };
    Token token19{ SEMICOLON, ";", 3 };
    Token token20{ RIGHT_CURLY, "}", 0 };
    Token token21{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20, token21 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with ifs followed by while") {
    source_prog = "procedure procedure { if (x = 1) then { read read; } else { print print; } while (x = 1) { read x; x = x + 1; } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "if", 1 };
    Token token5{ LEFT_PAREN, "(", 1 };
    Token token6{ LETTER, "x", 1 };
    Token token7{ OPERATOR, "=", 1 };
    Token token8{ DIGIT, "1", 1 };
    Token token9{ RIGHT_PAREN, ")", 1 };
    Token token10{ NAME, "then", 1 };
    Token token11{ LEFT_CURLY, "{", 1 };
    Token token12{ NAME, "read", 2 };
    Token token13{ NAME, "read", 2 };
    Token token14{ SEMICOLON, ";", 2 };
    Token token15{ RIGHT_CURLY, "}", 0 };
    Token token16{ NAME, "else", 0 };
    Token token17{ LEFT_CURLY, "{", 0 };
    Token token18{ NAME, "print", 3 };
    Token token19{ NAME, "print", 3 };
    Token token20{ SEMICOLON, ";", 3 };
    Token token21{ RIGHT_CURLY, "}", 0 };
    Token token22{ NAME, "while", 4 };
    Token token23{ LEFT_PAREN, "(", 4 };
    Token token24{ LETTER, "x", 4 };
    Token token25{ OPERATOR, "=", 4 };
    Token token26{ DIGIT, "1", 4 };
    Token token27{ RIGHT_PAREN, ")", 4 };
    Token token28{ LEFT_CURLY, "{", 4 };
    Token token29{ NAME, "read", 5 };
    Token token30{ LETTER, "x", 5 };
    Token token31{ SEMICOLON, ";", 5 };
    Token token32{ LETTER, "x", 6 };
    Token token33{ OPERATOR, "=", 6 };
    Token token34{ LETTER, "x", 6 };
    Token token35{ OPERATOR, "+", 6 };
    Token token36{ DIGIT, "1", 6 };
    Token token37{ SEMICOLON, ";", 6 };
    Token token38{ RIGHT_CURLY, "}", 0 };
    Token token39{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20, 
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30,
                           token31, token32, token33, token34, token35, token36, token37, token38, token39 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with while followed by ifs") {
    source_prog = "procedure procedure { while (x = 1) { read x; x = x + 1; } if (x = 1) then { read read; } else { print print; } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "while", 1 };
    Token token5{ LEFT_PAREN, "(", 1 };
    Token token6{ LETTER, "x", 1 };
    Token token7{ OPERATOR, "=", 1 };
    Token token8{ DIGIT, "1", 1 };
    Token token9{ RIGHT_PAREN, ")", 1 };
    Token token10{ LEFT_CURLY, "{", 1 };
    Token token11{ NAME, "read", 2 };
    Token token12{ LETTER, "x", 2 };
    Token token13{ SEMICOLON, ";", 2 };
    Token token14{ LETTER, "x", 3 };
    Token token15{ OPERATOR, "=", 3 };
    Token token16{ LETTER, "x", 3 };
    Token token17{ OPERATOR, "+", 3 };
    Token token18{ DIGIT, "1", 3 };
    Token token19{ SEMICOLON, ";", 3 };
    Token token20{ RIGHT_CURLY, "}", 0 };
    Token token21{ NAME, "if", 4 };
    Token token22{ LEFT_PAREN, "(", 4 };
    Token token23{ LETTER, "x", 4 };
    Token token24{ OPERATOR, "=", 4 };
    Token token25{ DIGIT, "1", 4 };
    Token token26{ RIGHT_PAREN, ")", 4 };
    Token token27{ NAME, "then", 4 };
    Token token28{ LEFT_CURLY, "{", 4 };
    Token token29{ NAME, "read", 5 };
    Token token30{ NAME, "read", 5 };
    Token token31{ SEMICOLON, ";", 5 };
    Token token32{ RIGHT_CURLY, "}", 0 };
    Token token33{ NAME, "else", 0 };
    Token token34{ LEFT_CURLY, "{", 0 };
    Token token35{ NAME, "print", 6 };
    Token token36{ NAME, "print", 6 };
    Token token37{ SEMICOLON, ";", 6 };
    Token token38{ RIGHT_CURLY, "}", 0 };
    Token token39{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30,
                           token31, token32, token33, token34, token35, token36, token37, token38, token39 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with ifs nested in while") {
    source_prog = "procedure procedure { while (s = 0) { if (x = 2) then { read read; } else { print print; } } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "while", 1 };
    Token token5{ LEFT_PAREN, "(", 1 };
    Token token6{ LETTER, "s", 1 };
    Token token7{ OPERATOR, "=", 1 };
    Token token8{ DIGIT, "0", 1 };
    Token token9{ RIGHT_PAREN, ")", 1 };
    Token token10{ LEFT_CURLY, "{", 1 };
    Token token11{ NAME, "if", 2 };
    Token token12{ LEFT_PAREN, "(", 2 };
    Token token13{ LETTER, "x", 2 };
    Token token14{ OPERATOR, "=", 2 };
    Token token15{ DIGIT, "2", 2 };
    Token token16{ RIGHT_PAREN, ")", 2 };
    Token token17{ NAME, "then", 2 };
    Token token18{ LEFT_CURLY, "{", 2 };
    Token token19{ NAME, "read", 3 };
    Token token20{ NAME, "read", 3 };
    Token token21{ SEMICOLON, ";", 3 };
    Token token22{ RIGHT_CURLY, "}", 0 };
    Token token23{ NAME, "else", 0 };
    Token token24{ LEFT_CURLY, "{", 0 };
    Token token25{ NAME, "print", 4 };
    Token token26{ NAME, "print", 4 };
    Token token27{ SEMICOLON, ";", 4 };
    Token token28{ RIGHT_CURLY, "}", 0 };
    Token token29{ RIGHT_CURLY, "}", 0 };
    Token token30{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with while nested in ifs") {
    source_prog = "procedure procedure { if (x = 2) then { while (s = 0) { read s; } } else { while (s = 0) { read s; } } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ NAME, "procedure", 0 };
    Token token2{ NAME, "procedure", 0 };
    Token token3{ LEFT_CURLY, "{", 0 };
    Token token4{ NAME, "if", 1 };
    Token token5{ LEFT_PAREN, "(", 1 };
    Token token6{ LETTER, "x", 1 };
    Token token7{ OPERATOR, "=", 1 };
    Token token8{ DIGIT, "2", 1 };
    Token token9{ RIGHT_PAREN, ")", 1 };
    Token token10{ NAME, "then", 1 };
    Token token11{ LEFT_CURLY, "{", 1 };
    Token token12{ NAME, "while", 2 };
    Token token13{ LEFT_PAREN, "(", 2 };
    Token token14{ LETTER, "s", 2 };
    Token token15{ OPERATOR, "=", 2 };
    Token token16{ DIGIT, "0", 2 };
    Token token17{ RIGHT_PAREN, ")", 2 };
    Token token18{ LEFT_CURLY, "{", 2 };
    Token token19{ NAME, "read", 3 };
    Token token20{ LETTER, "s", 3 };
    Token token21{ SEMICOLON, ";", 3 };
    Token token22{ RIGHT_CURLY, "}", 0 };
    Token token23{ RIGHT_CURLY, "}", 0 };
    Token token24{ NAME, "else", 0 };
    Token token25{ LEFT_CURLY, "{", 0 };
    Token token26{ NAME, "while", 4 };
    Token token27{ LEFT_PAREN, "(", 4 };
    Token token28{ LETTER, "s", 4 };
    Token token29{ OPERATOR, "=", 4 };
    Token token30{ DIGIT, "0", 4 };
    Token token31{ RIGHT_PAREN, ")", 4 };
    Token token32{ LEFT_CURLY, "{", 4 };
    Token token33{ NAME, "read", 5 };
    Token token34{ LETTER, "s", 5 };
    Token token35{ SEMICOLON, ";", 5 };
    Token token36{ RIGHT_CURLY, "}", 0 };
    Token token37{ RIGHT_CURLY, "}", 0 };
    Token token38{ RIGHT_CURLY, "}", 0 };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30,
                           token31, token32, token33, token34, token35, token36, token37, token38 };

    RequireTokenizer(expected_tokens == actual_tokens);
  }
}

TEST_CASE("Invalid tokenizer output") {
  Tokenizer tokenizer;
  string source_prog;
  vector<Token> actual_tokens;
  vector<Token> expected_tokens;

  SECTION("Invalid name of digits followed by letters") {
    source_prog = "procedure procName { print 123myInt; }";
    actual_tokens = tokenizer.parse(source_prog);
    expected_tokens = { };

    RequireTokenizer(expected_tokens == actual_tokens);
  }
}

// TODO: Move the validation test cases to test_validation
  /*
  SECTION("Procedure with no semicolon") {
    source_prog = "procedure procName { x = y + 2 }";
    actual_tokens = tokenizer.parse(source_prog);
    expected_tokens = { };

    RequireTokenizer(expected_tokens == actual_tokens);
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
  }*/

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


