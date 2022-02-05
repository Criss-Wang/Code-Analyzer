#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_multiset>

using namespace std;

#include "Query.h"
#include "PKB.h"

vector<string> evaluate(Query query) {
	Clause suchThatClause = query.getSuchThatClause();
	Clause patternClause = query.getPatternClause();
	unordered_map<Synonym, list<int>> hashmap;
	vector<Synonym> synonyms = query.getAllSynonymsUsed();

	//hashmap stores <synonym, domain> pair.
	for (const Synonym& synonym : synonyms) {
		hahsmap.insert({ synonym, PKB::getAllEntity(synonym.entType)});
	}

	if (patternClause != NULL) {
        Synonym s = patternClause.getSynonym();

        
	}

}

void updateHashmap(const unordered_map& hashmap, const Synonym & syn, const list<int> & lst) {
    //took from stack overflow https://stackoverflow.com/questions/38993415/how-to-apply-the-intersection-between-two-lists-in-c
    list<int> res;
    lst<int> oriDomain = hashmap[*syn];
    unordered_multiset<int> st;
    for_each(lst.begin(), lst.end(), [&st](const T& k) { st.insert(k); });
    for_each(oriDomain.begin(), oriDomain.end(),
        [&st, &res](const T& k) {
            auto iter = st.find(k);
            if (iter != st.end()) {
                rtn.push_back(k);
                st.erase(iter);
            }
        }
    );
    
    hashmap.emplace(*syn, res);
    
}

