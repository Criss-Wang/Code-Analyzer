//
// Created by Tan Xi Zhe on 28/1/22.
//

#include "Parser.h"

namespace PQL {

    char ParserState::peek() {
        return (char) ss.peek();
    }

    char ParserState::next() {
        return (char) ss.get();
    }

    bool ParserState::isEOF() {
        return ss.eof();
    }

    void ParserState::eatWhiteSpaces() {
        while (ss.peek() == ' ') {
            ss.get();
        }
    }

    void ParserState::consume() {
        ss.get();
    }

    char ParserState::expectLetter() {
        if (PQL::isLetter(peek())) {
            return next();
        } else {
            throw ParseException();
        }
    }

    void ParserState::expect(const std::string& s) {
        std::stringstream ssm;
        ssm << s;
        while (!ssm.eof()) {
            if (ss.get() != ssm.get()) {
                try {
                    throw ParseException();
                } catch (ParseException& e) {
                    std::cout << PQL::ParseException::getErrorMessage("Expecting '" + s + "' keyword!") << std::endl;
                }
            }
        }
    }

    void ParserState::expectEOF() {
        if (!ParserState::isEOF()) {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << PQL::ParseException::getErrorMessage("Expecting end of file!") << std::endl;
            }
        }
    }

    synonym ParserState::parseSynonym() {
        PQL::synonym sm;
        std::stringstream ssm;
        ParserState::eatWhiteSpaces();
        try {
            ssm << ParserState::expectLetter();
        } catch (ParseException& e) {
            std::cout << PQL::ParseException::getErrorMessage("A synonym must start with a letter!") << std::endl;
        }
        for (char nextChar = ParserState::peek(); PQL::isLetter(nextChar) or PQL::isDigit(nextChar); nextChar = ParserState::peek()) {
            ssm << ParserState::next();
        }
        ssm >> sm;
        ParserState::eatWhiteSpaces();
        return sm;
    }

}