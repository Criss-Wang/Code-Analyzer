#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>

using namespace std;

#include "../PKB/pkb.h"
#include "QueryEvaluator.h"
#include "Token.h"

namespace pql {
    list<int> Intersect(const list<int>& lst1, const vector<int>& lst2);
    void UpdateHashmap(unordered_map<string, list<int>>& hmap, string name, const vector<int>& lst);
    bool IsNumber(const string& str);

    bool GetRelExist(RelationshipToken& token, Pkb& pkb);
    vector<int> GetRelDomain(RelationshipToken& token, Pkb& pkb);
    vector<int> GetInverseRelDomain(RelationshipToken& token, Pkb& pkb);

    list<string> evaluate(Query query, Pkb pkb) {
        vector<RelationshipToken> suchThatClauses = query.GetSuchThatClause();
        unordered_map<string, list<int>> hashmap;
        vector<Synonym> synonyms = query.GetAllUsedSynonyms();

        //hashmap stores <synonym.name, domain> pair.
        for (Synonym& synonym : synonyms) {
            unordered_set<int> domainSet = pkb.GetAllEntityInt(synonym.GetDeclaration());
            list<int> domainList(std::begin(domainSet), std::end(domainSet));
            hashmap.insert({ synonym.GetName(), domainList});
        }

        if (false) {
            //this is for pattern clauses, haven't implemented yet from parser side
            /*
            Synonym s = patternClause.getSynonym();
            list<int> matched;

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
                list<string> res{};
                return res;
            }
            */
        }

        for (RelationshipToken& token : suchThatClauses) {
            bool isLeftNum = IsNumber(token.GetLeft());
            bool isRightNum = IsNumber(token.GetRight());
            if (isLeftNum && isRightNum) {
                bool relDoesExist = GetRelExist(token, pkb);
                if (!relDoesExist) {
                    //the relationship does not exist
                    //return empty list
                    list<string> res{};
                    return res;
                }
            } else if (isLeftNum) {
                vector<int> relDomain = GetRelDomain(token, pkb);
                string name = token.GetRight();

                if (name == "_") {
                    //there's no synonym involved
                    //just need to check whether the relDomain is empty
                    if (relDomain.empty()) {
                        list<string> res{};
                        return res;
                    }
                } else {
                    UpdateHashmap(hashmap, name, relDomain);

                    if (hashmap[name].empty()) {
                        list<string> res{};
                        return res;
                    }
                }
                
            } else if (isRightNum) {
                vector<int> relDomain = GetInverseRelDomain(token, pkb);
                string name = token.GetRight();

                if (name == "_") {
                    //there's no synonym involved
                    //just need to check whether the relDomain is empty
                    if (relDomain.empty()) {
                        list<string> res{};
                        return res;
                    }
                } else {
                    UpdateHashmap(hashmap, name, relDomain);

                    if (hashmap[name].empty()) {
                        list<string> res{};
                        return res;
                    }
                }
            } else {

            }
        }

        Synonym selectedSyn = query.GetResultSynonym();
        list<int> selectedSynDomain = hashmap[selectedSyn.GetName()];
        list<string> res;

        for (int val : selectedSynDomain) {
            res.push_back(to_string(val));
        }

        return res;
    }

    void UpdateHashmap(unordered_map<string, list<int>>& hmap, string name, const vector<int>& lst) {
        list<int> oriLst = hmap[name];
        list<int> inter = Intersect(oriLst, lst);
        hmap[name] = inter;
    }

    list<int> Intersect(const list<int>& lst1, const vector<int>& lst2) {
        //refer from stack overflow https://stackoverflow.com/questions/38993415/how-to-apply-the-intersection-between-two-lists-in-c
        list<int> res;
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
                res.push_back(pkb.GetStmtRightBefore(left));
                break;
            case RelationshipTypes::kFollowsT:
                res = pkb.GetStmtsBefore(left);
                break;
            case RelationshipTypes::kParent:
                res.push_back(pkb.GetParent(left));
                break;
            case RelationshipTypes::kParentT:
                res = pkb.GetAllParents(left);
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
            res.push_back(pkb.GetStmtRightAfter(right));
            break;
        case RelationshipTypes::kFollowsT:
            res = pkb.GetStmtsAfter(right);
            break;
        case RelationshipTypes::kParent:
            res = pkb.GetChild(right);
            break;
        case RelationshipTypes::kParentT:
            res = pkb.GetAllChildren(right);
            break;
        default:
            break;
        }

        return res;
    }
}

