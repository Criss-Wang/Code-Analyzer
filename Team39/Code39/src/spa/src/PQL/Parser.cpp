//
// Created by Tan Xi Zhe on 27/1/22.
//

#include <iostream>
#include <stdexcept>
#include <string>

#include "Parser.h"

namespace pql {

    std::vector<pql::Synonym> Parser::GetSynonyms() {
        std::vector<pql::Synonym> synonyms;
        while (ps.Peek() != ';') {
            ps.EatWhiteSpaces();
            synonyms.push_back(ps.ParseSynonym());
            ps.EatWhiteSpaces();
            if (ps.Peek() == ',') {
                ps.Consume();
            } else {
                try {
                    throw ParseException();
                } catch (ParseException& e) {
                    std::cout << pql::ParseException::GetErrorMessage("A synonym must contain only letters or digits!") << std::endl;
                }
            }
        }
        return synonyms;
    }

    void Parser::Parse() {
        Query result {};
        while (!ps.IsEOF()) {
            ps.EatWhiteSpaces();
            std::stringstream ks;
            while (IsLetter(ps.Peek())) {
                ks << ps.Next();
            }
            std::string keyword;
            ks >> keyword;
            if (auto d = pql::GetDeclarationType(keyword)) {
                for (const pql::Synonym& s : Parser::GetSynonyms()) {
                    result.AddSynonym(*d, s);
                }
            } else if (keyword == "Select") {
                ps.EatWhiteSpaces();
                result.SetResultSynonym(ps.ParseSynonym());
                ps.EatWhiteSpaces();
                if (!ps.IsEOF()) {
                    ps.Expect("such that");
                    ps.EatWhiteSpaces();
                    Parser::ParseRelationship(result);
                    ps.ExpectEOF();
                }
            }
        }
    }

    void Parser::ParseRelationship(Query& q) {
        std::string relationship;
        std::stringstream ssm;
        while (IsLetter(ps.Peek())) {
            ssm << ps.Next();
        }
        ssm >> relationship;
        ps.EatWhiteSpaces();
        ps.Expect("(");
        pql::Synonym left = ps.ParseSynonym();
        ps.Expect(",");
        pql::Synonym right = ps.ParseSynonym();
        ps.Expect(")");
        if (relationship == "Uses" or relationship == "Modifies") {
            if (q.IsProcedure(left)) {
                relationship.push_back('P');
            }
        }
        if (auto r = pql::GetRelationshipType(relationship)) {
            q.AddRelationship(*r, left, right);
        }
    }
}