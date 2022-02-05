//
// Created by Tan Xi Zhe on 28/1/22.
//

#pragma once
#include <iostream>
#include <sstream>
#include <utility>

#include "Token.h"

namespace PQL {

    bool isLetter(char c) {
        return c >= 65 and c <= 90;
    }

    bool isDigit(char c) {
        return c >= 48 and c <= 57;
    }

    class ParserState {
    private:
        std::stringstream ss;
    public:
        explicit ParserState(const std::string& input) {
            ss << input;
        }

        char peek();

        char next();

        bool isEOF();

        void eatWhiteSpaces();

        void consume();

        char expectLetter();

        void expect(const std::string& s);

        void expectEOF();

        synonym parseSynonym();
    };

    class Parser {
    private:
        ParserState ps;
        Token token;
    public:
        /*Constructor for Parser*/
        explicit Parser(const std::string& input) : ps(input), token() {};

        void parse();

        std::vector<synonym> getSynonyms();

        void parseRelationship(Query& q);
    };

}