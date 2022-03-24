#pragma once
#include "PKB/pkb.h"
#include "SP/tokenizer.h"

#include <vector>

class AssignmentPattern {
  public:
    ~AssignmentPattern() = default;
    AssignmentPattern() { };
    AssignmentPattern(std::vector<Token>& tokens);

    vector<string> GetVars();

    void PopulateEntities(Pkb& pkb, int stmt_num);

  private:
    vector<string> vars_ = {};
    vector<int> constants_ = {};
    string pattern_ = "";
};
