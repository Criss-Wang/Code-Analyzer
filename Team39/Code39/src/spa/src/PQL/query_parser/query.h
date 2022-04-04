#pragma once
#include <memory>

#include "token.h"
#include "../query_evaluator/clause/clause.h"

namespace pql {
  bool IsLetter(char c);

  bool IsDigit(char c);

  bool IsIdent(const std::string& ident);

  bool IsInteger(const std::string& s);

  bool IsOperator(char c);

  bool IsOpenBracket(char c);

  bool IsCloseBracket(char c);

  bool IsHash(char c);

  AttrIdentifier GetAttributeByString(const std::string& attr);

    
  class Query {
    private:
      std::vector <pql::Synonym> declarations;
      //an unordered_map would be a more efficient data structure because the order doesnt matter
      std::map <std::string, pql::Synonym> synonyms;
      std::vector <pql::Synonym> used_synonyms;
      std::vector <pql::AttrRef> attr_refs;
      std::vector <std::shared_ptr<pql_clause::Clause>> clauses;
      bool is_boolean = false;
      bool is_semantically_valid = true;
    public:
      void SetSemanticallyInvalid();

      bool IsValid(RelationshipTypes relationship, const std::string& left, const std::string& right);

      bool SynonymDeclared(const std::string &name);

      Synonym GetSynonymByName(const std::string &name);

      static bool IsAttrStringValid(const std::string& attribute);

      bool IsAssignSynonym(const std::string &name);

      void AddSynonym(EntityIdentifier d, const std::string &name);

      void AddResultSynonym(const std::string &name);

      void AddResultSynonym(const std::string& name, const std::string& attribute);

      bool IsWhileSynonym(const std::string &name);

      bool IsIfSynonym(const std::string &name);

      void AddUsedSynonym(const std::string &name);

      std::vector <pql::Synonym> GetAllUsedSynonyms();

      void AddAttrRef(Synonym& s);

      void AddAttrRef(Synonym& s, AttrIdentifier attr);

      std::vector <pql::AttrRef> GetAttrRef();

      bool IsProcedure(const std::string &name);

      void AddSuchThatClause(RelationshipTypes r, std::string &left, std::string &right, bool is_synonym_left, bool is_synonym_right);

      void AddPattern(EntityIdentifier syn_entity, std::string synonym, std::string left, std::string expression, bool exact);

      void AddWith(std::shared_ptr<AttrRef> left_attr, std::string left_entity, bool is_attr_ref_left,
                   std::shared_ptr<AttrRef> right_attr, std::string right_entity, bool is_attr_ref_right);

      std::vector <std::shared_ptr<pql_clause::Clause>> GetClauses();

      void SetBoolean(bool b);

      [[nodiscard]] bool GetBoolean() const;

      [[nodiscard]] bool IsSemanticallyValid() const;

      static bool IsAttrValidForSyn(Synonym& s, AttrIdentifier attr);
    };
}