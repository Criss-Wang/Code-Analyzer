#define NO_CURRENT_CLAUSE 0
#define IS_SUCH_THAT 1
#define IS_PATTERN 2
#define IS_WITH 3

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
      ps.ParseExpression();
      ps.Expect("\"");
      ps.EatWhiteSpaces();
      ps.Expect(")");
    } else {
      ps.Expect("_");
      ps.EatWhiteSpaces();
      if (ps.Peek() == '\"') {
        ps.Consume();
        ps.ParseExpression();
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
      expression = ps.ParseExpression();
      ps.Expect("\"");
      ps.EatWhiteSpaces();
      ps.Expect("_");
    } else {
      ps.Expect("\"");
      expression = ps.ParseExpression();
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

  void Parser::ParseWith() {
    std::string left = ps.ParseRef(Parser::query);
    std::shared_ptr<AttrRef> left_attr_ref = nullptr;
    std::string left_entity = "";
    bool is_attr_ref_left = false;
    bool is_ident_left = false;
    bool is_int_left = false;
    if (Parser::query.SynonymDeclared(left)) {
      ps.ExpectChar('.');
      std::string attr = ps.ParseAttribute();
      if (!Parser::query.IsAttrStringValid(attr)) {
        throw ParseException();
      }
      Synonym left_synonym = Parser::query.GetSynonymByName(left);
      AttrIdentifier left_attribute = GetAttributeByString(attr);
      if (!Parser::query.IsAttrValidForSyn(left_synonym, left_attribute)) {
        Parser::query.SetSemanticallyInvalid();
      }

      left_attr_ref = std::make_shared<AttrRef>(left_synonym, left_attribute);
      is_attr_ref_left = true;
    } else if (IsIdent(left)){
      left.erase(0, 1);
      int left_len = left.length();
      left.erase(left_len - 1, 1);
      left_entity = left;
      is_ident_left = true;
    } else if (IsInteger(left)) {
      left_entity = left;
      is_int_left = true;
    } else {
      //If it reaches here means that this is an undeclared synonym with attribute
      Parser::query.SetSemanticallyInvalid();
      ps.Expect(".");
      std::string attr = ps.ParseAttribute();
      if (!Parser::query.IsAttrStringValid(attr)) {
          throw ParseException();
      }
    }
    ps.EatWhiteSpaces();
    ps.ExpectChar('=');
    ps.EatWhiteSpaces();
    std::string right = ps.ParseRef(Parser::query);
    std::shared_ptr<AttrRef> right_attr_ref = nullptr;
    std::string right_entity = "";
    bool is_attr_ref_right = false;
    if (Parser::query.SynonymDeclared(right)) {
      ps.ExpectChar('.');
      std::string attr = ps.ParseAttribute();
      if (!Parser::query.IsAttrStringValid(attr)) {
        throw ParseException();
      }
      Synonym right_synonym = Parser::query.GetSynonymByName(right);
      //need to check if the attribute is valid for the synonym
      AttrIdentifier right_attribute = GetAttributeByString(attr);
      if (!Parser::query.IsAttrValidForSyn(right_synonym, right_attribute)) {
        Parser::query.SetSemanticallyInvalid();
      }

      right_attr_ref = std::make_shared<AttrRef>(right_synonym, right_attribute);
      is_attr_ref_right = true;
    } else if (IsIdent(right)) {
      if (is_int_left) {
        Parser::query.SetSemanticallyInvalid();
      }
      right.erase(0, 1);
      int right_len = right.length();
      right.erase(right_len - 1, 1);
      right_entity = right;
    } else if (IsInteger(right)) {
      if (is_ident_left) {
        Parser::query.SetSemanticallyInvalid();
      }
      right_entity = right;
    } else {
      Parser::query.SetSemanticallyInvalid();
      ps.Expect(".");
      std::string attr = ps.ParseAttribute();
      if (!Parser::query.IsAttrStringValid(attr)) {
        throw ParseException();
      }
    }
    Parser::query.AddWith(left_attr_ref, left_entity, is_attr_ref_left, right_attr_ref, right_entity, is_attr_ref_right);
  }

}