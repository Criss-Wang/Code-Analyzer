#pragma once

#include "Token.h"

namespace pql {
  class Query {
  private:
    std::vector <pql::Synonym> declarations;
    std::map <std::string, pql::Synonym> synonyms;
    std::vector <pql::Synonym> used_synonyms;
    std::optional <pql::Synonym> result_synonym;
    std::vector <RelationshipToken> such_that_clauses;
    std::optional <pql::PatternToken> pattern;
  public:
    bool SynonymDeclared(const std::string &name);

    bool IsAssignSynonym(const std::string &name);

    void AddSynonym(EntityIdentifier d, const std::string &name);

    void SetResultSynonym(const std::string &name);

    pql::Synonym GetResultSynonym();

    void AddUsedSynonym(const std::string &name);

    std::vector <pql::Synonym> GetAllUsedSynonyms();

    bool IsProcedure(const std::string &name);

    void AddSuchThatClause(RelationshipTypes r, const pql::Ref &left, const pql::Ref &right);

    std::vector <RelationshipToken> GetSuchThatClause();

    void AddPattern(std::string assign_synonym, pql::Ref left, std::string expression, bool exact);

    pql::PatternToken GetPattern();
  };
}