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

  bool IsDot(char c);

  bool IsHash(char c);

    
  class Query {
  private:
    std::vector <pql::Synonym> declarations;
    std::map <std::string, pql::Synonym> synonyms;
    std::vector <pql::Synonym> used_synonyms;
    std::vector <pql::Synonym> result_synonyms;
    std::vector <pql::AttrRef> attr_refs;
    std::vector <RelationshipToken> such_that_clauses;
    std::vector <pql::PatternToken> patterns;
    bool is_boolean = false;
    bool is_semantically_valid = true;
  public:
    void SetSemanticallyInvalid();

    bool IsValid(RelationshipTypes relationship, const pql::Ref& left, const pql::Ref& right);

    bool SynonymDeclared(const std::string &name);

    bool IsAttrStringValid(const std::string& attribute);

    bool IsAssignSynonym(const std::string &name);

    void AddSynonym(EntityIdentifier d, const std::string &name);

    void AddResultSynonym(const std::string &name);

    void AddResultSynonym(const std::string& name, const std::string& attribute);

    std::vector<pql::Synonym> GetResultSynonym();

    void AddUsedSynonym(const std::string &name);

    void AddAttrRef(Synonym s);

    void AddAttrRef(Synonym s, AttrIdentifier attr);

    std::vector <pql::AttrRef> GetAttrRef();

    std::vector <pql::Synonym> GetAllUsedSynonyms();

    bool IsProcedure(const std::string &name);

    void AddSuchThatClause(RelationshipTypes r, pql::Ref &left, pql::Ref &right, bool is_synonym_left, bool is_synonym_right);

    std::vector<RelationshipToken> GetSuchThatClause();

    void AddPattern(std::string assign_synonym, pql::Ref left, std::string expression, bool exact, bool is_synonym_left);

    std::vector<pql::PatternToken> GetPattern();

    void SetBoolean(bool b);

    bool GetBoolean();
  };
}