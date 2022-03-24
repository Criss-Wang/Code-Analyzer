#pragma once
#include "PKB/pkb.h"
#include "SP/tokenizer.h"

#include <vector>

class AssignmentPattern {
  public:
    ~AssignmentPattern() = default;
    AssignmentPattern() { };
    AssignmentPattern(std::vector<Token>& tokens, int stmt_num);

    vector<string> GetVars();

    void PopulateEntities(Pkb& pkb);

  private:
    int stmt_num_ = -1;
    vector<string> vars_ = {};
    vector<int> constants_ = {};
    string pattern_ = "";
};
