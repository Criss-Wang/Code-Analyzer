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
        ps.Expect(" that");
        ps.EatWhiteSpaces();
        Parser::ParseRelationship();
        ps.EatWhiteSpaces();
        current_clause = IS_SUCH_THAT;
      } else if (keyword == "pattern" && select_clause_parsed) {
        ps.EatWhiteSpaces();
        Parser::ParsePattern();
        current_clause = IS_PATTERN;
      } else if (keyword == "and" && select_clause_parsed) {
        if (current_clause == IS_SUCH_THAT) {
          ps.EatWhiteSpaces();
          Parser::ParseRelationship();
          ps.EatWhiteSpaces();
        } else if (current_clause == IS_PATTERN) {
          ps.EatWhiteSpaces();
          ps.Expect("pattern");
          ps.EatWhiteSpaces();
          Parser::ParsePattern();
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
    ps.EatWhiteSpaces();
    if (ps.Peek() == '<') {
      Parser::ParseTuple();
      Parser::query.SetBoolean(false);
    } else {
      std::string name = ps.ParseName();
      if (name == "BOOLEAN") {
        if (Parser::query.SynonymDeclared(name)) {
          Parser::query.AddResultSynonym(name);
          Parser::query.SetBoolean(false);
        } else {
          Parser::query.SetBoolean(true);
        }
      }
    }
    ps.EatWhiteSpaces();
  }

  void Parser::ParseTuple() {
    ps.Consume();
    while (ps.Peek() != '>') {
      Parser::query.AddResultSynonym(ps.ParseName());
      if (ps.Peek() != '>') {
        ps.Expect(",");
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
    pql::Ref left = ps.ParseRef(Parser::query);
    ps.EatWhiteSpaces();
    ps.Expect(",");
    pql::Ref right = ps.ParseRef(Parser::query);
    ps.Expect(")");
    if (relationship == "Uses" || relationship == "Modifies") {
      if (left == "_") {
        throw ParseException();
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
    std::string assign_synonym = ps.ParseName();
    std::string expression;
    bool exact = true;
    if (Parser::query.IsAssignSynonym(assign_synonym)) {
      Parser::query.AddUsedSynonym(assign_synonym);
      ps.EatWhiteSpaces();
      ps.Expect("(");
      pql::Ref left = ps.ParseRef(Parser::query);
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
        expression = ps.ParseExpression();
        ps.Expect("\"");
        ps.Expect("_");
      } else {
        expression = ps.ParseExpression();
        ps.Expect("\"");
      }
      ps.EatWhiteSpaces();
      ps.Expect(")");
      bool is_synonym_left = Parser::query.SynonymDeclared(left);
      if (!is_synonym_left && IsIdent(left)) {
        left.erase(0, 1);
        int left_len = left.length();
        left.erase(left_len - 1, 1);
      }
      Parser::query.AddPattern(assign_synonym, left, expression, exact, is_synonym_left);
    } else {
      throw ParseException();
    }
  }

}