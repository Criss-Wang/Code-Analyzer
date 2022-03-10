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

  pql::Ref RelationshipToken::GetLeft() {
    return RelationshipToken::left;
  }

  pql::Ref RelationshipToken::GetRight() {
    return RelationshipToken::right;
  }

  bool RelationshipToken::IsSynonymLeft() const {
    return RelationshipToken::is_synonym_left;
  }

  bool RelationshipToken::IsSynonymRight() const {
    return RelationshipToken::is_synonym_right;
  }

  pql::RelationshipTypes RelationshipToken::GetRelationship() {
    return RelationshipToken::relationship;
  }

  std::string PatternToken::GetAssignSynonym() {
    return PatternToken::assign_synonym;
  }

  pql::Ref PatternToken::GetLeft() {
    return PatternToken::left;
  }

  std::string PatternToken::GetExpression() {
    return PatternToken::expression;
  }

  bool PatternToken::IsSynonymLeft() const {
    return PatternToken::is_synonym_left;
  }

  bool PatternToken::IsExact() const {
    return PatternToken::exact;
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