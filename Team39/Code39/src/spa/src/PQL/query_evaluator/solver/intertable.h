#ifndef INTERTABLE_H
#define INTERTABLE_H

#include <vector>
#include <string>
#include <algorithm>

#include "predicate.h"

namespace pql_table {

  class InterTable {
    public:
      std::vector<std::string> header_;
      std::vector<std::vector<int>> rows_;
    
    public:
      InterTable(std::string syn_name, std::vector<int>& int_list);

      InterTable(std::vector<std::string>& header, std::vector<std::vector<int>>& rows);

      InterTable(std::vector<std::string>&& header, std::vector<std::vector<int>>&& rows);

    public:
      int GetColNum();

      int GetRowNum();

      int FindSynCol(std::string& syn_name);

      InterTable GetColsByIndices(std::vector<int>& valid_col_nums);

      InterTable Deduplicate();

      InterTable Merge(InterTable& t1);

      InterTable Filter(Predicate& pred);

      InterTable MergeAndFilter(InterTable& t1, Predicate& pred);

      bool equal(InterTable t);
  }; 
}

#endif
