//
// Created by Tan Xi Zhe on 27/1/22.
//

#pragma once
#include <iostream>
#include <stdexcept>
#include <string>

#include "Parser.h"

namespace pql {

    bool IsLetter(char c) {
      return (c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z');
    }

    bool IsDigit(char c) {
      return c >= 48 and c <= 57;
    }

    std::vector<std::string> Parser::GetSynonyms() {
        std::vector<std::string> synonyms;
        while (ps.Peek() != ';') {
            ps.EatWhiteSpaces();
            synonyms.push_back(ps.ParseSynonym());
            ps.EatWhiteSpaces();
            if (ps.Peek() == ',') {
                ps.Consume();
            }
        }
        ps.Consume();
        return synonyms;
    }

    void Parser::Parse() {
        while (!ps.IsEOF()) {
            ps.EatWhiteSpaces();
            std::stringstream ks;
            while (IsLetter(ps.Peek())) {
                ks << ps.Next();
            }
            std::string keyword;
            ks >> keyword;
            if (auto d = pql::GetDeclarationType(keyword)) {
                for (const std::string& s : Parser::GetSynonyms()) {
                    Parser::query.AddSynonym(*d, s);
                }
            } else if (keyword == "Select") {
                ps.EatWhiteSpaces();
                Parser::query.SetResultSynonym(ps.ParseSynonym());
                ps.EatWhiteSpaces();
                if (!ps.IsEOF()) {
                    ps.Expect("such that");
                    ps.EatWhiteSpaces();
                    Parser::ParseRelationship(Parser::query);
                    ps.ExpectEOF();
                }
            }
        }
    }

    pql::Query Parser::getQuery() {
        return Parser::query;
    }

    void Parser::ParseRelationship(Query& q) {
        std::string relationship;
        std::stringstream ssm;
        while (IsLetter(ps.Peek()) or ps.Peek() == '*') {
            ssm << ps.Next();
        }
        ssm >> relationship;
        ps.EatWhiteSpaces();
        ps.Expect("(");
        pql::Ref left = ps.ParseRef(q);
        ps.Expect(",");
        pql::Ref right = ps.ParseRef(q);
        ps.Expect(")");
        if (relationship == "Uses" or relationship == "Modifies") {
            if (left == "_") {
                try {
                    throw ParseException();
                } catch (ParseException& e) {
                    std::cout << "The first argument of Uses or Modifies Relationship cannot be a wildcard!" << std::endl;
                }
            }
            if (q.IsProcedure(left)) {
                relationship.push_back('P');
            }
        }
        if (auto r = pql::GetRelationshipType(relationship)) {
            q.AddSuchThatClause(*r, left, right);
        }
    }
}