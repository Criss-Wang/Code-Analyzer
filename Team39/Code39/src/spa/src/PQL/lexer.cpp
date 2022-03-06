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

  pql::Ref ParserState::ParseRef(Query &q) {
    pql::Ref ref;
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
        throw ParseException();
      }
      q.AddUsedSynonym(ref);
    }
    return ref;
  }

  std::string ParserState::ParseExpression() {
    std::stringstream ssm;
    std::string expression;
    ParserState::Expect("\"");
    if (IsDigit(ParserState::Peek())) {
      ssm << ParserState::ParseInteger();
    } else if (IsLetter(ParserState::Peek())) {
      ssm << ParserState::ParseName();
    } else {
      throw ParseException();
    }
    ParserState::Expect("\"");
    ssm >> expression;
    return expression;
  }

}