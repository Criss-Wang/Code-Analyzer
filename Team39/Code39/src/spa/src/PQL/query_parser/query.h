#pragma once

#include "token.h"

namespace pql {
  bool IsLetter(char c);

  bool IsDigit(char c);

  bool IsIdent(const std::string& ident);

  bool IsInteger(const std::string& s);

  bool IsOperator(char c);

  bool IsOpenBracket(char c);

  bool IsCloseBracket(char c);

    
  class Query {
  private:
    std::vector <pql::Synonym> declarations;
    std::map <std::string, pql::Synonym> synonyms;
    std::vector <pql::Synonym> used_synonyms;
    std::vector <pql::Synonym> result_synonyms;
    std::vector <RelationshipToken> such_that_clauses;
    std::vector <pql::PatternToken> patterns;
    bool is_boolean = false;
    bool is_semantically_valid = true;
  public:
    void SetSemanticallyInvalid();

    bool IsValid(RelationshipTypes relationship, const std::string& left, const std::string& right);

    bool SynonymDeclared(const std::string &name);

    bool IsAssignSynonym(const std::string &name);

    bool IsWhileSynonym(const std::string &name);

    bool IsIfSynonym(const std::string &name);

    void AddSynonym(EntityIdentifier d, const std::string &name);

    void AddResultSynonym(const std::string &name);

    std::vector<pql::Synonym> GetResultSynonym();

    void AddUsedSynonym(const std::string &name);

    std::vector <pql::Synonym> GetAllUsedSynonyms();

    bool IsProcedure(const std::string &name);

    void AddSuchThatClause(RelationshipTypes r, std::string &left, std::string &right, bool is_synonym_left, bool is_synonym_right);

    std::vector<RelationshipToken> GetSuchThatClause();

    void AddPattern(EntityIdentifier syn_entity, std::string synonym, std::string left, std::string expression, bool exact);

    std::vector<pql::PatternToken> GetPattern();

    void SetBoolean(bool b);

    bool GetBoolean();
  };
}