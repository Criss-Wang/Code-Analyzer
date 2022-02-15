#include <vector>
#include <string>
#include <algorithm>


#include "predicate.h"
#include "Token.h"

namespace pql_table {

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

      bool equal(InterTable& t);
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