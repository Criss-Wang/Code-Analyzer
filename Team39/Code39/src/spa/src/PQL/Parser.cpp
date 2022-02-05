//
// Created by Tan Xi Zhe on 27/1/22.
//

#include <iostream>
#include <stdexcept>
#include <string>

#include "Parser.h"

namespace PQL {

    std::vector<synonym> Parser::getSynonyms() {
        std::vector<synonym> synonyms;
        while (ps.peek() != ';') {
            ps.eatWhiteSpaces();
            synonyms.push_back(ps.parseSynonym());
            ps.eatWhiteSpaces();
            if (ps.peek() == ',') {
                ps.consume();
            } else {
                try {
                    throw ParseException();
                } catch (ParseException& e) {
                    std::cout << PQL::ParseException::getErrorMessage("A synonym must contain only letters or digits!") << std::endl;
                }
            }
        }
        return synonyms;
    }

    void Parser::parse() {
        Query result {};
        while (!ps.isEOF()) {
            ps.eatWhiteSpaces();
            std::stringstream ks;
            while (isLetter(ps.peek())) {
                ks << ps.next();
            }
            std::string keyword;
            ks >> keyword;
            if (auto d = PQL::getDeclarationType(keyword)) {
                for (const synonym& s : Parser::getSynonyms()) {
                    result.addSynonym(*d, s);
                }
            } else if (keyword == "Select") {
                ps.eatWhiteSpaces();
                result.setResultSynonym(ps.parseSynonym());
                ps.eatWhiteSpaces();
                if (!ps.isEOF()) {
                    ps.expect("such that");
                    ps.eatWhiteSpaces();
                    Parser::parseRelationship(result);
                    ps.expectEOF();
                }
            }
        }
    }

    void Parser::parseRelationship(Query& q) {
        std::string relationship;
        std::stringstream ssm;
        while (isLetter(ps.peek())) {
            ssm << ps.next();
        }
        ssm >> relationship;
        ps.eatWhiteSpaces();
        ps.expect("(");
        synonym left = ps.parseSynonym();
        ps.expect(",");
        synonym right = ps.parseSynonym();
        ps.expect(")");
        if (relationship == "Uses" or relationship == "Modifies") {
            if (q.isProcedure(left)) {
                relationship.push_back('P');
            }
        }
        if (auto r = PQL::getRelationshipType(relationship)) {
            q.addRelationship(*r, left, right);
        }
    }
}