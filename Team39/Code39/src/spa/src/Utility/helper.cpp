#include <vector>

#include "helper.h"
#include "../PKB/pkb.h"
#include "../PKB/pkb_exceptions.h"

int Helper::Dfs(shared_ptr<RelListTable> table_to_refer, shared_ptr<RelListTable> table_to_update, int key) {
  if (table_to_update->KeyExistsInTable(key)) {
    return key;
  }

  vector<int> children_lst;
  try {
    children_lst = table_to_refer->GetValueByKey(key);
  } catch (InvalidKeyException& e) {
    return key;
  }

  vector<int> ans;
  for (int child_key : children_lst) {
    int end_val = Helper::Dfs(table_to_refer, table_to_update, child_key);
    ans.push_back(end_val);
    // Add the children of the current key if the key exists
    if (table_to_update->KeyExistsInTable(end_val)) {
      // Merge the vectors of children
      vector<int> value = table_to_update->GetValueByKey(end_val);
      ans.insert(ans.end(), value.begin(), value.end());
    }
  }

  // Add into "cache" if ans is not empty. That means the current key must have children
  if (!ans.empty()) {
    table_to_update->AddKeyValuePair(key, ans);
  }

  return key;
}
