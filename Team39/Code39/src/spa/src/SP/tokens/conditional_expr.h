#pragma once
#include "PKB/pkb.h"
#include "SP/tokenizer.h"

#include <unordered_set>

class ConditionalExpression {
  public:
    ~ConditionalExpression() = default;
    ConditionalExpression() { };
    ConditionalExpression(std::vector<Token>& tokens);

    unordered_set<string> GetVars();

    void PopulateEntities(Pkb& pkb, int stmt_num);

  private:
    unordered_set<string> vars_ = {};
    unordered_set<int> constants_ = {};
};
