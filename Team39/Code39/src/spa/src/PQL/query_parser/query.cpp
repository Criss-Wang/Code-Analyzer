#include "query.h"
#include "../query_evaluator/clause/such_that_clause.h"
#include "../query_evaluator/clause/with_clause.h"
#include "../query_evaluator/clause/pattern_clause.h"

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
    if (s.length() > 1 && ssm.get() == '0') {
      return false;
    }
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

  bool IsHash(char c) {
    return c == '#';
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
  
  const std::map<std::string, AttrIdentifier> attributeMap{
    {"value",      AttrIdentifier::kValue},
    {"stmt#",      AttrIdentifier::kStmtNum},
    {"varName",    AttrIdentifier::kVarName},
    {"procName",   AttrIdentifier::kProcName}
  };

  const std::map<EntityIdentifier, AttrIdentifier> defaultAttrMap{
    {EntityIdentifier::kProc,      AttrIdentifier::kProcName},
    {EntityIdentifier::kVariable,  AttrIdentifier::kVarName},
    {EntityIdentifier::kConstant,  AttrIdentifier::kValue},
    {EntityIdentifier::kCall,      AttrIdentifier::kStmtNum},
    {EntityIdentifier::kRead,      AttrIdentifier::kStmtNum},
    {EntityIdentifier::kPrint,     AttrIdentifier::kStmtNum},
    {EntityIdentifier::kWhile,     AttrIdentifier::kStmtNum},
    {EntityIdentifier::kIf,        AttrIdentifier::kStmtNum},
    {EntityIdentifier::kAssign,    AttrIdentifier::kStmtNum},
    {EntityIdentifier::kStmt,      AttrIdentifier::kStmtNum}
  };

  std::unordered_set<AttrIdentifier> valid_attr_proc({
    AttrIdentifier::kProcName
  });

  std::unordered_set<AttrIdentifier> valid_attr_var({
    AttrIdentifier::kVarName
  });

  std::unordered_set<AttrIdentifier> valid_attr_const({
    AttrIdentifier::kValue
  });

  std::unordered_set<AttrIdentifier> valid_attr_call({
    AttrIdentifier::kProcName,
    AttrIdentifier::kStmtNum
  });

  std::unordered_set<AttrIdentifier> valid_attr_read_print({
    AttrIdentifier::kVarName,
    AttrIdentifier::kStmtNum
  });

  std::unordered_set<AttrIdentifier> valid_attr_stmt({
    AttrIdentifier::kStmtNum
  });

  const std::map<EntityIdentifier, std::unordered_set<AttrIdentifier>> validAttrMap{
    {EntityIdentifier::kProc,     valid_attr_proc},
    {EntityIdentifier::kVariable, valid_attr_var},
    {EntityIdentifier::kConstant, valid_attr_const},
    {EntityIdentifier::kCall,     valid_attr_call},
    {EntityIdentifier::kRead,     valid_attr_read_print},
    {EntityIdentifier::kPrint,    valid_attr_read_print},
    {EntityIdentifier::kWhile,    valid_attr_stmt},
    {EntityIdentifier::kIf,       valid_attr_stmt},
    {EntityIdentifier::kAssign,   valid_attr_stmt},
    {EntityIdentifier::kStmt,     valid_attr_stmt}
  };

  AttrIdentifier GetAttributeByString(const std::string& attr) {
    return attributeMap.at(attr);
  }

  std::unique_ptr<pql_clause::Clause> GenerateClause(RelationshipTypes relationship,
                                                     std::string &left, std::string &right, bool is_synonym_left, bool is_synonym_right) {
    switch (relationship) {
      case RelationshipTypes::kFollows:
        return std::make_unique<pql_clause::FollowsClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kFollowsT:
        return std::make_unique<pql_clause::FollowsTClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kParent:
        return std::make_unique<pql_clause::ParentClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kParentT:
        return std::make_unique<pql_clause::ParentTClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kUsesS:
        return std::make_unique<pql_clause::UsesSClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kModifiesS:
        return std::make_unique<pql_clause::ModifiesSClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kUsesP:
        return std::make_unique<pql_clause::UsesPClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kModifiesP:
        return std::make_unique<pql_clause::ModifiesPClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kCalls:
        return std::make_unique<pql_clause::CallsClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kCallsT:
        return std::make_unique<pql_clause::CallsTClause>(left, is_synonym_left, right, is_synonym_right);
      default:
        throw pql_exceptions::EmptyDomainException();
    }
  }
  
  void Query::SetSemanticallyInvalid() {
    Query::is_semantically_valid = false;
  }

  bool Query::IsValid(RelationshipTypes r, const std::string& left, const std::string& right) {
    std::unordered_set<EntityIdentifier> left_domains = pql::left_synonym_domains.at(r);
    std::unordered_set<EntityIdentifier> right_domains = pql::right_synonym_domains.at(r);
    if (Query::SynonymDeclared(left)) {
      if (left_domains.find(Query::synonyms.at(left).GetDeclaration()) == left_domains.end()) {
        return false;
      }
      if ((r == RelationshipTypes::kModifiesS && Query::synonyms.at(left).GetDeclaration() == EntityIdentifier::kPrint) ||
          (r == RelationshipTypes::kUsesS && Query::synonyms.at(left).GetDeclaration() == EntityIdentifier::kRead)) {
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

  Synonym Query::GetSynonymByName(const std::string &name) {
    return synonyms.at(name);
  }
  
  bool Query::IsAttrStringValid(const std::string& attribute) {
    return attributeMap.find(attribute) != attributeMap.end();
  }

  bool Query::IsAssignSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms.at(name).GetDeclaration() == EntityIdentifier::kAssign;
  }

  bool Query::IsWhileSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms.at(name).GetDeclaration() == EntityIdentifier::kWhile;
  }

  bool Query::IsIfSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms.at(name).GetDeclaration() == EntityIdentifier::kIf;
  }

  void Query::AddSynonym(EntityIdentifier d, const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      Query::SetSemanticallyInvalid();
    } else {
      pql::Synonym sm = Synonym(name, d);
      Query::declarations.push_back(sm);
      Query::synonyms.insert(std::pair<std::string, pql::Synonym>(name, sm));
    }
  }
  
  void Query::AddResultSynonym(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      Query::AddUsedSynonym(name);
      Query::AddAttrRef(Query::synonyms.at(name));
    } else {
      Query::SetSemanticallyInvalid();
    }
  }

  void Query::AddResultSynonym(const std::string& name, const std::string& attribute) {
    if (Query::SynonymDeclared(name) && IsAttrStringValid(attribute)) {
      Query::AddUsedSynonym(name);
      AttrIdentifier attr = attributeMap.at(attribute);
      Query::AddAttrRef(Query::synonyms.at(name), attr);
    } else if (!IsAttrStringValid(attribute)) {
      throw ParseException();
    } else {
      Query::SetSemanticallyInvalid();
    }
  }

  bool Query::IsProcedure(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      return Query::synonyms.at(name).GetDeclaration() == EntityIdentifier::kProc;
    }
    return false;
  }

  void Query::AddUsedSynonym(const std::string &name) {
    if (!Query::SynonymDeclared(name)) {
      Query::SetSemanticallyInvalid();
      return;
    }
    for (pql::Synonym s: Query::used_synonyms) {
      if (s.equal(Query::synonyms.at(name))) {
        return;
      }
    }
    Query::used_synonyms.push_back(Query::synonyms.at(name));
  }

  std::vector<pql::Synonym> Query::GetAllUsedSynonyms() {
    return Query::used_synonyms;
  }
  
  void Query::AddAttrRef(Synonym s) {
    AttrIdentifier attr = defaultAttrMap.at(s.GetDeclaration()); 
    AttrRef attr_ref = AttrRef(s, attr);
    Query::attr_refs.push_back(attr_ref);
  }

  void Query::AddAttrRef(Synonym s, AttrIdentifier attr) {
    EntityIdentifier entity_id = s.GetDeclaration();
    std::unordered_set<AttrIdentifier> expected_attrs = validAttrMap.at(entity_id);
    bool isAttrExpected = expected_attrs.find(attr) != expected_attrs.end();
    if (isAttrExpected) {
      AttrRef attr_ref = AttrRef(s, attr);
      Query::attr_refs.push_back(attr_ref);
    } else {
      Query::SetSemanticallyInvalid();
    }
  }

  std::vector<pql::AttrRef> Query::GetAttrRef() {
    return Query::attr_refs;
  }

  void Query::AddSuchThatClause(RelationshipTypes r, std::string &left, std::string &right, bool is_synonym_left, bool is_synonym_right) {
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
      Query::clauses.push_back(GenerateClause(r, left, right, is_synonym_left, is_synonym_right));
    } else {
      Query::SetSemanticallyInvalid();
    }
  }

  void Query::AddPattern(EntityIdentifier syn_entity, std::string synonym, std::string left, std::string expression, bool exact) {
    bool is_synonym_left = Query::SynonymDeclared(left);
    if (is_synonym_left && Query::synonyms.at(left).GetDeclaration() != EntityIdentifier::kVariable) {
      Query::SetSemanticallyInvalid();
    } else {
      if (IsIdent(left)) {
        left.erase(0, 1);
        int left_len = left.length();
        left.erase(left_len - 1, 1);
      }
      if (syn_entity == EntityIdentifier::kAssign) {
        Query::clauses.push_back(std::make_unique<pql_clause::AssignPatternClause>(synonym, left, is_synonym_left, expression, exact));
      } else if (syn_entity == EntityIdentifier::kWhile) {
        // add WhilePatternClause
      } else if (syn_entity == EntityIdentifier::kIf) {
        // add IfPatternClause
      }
    }
  }

  void Query::AddWith(std::optional<AttrRef> left_attr, std::optional<std::string> left_entity, bool is_attr_ref_left,
               std::optional<AttrRef> right_attr, std::optional<std::string> right_entity, bool is_attr_ref_right) {
    Query::clauses.push_back(
        std::make_unique<pql_clause::WithClause>(
            &(*left_attr), *left_entity, is_attr_ref_left, &(*right_attr), *right_entity, is_attr_ref_right));
  }

  std::vector <std::unique_ptr<pql_clause::Clause>> Query::GetClauses() {
    return Query::clauses;
  }

  void Query::SetBoolean(bool b) {
    Query::is_boolean = b;
  }

  bool Query::GetBoolean() {
    return Query::is_boolean;
  }
}