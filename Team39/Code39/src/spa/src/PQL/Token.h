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
    typedef std::string Synonym;

    struct ParseException : public std::exception {
        static std::string GetErrorMessage (const std::string& msg) {
            return msg;
        }
    };

    enum DeclarationTypes {
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
    };

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

    const std::map<std::string, DeclarationTypes> declarationMap {
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

    std::optional<DeclarationTypes> GetDeclarationType(const std::string& keyword) {
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
        const pql::Variable left;
        const pql::Variable right;
    public:
        RelationshipToken(pql::Variable left, pql::Variable right) : left(std::move(left)), right(std::move(right)) {};

        pql::Variable GetLeft();

        pql::Variable GetRight();
    };

    class Follows_Token: public RelationshipToken {
    public:
        Follows_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Follows_T_Token: public RelationshipToken {
    public:
        Follows_T_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Parent_Token: public RelationshipToken {
    public:
        Parent_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Parent_T_Token: public RelationshipToken {
    public:
        Parent_T_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Uses_S_Token: public RelationshipToken {
    public:
        Uses_S_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Uses_P_Token: public RelationshipToken {
    public:
        Uses_P_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Modifies_S_Token: public RelationshipToken {
    public:
        Modifies_S_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Modifies_P_Token: public RelationshipToken {
    public:
        Modifies_P_Token(pql::Variable left, pql::Variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Query {
    private:
        std::map<DeclarationTypes, std::vector<pql::Synonym>> declarations;
        std::vector<pql::Synonym> statements;
        std::vector<pql::Synonym> procedures;
        pql::Synonym result_synonym;
        std::vector<RelationshipToken> such_that_clauses;
    public:
        void AddSynonym(DeclarationTypes d, const pql::Synonym& s);

        void SetResultSynonym(const pql::Synonym& s);

        pql::Synonym GetResultSynonym();

        bool IsStatement(const pql::Synonym& s);

        bool IsProcedure(const pql::Synonym& s);

        void AddRelationship(RelationshipTypes r, const pql::Synonym& left, const pql::Synonym& right);
    };
}