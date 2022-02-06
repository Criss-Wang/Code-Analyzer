//
// Created by Tan Xi Zhe on 3/2/22.
//

#include <iostream>

#include "Token.h"

namespace pql {

    pql::Variable RelationshipToken::GetLeft() {
        return RelationshipToken::left;
    }

    pql::Variable RelationshipToken::GetRight() {
        return RelationshipToken::right;
    }

    void Query::AddSynonym(DeclarationTypes d, const pql::Synonym& s) {
        Query::declarations.at(d).push_back(s);
        if (d == DeclarationTypes::PROCEDURE) {
            Query::procedures.push_back(s);
        } else {
            Query::statements.push_back(s);
        }
    }

    void Query::SetResultSynonym(const pql::Synonym& s) {
        if (Query::IsProcedure(s) or Query::IsStatement(s)) {
            Query::result_synonym = s;
        } else {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << ParseException::GetErrorMessage("The select synonym must be declared first!") << std::endl;
            }
        }
    }

    pql::Synonym Query::GetResultSynonym() {
        return Query::result_synonym;
    }

    bool Query::IsStatement(const pql::Synonym& s) {
        return (std::find(Query::statements.begin(), Query::statements.end(), s) != Query::statements.end());
    }

    bool Query::IsProcedure(const pql::Synonym& s) {
        return (std::find(Query::procedures.begin(), Query::procedures.end(), s) != Query::procedures.end());
    }

    void Query::AddRelationship(RelationshipTypes r, const pql::Synonym& left, const pql::Synonym& right) {
        switch (r) {
            case FOLLOWS:
                Query::such_that_clauses.push_back(pql::Follows_Token(left, right));
            case FOLLOWS_T:
                Query::such_that_clauses.push_back(pql::Follows_T_Token(left, right));
            case PARENT:
                Query::such_that_clauses.push_back(pql::Parent_Token(left, right));
            case PARENT_T:
                Query::such_that_clauses.push_back(pql::Parent_T_Token(left, right));
            case USES_S:
                Query::such_that_clauses.push_back(pql::Uses_S_Token(left, right));
            case USES_P:
                Query::such_that_clauses.push_back(pql::Uses_P_Token(left, right));
            case MODIFIES_S:
                Query::such_that_clauses.push_back(pql::Modifies_S_Token(left, right));
            case MODIFIES_P:
                Query::such_that_clauses.push_back(pql::Modifies_P_Token(left, right));
        }
    }
}