#pragma once

#include <iostream>
#include <sstream>
#include <utility>

#include "token.h"
#include "query.h"

namespace pql {

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

      std::string ParseAttribute();

      std::string ParseInteger();

      std::string ParseRef(Query& q);

      std::string ParseExpression(Query& q);

      std::string IsValidExpression(Query& q);
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

      void ParseAttrRef();

      void ParseSelect();

      void ParseTuple();

      void ParseRelationship();

      void ParsePattern();

      std::tuple<std::string, std::unordered_set<EntityIdentifier>, std::string, bool> ParsePatternSyntax();

      void ParseWith();

      std::tuple<std::shared_ptr<AttrRef>, std::string, int> ParseWithArgument();
  };

}