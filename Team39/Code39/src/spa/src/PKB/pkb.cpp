#include "pkb.h"

// TODO(Zhenlin): https://github.com/nus-cs3203/21s2-cp-spa-team-39/issues/20
int Pkb::PopulateNestedRelationship() {
  try {
    PopulateNestedFollows();
    PopulateNestedParents();
    PopulateUses();
    PopulateModifies();
  } catch (exception& e) {
    return 0;
  }
  return 1;
}