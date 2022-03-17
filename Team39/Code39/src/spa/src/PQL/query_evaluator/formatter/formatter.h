#include <string>
#include <vector>

#include "../../../PKB/pkb.h"
#include "../solver/intertable.h"

class Formatter {
  private:
    Pkb pkb_;

  public:
    Formatter(Pkb& pkb);

  public:
    std::vector<std::string> FormatRawInput(pql_table::InterTable& table, std::vector<pql::AttrRef>& return_syns);
};
