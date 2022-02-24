#pragma once

#include "parser.h"

namespace pql {
  char ParserState::Peek() {
    return (char) ss.peek();
  }

  char ParserState::Next() {
    return (char) ss.get();
  }

  bool ParserState::IsEOF() {
    return (ss.peek() == -1);
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

  void ParserState::Expect(const std::string &s) {
    std::stringstream ssm;
    ssm << s;
    while (ssm.peek() != -1) {
      if (ss.get() != ssm.get()) {
        throw ParseException();
      }
    }
  }

  void ParserState::ExpectEOF() {
    if (!ParserState::IsEOF()) {
      throw ParseException();
    }
  }

  std::string ParserState::ParseSynonym() {
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

  pql::Ref ParserState::ParseRef(Query &q) {
    pql::Ref ref;
    std::stringstream ssm;
    bool is_synonym = false;
    ParserState::EatWhiteSpaces();
    if (ParserState::Peek() == '_') {
      ssm << ParserState::Next();
    } else if (IsDigit(ParserState::Peek())) {
      ssm << ParserState::Next();
      for (char next_char = ParserState::Peek(); pql::IsDigit(next_char); next_char = ParserState::Peek()) {
        ssm << ParserState::Next();
      }
    } else if (ParserState::Peek() == '\"') {
      ssm << ParserState::Next();
      ParserState::EatWhiteSpaces();
      if (!IsLetter(ParserState::Peek())) {
        throw ParseException();
      } else {
        ssm << ParserState::Next();
        while (ParserState::Peek() != '\"') {
          if (IsLetter(ParserState::Peek()) || IsDigit(ParserState::Peek())) {
            ssm << ParserState::Next();
          } else {
            throw ParseException();
          }
        }
        ssm << ParserState::Next();
      }
    } else {
      ssm << ParserState::ParseSynonym();
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
    std::stringstream s;
    std::string expression;
    ParserState::Expect("\"");
    ParserState::EatWhiteSpaces();
    if (IsDigit(ParserState::Peek())) {
      for (char next_char = ParserState::Peek(); IsDigit(next_char); next_char = ParserState::Peek()) {
        s << ParserState::Next();
      }
    } else if (IsLetter(ParserState::Peek())) {
      for (char next_char = ParserState::Peek(); IsDigit(next_char) || IsLetter(next_char); next_char = ParserState::Peek()) {
        s << ParserState::Next();
      }
    } else {
      throw ParseException();
    }
    ParserState::Expect("\"");
    s >> expression;
    return expression;
  }

}