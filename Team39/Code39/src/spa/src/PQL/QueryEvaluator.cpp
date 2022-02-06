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

namespace pql {
    list<int> Intersect(const list<int>& lst1, const vector<int>& lst2);
    void UpdateHashmap(unordered_map<string, list<int>>& hmap, string name, const vector<int>& lst);
    bool IsNumber(const string& str);

    list<string> evaluate(Query query) {
        vector<RelationshipToken> suchThatClauses = query.GetSuchThatClause();
        unordered_map<string, list<int>> hashmap;
        list<Synonym> synonyms = query.GetAllSynonymsUsed();

        //hashmap stores <synonym.name, domain> pair.
        for (const Synonym& synonym : synonyms) {
            hashmap.insert({ synonym.GetName(), pkb::getAllEntity(synonym.GetDeclaration()) });
        }

        if (false) {
            //this is for pattern clauses, havent implemented yet from parser side
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

        for (const RelationshipToken& token : suchThatClauses) {
            bool isLeftNum = IsNumber(token.GetLeft());
            bool isRightNum = IsNumber(token.GetRight());
            if (isLeftNum && isRightNum) {
                bool relDoesExist = GetRelExist(token);
                if (!relDoesExist) {
                    //the relationship does not exist
                    //return empty list
                    list<string> res{};
                    return res;
                }
            } else if (isLeftNum) {
                vector<int> relDomain = GetRelDomain(token);
                string name = token.GetRight();

                if (name == "_") {
                    //there's no synonym involved
                    //just need to check whether the relDomain is emtpty
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
                vector<int> relDomain = GetInverseRelDomain(token);
                string name = token.GetRight();

                if (name == "_") {
                    //there's no synonym involved
                    //just need to check whether the relDomain is emtpty
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

    bool GetRelExist(const RelationshipToken& token) {
        int left = stoi(token.GetLeft());
        int right = stoi(token.GetRight());
        bool res = false;
        switch (token.GetRelationship()) {
            case RelationshipTypes::FOLLOWS:
                res = pkb::IsFollows(left, right);
                break;
            case RelationshipTypes::FOLLOWS_T:
                res = pkb::IsTransitiveFollows(left, right);
                break;
            case RelationshipTypes::PARENT:
                res = pkb::IsParent(left, right);
                break;
            case RelationshipTypes::PARENT_T:
                res = pkb::IsTransitiveParent(left, right);
                break;
            default:
                break;
        }

        return res;
    }

    bool GetRelDomain(const RelationshipToken& token) {
        int left = stoi(token.GetLeft());
        vector<int> res;
        switch (token.GetRelationship()) {
            case RelationshipTypes::FOLLOWS:
                res.push_back(pkb::GetStmtRightBefore(left));
                break;
            case RelationshipTypes::FOLLOWS_T:
                res = pkb::GetStmtBefore(left);
                break;
            case RelationshipTypes::PARENT:
                res.push_back(pkb::GetParent(left));
                break;
            case RelationshipTypes::PARENT_T:
                res = pkb::GetAllParents(left);
                break;
            default:
                break;
        }

        return res;
    }

    bool GetInverseRelDomain(const RelationshipToken& token) {
        int right = stoi(token.GetRight());
        vector<int> res;
        switch (token.GetRelationship()) {
        case RelationshipTypes::FOLLOWS:
            res.push_back(pkb::GetStmtRightAfter(right));
            break;
        case RelationshipTypes::FOLLOWS_T:
            res = pkb::GetStmtAfter(right);
            break;
        case RelationshipTypes::PARENT:
            res = pkb::GetChild(right);
            break;
        case RelationshipTypes::PARENT_T:
            res = pkb::GetAllChildren(right);
            break;
        default:
            break;
        }

        return res;
    }
}

