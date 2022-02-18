#include "query.h"
#include <algorithm>

namespace pql {
  bool Query::SynonymDeclared(const std::string &name) {
    return synonyms.find(name) != synonyms.end();
  }

  bool Query::IsAssignSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms.at(name).GetDeclaration() == EntityIdentifier::kAssign;
  }

  void Query::AddSynonym(EntityIdentifier d, const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      throw ParseException();
    } else {
      pql::Synonym sm = Synonym(name, d);
      Query::declarations.push_back(sm);
      Query::synonyms.insert(std::pair<std::string, pql::Synonym>(name, sm));
    }
  }

  void Query::SetResultSynonym(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      Query::result_synonym = Query::synonyms.at(name);
      Query::AddUsedSynonym(name);
    } else {
      throw ParseException();
    }
  }

  pql::Synonym Query::GetResultSynonym() {
    return *Query::result_synonym;
  }

  bool Query::IsProcedure(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      return Query::synonyms.at(name).GetDeclaration() == EntityIdentifier::kProcedure;
    }
    return false;
  }

  bool Query::IsStatement(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      return !Query::IsProcedure(name) &&
        Query::synonyms.at(name).GetDeclaration() != EntityIdentifier::kConstant &&
        Query::synonyms.at(name).GetDeclaration() != EntityIdentifier::kVariable;
    }
    return false;
  }

  bool Query::IsVariable(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      return Query::synonyms.at(name).GetDeclaration() == EntityIdentifier::kVariable;
    }
    return false;
  }

  void Query::AddUsedSynonym(const std::string &name) {
    for (pql::Synonym s: Query::used_synonyms) {
      if (s.equal(Query::synonyms.at(name))) {
        return ;
      }
    }
    Query::used_synonyms.push_back(Query::synonyms.at(name));
  }

  std::vector<pql::Synonym> Query::GetAllUsedSynonyms() {
    return Query::used_synonyms;
  }

  void Query::AddSuchThatClause(RelationshipTypes r, const pql::Ref &left, const pql::Ref &right, bool is_synonym_left, bool is_synonym_right) {
    Query::such_that_clauses.emplace_back(r, left, right, is_synonym_left, is_synonym_right);
  }

  std::vector<RelationshipToken> Query::GetSuchThatClause() {
    return Query::such_that_clauses;
  }

  void Query::AddPattern(std::string assign_synonym, pql::Ref left, std::string expression, bool exact, bool is_synonym_left) {
    Query::pattern.emplace(PatternToken(std::move(assign_synonym), std::move(left), std::move(expression), exact, is_synonym_left));
  }

  std::optional<pql::PatternToken> Query::GetPattern() {
    return Query::pattern;
  }
}