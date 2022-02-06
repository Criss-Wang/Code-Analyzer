//
// Created by Tan Xi Zhe on 3/2/22.
//

#include <iostream>
#include "Token.h"

namespace PQL {

    variable RelationshipToken::getLeft() {
        return RelationshipToken::left;
    }

    variable RelationshipToken::getRight() {
        return RelationshipToken::right;
    }

    void Query::addSynonym(DeclarationTypes d, const PQL::synonym& s) {
        Query::declarations.at(d).push_back(s);
        if (d == DeclarationTypes::PROCEDURE) {
            Query::procedures.push_back(s);
        } else {
            Query::statements.push_back(s);
        }
    }

    void Query::setResultSynonym(const PQL::synonym& s) {
        if (Query::isProcedure(s) or Query::isStatement(s)) {
            Query::resultSynonym = s;
        } else {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << ParseException::getErrorMessage("The select synonym must be declared first!") << std::endl;
            }
        }
    }

    synonym Query::getResultSynonym() {
        return Query::resultSynonym;
    }

    bool Query::isStatement(const PQL::synonym& s) {
        return (std::find(Query::statements.begin(), Query::statements.end(), s) != Query::statements.end());
    }

    bool Query::isProcedure(const PQL::synonym& s) {
        return (std::find(Query::procedures.begin(), Query::procedures.end(), s) != Query::procedures.end());
    }

    void Query::addRelationship(RelationshipTypes r, const synonym& left, const synonym& right) {
        switch (r) {
            case FOLLOWS:
                Query::suchThatClauses.push_back(PQL::Follows_Token(left, right));
            case FOLLOWS_T:
                Query::suchThatClauses.push_back(PQL::Follows_T_Token(left, right));
            case PARENT:
                Query::suchThatClauses.push_back(PQL::Parent_T_Token(left, right));
            case PARENT_T:
                Query::suchThatClauses.push_back(PQL::Parent_T_Token(left, right));
            case USES_S:
                Query::suchThatClauses.push_back(PQL::Uses_S_Token(left, right));
            case USES_P:
                Query::suchThatClauses.push_back(PQL::Uses_P_Token(left, right));
            case MODIFIES_S:
                Query::suchThatClauses.push_back(PQL::Modifies_S_Token(left, right));
            case MODIFIES_P:
                Query::suchThatClauses.push_back(PQL::Modifies_P_Token(left, right));
        }
    }
}