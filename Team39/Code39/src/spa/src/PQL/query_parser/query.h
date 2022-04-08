#pragma once
#include <memory>
#include <unordered_map>

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

  template <typename T>
  bool WithinUnorderedSet(std::unordered_set<T>* set, T t);

  std::string RemoveQuotationMarks(std::string& ident);

  AttrIdentifier GetAttributeByString(const std::string& attr);

    
  class Query {
    private:
      std::vector <pql::Synonym> declarations_;
      std::unordered_map <std::string, pql::Synonym> synonyms_;
      std::vector <pql::Synonym> used_synonyms_;
      std::vector <pql::AttrRef> attr_refs_;
      std::vector <std::shared_ptr<pql_clause::Clause>> clauses_;
      bool is_boolean_ = true;
      bool is_semantically_valid_ = true;
    public:
      void SetSemanticallyInvalid();

      bool IsValidRelationshipArgument(const std::string& argument, std::unordered_set<EntityIdentifier> *domain);

      bool IsValidRelationship(RelationshipTypes relationship, const std::string& left, const std::string& right);

      bool SynonymDeclared(const std::string &name);

      std::shared_ptr<Synonym> GetSynonymByName(const std::string &name);

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

      void AddWith(std::tuple<std::shared_ptr<AttrRef>, std::string, int>& left, std::tuple<std::shared_ptr<AttrRef>, std::string, int>& right);

      std::vector <std::shared_ptr<pql_clause::Clause>> GetClauses();

      void SetBoolean(bool b);

      [[nodiscard]] bool GetBoolean() const;

      [[nodiscard]] bool IsSemanticallyValid() const;

      static bool IsAttrValidForSyn(Synonym& s, AttrIdentifier attr);
    };
}