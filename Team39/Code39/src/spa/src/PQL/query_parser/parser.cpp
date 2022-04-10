#include <iostream>
#include <stdexcept>
#include <string>

#include "parser.h"

#define NO_CURRENT_CLAUSE 0
#define IS_SUCH_THAT 1
#define IS_PATTERN 2
#define IS_WITH 3
#define ATTR_REF -1
#define IDENT -2
#define INTEGER -3
#define INDEX_OF_LEFT 0
#define INDEX_OF_DOMAIN 1
#define INDEX_OF_EXPRESSION 2
#define INDEX_OF_EXACT 3

namespace pql {
  std::vector<std::string> Parser::GetSynonyms() {
    std::vector<std::string> synonyms;
    while (ps_.Peek() != ';') {
      ps_.EatWhiteSpaces();
      synonyms.push_back(ps_.ParseName());
      ps_.EatWhiteSpaces();
      if (ps_.Peek() == ',') {
        ps_.Consume();
      }
    }
    ps_.Consume();
    return synonyms;
  }

  void Parser::Parse() {
    bool select_clause_parsed = false;
    bool declarations_parsed = false;
    int current_clause = NO_CURRENT_CLAUSE;
    while (!ps_.IsEOF()) {
      ps_.EatWhiteSpaces();
      std::stringstream ks;
      while (IsLetter(ps_.Peek())) {
        ks << ps_.Next();
      }
      std::string keyword;
      ks >> keyword;
      if (auto d = pql::GetDeclarationType(keyword)) {
        if (declarations_parsed) {
          throw ParseException();
        }
        for (const std::string &s: Parser::GetSynonyms()) {
          query_.AddSynonym(*d, s);
        }
      } else if (keyword == "Select") {
        Parser::ParseSelect();
        select_clause_parsed = true;
        declarations_parsed = true;
      } else if (select_clause_parsed) {
        if (keyword == "such") {
          ps_.EatWhiteSpaces();
          ps_.Expect("that");
          Parser::ParseRelationship();
          current_clause = IS_SUCH_THAT;
        } else if (keyword == "pattern" || (keyword == "and" && current_clause == IS_PATTERN)) {
          Parser::ParsePattern();
          current_clause = IS_PATTERN;
        } else if (keyword == "with" || (keyword == "and" && current_clause == IS_WITH)) {
          Parser::ParseWith();
          current_clause = IS_WITH;
        } else if (keyword == "and" && current_clause == IS_SUCH_THAT) {
          Parser::ParseRelationship();
          current_clause = IS_SUCH_THAT;
        } else {
          throw ParseException();
        }
      } else {
        throw ParseException();
      }
    }
    if (!select_clause_parsed) {
      throw ParseException();
    }
  }

  pql::Query Parser::GetQuery() {
    return query_;
  }

  void Parser::ParseAttrRef() {
    std::string name = ps_.ParseName();
    ps_.EatWhiteSpaces();
    if (name == "BOOLEAN" && query_.GetBoolean() && !query_.SynonymDeclared(name)) {
      query_.SetBoolean(true);
    } else {
      query_.SetBoolean(false);
      if (ps_.Peek() == '.') {
        ps_.Consume();
        std::string attr = ps_.ParseAttribute();
        query_.AddResultSynonym(name, attr);
      } else {
        query_.AddResultSynonym(name);
      }
    }
    ps_.EatWhiteSpaces();
  }

  void Parser::ParseSelect() {
    std::string attribute;
    ps_.EatWhiteSpaces();
    if (ps_.Peek() == '<') {
      query_.SetBoolean(false); // make sure there is no BOOLEAN in the tuple
      Parser::ParseTuple();
    } else {
      Parser::ParseAttrRef();
    }
  }

  void Parser::ParseTuple() {
    int tuple_elem_count = 0;
    ps_.Consume();
    ps_.EatWhiteSpaces();
    Parser::ParseAttrRef();
    tuple_elem_count++;
    while (ps_.Peek() != '>') {
      ps_.EatWhiteSpaces();
      ps_.ExpectChar(',');
      ps_.EatWhiteSpaces();
      Parser::ParseAttrRef();
      tuple_elem_count++;
    }
    ps_.Consume();
    if (tuple_elem_count == 0) {
      throw ParseException();
    }
  }

  void Parser::ParseRelationship() {
    std::string relationship;
    std::stringstream ssm;
    ps_.EatWhiteSpaces();
    while (IsLetter(ps_.Peek()) || ps_.Peek() == '*') {
      ssm << ps_.Next();
    }
    ssm >> relationship;
    ps_.EatWhiteSpaces();
    ps_.Expect("(");
    std::string left = ps_.ParseRef(query_);
    ps_.EatWhiteSpaces();
    ps_.Expect(",");
    std::string right = ps_.ParseRef(query_);
    ps_.Expect(")");
    ps_.EatWhiteSpaces();
    if (relationship == "Uses" || relationship == "Modifies") {
      if (left == "_") {
        query_.SetSemanticallyInvalid();
        return;
      }
      if (query_.IsProcedure(left) || IsIdent(left)) {
        relationship.push_back('P');
      }
    }

    if (auto r = pql::GetRelationshipType(relationship)) {
      bool is_synonym_left = query_.SynonymDeclared(left);
      bool is_synonym_right = query_.SynonymDeclared(right);
      query_.AddSuchThatClause(*r, left, right, is_synonym_left, is_synonym_right);
    } else {
      throw ParseException();
    }
  }

  void Parser::ParsePattern() {
    std::string synonym = ps_.ParseName();
    query_.AddUsedSynonym(synonym);
    auto pattern = Parser::ParsePatternSyntax();
    ps_.EatWhiteSpaces();
    std::string left = std::get<INDEX_OF_LEFT>(pattern);
    std::unordered_set<EntityIdentifier> domain = std::get<INDEX_OF_DOMAIN>(pattern);
    std::string expression = std::get<INDEX_OF_EXPRESSION>(pattern);
    bool is_exact = std::get<INDEX_OF_EXACT>(pattern);
    
    if (query_.IsAssignSynonym(synonym) && WithinUnorderedSet(&domain, EntityIdentifier::kAssign)) {
      query_.AddPattern(EntityIdentifier::kAssign, synonym, left, expression, is_exact);
    } else if (query_.IsWhileSynonym(synonym) && WithinUnorderedSet(&domain, EntityIdentifier::kWhile)) {
      query_.AddPattern(EntityIdentifier::kWhile, synonym, left, expression, is_exact);
    } else if (query_.IsIfSynonym(synonym) && WithinUnorderedSet(&domain, EntityIdentifier::kIf)) {
      query_.AddPattern(EntityIdentifier::kIf, synonym, left, expression, is_exact);
    } else {
      query_.SetSemanticallyInvalid();
    }
  }

  // returns a tuple <left most argument of a pattern, entity domain for the pattern synonym, expression, expression is exact match boolean>
  std::tuple<std::string, std::unordered_set<EntityIdentifier>, std::string, bool> Parser::ParsePatternSyntax() {
    std::string expression;
    bool is_exact = false;
    std::unordered_set<EntityIdentifier> pattern_synonym_entity_domain;
    ps_.Expect("(");
    ps_.EatWhiteSpaces();
    std::string left = ps_.ParseRef(query_);
    if (IsInteger(left)) {
      throw ParseException();
    }
    ps_.EatWhiteSpaces();
    ps_.Expect(",");
    ps_.EatWhiteSpaces();
    if (ps_.Peek() == '\"') {
      expression = ps_.ParseExpression(query_);
      ps_.EatWhiteSpaces();
      ps_.Expect(")");
      is_exact = true;
      pattern_synonym_entity_domain.insert(EntityIdentifier::kAssign);
    } else {
      ps_.Expect("_");
      ps_.EatWhiteSpaces();
      if (ps_.Peek() == '\"') {
        expression = ps_.ParseExpression(query_);
        ps_.EatWhiteSpaces();
        ps_.Expect("_");
        ps_.EatWhiteSpaces();
        ps_.Expect(")");
        pattern_synonym_entity_domain.insert(EntityIdentifier::kAssign);
      } else if (ps_.Peek() == ')') {
        ps_.Consume();
        expression = "_";
        pattern_synonym_entity_domain.insert(EntityIdentifier::kAssign);
        pattern_synonym_entity_domain.insert(EntityIdentifier::kWhile);
      } else {
        ps_.Expect(",");
        ps_.EatWhiteSpaces();
        ps_.Expect("_");
        ps_.EatWhiteSpaces();
        ps_.Expect(")");
        pattern_synonym_entity_domain.insert(EntityIdentifier::kIf);
      }
    }
    return make_tuple(left, pattern_synonym_entity_domain, expression, is_exact);
  }

  void Parser::ParseWith() {
    ps_.EatWhiteSpaces();
    auto left = Parser::ParseWithArgument();
    ps_.EatWhiteSpaces();

    ps_.ExpectChar('=');

    ps_.EatWhiteSpaces();
    auto right = Parser::ParseWithArgument();
    ps_.EatWhiteSpaces();

    query_.AddWith(left, right);
  }

  // returns a tuple <attr_ref_ptr, entity, type_of_the_argument>
  std::tuple<std::shared_ptr<AttrRef>, std::string, int> Parser::ParseWithArgument() {
    std::string ref = ps_.ParseRef(query_);
    std::shared_ptr<AttrRef> attr_ref = nullptr;
    std::string entity;
    int type = ATTR_REF;
    if (IsIdent(ref)){
      entity = RemoveQuotationMarks(ref);
      type = IDENT;
    } else if (IsInteger(ref)) {
      entity = ref;
      type = INTEGER;
    } else {
      ps_.Expect(".");
      std::string attr = ps_.ParseAttribute();
      if (!Query::IsAttrStringValid(attr)) {
        throw ParseException();
      }

      // check if the synonym is declared
      // skip the attribute validation if the synonym is not declared
      if (!query_.SynonymDeclared(ref)) {
        query_.SetSemanticallyInvalid();
        return make_tuple(attr_ref, entity, type);
      }

      std::shared_ptr<Synonym> synonym = query_.GetSynonymByName(ref);
      AttrIdentifier attribute = GetAttributeByString(attr);

      if (!Query::IsAttrValidForSyn(*synonym, attribute)) {
        query_.SetSemanticallyInvalid();
      }

      attr_ref = std::make_shared<AttrRef>(synonym, attribute);
    }

    return make_tuple(attr_ref, entity, type);
  }

}