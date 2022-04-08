#include <unordered_set>
#include <string>

#include "query.h"
#include "../query_evaluator/clause/such_that_clause.h"
#include "../query_evaluator/clause/with_clause.h"
#include "../query_evaluator/clause/pattern_clause.h"

#define ATTR_REF -1
#define IDENT -2
#define INTEGER -3
#define INDEX_OF_ATTR_REF 0
#define INDEX_OF_ENTITY 1
#define INDEX_OF_TYPE 2

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
    if (s.length() > 1 && s[0] == '0') {
      return false;
    }

    return std::all_of(s.begin(), s.end(), pql::IsDigit);
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

  template <typename T>
  bool WithinUnorderedSet(std::unordered_set<T>* set, T t) {
    return set->find(t) != set->end();
  }

  std::string RemoveQuotationMarks(std::string& ident) {
    if (IsIdent(ident)) {
      ident.erase(0, 1);
      size_t len = ident.length();
      ident.erase(len - 1, 1);
    }
    return ident;
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

  std::unordered_set<EntityIdentifier> uses_and_modifies_p_left_domain ({
    EntityIdentifier::kProc,
    EntityIdentifier::kIdent
  });

  std::unordered_set<EntityIdentifier> vars({
    EntityIdentifier::kVariable,
    EntityIdentifier::kWildcard,
    EntityIdentifier::kIdent
  });

  std::unordered_set<EntityIdentifier> assign({
    EntityIdentifier::kAssign,
    EntityIdentifier::kWildcard,
    EntityIdentifier::kStmtNumber,
  });

  std::unordered_set<EntityIdentifier> modifies_s_left_domain({
    EntityIdentifier::kStmt,
    EntityIdentifier::kAssign,
    EntityIdentifier::kIf,
    EntityIdentifier::kWhile,
    EntityIdentifier::kCall,
    EntityIdentifier::kRead,
    EntityIdentifier::kWildcard,
    EntityIdentifier::kStmtNumber
  });

  std::unordered_set<EntityIdentifier> uses_s_left_domain({
    EntityIdentifier::kStmt,
    EntityIdentifier::kAssign,
    EntityIdentifier::kIf,
    EntityIdentifier::kWhile,
    EntityIdentifier::kCall,
    EntityIdentifier::kPrint,
    EntityIdentifier::kWildcard,
    EntityIdentifier::kStmtNumber
  });

  const std::unordered_map<pql::RelationshipTypes, std::unordered_set<EntityIdentifier>> left_synonym_domains {
    {kFollows, stmts},
    {kFollowsT, stmts},
    {kParent, stmts},
    {kParentT, stmts},
    {kUsesS, uses_s_left_domain},
    {kUsesP, uses_and_modifies_p_left_domain},
    {kModifiesS, modifies_s_left_domain},
    {kModifiesP, uses_and_modifies_p_left_domain},
    {kCalls, procs},
    {kCallsT, procs},
    {kNext, stmts},
    {kNextT, stmts},
    {kAffects, assign},
    {kAffectsT, assign}
  };

  const std::unordered_map<pql::RelationshipTypes, std::unordered_set<EntityIdentifier>> right_synonym_domains {
    {kFollows, stmts},
    {kFollowsT, stmts},
    {kParent, stmts},
    {kParentT, stmts},
    {kUsesS, vars},
    {kUsesP, vars},
    {kModifiesS, vars},
    {kModifiesP, vars},
    {kCalls, procs},
    {kCallsT, procs},
    {kNext, stmts},
    {kNextT, stmts},
    {kAffects, assign},
    {kAffectsT, assign}
  };
  
  const std::unordered_map<std::string, AttrIdentifier> attributeMap{
    {"value",      AttrIdentifier::kValue},
    {"stmt#",      AttrIdentifier::kStmtNum},
    {"varName",    AttrIdentifier::kVarName},
    {"procName",   AttrIdentifier::kProcName}
  };

  const std::unordered_map<EntityIdentifier, AttrIdentifier> defaultAttrMap{
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

  const std::unordered_map<EntityIdentifier, std::unordered_set<AttrIdentifier>> validAttrMap{
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

  std::unordered_map<AttrIdentifier, int> attrDomain {
      {AttrIdentifier::kValue, INTEGER},
      {AttrIdentifier::kVarName, IDENT},
      {AttrIdentifier::kProcName, IDENT},
      {AttrIdentifier::kStmtNum, INTEGER}
  };

  AttrIdentifier GetAttributeByString(const std::string& attr) {
    return attributeMap.at(attr);
  }

  std::shared_ptr<pql_clause::Clause> GenerateSuchThatClause(RelationshipTypes relationship,
                                                     std::string &left, std::string &right, bool is_synonym_left, bool is_synonym_right) {
    switch (relationship) {
      case RelationshipTypes::kFollows:
        return std::make_shared<pql_clause::FollowsClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kFollowsT:
        return std::make_shared<pql_clause::FollowsTClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kParent:
        return std::make_shared<pql_clause::ParentClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kParentT:
        return std::make_shared<pql_clause::ParentTClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kUsesS:
        return std::make_shared<pql_clause::UsesSClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kModifiesS:
        return std::make_shared<pql_clause::ModifiesSClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kUsesP:
        return std::make_shared<pql_clause::UsesPClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kModifiesP:
        return std::make_shared<pql_clause::ModifiesPClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kCalls:
        return std::make_shared<pql_clause::CallsClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kCallsT:
        return std::make_shared<pql_clause::CallsTClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kNext:
        return std::make_shared<pql_clause::NextClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kNextT:
        return std::make_shared<pql_clause::NextTClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kAffects:
        return std::make_shared<pql_clause::AffectsClause>(left, is_synonym_left, right, is_synonym_right);
      case RelationshipTypes::kAffectsT:
        return std::make_shared<pql_clause::AffectsTClause>(left, is_synonym_left, right, is_synonym_right);
      default:
        return nullptr;
    }
  }

  std::shared_ptr<pql_clause::Clause> GeneratePatternClause(EntityIdentifier syn_entity, std::string& synonym, std::string& left, bool is_synonym_left,
                                                            std::string& expression, bool exact) {
    switch(syn_entity) {
      case EntityIdentifier::kAssign:
        return std::make_shared<pql_clause::AssignPatternClause>(synonym, left, is_synonym_left, expression, exact);
      case EntityIdentifier::kWhile:
        return std::make_shared<pql_clause::WhilePatternClause>(synonym, left, is_synonym_left);
      case EntityIdentifier::kIf:
        return std::make_shared<pql_clause::IfPatternClause>(synonym, left, is_synonym_left);
      default:
        return nullptr;
    }
  }
  
  void Query::SetSemanticallyInvalid() {
    Query::is_semantically_valid_ = false;
  }

  bool Query::IsValidRelationshipArgument(const std::string& argument, std::unordered_set<EntityIdentifier> *domain) {
    if (Query::SynonymDeclared(argument) && !WithinUnorderedSet(domain, synonyms_.at(argument).GetDeclaration())) {
      return false;
    } else {
      bool entity_invalid = (pql::IsIdent(argument) && !WithinUnorderedSet(domain, EntityIdentifier::kIdent)) ||
          (pql::IsInteger(argument) && !WithinUnorderedSet(domain, EntityIdentifier::kStmtNumber));
      if (argument == "_" && WithinUnorderedSet(domain, EntityIdentifier::kWildcard)) {
        return false;
      } else if (entity_invalid) {
        throw ParseException();
      }
    }

    return true;
  }

  bool Query::IsValidRelationship(RelationshipTypes r, const std::string& left, const std::string& right) {
    bool is_left_valid = Query::SynonymDeclared(left) || left == "_" || pql::IsIdent(left) || pql::IsInteger(left);
    bool is_right_valid = Query::SynonymDeclared(right) || right == "_" || pql::IsIdent(right) || pql::IsInteger(right);

    if (!(is_left_valid && is_right_valid)) {
      return false;
    }

    std::unordered_set<EntityIdentifier> left_domains = pql::left_synonym_domains.at(r);
    std::unordered_set<EntityIdentifier> right_domains = pql::right_synonym_domains.at(r);

    return IsValidRelationshipArgument(left, &left_domains) && IsValidRelationshipArgument(right, &right_domains);
  }

  bool Query::SynonymDeclared(const std::string &name) {
    return synonyms_.find(name) != synonyms_.end();
  }

  std::shared_ptr<Synonym> Query::GetSynonymByName(const std::string &name) {
    return std::make_shared<Synonym>(synonyms_.at(name));
  }
  
  bool Query::IsAttrStringValid(const std::string& attribute) {
    return attributeMap.find(attribute) != attributeMap.end();
  }

  bool Query::IsAssignSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms_.at(name).GetDeclaration() == EntityIdentifier::kAssign;
  }

  bool Query::IsWhileSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms_.at(name).GetDeclaration() == EntityIdentifier::kWhile;
  }

  bool Query::IsIfSynonym(const std::string &name) {
    return Query::SynonymDeclared(name) && synonyms_.at(name).GetDeclaration() == EntityIdentifier::kIf;
  }

  void Query::AddSynonym(EntityIdentifier d, const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      Query::SetSemanticallyInvalid();
    } else {
      pql::Synonym sm = Synonym(name, d);
      declarations_.push_back(sm);
      synonyms_.insert(std::pair<std::string, pql::Synonym>(name, sm));
    }
  }
  
  void Query::AddResultSynonym(const std::string &name) {
    if (!Query::SynonymDeclared(name)) {
      Query::SetSemanticallyInvalid();
      return;
    }

    Query::AddUsedSynonym(name);
    Query::AddAttrRef(synonyms_.at(name));
  }

  void Query::AddResultSynonym(const std::string& name, const std::string& attribute) {
    if (!IsAttrStringValid(attribute)) {
      throw ParseException();
    }

    if (!Query::SynonymDeclared(name)){
      Query::SetSemanticallyInvalid();
      return;
    }

    Query::AddUsedSynonym(name);
    AttrIdentifier attr = attributeMap.at(attribute);
    Query::AddAttrRef(synonyms_.at(name), attr);
  }

  bool Query::IsProcedure(const std::string &name) {
    if (Query::SynonymDeclared(name)) {
      return synonyms_.at(name).GetDeclaration() == EntityIdentifier::kProc;
    }
    return false;
  }

  void Query::AddUsedSynonym(const std::string &name) {
    if (!Query::SynonymDeclared(name)) {
      Query::SetSemanticallyInvalid();
      return;
    }

    // check if the synonym is already used
    for (pql::Synonym s: used_synonyms_) {
      if (s.equal(synonyms_.at(name))) {
        return;
      }
    }

    used_synonyms_.push_back(synonyms_.at(name));
  }

  std::vector<pql::Synonym> Query::GetAllUsedSynonyms() {
    return used_synonyms_;
  }
  
  void Query::AddAttrRef(Synonym& s) {
    AttrIdentifier attr = defaultAttrMap.at(s.GetDeclaration()); 
    AttrRef attr_ref = AttrRef(s, attr);
    attr_refs_.push_back(attr_ref);
  }

  bool Query::IsAttrValidForSyn(Synonym& s, AttrIdentifier attr) {
    EntityIdentifier entity_id = s.GetDeclaration();
    std::unordered_set<AttrIdentifier> expected_attrs = validAttrMap.at(entity_id);
    return WithinUnorderedSet(&expected_attrs, attr);
  }

  void Query::AddAttrRef(Synonym& s, AttrIdentifier attr) {
    if (!IsAttrValidForSyn(s, attr)) {
      Query::SetSemanticallyInvalid();
      return;
    }

    AttrRef attr_ref = AttrRef(s, attr);
    attr_refs_.push_back(attr_ref);
  }

  std::vector<pql::AttrRef> Query::GetAttrRef() {
    return attr_refs_;
  }

  void Query::AddSuchThatClause(RelationshipTypes r, std::string &left, std::string &right, bool is_synonym_left, bool is_synonym_right) {
    if (!Query::IsValidRelationship(r, left, right)) {
      Query::SetSemanticallyInvalid();
      return;
    }

    left = RemoveQuotationMarks(left);
    right = RemoveQuotationMarks(right);

    clauses_.push_back(GenerateSuchThatClause(r, left, right, is_synonym_left, is_synonym_right));
  }

  void Query::AddPattern(EntityIdentifier syn_entity, std::string synonym, std::string left, std::string expression, bool exact) {
    bool is_synonym_left = Query::SynonymDeclared(left);

    if (is_synonym_left && synonyms_.at(left).GetDeclaration() != EntityIdentifier::kVariable) {
      Query::SetSemanticallyInvalid();
      return;
    }

    left = RemoveQuotationMarks(left);

    clauses_.push_back(GeneratePatternClause(syn_entity, synonym, left, is_synonym_left, expression, exact));
  }

  int GetWithArgumentDomain(std::tuple<std::shared_ptr<AttrRef>, std::string, int>* argument) {
    int domain;
    if (std::get<INDEX_OF_TYPE>(*argument) == ATTR_REF) {
      domain = attrDomain.at(std::get<INDEX_OF_ATTR_REF>(*argument)->GetAttrIdentifier());
    } else {
      domain = std::get<INDEX_OF_TYPE>(*argument);
    }

    return domain;
  }

  bool IsValidWith(std::tuple<std::shared_ptr<AttrRef>, std::string, int>& left, std::tuple<std::shared_ptr<AttrRef>, std::string, int>& right) {
    return GetWithArgumentDomain(&left) == GetWithArgumentDomain(&right);
  }

  void Query::AddWith(std::tuple<std::shared_ptr<AttrRef>, std::string, int>& left, std::tuple<std::shared_ptr<AttrRef>, std::string, int>& right) {
    if (!IsValidWith(left, right)) {
      Query::SetSemanticallyInvalid();
      return;
    }
    clauses_.push_back(
      std::make_shared<pql_clause::WithClause>(std::get<INDEX_OF_ATTR_REF>(left), std::get<INDEX_OF_ENTITY>(left),
          std::get<INDEX_OF_TYPE>(left) == ATTR_REF,std::get<INDEX_OF_ATTR_REF>(right),
          std::get<INDEX_OF_ENTITY>(right), std::get<INDEX_OF_TYPE>(right) == ATTR_REF));
  }

  std::vector <std::shared_ptr<pql_clause::Clause>> Query::GetClauses() {
    return clauses_;
  }

  void Query::SetBoolean(bool b) {
    is_boolean_ = b;
  }

  bool Query::GetBoolean() const {
    return is_boolean_;
  }

  bool Query::IsSemanticallyValid() const {
    return is_semantically_valid_;
  }
}