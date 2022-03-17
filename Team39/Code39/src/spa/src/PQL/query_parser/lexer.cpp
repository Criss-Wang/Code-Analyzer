#pragma once
#define END_OF_FILE -1

#include "parser.h"

namespace pql {
  char ParserState::Peek() {
    return (char) ss.peek();
  }

  char ParserState::Next() {
    return (char) ss.get();
  }

  bool ParserState::IsEOF() {
    return (ss.peek() == END_OF_FILE);
  }

  void ParserState::EatWhiteSpaces() {
    while (ss.peek() == ' ' || ss.peek() == '\n' || ss.peek() == '\r' || ss.peek() == '\t' || ss.peek() == '\0') {
      ss.get();
    }
  }

  void ParserState::Consume() {
    ss.get();
  }

  char ParserState::ExpectLetter() {
    if (pql::IsLetter(ParserState::Peek())) {
      return ParserState::Next();
    } else {
      throw ParseException();
    }
  }

  char ParserState::ExpectChar(const char& c) {
    char expected = ParserState::Next();
    if (expected == c) {
      return expected;
    } else {
      throw ParseException();
    }
  }

  void ParserState::Expect(const std::string& s) {
    std::stringstream ssm;
    ssm << s;
    while (ssm.peek() != END_OF_FILE) {
      if (ss.get() != ssm.get()) {
        throw ParseException();
      }
    }
  }

  std::string ParserState::ParseName() {
    std::string sm;
    std::stringstream ssm;
    ParserState::EatWhiteSpaces();
    ssm << ParserState::ExpectLetter();
    for (char next_char = ParserState::Peek();
         pql::IsLetter(next_char) || pql::IsDigit(next_char);
         next_char = ParserState::Peek()) {
      ssm << ParserState::Next();     
    }
    ssm >> sm;
    ParserState::EatWhiteSpaces();
    return sm;
  }
  
  std::string ParserState::ParseAttribute() {
    std::string sm;
    std::stringstream ssm;

    ParserState::EatWhiteSpaces();
    ssm << ParserState::ExpectLetter();
    for (char next_char = ParserState::Peek();
        IsLetter(next_char) || IsHash(next_char);
        next_char = ParserState::Peek()) {
      ssm << ParserState::Next();
    }
    ssm >> sm;
    ParserState::EatWhiteSpaces();
    return sm;
  }

  std::string ParserState::ParseInteger() {
    std::stringstream ssm;
    std::string integer;
    ParserState::EatWhiteSpaces();
    if (ParserState::Peek() == '0') {
      ssm << ParserState::Next();
    } else {
      ssm << ParserState::Next();
      for (char next_char = ParserState::Peek(); pql::IsDigit(next_char); next_char = ParserState::Peek()) {
        ssm << ParserState::Next();
      }
    }
    ssm >> integer;
    ParserState::EatWhiteSpaces();
    return integer;
  }

  std::string ParserState::ParseRef(Query &q) {
    std::string ref;
    std::stringstream ssm;
    bool is_synonym = false;
    ParserState::EatWhiteSpaces();
    if (ParserState::Peek() == '_') {
      ssm << ParserState::Next();
    } else if (IsDigit(ParserState::Peek())) {
      ssm << ParserState::ParseInteger();
    } else if (ParserState::Peek() == '\"') {
      ssm << ParserState::Next();
      ssm << ParserState::ParseName();
      ssm << ParserState::ExpectChar('\"');
    } else {
      ssm << ParserState::ParseName();
      is_synonym = true;
    }
    ssm >> ref;
    if (is_synonym) {
      if (!q.SynonymDeclared(ref)) {
        throw SemanticallyInvalidException();
      }
      q.AddUsedSynonym(ref);
    }
    return ref;
  }

  std::string ParserState::ParseExpression() {
    std::string expression;
    ParserState::Expect("\"");
    ParserState::EatWhiteSpaces();
    expression = IsValidExpression();
    return expression;
  }

  std::string ParserState::IsValidExpression() {
    std::stringstream s;
    std::string expression;
    int bracket_count = 0;
    bool incomplete_operator = false;
    char next_char = ParserState::Peek();
    char curr_char;

    bool expected_next = IsDigit(next_char) || IsLetter(next_char) || IsOpenBracket(next_char);

    for (next_char = ParserState::Peek(); next_char != '\"' && !ParserState::IsEOF(); next_char = ParserState::Peek()) {
      // Check that char is expected 

      if (IsDigit(next_char) && expected_next) {
        s << ParserState::Next();
        incomplete_operator = false;
      } else if (IsLetter(next_char) && expected_next) {
        s << ParserState::Next();
        incomplete_operator = false;
      } else if (IsOperator(next_char) && expected_next) {
        s << ParserState::Next();
        incomplete_operator = true;
      } else if (IsOpenBracket(next_char) && expected_next) {
        s << ParserState::Next();
        bracket_count++;
      } else if (IsCloseBracket(next_char) && expected_next && bracket_count > 0) {
        s << ParserState::Next();
        bracket_count--;
      } else {
        throw ParseException();
      }

      // Set expected_next bool
      ParserState::EatWhiteSpaces();
      curr_char = next_char;
      next_char = ParserState::Peek();
      if (IsDigit(curr_char)) {
        expected_next = IsDigit(next_char) || IsOperator(next_char) || IsCloseBracket(next_char);
      } else if (IsLetter(curr_char)) {
        expected_next = IsDigit(next_char) || IsLetter(next_char) || IsOperator(next_char) || IsCloseBracket(next_char);
      } else if (IsOperator(curr_char)) {
        expected_next = IsDigit(next_char) || IsLetter(next_char) || IsOpenBracket(next_char);
      } else if (IsOpenBracket(curr_char)) {
        expected_next = IsDigit(next_char) || IsLetter(next_char);
      } else if (IsCloseBracket(curr_char)) {
        expected_next = IsOperator(next_char) || IsCloseBracket(next_char);
      } else {
        throw ParseException();
      }
    }

    if (!incomplete_operator && bracket_count == 0) {
      s >> expression;
      return expression;
    } else {
      throw ParseException();
    }
  }



}