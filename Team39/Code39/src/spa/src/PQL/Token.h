//
// Created by Tan Xi Zhe on 3/2/22.
//

#pragma once
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <exception>

namespace PQL {
    typedef std::string variable;
    typedef std::string synonym;

    struct ParseException : public std::exception {
        static std::string getErrorMessage (const std::string& msg) {
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
            {"procedure", PROCEDURE},
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

    std::optional<DeclarationTypes> getDeclarationType(const std::string& keyword) {
        if (declarationMap.find(keyword) != declarationMap.end()) {
            return declarationMap.at(keyword);
        } else {
            return std::nullopt;
        }
    }

    std::optional<RelationshipTypes> getRelationshipType(const std::string& relationship) {
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
        const PQL::variable left;
        const PQL::variable right;
    public:
        RelationshipToken(PQL::variable left, PQL::variable right) : left(std::move(left)), right(std::move(right)) {};

        PQL::variable getLeft();

        PQL::variable getRight();
    };

    class Follows_Token: public RelationshipToken {
    public:
        Follows_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Follows_T_Token: public RelationshipToken {
    public:
        Follows_T_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Parent_Token: public RelationshipToken {
    public:
        Parent_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Parent_T_Token: public RelationshipToken {
    public:
        Parent_T_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Uses_S_Token: public RelationshipToken {
    public:
        Uses_S_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Uses_P_Token: public RelationshipToken {
    public:
        Uses_P_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Modifies_S_Token: public RelationshipToken {
    public:
        Modifies_S_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Modifies_P_Token: public RelationshipToken {
    public:
        Modifies_P_Token(PQL::variable left, PQL::variable right) : RelationshipToken(std::move(left), std::move(right)) {};
    };

    class Query {
    private:
        std::map<DeclarationTypes, std::vector<synonym>> declarations;
        std::vector<synonym> statements;
        std::vector<synonym> procedures;
        synonym resultSynonym;
        std::vector<RelationshipToken> suchThatClauses;
    public:
        void addSynonym(DeclarationTypes d, const PQL::synonym& s);

        void setResultSynonym(const PQL::synonym& s);

        synonym getResultSynonym();

        bool isStatement(const PQL::synonym& s);

        bool isProcedure(const PQL::synonym& s);

        void addRelationship(RelationshipTypes r, const synonym& left, const synonym& right);
    };
}