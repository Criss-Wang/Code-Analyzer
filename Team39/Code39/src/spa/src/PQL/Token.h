//
// Created by Tan Xi Zhe on 3/2/22.
//

#pragma once

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <exception>

namespace pql {
    typedef std::string Variable;
    typedef std::string Ref;
    typedef enum {
        STMT,
        READ,
        PRINT,
        CALL,
        WHILE,
        IF,
        ASSIGN,
        VARIABLE,
        CONSTANT,
        PROCEDURE
    } DeclarationType;

    class Synonym {
    private:
        std::string name;
        DeclarationType declaration;
    public:
        Synonym(std::string name, DeclarationType declaration) : name(std::move(name)), declaration(declaration) {};

        std::string GetName();

        DeclarationType GetDeclaration();

        bool equal(const Synonym& s);
    };

    struct ParseException : public std::exception {};

    enum RelationshipTypes {
        FOLLOWS,
        FOLLOWS_T,
        PARENT,
        PARENT_T,
        USES_S,
        USES_P,
        MODIFIES_S,
        MODIFIES_P
    };

    const std::map<std::string, DeclarationType> declarationMap {
            {"stmt", STMT},
            {"read", READ},
            {"print", PRINT},
            {"call", CALL},
            {"while", WHILE},
            {"if", IF},
            {"assign", ASSIGN},
            {"variable", VARIABLE},
            {"constant", CONSTANT},
            {"procedure", PROCEDURE}
    };

    const std::map<std::string, RelationshipTypes> relationshipMap {
            {"Follows", FOLLOWS},
            {"Follows*", FOLLOWS_T},
            {"Parent", PARENT},
            {"Parent*", PARENT_T},
            {"Uses", USES_S},
            {"UsesP", USES_P},
            {"Modifies", MODIFIES_S},
            {"ModifiesP", MODIFIES_P}
    };

    std::optional<DeclarationType> GetDeclarationType(const std::string& keyword) {
        if (declarationMap.find(keyword) != declarationMap.end()) {
            return declarationMap.at(keyword);
        } else {
            return std::nullopt;
        }
    }

    std::optional<RelationshipTypes> GetRelationshipType(const std::string& relationship) {
        if (relationshipMap.find(relationship) != relationshipMap.end()) {
            return relationshipMap.at(relationship);
        } else {
            return std::nullopt;
        }
    }

    class Token {
    public:
        Token() = default;
    };

    class RelationshipToken : public Token {
    private:
        enum RelationshipTypes relationship;
        const pql::Ref left;
        const pql::Ref right;
    public:
        RelationshipToken(pql::RelationshipTypes relationship, pql::Ref left, pql::Ref right) :
                relationship(relationship), left(std::move(left)), right(std::move(right)) {};

        pql::Ref GetLeft();

        pql::Ref GetRight();

        pql::RelationshipTypes GetRelationship();
    };

    class Query {
    private:
        std::vector<pql::Synonym> declarations;
        std::map<std::string, pql::Synonym> synonyms;
        std::vector<pql::Synonym> statements;
        std::vector<pql::Synonym> procedures;
        std::optional<pql::Synonym> result_synonym;
        std::vector<RelationshipToken> such_that_clauses;
    public:
        bool SynonymDeclared(const std::string& name);

        void AddSynonym(DeclarationType d, const std::string& name);

        void SetResultSynonym(const std::string& name);

        pql::Synonym GetResultSynonym();

        bool IsProcedure(const std::string& name);

        void AddSuchThatClause(RelationshipTypes r, const pql::Ref& left, const pql::Ref& right);

        std::vector<RelationshipToken> GetSuchThatClauses();
    };
}