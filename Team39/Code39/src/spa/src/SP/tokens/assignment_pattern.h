#pragma once
#include "PKB/pkb.h"
#include "SP/tokenizer.h"

#include <unordered_set>

class AssignmentPattern {
  public:
    ~AssignmentPattern() = default;
    AssignmentPattern() { };
    AssignmentPattern(std::vector<Token>& tokens);

    unordered_set<string> GetVars();

    void PopulateEntities(Pkb& pkb, int stmt_num);

  private:
    unordered_set<string> vars_ = {};
    unordered_set<int> constants_ = {};
    string pattern_ = "";
};
