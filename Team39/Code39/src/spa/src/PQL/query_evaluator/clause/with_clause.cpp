#include <map>

#include "with_clause.h"

#define INVALID_INDEX -1
#define ENTITY 0
#define ATTR_REF 1

namespace pql_clause {
  typedef std::vector<int>(Pkb::* GetDomainByAttribute)(const int) const;
  typedef void (SuchThatClause::* EvaluateFn)(Pkb&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);

  const map<AttrIdentifier, GetDomainByAttribute> CallFnMap = {
    { AttrIdentifier::kProcName, &Pkb::GetCallFromProc }
  };

  const map<AttrIdentifier, GetDomainByAttribute> ReadFnMap = {
    { AttrIdentifier::kVarName, &Pkb::GetReadByVar }
  };

  const map<AttrIdentifier, GetDomainByAttribute> PrintFnMap = {
    { AttrIdentifier::kVarName, &Pkb::GetPrintByVar }
  };

  const map<EntityIdentifier, map<AttrIdentifier, GetDomainByAttribute>> GetDomainByAttributeMap = {
    { EntityIdentifier::kCall, CallFnMap },
    { EntityIdentifier::kRead, ReadFnMap },
    { EntityIdentifier::kPrint, PrintFnMap }
  };

  const map<int, EvaluateFn> EntEvaluateFnMap = {
    { ENTITY   , &WithClause::EvaluateEntEnt    },
    { ATTR_REF , &WithClause::EvaluateEntAttr   }
  };

  const map<int, EvaluateFn> AttrEvaluateFnMap = {
    { ENTITY   , &WithClause::EvaluateAttrEnt   },
    { ATTR_REF , &WithClause::EvaluateAttrAttr  }
  };

  const map<int, map<int, EvaluateFn>> EvaluateFnMap = {
    { ENTITY, EntEvaluateFnMap },
    { ATTR_REF,  AttrEvaluateFnMap }
  };

  int GetArgumentType(bool is_attr_ref) {
      return is_attr_ref ? ATTR_REF : ENTITY;
  }

  void WithClause::EvaluateEntEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    bool is_equal = left_entity_ == right_entity_;

    if (!is_equal) {
      throw pql_exceptions::UnequalWithClauseException();
    }
  }

  int GetIntRepresentation(Pkb& pkb, AttrIdentifier attr_type, std::string& entity) {
    if (attr_type == AttrIdentifier::kProcName) {
      int proc_index = pkb.GetIndexByProc(entity);

      if (proc_index == INVALID_INDEX) {
        throw pql_exceptions::ProcedureDoesNotExistException();
      } else {
        return proc_index;
      }
    }

    if (attr_type == AttrIdentifier::kVarName) {
      int var_index = pkb.GetIndexByVar(entity);

      if (var_index == INVALID_INDEX) {
        throw pql_exceptions::VariableDoesNotExistException();
      } else {
        return var_index;
      }
    }

    //at this point the type would just be value/stmt#
    return stoi(entity);
  }

  std::vector<int> GetSynDomainByEntity(Pkb& pkb, pql::AttrRef& attr_ref, int entity) {
    //need to do extra work for read.varName, call.procName and print.varName 
    //since we are not directly comparing the value in domain
    EntityIdentifier syn_type = attr_ref.GetSynonym().GetDeclaration();
    AttrIdentifier attr_type = attr_ref.GetAttrIndentifier();

    if (GetDomainByAttributeMap.find(syn_type) != GetDomainByAttributeMap.end()
        && GetDomainByAttributeMap.at(syn_type).find(attr_type) != GetDomainByAttributeMap.at(syn_type).end()) {
      GetDomainByAttribute fn = GetDomainByAttributeMap.at(syn_type).at(attr_type);
      return (pkb.*fn)(entity);
    }

    return std::vector<int> { entity };
  }

  void WithClause::EvaluateEntAttr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int left = GetIntRepresentation(pkb, right_attr_->GetAttrIndentifier(), left_entity_);
    std::vector<int> syn_domain = GetSynDomainByEntity(pkb, *right_attr_, left);
    UpdateHashmap<int>(domain, right_attr_->GetSynonym().GetName(), syn_domain);
  }

  void WithClause::EvaluateAttrEnt(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int right = GetIntRepresentation(pkb, left_attr_->GetAttrIndentifier(), right_entity_);
    std::vector<int> syn_domain = GetSynDomainByEntity(pkb, *left_attr_, right);
    UpdateHashmap<int>(domain, left_attr_->GetSynonym().GetName(), syn_domain);
  }

  void WithClause::EvaluateAttrAttr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    
  }

  void WithClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<pql_table::Predicate>& predicates) {
    int left_type = GetArgumentType(is_attr_ref_left_);
    int right_type = GetArgumentType(is_attr_ref_right_);

    EvaluateFn fn = EvaluateFnMap.at(left_type).at(right_type);
    (this->*fn)(pkb, domain, predicates);
  }

}