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
  typedef std::string Ref;

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
      Synonym s;
      AttrIdentifier attribute;
    public:
      AttrRef(Synonym s, AttrIdentifier attribute) : s(std::move(s)), attribute(attribute) {};
  };

  struct ParseException : public std::exception {
    public:
      [[nodiscard]] const char * what() const noexcept override {
        return "The query is syntactically invalid!";
      }
  };

  struct SemanticallyInvalidException : public std::exception {
  public:
    [[nodiscard]] const char * what() const noexcept override {
      return "The query is semantically invalid!";
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
    kCallsT
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

  class RelationshipToken {
    private:
      enum RelationshipTypes relationship;
      const pql::Ref left;
      const pql::Ref right;
      bool is_synonym_left;
      bool is_synonym_right;
    public:
      RelationshipToken(pql::RelationshipTypes relationship, pql::Ref left, pql::Ref right, bool is_synonym_left, bool is_synonym_right) :
      relationship(relationship), left(std::move(left)), right(std::move(right)), is_synonym_left(is_synonym_left), is_synonym_right(is_synonym_right) {}

      pql::Ref GetLeft();

      pql::Ref GetRight();

      [[nodiscard]] bool IsSynonymLeft() const;

      [[nodiscard]] bool IsSynonymRight() const;

      pql::RelationshipTypes GetRelationship();
  };

  class PatternToken {
    private:
      const std::string assign_synonym;
      const pql::Ref left;
      const std::string expression;
      bool is_synonym_left;
      bool exact;
    public:
      PatternToken(std::string assign_synonym, pql::Ref left, std::string expression, bool exact, bool is_synonym_left) :
        assign_synonym(std::move(assign_synonym)), left(std::move(left)), expression(std::move(expression)),
        exact(exact), is_synonym_left(is_synonym_left) {};

      std::string GetAssignSynonym();

      pql::Ref GetLeft();

      std::string GetExpression();

      [[nodiscard]] bool IsSynonymLeft() const;

      [[nodiscard]] bool IsExact() const;
  };
}
