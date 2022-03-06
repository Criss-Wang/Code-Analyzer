#pragma once

#include "token.h"

namespace pql {
  bool IsLetter(char c);

  bool IsDigit(char c);

  bool IsIdent(const std::string& ident);

  class Query {
  private:
    std::vector <pql::Synonym> declarations;
    std::map <std::string, pql::Synonym> synonyms;
    std::vector <pql::Synonym> used_synonyms;
    std::vector <pql::Synonym> result_synonym;
    std::vector <RelationshipToken> such_that_clauses;
    std::vector <pql::PatternToken> patterns;
  public:
    bool IsValid(RelationshipTypes relationship, const pql::Ref& left, const pql::Ref& right);

    bool SynonymDeclared(const std::string &name);

    bool IsAssignSynonym(const std::string &name);

    void AddSynonym(EntityIdentifier d, const std::string &name);

    void AddResultSynonym(const std::string &name);

    std::vector<pql::Synonym> GetResultSynonym();

    void AddUsedSynonym(const std::string &name);

    std::vector <pql::Synonym> GetAllUsedSynonyms();

    bool IsProcedure(const std::string &name);

    void AddSuchThatClause(RelationshipTypes r, pql::Ref &left, pql::Ref &right, bool is_synonym_left, bool is_synonym_right);

    std::vector<RelationshipToken> GetSuchThatClause();

    void AddPattern(std::string assign_synonym, pql::Ref left, std::string expression, bool exact, bool is_synonym_left);

    std::vector<pql::PatternToken> GetPattern();
  };
}