#include "SP/tokenizer.h"
#include "SP/sp_exceptions.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

TEST_CASE("Valid tokenizer output") {
  Tokenizer tokenizer;
  string source_prog;
  vector<Token> actual_tokens;
  vector<Token> expected_tokens;

  SECTION("Digits and integer") {
    source_prog = "1 23";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::DIGIT, "1"};
    Token token2{ TokenType::INTEGER, "23"};
    expected_tokens = { token1, token2 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Letters and names") {
    source_prog = "x read myInt123";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::LETTER, "x"};
    Token token2{ TokenType::NAME, "read"};
    Token token3{ TokenType::NAME, "myInt123"};
    expected_tokens = { token1, token2, token3 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Brackets") {
    source_prog = "( ) { }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::LEFT_PAREN, "("};
    Token token2{ TokenType::RIGHT_PAREN, ")"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Operators and semicolon") {
    source_prog = "A + B - C * D / E % F = G < H > I ;"; // To avoid syntax error exception
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::LETTER, "A"};
    Token token2{ TokenType::OPERATOR, "+"};
    Token token3{ TokenType::LETTER, "B"};
    Token token4{ TokenType::OPERATOR, "-"};
    Token token5{ TokenType::LETTER, "C"};
    Token token6{ TokenType::OPERATOR, "*"};
    Token token7{ TokenType::LETTER, "D"};
    Token token8{ TokenType::OPERATOR, "/"};
    Token token9{ TokenType::LETTER, "E"};
    Token token10{ TokenType::OPERATOR, "%"};
    Token token11{ TokenType::LETTER, "F"};
    Token token12{ TokenType::OPERATOR, "="};
    Token token13{ TokenType::LETTER, "G"};
    Token token14{ TokenType::OPERATOR, "<"};
    Token token15{ TokenType::LETTER, "H"};
    Token token16{ TokenType::OPERATOR, ">"};
    Token token17{ TokenType::LETTER, "I"};
    Token token18{ TokenType::SEMICOLON, ";"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9,
                            token10, token11, token12, token13, token14, token15, token16, token17, token18 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with read and print statements") {
    source_prog = "procedure procName { read x; print print; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procName"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "read"};
    Token token5{ TokenType::LETTER, "x"};
    Token token6{ TokenType::SEMICOLON, ";"};
    Token token7{ TokenType::NAME, "print"};
    Token token8{ TokenType::NAME, "print"};
    Token token9{ TokenType::SEMICOLON, ";"};
    Token token10{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with assign statement") {
    source_prog = "procedure proc1Name { x = y + 2; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "proc1Name"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::LETTER, "x"};
    Token token5{ TokenType::OPERATOR, "="};
    Token token6{ TokenType::LETTER, "y"};
    Token token7{ TokenType::OPERATOR, "+"};
    Token token8{ TokenType::DIGIT, "2"};
    Token token9{ TokenType::SEMICOLON, ";"};
    Token token10{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Same procedure name, variable name and terminal") {
    source_prog = "procedure read { read read; read = read + 1; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "read"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "read"};
    Token token5{ TokenType::NAME, "read"};
    Token token6{ TokenType::SEMICOLON, ";"};
    Token token7{ TokenType::NAME, "read"};
    Token token8{ TokenType::OPERATOR, "="};
    Token token9{ TokenType::NAME, "read"};
    Token token10{ TokenType::OPERATOR, "+"};
    Token token11{ TokenType::DIGIT, "1"};
    Token token12{ TokenType::SEMICOLON, ";"};
    Token token13{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                            token11, token12, token13 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with whitespaces") {
    source_prog = "procedure     procName   {    read       x    ;      }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procName"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "read"};
    Token token5{ TokenType::LETTER, "x"};
    Token token6{ TokenType::SEMICOLON, ";"};
    Token token7{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with newline") {
    source_prog = "procedure procName \n { read read;\n}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procName"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "read"};
    Token token5{ TokenType::NAME, "read"};
    Token token6{ TokenType::SEMICOLON, ";"};
    Token token7{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Stmt with brackets") {
    source_prog = "procedure procName {x = y + (5 + z);}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procName"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::LETTER, "x"};
    Token token5{ TokenType::OPERATOR, "="};
    Token token6{ TokenType::LETTER, "y"};
    Token token7{ TokenType::OPERATOR, "+"};
    Token token8{ TokenType::LEFT_PAREN, "("};
    Token token9{ TokenType::DIGIT, "5"};
    Token token10{ TokenType::OPERATOR, "+"};
    Token token11{ TokenType::LETTER, "z"};
    Token token12{ TokenType::RIGHT_PAREN, ")"};
    Token token13{ TokenType::SEMICOLON, ";"};
    Token token14{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Stmt with brackets") {
    source_prog = "procedure procedure {x = y + (5 + (z + 5));}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procedure"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::LETTER, "x"};
    Token token5{ TokenType::OPERATOR, "="};
    Token token6{ TokenType::LETTER, "y"};
    Token token7{ TokenType::OPERATOR, "+"};
    Token token8{ TokenType::LEFT_PAREN, "("};
    Token token9{ TokenType::DIGIT, "5"};
    Token token10{ TokenType::OPERATOR, "+"};
    Token token11{ TokenType::LEFT_PAREN, "("};
    Token token12{ TokenType::LETTER, "z"};
    Token token13{ TokenType::OPERATOR, "+"};
    Token token14{ TokenType::DIGIT, "5"};
    Token token15{ TokenType::RIGHT_PAREN, ")"};
    Token token16{ TokenType::RIGHT_PAREN, ")"};
    Token token17{ TokenType::SEMICOLON, ";"};
    Token token18{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with ifs") {
    source_prog = "procedure procedure { if (x = 1) then { read read; } else { print print; }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procedure"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "if"};
    Token token5{ TokenType::LEFT_PAREN, "("};
    Token token6{ TokenType::LETTER, "x"};
    Token token7{ TokenType::OPERATOR, "="};
    Token token8{ TokenType::DIGIT, "1"};
    Token token9{ TokenType::RIGHT_PAREN, ")"};
    Token token10{ TokenType::NAME, "then"};
    Token token11{ TokenType::LEFT_CURLY, "{"};
    Token token12{ TokenType::NAME, "read"};
    Token token13{ TokenType::NAME, "read"};
    Token token14{ TokenType::SEMICOLON, ";"};
    Token token15{ TokenType::RIGHT_CURLY, "}"};
    Token token16{ TokenType::NAME, "else"};
    Token token17{ TokenType::LEFT_CURLY, "{"};
    Token token18{ TokenType::NAME, "print"};
    Token token19{ TokenType::NAME, "print"};
    Token token20{ TokenType::SEMICOLON, ";"};
    Token token21{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20, token21 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with while") {
    source_prog = "procedure procName { while (x = 1) { read x; x = x + 1; } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procName"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "while"};
    Token token5{ TokenType::LEFT_PAREN, "("};
    Token token6{ TokenType::LETTER, "x"};
    Token token7{ TokenType::OPERATOR, "="};
    Token token8{ TokenType::DIGIT, "1"};
    Token token9{ TokenType::RIGHT_PAREN, ")"};
    Token token10{ TokenType::LEFT_CURLY, "{"};
    Token token11{ TokenType::NAME, "read"};
    Token token12{ TokenType::LETTER, "x"};
    Token token13{ TokenType::SEMICOLON, ";"};
    Token token14{ TokenType::LETTER, "x"};
    Token token15{ TokenType::OPERATOR, "="};
    Token token16{ TokenType::LETTER, "x"};
    Token token17{ TokenType::OPERATOR, "+"};
    Token token18{ TokenType::DIGIT, "1"};
    Token token19{ TokenType::SEMICOLON, ";"};
    Token token20{ TokenType::RIGHT_CURLY, "}"};
    Token token21{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20, token21 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with ifs followed by while") {
    source_prog = "procedure procedure { if (x = 1) then { read read; } else { print print; } while (x = 1) { read x; x = x + 1; } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procedure"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "if"};
    Token token5{ TokenType::LEFT_PAREN, "("};
    Token token6{ TokenType::LETTER, "x"};
    Token token7{ TokenType::OPERATOR, "="};
    Token token8{ TokenType::DIGIT, "1"};
    Token token9{ TokenType::RIGHT_PAREN, ")"};
    Token token10{ TokenType::NAME, "then"};
    Token token11{ TokenType::LEFT_CURLY, "{" };
    Token token12{ TokenType::NAME, "read" };
    Token token13{ TokenType::NAME, "read"};
    Token token14{ TokenType::SEMICOLON, ";"};
    Token token15{ TokenType::RIGHT_CURLY, "}"};
    Token token16{ TokenType::NAME, "else"};
    Token token17{ TokenType::LEFT_CURLY, "{"};
    Token token18{ TokenType::NAME, "print"};
    Token token19{ TokenType::NAME, "print"};
    Token token20{ TokenType::SEMICOLON, ";"};
    Token token21{ TokenType::RIGHT_CURLY, "}"};
    Token token22{ TokenType::NAME, "while"};
    Token token23{ TokenType::LEFT_PAREN, "("};
    Token token24{ TokenType::LETTER, "x"};
    Token token25{ TokenType::OPERATOR, "="};
    Token token26{ TokenType::DIGIT, "1"};
    Token token27{ TokenType::RIGHT_PAREN, ")"};
    Token token28{ TokenType::LEFT_CURLY, "{"};
    Token token29{ TokenType::NAME, "read"};
    Token token30{ TokenType::LETTER, "x"};
    Token token31{ TokenType::SEMICOLON, ";"};
    Token token32{ TokenType::LETTER, "x"};
    Token token33{ TokenType::OPERATOR, "="};
    Token token34{ TokenType::LETTER, "x"};
    Token token35{ TokenType::OPERATOR, "+"};
    Token token36{ TokenType::DIGIT, "1"};
    Token token37{ TokenType::SEMICOLON, ";"};
    Token token38{ TokenType::RIGHT_CURLY, "}",};
    Token token39{ TokenType::RIGHT_CURLY, "}"};
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30,
                           token31, token32, token33, token34, token35, token36, token37, token38, token39 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with while followed by ifs") {
    source_prog = "procedure procedure { while (x = 1) { read x; x = x + 1; } if (x = 1) then { read read; } else { print print; } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure"};
    Token token2{ TokenType::NAME, "procedure"};
    Token token3{ TokenType::LEFT_CURLY, "{"};
    Token token4{ TokenType::NAME, "while"};
    Token token5{ TokenType::LEFT_PAREN, "("};
    Token token6{ TokenType::LETTER, "x"};
    Token token7{ TokenType::OPERATOR, "="};
    Token token8{ TokenType::DIGIT, "1"};
    Token token9{ TokenType::RIGHT_PAREN, ")"};
    Token token10{ TokenType::LEFT_CURLY, "{"};
    Token token11{ TokenType::NAME, "read"};
    Token token12{ TokenType::LETTER, "x"};
    Token token13{ TokenType::SEMICOLON, ";"};
    Token token14{ TokenType::LETTER, "x"};
    Token token15{ TokenType::OPERATOR, "="};
    Token token16{ TokenType::LETTER, "x"};
    Token token17{ TokenType::OPERATOR, "+"};
    Token token18{ TokenType::DIGIT, "1"};
    Token token19{ TokenType::SEMICOLON, ";"};
    Token token20{ TokenType::RIGHT_CURLY, "}"};
    Token token21{ TokenType::NAME, "if" };
    Token token22{ TokenType::LEFT_PAREN, "(" };
    Token token23{ TokenType::LETTER, "x" };
    Token token24{ TokenType::OPERATOR, "=" };
    Token token25{ TokenType::DIGIT, "1" };
    Token token26{ TokenType::RIGHT_PAREN, ")" };
    Token token27{ TokenType::NAME, "then" };
    Token token28{ TokenType::LEFT_CURLY, "{" };
    Token token29{ TokenType::NAME, "read" };
    Token token30{ TokenType::NAME, "read" };
    Token token31{ TokenType::SEMICOLON, ";" };
    Token token32{ TokenType::RIGHT_CURLY, "}" };
    Token token33{ TokenType::NAME, "else" };
    Token token34{ TokenType::LEFT_CURLY, "{" };
    Token token35{ TokenType::NAME, "print" };
    Token token36{ TokenType::NAME, "print" };
    Token token37{ TokenType::SEMICOLON, ";" };
    Token token38{ TokenType::RIGHT_CURLY, "}" };
    Token token39{ TokenType::RIGHT_CURLY, "}" };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30,
                           token31, token32, token33, token34, token35, token36, token37, token38, token39 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with ifs nested in while") {
    source_prog = "procedure procedure { while (s = 0) { if (x = 2) then { read read; } else { print print; } } }";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure" };
    Token token2{ TokenType::NAME, "procedure" };
    Token token3{ TokenType::LEFT_CURLY, "{" };
    Token token4{ TokenType::NAME, "while" };
    Token token5{ TokenType::LEFT_PAREN, "(" };
    Token token6{ TokenType::LETTER, "s" };
    Token token7{ TokenType::OPERATOR, "=" };
    Token token8{ TokenType::DIGIT, "0" };
    Token token9{ TokenType::RIGHT_PAREN, ")" };
    Token token10{ TokenType::LEFT_CURLY, "{" };
    Token token11{ TokenType::NAME, "if" };
    Token token12{ TokenType::LEFT_PAREN, "(" };
    Token token13{ TokenType::LETTER, "x" };
    Token token14{ TokenType::OPERATOR, "=" };
    Token token15{ TokenType::DIGIT, "2" };
    Token token16{ TokenType::RIGHT_PAREN, ")" };
    Token token17{ TokenType::NAME, "then" };
    Token token18{ TokenType::LEFT_CURLY, "{" };
    Token token19{ TokenType::NAME, "read" };
    Token token20{ TokenType::NAME, "read" };
    Token token21{ TokenType::SEMICOLON, ";" };
    Token token22{ TokenType::RIGHT_CURLY, "}" };
    Token token23{ TokenType::NAME, "else" };
    Token token24{ TokenType::LEFT_CURLY, "{" };
    Token token25{ TokenType::NAME, "print" };
    Token token26{ TokenType::NAME, "print" };
    Token token27{ TokenType::SEMICOLON, ";" };
    Token token28{ TokenType::RIGHT_CURLY, "}" };
    Token token29{ TokenType::RIGHT_CURLY, "}" };
    Token token30{ TokenType::RIGHT_CURLY, "}" };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with while nested in ifs") {
    source_prog = "procedure procedure {if (x == 2) then { while (s == 0) { read s;} }else{ while (s == 0) { read s; } }}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure" };
    Token token2{ TokenType::NAME, "procedure" };
    Token token3{ TokenType::LEFT_CURLY, "{" };
    Token token4{ TokenType::NAME, "if" };
    Token token5{ TokenType::LEFT_PAREN, "(" };
    Token token6{ TokenType::LETTER, "x" };
    Token token7{ TokenType::OPERATOR, "==" };
    Token token8{ TokenType::DIGIT, "2" };
    Token token9{ TokenType::RIGHT_PAREN, ")" };
    Token token10{ TokenType::NAME, "then" };
    Token token11{ TokenType::LEFT_CURLY, "{" };
    Token token12{ TokenType::NAME, "while" };
    Token token13{ TokenType::LEFT_PAREN, "(" };
    Token token14{ TokenType::LETTER, "s" };
    Token token15{ TokenType::OPERATOR, "==" };
    Token token16{ TokenType::DIGIT, "0" };
    Token token17{ TokenType::RIGHT_PAREN, ")" };
    Token token18{ TokenType::LEFT_CURLY, "{" };
    Token token19{ TokenType::NAME, "read" };
    Token token20{ TokenType::LETTER, "s" };
    Token token21{ TokenType::SEMICOLON, ";" };
    Token token22{ TokenType::RIGHT_CURLY, "}" };
    Token token23{ TokenType::RIGHT_CURLY, "}" };
    Token token24{ TokenType::NAME, "else" };
    Token token25{ TokenType::LEFT_CURLY, "{" };
    Token token26{ TokenType::NAME, "while" };
    Token token27{ TokenType::LEFT_PAREN, "(" };
    Token token28{ TokenType::LETTER, "s" };
    Token token29{ TokenType::OPERATOR, "==" };
    Token token30{ TokenType::DIGIT, "0" };
    Token token31{ TokenType::RIGHT_PAREN, ")" };
    Token token32{ TokenType::LEFT_CURLY, "{" };
    Token token33{ TokenType::NAME, "read" };
    Token token34{ TokenType::LETTER, "s" };
    Token token35{ TokenType::SEMICOLON, ";" };
    Token token36{ TokenType::RIGHT_CURLY, "}" };
    Token token37{ TokenType::RIGHT_CURLY, "}" };
    Token token38{ TokenType::RIGHT_CURLY, "}" };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30,
                           token31, token32, token33, token34, token35, token36, token37, token38 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with two-symbol operator in cond expr and rel expr") {
    source_prog = "x==0; x >=0; x<= 0; x != 0; x && y; x || y; !(x == 0);";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::LETTER, "x" };
    Token token2{ TokenType::OPERATOR, "==" };
    Token token3{ TokenType::DIGIT, "0" };
    Token token4{ TokenType::SEMICOLON, ";" };
    Token token5{ TokenType::LETTER, "x" };
    Token token6{ TokenType::OPERATOR, ">=" };
    Token token7{ TokenType::DIGIT, "0" };
    Token token8{ TokenType::SEMICOLON, ";" };
    Token token9{ TokenType::LETTER, "x" };
    Token token10{ TokenType::OPERATOR, "<=" };
    Token token11{ TokenType::DIGIT, "0" };
    Token token12{ TokenType::SEMICOLON, ";" };
    Token token13{ TokenType::LETTER, "x" };
    Token token14{ TokenType::OPERATOR, "!=" };
    Token token15{ TokenType::DIGIT, "0" };
    Token token16{ TokenType::SEMICOLON, ";" };
    Token token17{ TokenType::LETTER, "x" };
    Token token18{ TokenType::OPERATOR, "&&" };
    Token token19{ TokenType::LETTER, "y" };
    Token token20{ TokenType::SEMICOLON, ";" };
    Token token21{ TokenType::LETTER, "x" };
    Token token22{ TokenType::OPERATOR, "||" };
    Token token23{ TokenType::LETTER, "y" };
    Token token24{ TokenType::SEMICOLON, ";" };
    Token token25{ TokenType::OPERATOR, "!" };
    Token token26{ TokenType::LEFT_PAREN, "(" };
    Token token27{ TokenType::LETTER, "x" };
    Token token28{ TokenType::OPERATOR, "==" };
    Token token29{ TokenType::DIGIT, "0" };
    Token token30{ TokenType::RIGHT_PAREN, ")" };
    Token token31{ TokenType::SEMICOLON, ";" };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24, token25, token26, token27, token28, token29, token30, token31 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with two-symbol operator in while stmt") {
    source_prog = "procedure procedure {while !(x >= 0) {x = y;}}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType:: NAME, "procedure" };
    Token token2{ TokenType::NAME, "procedure" };
    Token token3{ TokenType::LEFT_CURLY, "{" };
    Token token4{ TokenType::NAME, "while" };
    Token token5{ TokenType::OPERATOR, "!" };
    Token token6{ TokenType::LEFT_PAREN, "(" };
    Token token7{ TokenType::LETTER, "x" };
    Token token8{ TokenType::OPERATOR, ">=" };
    Token token9{ TokenType::DIGIT, "0" };
    Token token10{ TokenType::RIGHT_PAREN, ")" };
    Token token11{ TokenType::LEFT_CURLY, "{" };
    Token token12{ TokenType::LETTER, "x" };
    Token token13{ TokenType::OPERATOR, "=" };
    Token token14{ TokenType::LETTER, "y" };
    Token token15{ TokenType::SEMICOLON, ";" };
    Token token16{ TokenType::RIGHT_CURLY, "}" };
    Token token17{ TokenType::RIGHT_CURLY, "}" };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17 };

    REQUIRE(expected_tokens == actual_tokens);
  }

  SECTION("Procedure with else var") {
    source_prog = "procedure procedure { if (else = if) then { then = 1; } else { else = 3; }}";
    actual_tokens = tokenizer.parse(source_prog);

    Token token1{ TokenType::NAME, "procedure" };
    Token token2{ TokenType::NAME, "procedure" };
    Token token3{ TokenType::LEFT_CURLY, "{" };
    Token token4{ TokenType::NAME, "if" };
    Token token5{ TokenType::LEFT_PAREN, "(" };
    Token token6{ TokenType::NAME, "else" };
    Token token7{ TokenType::OPERATOR, "=" };
    Token token8{ TokenType::NAME, "if" };
    Token token9{ TokenType::RIGHT_PAREN, ")" };
    Token token10{ TokenType::NAME, "then" };
    Token token11{ TokenType::LEFT_CURLY, "{" };
    Token token12{ TokenType::NAME, "then" };
    Token token13{ TokenType::OPERATOR, "=" };
    Token token14{ TokenType::DIGIT, "1" };
    Token token15{ TokenType::SEMICOLON, ";" };
    Token token16{ TokenType::RIGHT_CURLY, "}" };
    Token token17{ TokenType::NAME, "else" };
    Token token18{ TokenType::LEFT_CURLY, "{" };
    Token token19{ TokenType::NAME, "else" };
    Token token20{ TokenType::OPERATOR, "=" };
    Token token21{ TokenType::DIGIT, "3" };
    Token token22{ TokenType::SEMICOLON, ";" };
    Token token23{ TokenType::RIGHT_CURLY, "}" };
    Token token24{ TokenType::RIGHT_CURLY, "}" };
    expected_tokens = { token1, token2, token3, token4, token5, token6, token7, token8, token9, token10,
                         token11, token12, token13, token14, token15, token16, token17, token18, token19, token20,
                          token21, token22, token23, token24 };

    REQUIRE(expected_tokens == actual_tokens);
  }
}

TEST_CASE("Invalid tokenizer output") {
  Tokenizer tokenizer;
  string source_prog;
  vector<Token> actual_tokens;

  SECTION("Invalid name of digits followed by letters") {
    source_prog = "procedure procName { print 123myInt; }";
    try {
      actual_tokens = tokenizer.parse(source_prog);
      REQUIRE(0 == 1);
    } catch (InvalidSyntaxException) {
      REQUIRE(1 == 1);
    }
  }
}
