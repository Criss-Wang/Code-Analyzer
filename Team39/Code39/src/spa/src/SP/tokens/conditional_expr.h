#pragma once
#include "PKB/pkb.h"
#include "SP/tokenizer.h"

#include <vector>

class ConditionalExpression {
  public:
    ~ConditionalExpression() = default;
    ConditionalExpression() { };
    ConditionalExpression(std::vector<Token>& tokens);

    vector<string> GetVars();

    void PopulateEntities(Pkb& pkb, int stmt_num);

  private:
    int stmt_num_ = -1;
    vector<string> vars_ = {};
    vector<int> constants_ = {};
};
