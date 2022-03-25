#pragma once

#include <unordered_set>
#include <string>
#include <stack>

#include "../PKB/pkb.h"

using namespace std;

class Helper {
  public:
    // Helper method for populating nested relations such as Parent* and Calls*
    static int Dfs(shared_ptr<RelListTable> table_to_refer, shared_ptr<RelListTable> table_to_update, int key);
};

class PatternHelper : public Helper {
  private:
    static bool IsOperator(char c);
    static bool IsExprSpec(char c);
    // Function to find priority of given operator.
    static int GetPriority(char c);
    static string GenerateSubPattern(stack<char>& operators, stack<string>& operands);

  public:
    static string PreprocessPattern(const string& pattern);
    static unordered_set<string> GetPatternSetPostfix(const string& input, const bool is_full);
    static unordered_set<string> GetContainerPatterns(const string& input);
};
