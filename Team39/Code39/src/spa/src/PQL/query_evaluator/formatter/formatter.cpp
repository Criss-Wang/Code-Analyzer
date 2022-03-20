#include <string>
#include <vector>

#include "formatter.h"

Formatter::Formatter(Pkb& pkb) {
  pkb_ = pkb;
}

std::vector<std::string> Formatter::FormatRawInput(pql_table::InterTable& table, std::vector<pql::Synonym>& return_syns) {
  std::vector<std::string> result_string(table.GetRowNum());
    
  //We add the synonym according to their position in return_syns_
  for (auto& syn : return_syns) {
    std::string syn_name = syn.GetName();
    int col_num_in_table = table.FindSynCol(syn_name);

    for (int index = 0; index < table.GetRowNum(); index++) {
      std::string cur_string = "";

      if (syn.GetDeclaration() == EntityIdentifier::kVariable) {
        cur_string = pkb_.GetVarByIndex(table.rows_[index][col_num_in_table]);
      } else if (syn.GetDeclaration() == EntityIdentifier::kProc) {
        cur_string = pkb_.GetProcByIndex(table.rows_[index][col_num_in_table]);
      } else {
        cur_string = std::to_string(table.rows_[index][col_num_in_table]);
      }

      if (result_string[index] != "") {
        result_string[index] += " ";
      }

      result_string[index] += cur_string;
    }
  }

  return result_string;
}