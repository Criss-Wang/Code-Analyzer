#pragma once

#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>
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
      std::shared_ptr<Synonym> synonym_;
      AttrIdentifier attribute_;
    public:
      AttrRef(std::shared_ptr<Synonym> s, AttrIdentifier attribute) : synonym_(s), attribute_(attribute) {};

      AttrRef(Synonym& s, AttrIdentifier attribute) {
        synonym_ = std::make_shared<Synonym>(s);
        attribute_ = attribute;
      }

      AttrIdentifier GetAttrIdentifier();

      std::string GetSynName();

      EntityIdentifier GetSynDeclaration();
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
    kNext,
    kNextT,
    kAffects,
    kAffectsT,
    kAssignPattern,
    kIfPattern,
    kWhilePattern,
    kWith,
  };

  const std::unordered_map<std::string, EntityIdentifier> declarationMap {
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

  const std::unordered_map<std::string, RelationshipTypes> relationshipMap {
      {"Follows",   kFollows},
      {"Follows*",  kFollowsT},
      {"Parent",    kParent},
      {"Parent*",   kParentT},
      {"Uses",      kUsesS},
      {"UsesP",     kUsesP},
      {"Modifies",  kModifiesS},
      {"ModifiesP", kModifiesP},
      {"Calls", kCalls},
      {"Calls*", kCallsT},
      {"Next", kNext},
      {"Next*", kNextT},
      {"Affects", kAffects},
      {"Affects*", kAffectsT}
  };

  std::optional<EntityIdentifier> GetDeclarationType(const std::string &keyword);

  std::optional<RelationshipTypes> GetRelationshipType(const std::string &relationship);

}
