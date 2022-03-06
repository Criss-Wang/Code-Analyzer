#pragma once

#include <iostream>
#include <sstream>
#include <utility>

#include "token.h"
#include "query.h"

namespace pql {

  bool IsLetter(char c);

  bool IsDigit(char c);

  class ParserState {
    private:
      std::stringstream ss;
    public:
      explicit ParserState(const std::string& input) {
        ss << input;
      }

      char Peek();

      char Next();

      bool IsEOF();

      void EatWhiteSpaces();

      void Consume();

      char ExpectLetter();

      char ExpectChar(const char& c);

      void Expect(const std::string& s);

      std::string ParseName();

      std::string ParseInteger();

      pql::Ref ParseRef(Query& q);

      std::string ParseExpression();
  };

  class Parser {
    private:
      ParserState ps;
      Query query;
    public:
      /*Constructor for Parser*/
      explicit Parser(const std::string& input) : ps(input), query(Query()) {};

      void Parse();

      pql::Query GetQuery();

      std::vector<std::string> GetSynonyms();

      void ParseRelationship();

      void ParsePattern();
  };

}