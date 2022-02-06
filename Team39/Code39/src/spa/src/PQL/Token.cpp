//
// Created by Tan Xi Zhe on 3/2/22.
//

#include <iostream>

#include "Token.h"

namespace pql {

    std::string Synonym::GetName() {
        return Synonym::name;
    }

    DeclarationType Synonym::GetDeclaration() {
        return Synonym::declaration;
    }

    bool Synonym::equal(const Synonym& s) {
        return Synonym::name == s.name and Synonym::declaration == s.declaration;
    }

    pql::Variable RelationshipToken::GetLeft() {
        return RelationshipToken::left;
    }

    pql::Variable RelationshipToken::GetRight() {
        return RelationshipToken::right;
    }

    pql::RelationshipTypes RelationshipToken::GetRelationship() {
        return RelationshipToken::relationship;
    }

    bool Query::SynonymDeclared(const std::string& name) {
        return synonyms.find(name) != synonyms.end();
    }

    void Query::AddSynonym(DeclarationType d, const std::string& name) {
        if (Query::SynonymDeclared(name)) {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << "The declaring synonym must not be declared before!" << std::endl;
            }
        } else {
            pql::Synonym sm = Synonym(name, d);
            Query::declarations.push_back(sm);
            Query::synonyms[name] = sm;
        }
    }

    void Query::SetResultSynonym(const std::string& name) {
        if (Query::SynonymDeclared(name)) {
            Query::result_synonym = Query::synonyms[name];
            Query::AddUsedSynonym(name);
        } else {
            try {
                throw ParseException();
            } catch (ParseException& e) {
                std::cout << "The select synonym must be declared first!" << std::endl;
            }
        }
    }

    pql::Synonym Query::GetResultSynonym() {
        return *Query::result_synonym;
    }

    bool Query::IsProcedure(const std::string& name) {
        return Query::synonyms[name].GetDeclaration() == PROCEDURE;
    }

    void Query::AddUsedSynonym(const std::string& name) {
        Query::used_synonyms.push_back(Query::synonyms[name]);
    }

    std::vector<pql::Synonym> Query::GetAllUsedSynonyms() {
        return Query::used_synonyms;
    }

    void Query::AddSuchThatClause(RelationshipTypes r, const pql::Ref& left, const pql::Ref& right) {
        Query::such_that_clauses.emplace_back(r, left, right);
    }

    std::vector<RelationshipToken> Query::GetSuchThatClauses() {
        return Query::such_that_clauses;
    }
}