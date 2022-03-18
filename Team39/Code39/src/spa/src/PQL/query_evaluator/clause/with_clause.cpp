#include <map>
#include <unordered_set>
#include <unordered_map>

#include "with_clause.h"

#define INVALID_INDEX -1
#define ENTITY 0
#define ATTR_REF 1

namespace pql_clause {
  typedef std::vector<int>(Pkb::* GetDomainByAttribute)(const int) const;
  typedef void (SuchThatClause::* EvaluateFn)(Pkb&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);
  typedef (Pkb::* GetAttrFn)(const int) const;

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

  const map<EntityIdentifier, GetAttrFn> GetAttrFnMap = {
    { EntityIdentifier::kCall, &Pkb::GetProcFromCall },
    { EntityIdentifier::kRead, &Pkb::GetVarFromRead },
    { EntityIdentifier::kPrint, &Pkb::GetVarFromPrint }
  };

  struct hashFunction {
    size_t operator()(const pair<int, int>& x) const {
      return x.first ^ x.second;
    }
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

  void WithClause::EvaluateAttrAttrNum(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    std::string left_syn_name = left_attr_->GetSynonym().GetName();
    std::string right_syn_name = right_attr_->GetSynonym().GetName();
    std::vector<int> left_domain = domain[left_syn_name];
    std::vector<int> right_domain = domain[right_syn_name];

    std::unordered_set<std::pair<int, int>, hashFunction> s;
    
    //the attribute value will be same as the int representation
    for (int& left_val : left_domain) {
      s.insert(std::make_pair(left_val, left_val));
    }

    for (int& right_val : right_domain) {
      s.insert(std::make_pair(right_val, right_val));
    }

    std::vector<std::pair<int, int>> pred_lst(s.begin(), s.end());
    pql_table::Predicate predicate(left_syn_name, right_syn_name, pred_lst);
    predicates.push_back(predicate);
  }
  
  int GetAttributeByType(Pkb& pkb, EntityIdentifier ent_type, int entity) {
    if (GetAttrFnMap.find(ent_type) != GetAttrFnMap.end()) {
      GetAttrFn fn = GetAttrFnMap.at(ent_type);
      return (pkb.*fn)(entity);
    }

    //Until here, we are left with proc.procName and variable.varName
    //which is the same as the entity
    return entity;
  }

  //This function maps the value to a list of entities such that entity.attribute = value
  std::unordered_map<int, std::vector<int>> GenerateAttrToEntitiesMap(Pkb& pkb, std::vector<int>& domain_lst, pql::AttrRef& attr_ref) {
    std::unordered_map<int, std::vector<int>> res_map;
    
    for (int& ent_val : domain_lst) {
      int attribute_val = GetAttributeByType(pkb, attr_ref.GetSynonym().GetDeclaration(), ent_val);

      if (res_map.find(attribute_val) == res_map.end()) {
        res_map[attribute_val] = std::vector<int> { ent_val };
      } else {
        res_map[attribute_val].push_back(ent_val);
      }
    } 

    return res_map;
  }

  void AddCrossProductIntoSet(std::unordered_set<std::pair<int, int>, hashFunction>& s, std::vector<int>& first, std::vector<int>& second) {
    for (int& first_val : first) {
      for (int& second_val : second) {
        s.insert(std::make_pair(first_val, second_val));
      }
    }
  }

  void WithClause::EvaluateAttrAttrVar(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<pql_table::Predicate>& predicates) {
    std::string left_syn_name = left_attr_->GetSynonym().GetName();
    std::string right_syn_name = right_attr_->GetSynonym().GetName();
    std::vector<int> left_domain = domain[left_syn_name];
    std::vector<int> right_domain = domain[right_syn_name];

    std::unordered_set<std::pair<int, int>, hashFunction> s;

    std::unordered_map<int, std::vector<int>> left_attr_ent_map = GenerateAttrToEntitiesMap(pkb, left_domain, *left_attr_);
    std::unordered_map<int, std::vector<int>> right_attr_ent_map = GenerateAttrToEntitiesMap(pkb, right_domain, *right_attr_);

    //We iterate through curr_map which has shorter length
    bool is_left_map_smaller = left_attr_ent_map.size() < right_attr_ent_map.size();
    std::unordered_map<int, std::vector<int>> curr_map = is_left_map_smaller ? left_attr_ent_map : right_attr_ent_map;
    std::unordered_map<int, std::vector<int>> other_map = !is_left_map_smaller ? left_attr_ent_map : right_attr_ent_map;

    for (std::map<int, std::vector<int>>::iterator curr_it = curr_map.begin(); curr_it != curr_map.end(); curr_it++) {
      int curr_attr_val = curr_it->first;

      if (other_map.find(curr_attr_val) == other_map.end()) {
        continue;
      }

      std::vector<int> curr_domain = curr_it->second;
      std::vector<int> other_domain = other_map[curr_attr_val];

      AddCrossProductIntoSet(s, curr_domain, other_domain);
    }

    std::string first = is_left_map_smaller ? left_syn_name : right_syn_name;
    std::string second = !is_left_map_smaller ? left_syn_name : right_syn_name;
    std::vector<std::pair<int, int>> pred_lst(s.begin(), s.end());
    pql_table::Predicate predicate(first, second, pred_lst);
    predicates.push_back(predicate);
  }

  void WithClause::EvaluateAttrAttr(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    if (left_attr_->GetAttrIndentifier() == AttrIdentifier::kStmtNum
        || left_attr_->GetAttrIndentifier() == AttrIdentifier::kValue) {
      EvaluateAttrAttrNum(pkb, domain, predicates);
    } else {
      EvaluateAttrAttrVar(pkb, domain, predicates);
    }
  }

  void WithClause::Evaluate(Pkb& pkb, std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<pql_table::Predicate>& predicates) {
    int left_type = GetArgumentType(is_attr_ref_left_);
    int right_type = GetArgumentType(is_attr_ref_right_);

    EvaluateFn fn = EvaluateFnMap.at(left_type).at(right_type);
    (this->*fn)(pkb, domain, predicates);
  }

}