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

#include <iostream>
#include <stdexcept>
#include <string>

#include "parser.h"

namespace pql {
  std::vector<std::string> Parser::GetSynonyms() {
    std::vector<std::string> synonyms;
    while (ps.Peek() != ';') {
      ps.EatWhiteSpaces();
      synonyms.push_back(ps.ParseName());
      ps.EatWhiteSpaces();
      if (ps.Peek() == ',') {
        ps.Consume();
      }
    }
    ps.Consume();
    return synonyms;
  }

  void Parser::Parse() {
    bool select_clause_parsed = false;
    bool declarations_parsed = false;
    int current_clause = NO_CURRENT_CLAUSE;
    while (!ps.IsEOF()) {
      ps.EatWhiteSpaces();
      std::stringstream ks;
      while (IsLetter(ps.Peek())) {
        ks << ps.Next();
      }
      std::string keyword;
      ks >> keyword;
      if (auto d = pql::GetDeclarationType(keyword)) {
        if (declarations_parsed) {
          throw ParseException();
        }
        for (const std::string &s: Parser::GetSynonyms()) {
          Parser::query.AddSynonym(*d, s);
        }
      } else if (keyword == "Select") {
        Parser::ParseSelect();
        select_clause_parsed = true;
        declarations_parsed = true;
      } else if (keyword == "such" && select_clause_parsed) {
        if (ps.Peek() != ' ') {
          throw ParseException();
        }
        ps.EatWhiteSpaces();
        ps.Expect("that");
        Parser::ParseRelationship();
        current_clause = IS_SUCH_THAT;
      } else if (keyword == "pattern" && select_clause_parsed) {
        Parser::ParsePattern();
        current_clause = IS_PATTERN;
      } else if (keyword == "with" && select_clause_parsed) {
        Parser::ParseWith();
        current_clause = IS_WITH;
      } else if (keyword == "and" && select_clause_parsed) {
        if (current_clause == IS_SUCH_THAT) {
          Parser::ParseRelationship();
        } else if (current_clause == IS_PATTERN) {
          Parser::ParsePattern();
        } else if (current_clause == IS_WITH) {
          Parser::ParseWith();
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
    return Parser::query;
  }

  void Parser::ParseAttrRef() {
    std::string name = ps.ParseName();
    ps.EatWhiteSpaces();
    if (name == "BOOLEAN" && Parser::query.GetBoolean()) {
      Parser::query.SetBoolean(true);
    } else {
      Parser::query.SetBoolean(false);
      if (ps.Peek() == '.') {
        ps.Consume();
        std::string attr = ps.ParseAttribute();
        Parser::query.AddResultSynonym(name, attr);
      } else {
        Parser::query.AddResultSynonym(name);
      }
    }
    ps.EatWhiteSpaces();
  }

  void Parser::ParseSelect() {
    std::string attribute;
    ps.EatWhiteSpaces();
    if (ps.Peek() == '<') {
      Parser::query.SetBoolean(false); // make sure there is no BOOLEAN in the tuple
      Parser::ParseTuple();
    } else {
      Parser::ParseAttrRef();
    }
  }

  void Parser::ParseTuple() {
    int tuple_elem_count = 0;
    ps.Consume();
    ps.EatWhiteSpaces();
    while (ps.Peek() != '>') {
      Parser::ParseAttrRef();
      tuple_elem_count++;
      if (ps.Peek() == ',') {
        ps.Next();
      }
      ps.EatWhiteSpaces();
    }
    ps.Consume();
    if (tuple_elem_count == 0) {
      throw ParseException();
    }
  }

  void Parser::ParseRelationship() {
    std::string relationship;
    std::stringstream ssm;
    ps.EatWhiteSpaces();
    while (IsLetter(ps.Peek()) || ps.Peek() == '*') {
      ssm << ps.Next();
    }
    ssm >> relationship;
    ps.EatWhiteSpaces();
    ps.Expect("(");
    std::string left = ps.ParseRef(Parser::query);
    ps.EatWhiteSpaces();
    ps.Expect(",");
    std::string right = ps.ParseRef(Parser::query);
    ps.Expect(")");
    ps.EatWhiteSpaces();
    if (relationship == "Uses" || relationship == "Modifies") {
      if (left == "_") {
        Parser::query.SetSemanticallyInvalid();
        return;
      }
      if (Parser::query.IsProcedure(left) || IsIdent(left)) {
        relationship.push_back('P');
      }
    }

    if (auto r = pql::GetRelationshipType(relationship)) {
      bool is_synonym_left = Parser::query.SynonymDeclared(left);
      bool is_synonym_right = Parser::query.SynonymDeclared(right);
      Parser::query.AddSuchThatClause(*r, left, right, is_synonym_left, is_synonym_right);
    } else {
      throw ParseException();
    }
  }

  void Parser::ParsePattern() {
    std::string synonym = ps.ParseName();
    Parser::query.AddUsedSynonym(synonym);
    auto pattern = Parser::ParsePatternSyntax();
    ps.EatWhiteSpaces();
    std::string left = std::get<INDEX_OF_LEFT>(pattern);
    std::unordered_set<EntityIdentifier> domain = std::get<INDEX_OF_DOMAIN>(pattern);
    std::string expression = std::get<INDEX_OF_EXPRESSION>(pattern);
    bool is_exact = std::get<INDEX_OF_EXACT>(pattern);
    
    if (Parser::query.IsAssignSynonym(synonym) && WithinUnorderedSet(&domain, EntityIdentifier::kAssign)) {
      Parser::query.AddPattern(EntityIdentifier::kAssign, synonym, left, expression, is_exact);
    } else if (Parser::query.IsWhileSynonym(synonym) && WithinUnorderedSet(&domain, EntityIdentifier::kWhile)) {
      Parser::query.AddPattern(EntityIdentifier::kWhile, synonym, left, expression, is_exact);
    } else if (Parser::query.IsIfSynonym(synonym) && WithinUnorderedSet(&domain, EntityIdentifier::kIf)) {
      Parser::query.AddPattern(EntityIdentifier::kIf, synonym, left, expression, is_exact);
    } else {
      Parser::query.SetSemanticallyInvalid();
    }
  }

  // returns a tuple <left most argument of a pattern, entity domain for the pattern synonym, expression, expression is exact match boolean>
  std::tuple<std::string, std::unordered_set<EntityIdentifier>, std::string, bool> Parser::ParsePatternSyntax() {
    std::string expression;
    bool is_exact = false;
    std::unordered_set<EntityIdentifier> pattern_synonym_entity_domain;
    ps.Expect("(");
    ps.EatWhiteSpaces();
    std::string left = ps.ParseRef(Parser::query);
    if (IsInteger(left)) {
      throw ParseException();
    }
    ps.EatWhiteSpaces();
    ps.Expect(",");
    ps.EatWhiteSpaces();
    if (ps.Peek() == '\"') {
      expression = ps.ParseExpression(Parser::query);
      ps.EatWhiteSpaces();
      ps.Expect(")");
      is_exact = true;
      pattern_synonym_entity_domain.insert(EntityIdentifier::kAssign);
    } else {
      ps.Expect("_");
      ps.EatWhiteSpaces();
      if (ps.Peek() == '\"') {
        expression = ps.ParseExpression(Parser::query);
        ps.EatWhiteSpaces();
        ps.Expect("_");
        ps.EatWhiteSpaces();
        ps.Expect(")");
        pattern_synonym_entity_domain.insert(EntityIdentifier::kAssign);
      } else if (ps.Peek() == ')') {
        ps.Consume();
        expression = "_";
        pattern_synonym_entity_domain.insert(EntityIdentifier::kAssign);
        pattern_synonym_entity_domain.insert(EntityIdentifier::kWhile);
      } else {
        ps.Expect(",");
        ps.EatWhiteSpaces();
        ps.Expect("_");
        ps.EatWhiteSpaces();
        ps.Expect(")");
        pattern_synonym_entity_domain.insert(EntityIdentifier::kIf);
      }
    }
    return make_tuple(left, pattern_synonym_entity_domain, expression, is_exact);
  }

  void Parser::ParseWith() {
    ps.EatWhiteSpaces();
    auto left = Parser::ParseWithArgument();
    ps.EatWhiteSpaces();

    ps.ExpectChar('=');

    ps.EatWhiteSpaces();
    auto right = Parser::ParseWithArgument();
    ps.EatWhiteSpaces();

    Parser::query.AddWith(left, right);
  }

  // returns a tuple <attr_ref_ptr, entity, type_of_the_argument>
  std::tuple<std::shared_ptr<AttrRef>, std::string, int> Parser::ParseWithArgument() {
    std::string ref = ps.ParseRef(Parser::query);
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
      ps.Expect(".");
      std::string attr = ps.ParseAttribute();
      if (!Query::IsAttrStringValid(attr)) {
        throw ParseException();
      }

      // check if the synonym is declared
      // skip the attribute validation if the synonym is not declared
      if (!Parser::query.SynonymDeclared(ref)) {
        Parser::query.SetSemanticallyInvalid();
        return make_tuple(attr_ref, entity, type);
      }

      std::shared_ptr<Synonym> synonym = Parser::query.GetSynonymByName(ref);
      AttrIdentifier attribute = GetAttributeByString(attr);

      if (!Query::IsAttrValidForSyn(*synonym, attribute)) {
        Parser::query.SetSemanticallyInvalid();
      }

      attr_ref = std::make_shared<AttrRef>(synonym, attribute);
    }

    return make_tuple(attr_ref, entity, type);
  }

}