#pragma once

#include "tables/entity_tables.h"
#include "tables/relation_tables.h"

class Pkb {
  private:
    // Entity tables
    AssignTable assign_table = AssignTable();
    ReadTable read_table = ReadTable();

    // Relation tables
    FollowsByTable follows_by_table = FollowsByTable();

  public:
    // TODO(Zhenlin): https://github.com/nus-cs3203/21s2-cp-spa-team-39/issues/20
};
