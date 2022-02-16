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

    bool equal(const Synonym &s);
  };

  struct ParseException : public std::exception {
  };

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
    {"stmt",      EntityIdentifier::kStmt},
    {"read",      EntityIdentifier::kRead},
    {"print",     EntityIdentifier::kPrint},
    {"call",      EntityIdentifier::kCall},
    {"while",     EntityIdentifier::kWhile},
    {"if",        EntityIdentifier::kIf},
    {"assign",    EntityIdentifier::kAssign},
    {"variable",  EntityIdentifier::kVariable},
    {"constant",  EntityIdentifier::kConstant},
    {"procedure", EntityIdentifier::kProcedure}
  };

  const std::map<std::string, RelationshipTypes> relationshipMap {
      {"Follows",   kFollows},
      {"Follows*",  kFollowsT},
      {"Parent",    kParent},
      {"Parent*",   kParentT},
      {"Uses",      kUsesS},
      {"UsesP",     kUsesP},
      {"Modifies",  kModifiesS},
      {"ModifiesP", kModifiesP}
  };

  std::optional<EntityIdentifier> GetDeclarationType(const std::string &keyword);

  std::optional<RelationshipTypes> GetRelationshipType(const std::string &relationship);

  class RelationshipToken {
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

  class PatternToken {
  private:
    const std::string assign_synonym;
    const pql::Ref left;
    const std::string expression;
    bool exact;
  public:
    PatternToken(std::string assign_synonym, pql::Ref left, std::string expression, bool exact) :
        assign_synonym(std::move(assign_synonym)), left(std::move(left)), expression(std::move(expression)), exact(exact) {};

    std::string GetAssignSynonym();

    pql::Ref GetLeft();

    std::string GetExpression();

    bool IsExact();
  };
}