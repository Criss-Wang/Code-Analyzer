#include "query.h"
#include <iostream>

namespace pql {
  bool Query::SynonymDeclared(const std::string &name) {
    return synonyms.find(name) != synonyms.end();
  }

  bool Query::IsAssignSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms.at(name).GetDeclaration() == EntityIdentifier::kAssign;
  }

  void Query::AddSynonym(EntityIdentifier d, const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      try {
        throw ParseException();
      } catch (ParseException &e) {
        std::cout << "The declaring synonym must not be declared before!" << std::endl;
      }
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
      try {
        throw ParseException();
      } catch (ParseException &e) {
        std::cout << "The select synonym must be declared first!" << std::endl;
      }
    }
  }

  pql::Synonym Query::GetResultSynonym() {
    return *Query::result_synonym;
  }

  bool Query::IsProcedure(const std::string &name) {
    return Query::synonyms.at(name).GetDeclaration() == EntityIdentifier::kProcedure;
  }

  void Query::AddUsedSynonym(const std::string &name) {
    Query::used_synonyms.push_back(Query::synonyms.at(name));
  }

  std::vector<pql::Synonym> Query::GetAllUsedSynonyms() {
    return Query::used_synonyms;
  }

  void Query::AddSuchThatClause(RelationshipTypes r, const pql::Ref &left, const pql::Ref &right) {
    Query::such_that_clauses.emplace_back(r, left, right);
  }

  std::vector<RelationshipToken> Query::GetSuchThatClause() {
    return Query::such_that_clauses;
  }

  void Query::AddPattern(std::string assign_synonym, pql::Ref left, std::string expression, bool exact) {
    Query::pattern.emplace(PatternToken(std::move(assign_synonym), std::move(left), std::move(expression), exact));
  }

  pql::PatternToken Query::GetPattern() {
    return *Query::pattern;
  }
}