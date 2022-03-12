#include <vector>
#include <string>
#include <algorithm>


#include "predicate.h"
#include "PQL/query_parser/token.h"

namespace pql_table {

  class InterTable {
    public:
      std::vector<std::string> header_;
      std::vector<std::vector<element>> rows_;
    
    public:
      InterTable(pql::Synonym& synonym, std::vector<int>& int_list);

      InterTable(pql::Synonym& synonym, std::vector<std::string>& str_list);

      InterTable(std::vector<std::string>& header, std::vector<std::vector<element>>& rows);

    public:
      int GetColNum();

      int GetRowNum();

      int FindSynCol(std::string& syn_name);

      std::vector<element> GetColByName(std::string& name);

      InterTable GetColsByIndices(std::vector<int>& valid_col_nums);

      InterTable Deduplicate();

      InterTable Merge(InterTable& t1);

      InterTable Filter(Predicate& pred);

      InterTable MergeAndFilter(InterTable& t1, Predicate& pred);

      bool equal(InterTable& t);

    private:
      void DeleteRow(int row_index);

      void DeleteCol(int col_index);
  }; 
}
