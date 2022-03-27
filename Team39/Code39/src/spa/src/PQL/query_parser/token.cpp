#include <utility>

#include "token.h"

namespace pql {

  std::string Synonym::GetName() {
    return Synonym::name;
  }

  EntityIdentifier Synonym::GetDeclaration() {
    return Synonym::declaration;
  }

  bool Synonym::equal(const Synonym &s) {
    return Synonym::name == s.name && Synonym::declaration == s.declaration;
  }

  std::string AttrRef::GetSynName() {
    return synonym_->GetName();
  }

  EntityIdentifier AttrRef::GetSynDeclaration() {
    return synonym_->GetDeclaration();
  }

  AttrIdentifier AttrRef::GetAttrIdentifier() {
    return attribute_;
  }

  std::optional<EntityIdentifier> GetDeclarationType(const std::string &keyword) {
    if (declarationMap.find(keyword) != declarationMap.end()) {
      return declarationMap.at(keyword);
    } else {
      return std::nullopt;
    }
  }

  std::optional<RelationshipTypes> GetRelationshipType(const std::string &relationship) {
    if (relationshipMap.find(relationship) != relationshipMap.end()) {
      return relationshipMap.at(relationship);
    } else {
      return std::nullopt;
    }
  }
}