//
// Created by Tan Xi Zhe on 28/1/22.
//

#pragma once
#include <iostream>
#include <sstream>
#include <utility>

#include "Token.h"

namespace pql {

    bool IsLetter(char c) {
        return c >= 65 and c <= 90;
    }

    bool IsDigit(char c) {
        return c >= 48 and c <= 57;
    }

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
    };

}