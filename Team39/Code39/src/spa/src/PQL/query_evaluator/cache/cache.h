#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../query_parser/token.h"
#include "../../../PKB/pkb.h"
#include "../../../Utility/CFG/control_flow_graph.h"

//Cache computes the Next* and Affect relationship
//It will stores the computed result to speed up the evaulaution process
namespace pql_cache {
  struct hash_pair_fn {
    std::size_t operator() (const std::pair<int, int>& p) const {
      std::size_t h1 = std::hash<int>{}(p.first);
      std::size_t h2 = std::hash<int>{}(p.second);

      return h1 ^ h2;
    }
  };

  const unordered_set<pql::RelationshipTypes> cache_store_type = {
    pql::kNextT, pql::kAffects, pql::kAffectsT
  };

  class Cache {
    private:
      //pair_cache_ stores the pair relationship such that all <x,y> in vector satisfy the relationship
      //For clauses like Next(syn, syn) 
      std::unordered_map<pql::RelationshipTypes, std::vector<std::pair<int, int>>> pair_cache_;
      std::unordered_map<pql::RelationshipTypes, bool> pair_cache_boolean_;
      //rel_cache_ stores the table mapping the key to vector of values, where <key, value> satisfy the relationship
      //For clauses like Next(ent, syn) or Next(ent, ent)
      std::unordered_map<pql::RelationshipTypes, std::unordered_map<int, std::vector<int>>> rel_cache_;
      std::unordered_map<pql::RelationshipTypes, bool> rel_cache_boolean_;
      //inverse_rel_cache_ stores the table mapping the key to vector of values, where <value, key> satisfy the relationship
      //For clauses like Next(syn, ent)
      std::unordered_map<pql::RelationshipTypes, std::unordered_map<int, std::vector<int>>> inverse_rel_cache_;
      std::unordered_map<pql::RelationshipTypes, bool> inverse_rel_cache_boolean_;

      Pkb pkb_;

    public:
      Cache(Pkb& pkb) {
        pkb_ = pkb;

        //we havent create any table for these types
        for (auto type : cache_store_type) {
          pair_cache_boolean_[type] = false;
          rel_cache_boolean_[type] = false;
          inverse_rel_cache_boolean_[type] = false;
        }
      }

    public:
    /*-------------------------------------------------------API for clause---------------------------------------------------------------*/
      bool IsRelHolds(pql::RelationshipTypes type, int left, int right);

      std::vector<int> GetRelDomain(pql::RelationshipTypes type, int left);

      std::vector<int> GetInverseRelDomain(pql::RelationshipTypes type, int right);

      std::vector<std::pair<int, int>> GetAllRelPairs(pql::RelationshipTypes type);

      bool isRelExists(pql::RelationshipTypes type);
    /*---------------------------------------------------Next* and Affects*----------------------------------------------------------*/
      void GenerateNextTRelDomain();

      void GenerateNextTOrAffectsTRelDomain(pql::RelationshipTypes type);

      void GenerateNextTOrAffectsTInverseRelDomain(pql::RelationshipTypes type);

      void GenerateNextTOrAffectsTPairDomain(pql::RelationshipTypes type);
    /*---------------------------------------------------------Affect---------------------------------------------------------------*/
      void GenerateAffectsRelDomain(pql::RelationshipTypes type);

      void GenerateAffectsInverseRelDomain(pql::RelationshipTypes type);

      void GenerateAffectsPairDomain(pql::RelationshipTypes type);

      std::vector<std::pair<int, int>> ComputeAffectsRelationship(GraphNode& head);

      void ConstructAssignAffectPair(int assign_stmt,
         std::unordered_map<int, std::unordered_set<int>>& last_modified_table, unordered_set<pair<int, int>, hash_pair_fn>& affect_lst);
      
      static void MergeTable(unordered_map<int, unordered_set<int>>& dst, unordered_map<int, unordered_set<int>>& src);      
  };
}
