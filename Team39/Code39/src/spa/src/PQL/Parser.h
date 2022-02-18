#pragma once

#include <iostream>
#include <sstream>
#include <utility>

#include "Token.h"
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

      void Expect(const std::string& s);

      void ExpectEOF();

      std::string ParseSynonym();

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

      pql::Query getQuery();

      std::vector<std::string> GetSynonyms();

      void ParseRelationship(Query& q);

      void ParsePattern(Query& q);
  };

}