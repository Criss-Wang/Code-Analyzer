//
// Created by Tan Xi Zhe on 3/2/22.
//

#pragma once

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <exception>
#include <optional>

#include "../Utility/Entity.h"

namespace pql {
    typedef std::string Variable;
    typedef std::string Ref;

    class Synonym {
    private:
        std::string name;
        EntityIdentifier declaration;
    public:
        Synonym(std::string name, EntityIdentifier declaration) : name(std::move(name)), declaration(declaration) {};

        std::string GetName();

        EntityIdentifier GetDeclaration();

        bool equal(const Synonym& s);
    };

    struct ParseException : public std::exception {};

    enum RelationshipTypes {
        kFollows,
        kFollowsT,
        kParent,
        kParentT,
        kUsesS,
        kUsesP,
        kModifiesS,
        kModifiesP
    };

    const std::map<std::string, EntityIdentifier> declarationMap {
            {"stmt", EntityIdentifier::kStmt},
            {"read", EntityIdentifier::kRead},
            {"print", EntityIdentifier::kPrint},
            {"call", EntityIdentifier::kCall},
            {"while", EntityIdentifier::kWhile},
            {"if", EntityIdentifier::kIf},
            {"assign", EntityIdentifier::kAssign},
            {"variable", EntityIdentifier::kVariable},
            {"constant", EntityIdentifier::kConstant},
            {"procedure", EntityIdentifier::kProcedure}
    };

    const std::map<std::string, RelationshipTypes> relationshipMap {
            {"Follows", kFollows},
            {"Follows*", kFollowsT},
            {"Parent", kParent},
            {"Parent*", kParentT},
            {"Uses", kUsesS},
            {"UsesP", kUsesP},
            {"Modifies", kModifiesS},
            {"ModifiesP", kModifiesP}
    };

    std::optional<EntityIdentifier> GetDeclarationType(const std::string& keyword) {
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
        std::vector<pql::Synonym> used_synonyms;
        std::optional<pql::Synonym> result_synonym;
        std::vector<RelationshipToken> such_that_clauses;
    public:
        bool SynonymDeclared(const std::string& name);

        void AddSynonym(EntityIdentifier d, const std::string& name);

        void SetResultSynonym(const std::string& name);

        pql::Synonym GetResultSynonym();

        void AddUsedSynonym(const std::string& name);

        std::vector<pql::Synonym> GetAllUsedSynonyms();

        bool IsProcedure(const std::string& name);

        void AddSuchThatClause(RelationshipTypes r, const pql::Ref& left, const pql::Ref& right);

        std::vector<RelationshipToken> GetSuchThatClause();
    };
}