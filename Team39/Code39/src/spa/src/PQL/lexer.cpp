//
// Created by Tan Xi Zhe on 28/1/22.
//

#include "Parser.h"

namespace pql {

    char ParserState::Peek() {
        return (char) ss.peek();
    }

    char ParserState::Next() {
        return (char) ss.get();
    }

    bool ParserState::IsEOF() {
        return ss.eof();
    }

    void ParserState::EatWhiteSpaces() {
        while (ss.peek() == ' ') {
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

    void ParserState::Expect(const std::string& s) {
        std::stringstream ssm;
        ssm << s;
        while (!ssm.eof()) {
            if (ss.get() != ssm.get()) {
                try {
                    throw ParseException();
                } catch (ParseException& e) {
                    std::cout << pql::ParseException::GetErrorMessage("Expecting '" + s + "' keyword!") << std::endl;
                }
            }
        }
    }

    void ParserState::ExpectEOF() {
        if (!ParserState::IsEOF()) {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << pql::ParseException::GetErrorMessage("Expecting end of file!") << std::endl;
            }
        }
    }

    pql::Synonym ParserState::ParseSynonym() {
        pql::Synonym sm;
        std::stringstream ssm;
        ParserState::EatWhiteSpaces();
        try {
            ssm << ParserState::ExpectLetter();
        } catch (ParseException& e) {
            std::cout << pql::ParseException::GetErrorMessage("A synonym must start with a letter!") << std::endl;
        }
        for (char nextChar = ParserState::Peek(); pql::IsLetter(nextChar) or pql::IsDigit(nextChar); nextChar = ParserState::Peek()) {
            ssm << ParserState::Next();
        }
        ssm >> sm;
        ParserState::EatWhiteSpaces();
        return sm;
    }

}