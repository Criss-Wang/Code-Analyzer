#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

#include "Parser.h"

namespace pql {

  bool IsLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
  }

  bool IsDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool IsIdent(const std::string& ident) {
    return ident[0] == '\"' && ident[ident.length() - 1] == '\"';
  }

  bool IsInteger(const std::string& s) {
    std::stringstream ssm;
    ssm << s;
    if (!IsDigit(ssm.get())) {
      return false;
    }
    return true;
  }

  std::vector<std::string> Parser::GetSynonyms() {
    std::vector<std::string> synonyms;
    while (ps.Peek() != ';') {
      ps.EatWhiteSpaces();
      synonyms.push_back(ps.ParseSynonym());
      ps.EatWhiteSpaces();
      if (ps.Peek() == ',') {
        ps.Consume();
      }
    }
    ps.Consume();
    return synonyms;
  }

  void Parser::Parse() {
    try {
      while (!ps.IsEOF()) {
        ps.EatWhiteSpaces();
        std::stringstream ks;
        while (IsLetter(ps.Peek())) {
          ks << ps.Next();
        }
        std::string keyword;
        ks >> keyword;
        if (auto d = pql::GetDeclarationType(keyword)) {
          for (const std::string &s: Parser::GetSynonyms()) {
            Parser::query.AddSynonym(*d, s);
          }
        } else if (keyword == "Select") {
          ps.EatWhiteSpaces();
          Parser::query.SetResultSynonym(ps.ParseSynonym());
          ps.EatWhiteSpaces();
          if (!ps.IsEOF()) {
            ps.Expect("such that");
            ps.EatWhiteSpaces();
            Parser::ParseRelationship(Parser::query);
          }
          ps.EatWhiteSpaces();
          if (!ps.IsEOF()) {
            ps.Expect("pattern");
            ps.EatWhiteSpaces();
            Parser::ParsePattern(Parser::query);
          }
          ps.EatWhiteSpaces();
          ps.ExpectEOF();
        }
      }
    } catch (ParseException& e) {
      std::cout << "The PQL query is invalid!" << std::endl;
    }
  }

  pql::Query Parser::getQuery() {
    return Parser::query;
  }

  void Parser::ParseRelationship(Query &q) {
    std::string relationship;
    std::stringstream ssm;
    while (IsLetter(ps.Peek()) || ps.Peek() == '*') {
      ssm << ps.Next();
    }
    ssm >> relationship;
    ps.EatWhiteSpaces();
    ps.Expect("(");
    pql::Ref left = ps.ParseRef(q);
    ps.Expect(",");
    pql::Ref right = ps.ParseRef(q);
    ps.Expect(")");
    if (relationship == "Uses" || relationship == "Modifies") {
      if (left == "_") {
        throw ParseException();
      }
      if (q.IsProcedure(left) || IsIdent(left)) {
        relationship.push_back('P');
      }
    }
    if (auto r = pql::GetRelationshipType(relationship)) {
      if (*r != pql::RelationshipTypes::kUsesP && *r != pql::RelationshipTypes::kModifiesP) {
        if (!q.IsStatement(left) && left != "_" && !IsInteger(left)) {
          throw ParseException();
        }
      }
      if (*r == pql::RelationshipTypes::kFollows || *r == pql::RelationshipTypes::kFollowsT
        || *r == pql::RelationshipTypes::kParent || *r == pql::RelationshipTypes::kParentT) {
        if (!q.IsStatement(right) && right != "_" && !IsInteger(right)) {
          throw ParseException();
        }
      } else {
        if (!q.IsVariable(right) && right != "_" && !IsIdent(right)) {
          throw ParseException();
        }
      }
      q.AddSuchThatClause(*r, left, right, q.SynonymDeclared(left), q.SynonymDeclared(right));
    }
  }

  void Parser::ParsePattern(Query& q) {
    std::string assign_synonym = ps.ParseSynonym();
    std::string expression;
    bool exact = true;
    if (q.IsAssignSynonym(assign_synonym)) {
      ps.EatWhiteSpaces();
      ps.Expect("(");
      pql::Ref left = ps.ParseRef(q);
      ps.EatWhiteSpaces();
      ps.Expect(",");
      ps.EatWhiteSpaces();
      if (ps.Peek() == '_') {
        exact = false;
      }
      expression = ps.ParseExpression();
      if (!exact) {
        ps.Expect("_");
      }
      ps.EatWhiteSpaces();
      ps.Expect(")");
      q.AddPattern(assign_synonym, left, expression, exact, q.SynonymDeclared(left));
    }
  }

}