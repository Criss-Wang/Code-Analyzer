#include <string>
#include <vector>

#include "formatter.h"
#include "../../../Utility/entity.h"

#define EMPTY_STRING ""
#define WHITESPACE " "

namespace pql_formatter {
  Formatter::Formatter(pql_cache::Cache* cache) {
    cache_ = cache;
  }

  std::string Formatter::GetStringForProcNameAndVarName(int entity, EntityIdentifier ent_type, AttrIdentifier attr_type) {
    //only left with procedure.procName, read.varName, call.procName, variable.varName and print.varName
    std::string res;

    if (ent_type == EntityIdentifier::kCall || ent_type == EntityIdentifier::kPrint || ent_type == EntityIdentifier::kRead) {
      entity = cache_->GetStringAttribute(ent_type, entity);
    }

    res = attr_type == AttrIdentifier::kProcName 
            ? cache_->GetStringByIndex(IndexTableType::kProc, entity)
            : cache_->GetStringByIndex(IndexTableType::kVar , entity);

    return res;
  }

  std::vector<std::string> Formatter::FormatRawInput(pql_table::InterTable& table, std::vector<pql::AttrRef>& return_syns) {
    std::vector<std::string> result_string;
    result_string.reserve(table.GetRowNum());
    
    //We add the synonym according to their position in return_syns_
    for (auto& attr_ref : return_syns) {
      std::string syn_name = attr_ref.GetSynName();
      EntityIdentifier type = attr_ref.GetSynDeclaration();
      AttrIdentifier attribute = attr_ref.GetAttrIdentifier();
      int col_num_in_table = table.FindSynCol(syn_name);

      for (int index = 0; index < table.GetRowNum(); index++) {
        std::string cur_string;

        if (attribute == AttrIdentifier::kValue || attribute == AttrIdentifier::kStmtNum) {
          cur_string = std::to_string(table.rows_[index][col_num_in_table]);
        } else {
          cur_string = move(GetStringForProcNameAndVarName(table.rows_[index][col_num_in_table], type, attribute));
        }

        if (result_string[index] != EMPTY_STRING) {
          result_string[index] += WHITESPACE;
        }

        result_string[index] += cur_string;
      }
    }

    return result_string;
  }
}
