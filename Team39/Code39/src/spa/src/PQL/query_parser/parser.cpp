#define NO_CURRENT_CLAUSE 0
#define IS_SUCH_THAT 1
#define IS_PATTERN 2
#define IS_WITH 3
#define ATTR_REF -1
#define IDENT -2
#define INTEGER -3
#define INDEX_OF_ATTR_REF 0
#define INDEX_OF_ENTITY 1
#define INDEX_OF_TYPE 2
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
        //Can combine the coditions above
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
      //This part of the code is similar to line 145 - 153
      //Can consider to reorganize the code to have less duplication
      std::string name = ps.ParseName();
      ps.EatWhiteSpaces();
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
    int tuple_elem_count = 0;
    ps.Consume();
    ps.EatWhiteSpaces();
    while (ps.Peek() != '>') {
      std::string name = ps.ParseName();
      ps.EatWhiteSpaces();
      if (ps.Peek() == '.') {
        ps.Next();
        std::string attr = ps.ParseAttribute();
        Parser::query.AddResultSynonym(name, attr);
      } else {
        Parser::query.AddResultSynonym(name);
      }
      ps.EatWhiteSpaces();
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
    
    //Can follow the happy path practice
    //The AddUsedSynonym for such that clause is called in ParseRef, can be moved here to be consistent with other clauses
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
    std::string left = std::get<INDEX_OF_LEFT>(pattern);
    std::vector<EntityIdentifier> domain = std::get<INDEX_OF_DOMAIN>(pattern);
    std::string expression = std::get<INDEX_OF_EXPRESSION>(pattern);
    bool is_exact = std::get<INDEX_OF_EXACT>(pattern);
   
    if (Parser::query.IsAssignSynonym(synonym) && std::find(domain.begin(), domain.end(), EntityIdentifier::kAssign) != domain.end()) {
      Parser::query.AddPattern(EntityIdentifier::kAssign, synonym, left, expression, is_exact);
    } else if (Parser::query.IsWhileSynonym(synonym) && std::find(domain.begin(), domain.end(), EntityIdentifier::kWhile) != domain.end()) {
      Parser::query.AddPattern(EntityIdentifier::kWhile, synonym, left, expression, is_exact);
    } else if (Parser::query.IsIfSynonym(synonym) && std::find(domain.begin(), domain.end(), EntityIdentifier::kIf) != domain.end()) {
      Parser::query.AddPattern(EntityIdentifier::kIf, synonym, left, expression, is_exact);
    } else {
      Parser::query.SetSemanticallyInvalid();
    }
  }

  std::tuple<std::string, std::vector<EntityIdentifier>, std::string, bool> Parser::ParsePatternSyntax() {
    //Can explain what does each element means in the return result
    std::string expression;
    bool is_exact = false;
    std::vector<EntityIdentifier> domain; //what is the domain here
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
      //line 239 - 242 is similar to line 250 - 253 where it is parsing the expression
      //can be abstracted out
      ps.Consume();
      expression = ps.ParseExpression(Parser::query);
      ps.Expect("\"");
      ps.EatWhiteSpaces();
      ps.Expect(")");
      is_exact = true;
      domain.push_back(EntityIdentifier::kAssign);
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
        domain.push_back(EntityIdentifier::kAssign);
      } else if (ps.Peek() == ')') {
        ps.Consume();
        domain.push_back(EntityIdentifier::kAssign);
        domain.push_back(EntityIdentifier::kWhile);
      } else {
        ps.Expect(",");
        ps.EatWhiteSpaces();
        ps.Expect("_");
        ps.EatWhiteSpaces();
        ps.Expect(")");
        domain.push_back(EntityIdentifier::kIf);
      }
    }
    return make_tuple(left, domain, expression, is_exact);
  }

  std::map<AttrIdentifier, int> attrDomain {
    {AttrIdentifier::kValue, INTEGER},
    {AttrIdentifier::kVarName, IDENT},
    {AttrIdentifier::kProcName, IDENT},
    {AttrIdentifier::kStmtNum, INTEGER}
  };

  //Can have a better naming e.g. IsValidWith
  //IsValidRelation checked in query.cpp but isValidWith is checked here, might need to be consistent
  bool IsValid(std::tuple<std::shared_ptr<AttrRef>, std::string, int>& left, std::tuple<std::shared_ptr<AttrRef>, std::string, int>& right) {
    int left_domain;
    if (std::get<INDEX_OF_TYPE>(left) == ATTR_REF) {
      left_domain = attrDomain.at(std::get<INDEX_OF_ATTR_REF>(left)->GetAttrIdentifier());
    } else {
      left_domain = std::get<INDEX_OF_TYPE>(left);
    }
    //The left and right are doing the same thing, can consider to have a function to abstract this out
    int right_domain;
    if (std::get<INDEX_OF_TYPE>(right) == ATTR_REF) {
      right_domain = attrDomain.at(std::get<INDEX_OF_ATTR_REF>(right)->GetAttrIdentifier());
    } else {
      right_domain = std::get<INDEX_OF_TYPE>(right);
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
      Parser::query.AddWith(std::get<INDEX_OF_ATTR_REF>(left), std::get<INDEX_OF_ENTITY>(left),
                std::get<INDEX_OF_TYPE>(left) == ATTR_REF,std::get<INDEX_OF_ATTR_REF>(right),
                  std::get<INDEX_OF_ENTITY>(right), std::get<INDEX_OF_TYPE>(right) == ATTR_REF);
    } else {
      Parser::query.SetSemanticallyInvalid();
    }
  }

  std::tuple<std::shared_ptr<AttrRef>, std::string, int> Parser::ParseWithArgument() {
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
      size_t len = ref.length();
      ref.erase(len - 1, 1);
      entity = ref;
      type = IDENT;
    } else if (IsInteger(ref)) {
      entity = ref;
      type = INTEGER;
    } else {
      //If it reaches here means that this is an undeclared synonym with attribute
      Parser::query.SetSemanticallyInvalid();
      //This is similar to line 321 - 325
      ps.Expect(".");
      std::string attr = ps.ParseAttribute();
      if (!Query::IsAttrStringValid(attr)) {
        throw ParseException();
      }
    }
    return make_tuple(attr_ref, entity, type);
  }

}