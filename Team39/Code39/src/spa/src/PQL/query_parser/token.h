#pragma once

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <exception>
#include <optional>

#include "Utility/entity.h"
#include "Utility/attr.h"

namespace pql {
  class Synonym {
    private:
      std::string name;
      EntityIdentifier declaration;
    public:
      Synonym(std::string name, EntityIdentifier declaration) : name(std::move(name)), declaration(declaration) {};

      std::string GetName();

      EntityIdentifier GetDeclaration();

      bool equal(const Synonym &s);
  };
  
  class AttrRef {
    private:
      Synonym synonym_;
      AttrIdentifier attribute_;
    public:
      AttrRef(Synonym s, AttrIdentifier attribute) : s(std::move(s)), attribute(attribute) {};

      Synonym GetSynonym();

      AttrIdentifier GetAttrIndentifier();
  };

  struct ParseException : public std::exception {
    public:
      [[nodiscard]] const char * what() const noexcept override {
        return "The query is syntactically invalid!";
      }
  };

  enum RelationshipTypes {
    kFollows,
    kFollowsT,
    kParent,
    kParentT,
    kUsesS,
    kUsesP,
    kModifiesS,
    kModifiesP,
    kCalls,
    kCallsT,
    kAssignPattern,
    kIfPattern,
    kWhilePattern,
    kWith
  };

  const std::map<std::string, EntityIdentifier> declarationMap {
    {"stmt",      EntityIdentifier::kStmt},
    {"read",      EntityIdentifier::kRead},
    {"print",     EntityIdentifier::kPrint},
    {"call",      EntityIdentifier::kCall},
    {"while",     EntityIdentifier::kWhile},
    {"if",        EntityIdentifier::kIf},
    {"assign",    EntityIdentifier::kAssign},
    {"variable",  EntityIdentifier::kVariable},
    {"constant",  EntityIdentifier::kConstant},
    {"procedure", EntityIdentifier::kProc}
  };

  const std::map<std::string, RelationshipTypes> relationshipMap {
      {"Follows",   kFollows},
      {"Follows*",  kFollowsT},
      {"Parent",    kParent},
      {"Parent*",   kParentT},
      {"Uses",      kUsesS},
      {"UsesP",     kUsesP},
      {"Modifies",  kModifiesS},
      {"ModifiesP", kModifiesP},
      {"Calls", kCalls},
      {"Calls*", kCallsT}
  };

  std::optional<EntityIdentifier> GetDeclarationType(const std::string &keyword);

  std::optional<RelationshipTypes> GetRelationshipType(const std::string &relationship);

//  class RelationshipToken {
//    private:
//      enum RelationshipTypes relationship;
//      const std::string left;
//      const std::string right;
//      bool is_synonym_left;
//      bool is_synonym_right;
//    public:
//      RelationshipToken(pql::RelationshipTypes relationship, std::string left, std::string right, bool is_synonym_left, bool is_synonym_right) :
//      left(std::move(left)), right(std::move(right)), relationship(relationship), is_synonym_left(is_synonym_left), is_synonym_right(is_synonym_right) {}
//
//      std::string GetLeft();
//
//      std::string GetRight();
//
//      [[nodiscard]] bool IsSynonymLeft() const;
//
//      [[nodiscard]] bool IsSynonymRight() const;
//
//      pql::RelationshipTypes GetRelationship();
//  };
//
//  class PatternToken {
//    private:
//      enum EntityIdentifier syn_entity;
//      const std::string synonym;
//      const std::string left;
//      const std::string expression;
//      bool is_synonym_left;
//      bool exact;
//    public:
//      PatternToken(EntityIdentifier syn_entity, std::string synonym, std::string left, std::string expression, bool exact, bool is_synonym_left) :
//        syn_entity(syn_entity), left(left), synonym(std::move(synonym)), expression(std::move(expression)),
//        exact(exact), is_synonym_left(is_synonym_left) {};
//
//      std::string GetLeft();
//
//      std::string GetSynonym();
//
//      EntityIdentifier GetSynEntity();
//
//      std::string GetExpression();
//
//      [[nodiscard]] bool IsSynonymLeft() const;
//
//      [[nodiscard]] bool IsExact() const;
//  };
}
