#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "QueryEvaluator.h"

using namespace std;

namespace pql {
  vector<int> Intersect(const vector<int>& lst1, const vector<int>& lst2);
  void UpdateHashmap(unordered_map<string, vector<int>>& hmap, string name, const vector<int>& lst);
  bool IsNumber(const string& str);

  bool GetRelExist(RelationshipToken& token, Pkb& pkb);
  vector<int> GetRelDomain(RelationshipToken& token, Pkb& pkb);
  vector<int> GetInverseRelDomain(RelationshipToken& token, Pkb& pkb);

  std::vector<std::string> EvaluateQuery(Query& query, Pkb& pkb) {
    vector<RelationshipToken> such_that_clauses = query.GetSuchThatClause();
    unordered_map<string, vector<int>> hashmap;
    vector<Synonym> synonyms = query.GetAllUsedSynonyms();

    //hashmap stores <synonym.name, domain> pair.
    for (Synonym& synonym : synonyms) {
      unordered_set<int> domain_set = pkb.GetAllEntityInt(synonym.GetDeclaration());
      vector<int> domain_list(std::begin(domain_set), std::end(domain_set));
      hashmap.insert({ synonym.GetName(), domain_list });
    }

    if (false) {
      //this is for pattern clauses, haven't implemented yet from parser side
      /*
      Synonym s = patternClause.getSynonym();
      vector<int> matched;

      if (patternClause.getExactMatch()) {
          matched = PKB::getExactMatchAssign(patternClause.getVar(), patternClause.getExpr());
      }
      else {
          matched = PKB::getSubMatchAssign(patternClause.getVar(), patternClause.getExpr());
      }

      UpdateHashmap(hashmap, s.name, matched);

      if (hashmap[s.name].empty()) {
          //there is no assign statement that satisfy the pattern
          //return empty list
          vector<string> res{};
          return res;
      }
      */

    }

    for (RelationshipToken& token : such_that_clauses) {
      bool is_left_num = IsNumber(token.GetLeft());
      bool is_right_num = IsNumber(token.GetRight());
      if (is_left_num && is_right_num) {
        bool rel_does_exist = GetRelExist(token, pkb);
        if (!rel_does_exist) {
          //the relationship does not exist
          //return empty list
          vector<string> res{};
          return res;
        }
      } else if (is_left_num) {
        vector<int> rel_domain = GetRelDomain(token, pkb);
        string name = token.GetRight();

        if (name == "_") {
          //there's no synonym involved
          //just need to check whether the rel_domain is empty
          if (rel_domain.empty()) {
            vector<string> res{};
            return res;
          }
        } else {
          UpdateHashmap(hashmap, name, rel_domain);

          if (hashmap[name].empty()) {
            vector<string> res{};
            return res;
          }
        }

      } else if (is_right_num) {
        vector<int> rel_domain = GetInverseRelDomain(token, pkb);
        string name = token.GetLeft();

        if (name == "_") {
          //there's no synonym involved
          //just need to check whether the relDomain is empty
          if (rel_domain.empty()) {
            vector<string> res{};
            return res;
          }
        } else {    
          UpdateHashmap(hashmap, name, rel_domain);

          if (hashmap[name].empty()) {
            vector<string> res{};
            return res;
          }
        }
      } else {

      }
    }

    Synonym selected_syn = query.GetResultSynonym();
    vector<int> selected_syn_domain = hashmap[selected_syn.GetName()];
    vector<string> res;

    for (int val : selected_syn_domain) {
      res.push_back(to_string(val));
    }

    return res;
  }

  void UpdateHashmap(unordered_map<string, vector<int>>& hmap, string name, const vector<int>& lst) {
    vector<int> oriLst = hmap[name];
    vector<int> inter = Intersect(oriLst, lst);
    hmap[name] = inter;
  }

  vector<int> Intersect(const vector<int>& lst1, const vector<int>& lst2) {
    //refer from stack overflow https://stackoverflow.com/questions/38993415/how-to-apply-the-intersection-between-two-lists-in-c
    vector<int> res;
    unordered_set<int> st;
    for_each(lst2.begin(), lst2.end(), [&st](const int& k) { st.insert(k); });
    for_each(lst1.begin(), lst1.end(),
      [&st, &res](const int& k) {
        auto iter = st.find(k);
        if (iter != st.end()) {
          res.push_back(k);
          st.erase(iter);
        }
      }
    );

    return res;
  }

  bool IsNumber(const string& str) {
    for (char const& c : str) {
      if (isdigit(c) == 0) {
        return false;
      }
    }
    return true;
  }

  bool GetRelExist(RelationshipToken& token, Pkb& pkb) {
    int left = stoi(token.GetLeft());
    int right = stoi(token.GetRight());
    bool res = false;
    switch (token.GetRelationship()) {
      case RelationshipTypes::kFollows:
        res = pkb.IsFollows(left, right);
        break;
      case RelationshipTypes::kFollowsT:
        res = pkb.IsTransitiveFollows(left, right);
        break;
      case RelationshipTypes::kParent:
        res = pkb.IsParent(left, right);
        break;
      case RelationshipTypes::kParentT:
        res = pkb.IsTransitiveParent(left, right);
        break;
      default:
        break;
    }

    return res;
  }

  vector<int> GetRelDomain(RelationshipToken& token, Pkb& pkb) {
    int left = stoi(token.GetLeft());
    vector<int> res;
    switch (token.GetRelationship()) {
      case RelationshipTypes::kFollows:
        res.push_back(pkb.GetStmtRightAfter(left));
        break;
      case RelationshipTypes::kFollowsT:
        res = pkb.GetStmtsAfter(left);
        break;
      case RelationshipTypes::kParent:
        res = pkb.GetChild(left);
        break;
      case RelationshipTypes::kParentT:
        res = pkb.GetAllChildren(left);
        break;
      default:
        break;
    }

    return res;
  }

  vector<int> GetInverseRelDomain(RelationshipToken& token, Pkb& pkb) {
    int right = stoi(token.GetRight());
    vector<int> res;
    switch (token.GetRelationship()) {
      case RelationshipTypes::kFollows:
        res.push_back(pkb.GetStmtRightBefore(right));
        break;
      case RelationshipTypes::kFollowsT:
        res = pkb.GetStmtsBefore(right);
        break;
      case RelationshipTypes::kParent:
        res = pkb.GetParent(right);
        break;
      case RelationshipTypes::kParentT:
        res = pkb.GetAllParents(right);
        break;
      default:
        break;
    }

    return res;
  }
}

