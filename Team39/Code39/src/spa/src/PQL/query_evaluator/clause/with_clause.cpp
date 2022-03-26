#include <map>
#include <unordered_set>
#include <unordered_map>

#include "with_clause.h"
#include "../query_evaluator_exceptions.h"

#define INVALID_INDEX -1
#define ENTITY 0
#define ATTR_REF 1

namespace pql_clause {
  typedef void (WithClause::* EvaluateFn)(pql_cache::Cache&, std::unordered_map<std::string, std::vector<int>>&, std::vector<pql_table::Predicate>&);

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

  struct hash_pair_fn {
    std::size_t operator() (const std::pair<int, int>& p) const {
      std::size_t h1 = std::hash<int>{}(p.first);
      std::size_t h2 = std::hash<int>{}(p.second);

      return h1 ^ h2;
    }
  };

  int GetArgumentType(bool is_attr_ref) {
      return is_attr_ref ? ATTR_REF : ENTITY;
  }

  void WithClause::EvaluateEntEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    bool is_equal = left_entity_ == right_entity_;

    if (!is_equal) {
      throw pql_exceptions::UnequalWithClauseException();
    }
  }

  int GetIntRepresentation(pql_cache::Cache& cache, AttrIdentifier attr_type, std::string& entity) {
    if (attr_type == AttrIdentifier::kProcName) {
      int proc_index = cache.GetIndexByString(IndexTableType::kProcIndex, entity);

      if (proc_index == INVALID_INDEX) {
        throw pql_exceptions::ProcedureDoesNotExistException();
      } else {
        return proc_index;
      }
    }

    if (attr_type == AttrIdentifier::kVarName) {
      int var_index = cache.GetIndexByString(IndexTableType::kVarIndex, entity);

      if (var_index == INVALID_INDEX) {
        throw pql_exceptions::VariableDoesNotExistException();
      } else {
        return var_index;
      }
    }

    //at this point the type would just be value/stmt#
    return stoi(entity);
  }

  const unordered_set<EntityIdentifier> StmtTypeWithStringAttrSet = {
    EntityIdentifier::kCall, EntityIdentifier::kRead, EntityIdentifier::kPrint
  };

  const unordered_set<AttrIdentifier> AttributeTypeAsStmtAttrSet = {
    AttrIdentifier::kVarName, AttrIdentifier::kProcName
  };

  std::vector<int> GetSynDomainByEntity(pql_cache::Cache& cache, pql::AttrRef& attr_ref, int entity) {
    //need to do extra work for read.varName, call.procName and print.varName 
    //since we are not directly comparing the value in domain
    EntityIdentifier syn_type = attr_ref.GetSynDeclaration();
    AttrIdentifier attr_type = attr_ref.GetAttrIdentifier();

    if (StmtTypeWithStringAttrSet.find(syn_type) != StmtTypeWithStringAttrSet.end()
        && AttributeTypeAsStmtAttrSet.find(attr_type) != AttributeTypeAsStmtAttrSet.end()) {
      return cache.GetStmtNumByStringAttribute(syn_type, entity);
    }

    return std::vector<int> { entity };
  }

  void WithClause::EvaluateEntAttr(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int left = GetIntRepresentation(cache, right_attr_->GetAttrIdentifier(), left_entity_);
    std::vector<int> syn_domain = GetSynDomainByEntity(cache, *right_attr_, left);
    UpdateHashmap<int>(domain, right_attr_->GetSynName(), syn_domain);
  }

  void WithClause::EvaluateAttrEnt(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    int right = GetIntRepresentation(cache, left_attr_->GetAttrIdentifier(), right_entity_);
    std::vector<int> syn_domain = GetSynDomainByEntity(cache, *left_attr_, right);
    UpdateHashmap<int>(domain, left_attr_->GetSynName(), syn_domain);
  }

  void WithClause::EvaluateAttrAttrNum(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    std::string left_syn_name = left_attr_->GetSynName();
    std::string right_syn_name = right_attr_->GetSynName();
    std::vector<int> left_domain = domain[left_syn_name];
    std::vector<int> right_domain = domain[right_syn_name];

    std::unordered_set<std::pair<int, int>, hash_pair_fn> s;
    
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

  //Before calling this function, we assume the attribute is of type string
  int GetAttributeByType(pql_cache::Cache& cache, EntityIdentifier ent_type, int entity) {
    //this is for print.varName, read.varName and call.procName
    if (StmtTypeWithStringAttrSet.find(ent_type) != StmtTypeWithStringAttrSet.end()) {
      return cache.GetStringAttribute(ent_type, entity);
    }

    //Until here, we are left with proc.procName and variable.varName
    //which is the same as the entity
    return entity;
  }

  //This function maps the value to a list of entities such that entity.attribute = value
  std::unordered_map<int, std::vector<int>> GenerateAttrToEntitiesMap(pql_cache::Cache& cache, std::vector<int>& domain_lst, pql::AttrRef& attr_ref) {
    std::unordered_map<int, std::vector<int>> res_map;
    
    for (int& ent_val : domain_lst) {
      int attribute_val = GetAttributeByType(cache, attr_ref.GetSynDeclaration(), ent_val);

      if (res_map.find(attribute_val) == res_map.end()) {
        res_map[attribute_val] = std::vector<int> { ent_val };
      } else {
        res_map[attribute_val].push_back(ent_val);
      }
    } 

    return res_map;
  }

  void AddCrossProductIntoSet(std::unordered_set<std::pair<int, int>, hash_pair_fn>& s, std::vector<int>& first, std::vector<int>& second) {
    for (int& first_val : first) {
      for (int& second_val : second) {
        s.insert(std::make_pair(first_val, second_val));
      }
    }
  }

  void WithClause::EvaluateAttrAttrVar(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<pql_table::Predicate>& predicates) {
    std::string left_syn_name = left_attr_->GetSynName();
    std::string right_syn_name = right_attr_->GetSynName();
    std::vector<int> left_domain = domain[left_syn_name];
    std::vector<int> right_domain = domain[right_syn_name];

    std::unordered_set<std::pair<int, int>, hash_pair_fn> s;

    std::unordered_map<int, std::vector<int>> left_attr_ent_map = GenerateAttrToEntitiesMap(cache, left_domain, *left_attr_);
    std::unordered_map<int, std::vector<int>> right_attr_ent_map = GenerateAttrToEntitiesMap(cache, right_domain, *right_attr_);

    //We iterate through curr_map which has shorter length
    bool is_left_map_smaller = left_attr_ent_map.size() < right_attr_ent_map.size();
    std::unordered_map<int, std::vector<int>> curr_map = is_left_map_smaller ? left_attr_ent_map : right_attr_ent_map;
    std::unordered_map<int, std::vector<int>> other_map = !is_left_map_smaller ? left_attr_ent_map : right_attr_ent_map;

    for (std::unordered_map<int, std::vector<int>>::iterator curr_it = curr_map.begin(); 
        curr_it != curr_map.end(); curr_it++) {
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

  void WithClause::EvaluateAttrAttr(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
      std::vector<pql_table::Predicate>& predicates) {
    if (left_attr_->GetAttrIdentifier() == AttrIdentifier::kStmtNum
        || left_attr_->GetAttrIdentifier() == AttrIdentifier::kValue) {
      EvaluateAttrAttrNum(cache, domain, predicates);
    } else {
      EvaluateAttrAttrVar(cache, domain, predicates);
    }
  }

  void WithClause::Evaluate(pql_cache::Cache& cache, std::unordered_map<std::string, std::vector<int>>& domain,
    std::vector<pql_table::Predicate>& predicates) {
    int left_type = GetArgumentType(is_attr_ref_left_);
    int right_type = GetArgumentType(is_attr_ref_right_);

    EvaluateFn fn = EvaluateFnMap.at(left_type).at(right_type);
    (this->*fn)(cache, domain, predicates);
  }

}