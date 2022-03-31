#include <string>
#include <vector>

#include "../solver/intertable.h"
#include "../cache/cache.h"

namespace pql_formatter {
  class Formatter {
    private:
      pql_cache::Cache* cache_;

    public:
      Formatter(pql_cache::Cache* cache);

    public:
      std::vector<std::string> FormatRawInput(pql_table::InterTable& table, std::vector<pql::AttrRef>& return_syns);
  };
}
