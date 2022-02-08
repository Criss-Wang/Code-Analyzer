//
// Created by Tan Xi Zhe on 28/1/22.
//

#pragma once
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
        while (ss.peek() == ' ' or ss.peek() == '\n') {
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
                    std::cout << "Expecting '" + s + "' keyword!" << std::endl;
                }
            }
        }
    }

    void ParserState::ExpectEOF() {
        if (!ParserState::IsEOF()) {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << "Expecting end of file!" << std::endl;
            }
        }
    }

    std::string ParserState::ParseSynonym() {
        std::string sm;
        std::stringstream ssm;
        ParserState::EatWhiteSpaces();
        try {
            ssm << ParserState::ExpectLetter();
        } catch (ParseException& e) {
            std::cout << "A synonym must start with a letter!" << std::endl;
        }
        for (char nextChar = ParserState::Peek(); pql::IsLetter(nextChar) or pql::IsDigit(nextChar); nextChar = ParserState::Peek()) {
            ssm << ParserState::Next();
        }
        ssm >> sm;
        ParserState::EatWhiteSpaces();
        return sm;
    }

    pql::Ref ParserState::ParseRef(Query& q) {
        pql::Ref ref;
        std::stringstream ssm;
        bool is_synonym = false;
        ParserState::EatWhiteSpaces();
        if (ParserState::Peek() == '_') {
            ssm <<ParserState::Next();
        } else if (IsDigit(ParserState::Peek())) {
            ssm << ParserState::Next();
            for (char nextChar = ParserState::Peek(); pql::IsDigit(nextChar); nextChar = ParserState::Peek()) {
                ssm << ParserState::Next();
            }
        } else {
            ssm << ParserState::ParseSynonym();
            is_synonym = true;
        }
        ssm >> ref;
        if (is_synonym) {
            if (!q.SynonymDeclared(ref)) {
                try {
                    throw ParseException();
                } catch (ParseException& e) {
                    std::cout << "The synonym used in a relationship must be declared" << std::endl;
                }
            }
            q.AddUsedSynonym(ref);
        }
        return ref;
    }

}