#include <vector>
#include <string>
#include <algorithm>


#include "predicate.h"
#include "Token.h"

namespace pql_table {
  
  typedef struct element {
    int val;
    std::string name;

    bool operator< (const element& e1, const element& e2) const {
      if (e1.val == e2.val) {
        return e1.name < e2.name;
      }
      else {
        return e1.val < e2.val;
      }
    }

    bool operator==(const element& e) const {
      return val == e.val && name == e.name;
    }
  } element;

  //specialized hash function for struct element
  struct hash_fn { 
    std::size_t operator() (const elemenet& e) const {
      std::size_t h1 = std::hash<int>()(e.val);
      std::size_t h2 = std::hash<std::string>()(e.name);

      return h1 ^ h2;
    }
  };

  //specialized hash function for pair of struct element
  struct hash_pair_fn {
    std::size_t operator() (const std::pair<element, element>& p) const {
      std::size_t h1 = std::hash<int>()(p.first.val);
      std::size_t h2 = std::hash<std::string>()(p.first.name);
      std::size_t h3 = std::hash<int>()(p.second.val);
      std::size_t h4 = std::hash<std::string>()(p.second.name);

      return h1 ^ h2 ^ h3 ^ h4;
    }
  };

  class InterTable {
    public:
      std::vector<pql::Synonym> header_;
      std::vector<std::vector<element>> rows_;
    
    public:
      InterTable(pql::Synonym& synonym, std::vector<int>& int_list);

      InterTable(pql::Synonym& synonym, std::vector<std::string>& str_list);

    public:
      int GetColNum();

      int GetRowNum();

      int FindSynCol(pql::Synonym& syn);

      void DeleteRow(int row_index);

      void DeleteCol(int col_index);

      void Deduplicate();

      void Merge(InterTable& t1);

      void Filter(Predicate& pred);

      void MergeAndFilter(InterTable& t1, Predicate& pred);

      bool equal(const InterTable& t);
  };

  /*
  template <class T>
  class Row {
    private:
      std::vector<T> elements_;

    public:
      Row(T val) {
        elements_.push_back(val);
      }

    public:
      int size() {
        return elements_.size();
      }
  }; 
  */
 
}