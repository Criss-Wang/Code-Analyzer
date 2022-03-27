#define NO_CURRENT_CLAUSE 0
#define IS_SUCH_THAT 1
#define IS_PATTERN 2
#define IS_WITH 3
#define ATTR_REF 4
#define IDENT 5
#define INTEGER 6

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
        ps.EatWhiteSpaces();
        ps.Expect("that");
        ps.EatWhiteSpaces();
        Parser::ParseRelationship();
        ps.EatWhiteSpaces();
        current_clause = IS_SUCH_THAT;
      } else if (keyword == "pattern" && select_clause_parsed) {
        ps.EatWhiteSpaces();
        Parser::ParsePattern();
        ps.EatWhiteSpaces();
        current_clause = IS_PATTERN;
      } else if (keyword == "with" && select_clause_parsed) {
        ps.EatWhiteSpaces();
        Parser::ParseWith();
        ps.EatWhiteSpaces();
        current_clause = IS_WITH;
      } else if (keyword == "and" && select_clause_parsed) {
        if (current_clause == IS_SUCH_THAT) {
          ps.EatWhiteSpaces();
          Parser::ParseRelationship();
          ps.EatWhiteSpaces();
        } else if (current_clause == IS_PATTERN) {
          ps.EatWhiteSpaces();
          Parser::ParsePattern();
          ps.EatWhiteSpaces();
        } else if (current_clause == IS_WITH) {
          ps.EatWhiteSpaces();
          Parser::ParseWith();
          ps.EatWhiteSpaces();
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

  void Parser::ParseSelect() {
    bool has_attribute = false;
    std::string attribute;
    ps.EatWhiteSpaces();
    if (ps.Peek() == '<') {
      Parser::ParseTuple();
      Parser::query.SetBoolean(false);
    } else {
      std::string name = ps.ParseName();
      if (ps.Peek() == '.') {
        ps.Next(); 
        attribute = ps.ParseAttribute();
        has_attribute = true; 
      }
      if (name == "BOOLEAN" && !Parser::query.SynonymDeclared(name)) {
        Parser::query.SetBoolean(true);
      } else if (Parser::query.SynonymDeclared(name) && has_attribute) {
        Parser::query.AddResultSynonym(name, attribute);
        Parser::query.SetBoolean(false);
      } else if (Parser::query.SynonymDeclared(name) && !has_attribute) {
        Parser::query.AddResultSynonym(name);
        Parser::query.SetBoolean(false);
      } else if (!Parser::query.SynonymDeclared(name)) {
        Parser::query.SetSemanticallyInvalid();
      }
    }
    ps.EatWhiteSpaces();
  }

  void Parser::ParseTuple() {
    ps.Consume();
    while (ps.Peek() != '>') {
      std::string name = ps.ParseName();
      if (ps.Peek() == '.') {
        ps.Next();
        std::string attr = ps.ParseAttribute();
        Parser::query.AddResultSynonym(name, attr);
      } else {
        Parser::query.AddResultSynonym(name);
      }

      if (ps.Peek() == ',') {
        ps.Next();
      }
    }
    ps.Consume();
  }

  void Parser::ParseRelationship() {
    std::string relationship;
    std::stringstream ssm;
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
    if (relationship == "Uses" || relationship == "Modifies") {
      if (left == "_") {
        Parser::query.SetSemanticallyInvalid();
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
    if (Parser::query.IsAssignSynonym(synonym)) {
      Parser::ParseAssignPattern(synonym);
    } else if (Parser::query.IsWhileSynonym(synonym)) {
      Parser::ParseWhilePattern(synonym);
    } else if (Parser::query.IsIfSynonym(synonym)) {
      Parser::ParseIfPattern(synonym);
    } else {
      Parser::query.SetSemanticallyInvalid();
      ps.EatWhiteSpaces();
      Parser::ParsePatternSyntax();
    }
  }

  void Parser::ParsePatternSyntax() {
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
      ps.Consume();
      ps.ParseExpression(Parser::query);
      ps.Expect("\"");
      ps.EatWhiteSpaces();
      ps.Expect(")");
    } else {
      ps.Expect("_");
      ps.EatWhiteSpaces();
      if (ps.Peek() == '\"') {
        ps.Consume();
        ps.ParseExpression(Parser::query);
        ps.Expect("\"");
        ps.EatWhiteSpaces();
        ps.Expect("_");
        ps.EatWhiteSpaces();
        ps.Expect(")");
      } else if (ps.Peek() == ')') {
        ps.Consume();
      } else {
        ps.Expect(",");
        ps.EatWhiteSpaces();
        ps.Expect("_");
        ps.EatWhiteSpaces();
        ps.Expect(")");
      }
    }
  }

  void Parser::ParseAssignPattern(const std::string& synonym) {
    std::string expression;
    bool exact = true;
    Parser::query.AddUsedSynonym(synonym);
    ps.EatWhiteSpaces();
    ps.Expect("(");
    std::string left = ps.ParseRef(Parser::query);
    if (IsInteger(left)) {
      throw ParseException();
    }
    ps.EatWhiteSpaces();
    ps.Expect(",");
    ps.EatWhiteSpaces();
    if (ps.Peek() == '_') {
      exact = false;
      ps.Consume();
    }
    ps.EatWhiteSpaces();
    if (!exact && ps.Peek() == ')') {
      expression = "_";
    } else if (!exact) {
      ps.Expect("\"");
      expression = ps.ParseExpression(Parser::query);
      ps.Expect("\"");
      ps.EatWhiteSpaces();
      ps.Expect("_");
    } else {
      ps.Expect("\"");
      expression = ps.ParseExpression(Parser::query);
      ps.Expect("\"");
    }
    ps.EatWhiteSpaces();
    ps.Expect(")");
    Parser::query.AddPattern(EntityIdentifier::kAssign, synonym, left, expression, exact);
  }

  void Parser::ParseWhilePattern(const std::string &synonym) {
    Parser::query.AddUsedSynonym(synonym);
    ps.EatWhiteSpaces();
    ps.Expect("(");
    std::string left = ps.ParseRef(Parser::query);
    if (IsInteger(left)) {
      throw ParseException();
    }
    ps.EatWhiteSpaces();
    ps.Expect(",");
    ps.EatWhiteSpaces();
    ps.Expect("_"),
    ps.EatWhiteSpaces();
    ps.Expect(")");
    Parser::query.AddPattern(EntityIdentifier::kWhile, synonym, left, "", false);
  }

  void Parser::ParseIfPattern(const std::string &synonym) {
    Parser::query.AddUsedSynonym(synonym);
    ps.EatWhiteSpaces();
    ps.Expect("(");
    std::string left = ps.ParseRef(Parser::query);
    if (IsInteger(left)) {
      throw ParseException();
    }
    ps.EatWhiteSpaces();
    ps.Expect(",");
    ps.EatWhiteSpaces();
    ps.Expect("_"),
    ps.EatWhiteSpaces();
    ps.Expect(",");
    ps.EatWhiteSpaces();
    ps.Expect("_"),
    ps.EatWhiteSpaces();
    ps.Expect(")");
    Parser::query.AddPattern(EntityIdentifier::kIf, synonym, left, "", false);
  }

  std::map<AttrIdentifier, int> attrDomain {
    {AttrIdentifier::kValue, INTEGER},
    {AttrIdentifier::kVarName, IDENT},
    {AttrIdentifier::kProcName, IDENT},
    {AttrIdentifier::kStmtNum, INTEGER}
  };

  bool IsValid(pair<pair<std::shared_ptr<AttrRef>, std::string>, int>& left, pair<pair<std::shared_ptr<AttrRef>, std::string>, int>& right) {
    int left_domain;
    if (left.second == ATTR_REF) {
      left_domain = attrDomain.at(left.first.first->GetAttrIdentifier());
    } else {
      left_domain = left.second;
    }
    int right_domain;
    if (right.second == ATTR_REF) {
      right_domain = attrDomain.at(right.first.first->GetAttrIdentifier());
    } else {
      right_domain = right.second;
    }
    return (left_domain == right_domain);
  }

  void Parser::ParseWith() {
    auto left = Parser::ParseWithArgument();
    ps.EatWhiteSpaces();
    ps.ExpectChar('=');
    ps.EatWhiteSpaces();
    auto right = Parser::ParseWithArgument();
    if (IsValid(left, right)) {
      Parser::query.AddWith(left.first.first, left.first.second, left.second == ATTR_REF,
                            right.first.first, right.first.second, right.second == ATTR_REF);
    } else {
      Parser::query.SetSemanticallyInvalid();
    }
  }

  pair<pair<std::shared_ptr<AttrRef>, std::string>, int> Parser::ParseWithArgument() {
    std::string ref = ps.ParseRef(Parser::query);
    std::shared_ptr<AttrRef> attr_ref = nullptr;
    std::string entity;
    int type = ATTR_REF;
    if (Parser::query.SynonymDeclared(ref)) {
      ps.ExpectChar('.');
      std::string attr = ps.ParseAttribute();
      if (!Query::IsAttrStringValid(attr)) {
        throw ParseException();
      }
      Synonym synonym = Parser::query.GetSynonymByName(ref);
      AttrIdentifier attribute = GetAttributeByString(attr);
      if (!Query::IsAttrValidForSyn(synonym, attribute)) {
        Parser::query.SetSemanticallyInvalid();
      }
      attr_ref = std::make_shared<AttrRef>(synonym, attribute);
    } else if (IsIdent(ref)){
      ref.erase(0, 1);
      int len = ref.length();
      ref.erase(len - 1, 1);
      entity = ref;
      type = IDENT;
    } else if (IsInteger(ref)) {
      entity = ref;
      type = INTEGER;
    } else {
      //If it reaches here means that this is an undeclared synonym with attribute
      Parser::query.SetSemanticallyInvalid();
      ps.Expect(".");
      std::string attr = ps.ParseAttribute();
      if (!Query::IsAttrStringValid(attr)) {
        throw ParseException();
      }
    }
    return make_pair(make_pair(attr_ref, entity), type);
  }

}