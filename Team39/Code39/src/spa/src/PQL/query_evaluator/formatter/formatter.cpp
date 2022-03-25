#include <string>
#include <vector>

#include "formatter.h"

Formatter::Formatter(Pkb& pkb) {
  pkb_ = pkb;
}

std::vector<std::string> Formatter::FormatRawInput(pql_table::InterTable& table, std::vector<pql::AttrRef>& return_syns) {
  std::vector<std::string> result_string(table.GetRowNum());
    
  //We add the synonym according to their position in return_syns_
  for (auto& attr_ref : return_syns) {
    std::string syn_name = attr_ref.GetSynName();
    EntityIdentifier type = attr_ref.GetSynDeclaration();
    AttrIdentifier attribute = attr_ref.GetAttrIdentifier();
    int col_num_in_table = table.FindSynCol(syn_name);

    for (int index = 0; index < table.GetRowNum(); index++) {
      std::string cur_string = "";

      if (attribute == AttrIdentifier::kValue || attribute == AttrIdentifier::kStmtNum) {
        cur_string = std::to_string(table.rows_[index][col_num_in_table]);
      } else {
        //left procedure.procName, read.varName, call.procName, variable.varName and print.varName
        int name_index = table.rows_[index][col_num_in_table];

        if (type == EntityIdentifier::kCall || type == EntityIdentifier::kPrint || type == EntityIdentifier::kRead) {
          name_index = pkb_.GetStringAttribute(type, name_index);
        }

        cur_string = attribute == AttrIdentifier::kProcName 
                               ? pkb_.GetStringByIndex(IndexTableType::kProc, name_index)
                               : pkb_.GetStringByIndex(IndexTableType::kVar, name_index);
      }

      if (result_string[index] != "") {
        result_string[index] += " ";
      }

      result_string[index] += cur_string;
    }
  }

  return result_string;
}