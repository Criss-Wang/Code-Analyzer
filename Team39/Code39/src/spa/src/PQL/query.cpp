#include "query.h"

#include <algorithm>
#include <map>
#include <unordered_set>
#include <string>
#include <sstream>

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
    if (!pql::IsDigit(ssm.get())) {
        return false;
    }
    return true;
  }

  bool IsOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
  }


  bool IsOpenBracket(char c) {
    return c == '(';
  }


  bool IsCloseBracket(char c) {
    return c == ')';
  }

  std::unordered_set<EntityIdentifier> stmts({
    EntityIdentifier::kStmt,
    EntityIdentifier::kAssign,
    EntityIdentifier::kIf,
    EntityIdentifier::kWhile,
    EntityIdentifier::kCall,
    EntityIdentifier::kRead,
    EntityIdentifier::kPrint,
    EntityIdentifier::kWildcard,
    EntityIdentifier::kStmtNumber
  });

  std::unordered_set<EntityIdentifier> procs({
    EntityIdentifier::kProc,
    EntityIdentifier::kIdent,
    EntityIdentifier::kWildcard
  });

  std::unordered_set<EntityIdentifier> uses_and_modifies_left_domain ({
    EntityIdentifier::kProc,
    EntityIdentifier::kIdent
  });

  std::unordered_set<EntityIdentifier> vars({
    EntityIdentifier::kVariable,
    EntityIdentifier::kWildcard,
    EntityIdentifier::kIdent
  });

  const std::map<pql::RelationshipTypes, std::unordered_set<EntityIdentifier>> left_synonym_domains {
      {kFollows, stmts},
      {kFollowsT, stmts},
      {kParent, stmts},
      {kParentT, stmts},
      {kUsesS, stmts},
      {kUsesP, uses_and_modifies_left_domain},
      {kModifiesS, stmts},
      {kModifiesP, uses_and_modifies_left_domain},
      {kCalls, procs},
      {kCallsT, procs}
  };

  const std::map<pql::RelationshipTypes, std::unordered_set<EntityIdentifier>> right_synonym_domains {
      {kFollows, stmts},
      {kFollowsT, stmts},
      {kParent, stmts},
      {kParentT, stmts},
      {kUsesS, vars},
      {kUsesP, vars},
      {kModifiesS, vars},
      {kModifiesP, vars},
      {kCalls, procs},
      {kCallsT, procs}
  };

  bool Query::IsValid(RelationshipTypes r, const pql::Ref& left, const pql::Ref& right) {
    std::unordered_set<EntityIdentifier> left_domains = pql::left_synonym_domains.at(r);
    std::unordered_set<EntityIdentifier> right_domains = pql::right_synonym_domains.at(r);
    if (Query::SynonymDeclared(left)) {
      if (left_domains.find(Query::synonyms.at(left).GetDeclaration()) == left_domains.end()) {
        return false;
      }
    } else {
      if (left == "_" && left_domains.find(EntityIdentifier::kWildcard) == left_domains.end()) {
        return false;
      } else if (pql::IsIdent(left) && left_domains.find(EntityIdentifier::kIdent) == left_domains.end()) {
        return false;
      } else if (pql::IsInteger(left) && left_domains.find(EntityIdentifier::kStmtNumber) == left_domains.end()) {
        return false;
      }
    }

    if (Query::SynonymDeclared(right)) {
      if (right_domains.find(Query::synonyms.at(right).GetDeclaration()) == right_domains.end()) {
        return false;
      }
    } else {
      if (right == "_" && right_domains.find(EntityIdentifier::kWildcard) == right_domains.end()) {
        return false;
      } else if (pql::IsIdent(right) && right_domains.find(EntityIdentifier::kIdent) == right_domains.end()) {
        return false;
      } else if (pql::IsInteger(right) && right_domains.find(EntityIdentifier::kStmtNumber) == right_domains.end()) {
        return false;
      }
    }

    return (Query::SynonymDeclared(left) || left == "_" || pql::IsIdent(left) || pql::IsInteger(left)) &&
        (Query::SynonymDeclared(right) || right == "_" || pql::IsIdent(right) || pql::IsInteger(right));
  }

  bool Query::SynonymDeclared(const std::string &name) {
    return synonyms.find(name) != synonyms.end();
  }

  bool Query::IsAssignSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms.at(name).GetDeclaration() == EntityIdentifier::kAssign;
  }

  void Query::AddSynonym(EntityIdentifier d, const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      throw ParseException();
    } else {
      pql::Synonym sm = Synonym(name, d);
      Query::declarations.push_back(sm);
      Query::synonyms.insert(std::pair<std::string, pql::Synonym>(name, sm));
    }
  }

  void Query::AddResultSynonym(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      Query::result_synonyms.push_back(Query::synonyms.at(name));
      Query::AddUsedSynonym(name);
    } else {
      throw ParseException();
    }
  }

  std::vector<pql::Synonym> Query::GetResultSynonym() {
    return Query::result_synonyms;
  }

  bool Query::IsProcedure(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      return Query::synonyms.at(name).GetDeclaration() == EntityIdentifier::kProc;
    }
    return false;
  }

  void Query::AddUsedSynonym(const std::string &name) {
    for (pql::Synonym s: Query::used_synonyms) {
      if (s.equal(Query::synonyms.at(name))) {
        return ;
      }
    }
    Query::used_synonyms.push_back(Query::synonyms.at(name));
  }

  std::vector<pql::Synonym> Query::GetAllUsedSynonyms() {
    return Query::used_synonyms;
  }

  void Query::AddSuchThatClause(RelationshipTypes r, pql::Ref &left, pql::Ref &right, bool is_synonym_left, bool is_synonym_right) {
    if (Query::IsValid(r, left, right)) {
      if (!is_synonym_left && IsIdent(left)) {
        left.erase(0, 1);
        int left_len = left.length();
        left.erase(left_len - 1, 1);
      }
      if (!is_synonym_right && IsIdent(right)) {
        right.erase(0, 1);
        int right_len = right.length();
        right.erase(right_len - 1, 1);
      }
      Query::such_that_clauses.emplace_back(r, left, right, is_synonym_left, is_synonym_right);
    } else {
      throw ParseException();
    }
  }

  std::vector<RelationshipToken> Query::GetSuchThatClause() {
    return Query::such_that_clauses;
  }

  void Query::AddPattern(std::string assign_synonym, pql::Ref left, std::string expression, bool exact, bool is_synonym_left) {
    Query::patterns.emplace_back(PatternToken(std::move(assign_synonym), std::move(left), std::move(expression), exact, is_synonym_left));
  }

  std::vector<pql::PatternToken> Query::GetPattern() {
    return Query::patterns;
  }

  void Query::SetBoolean(bool b) {
    Query::is_boolean = b;
  }

  bool Query::GetBoolean() {
    return Query::is_boolean;
  }
}