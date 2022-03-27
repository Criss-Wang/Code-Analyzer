#pragma once

#include <unordered_set>
#include <string>
#include <stack>

using namespace std;

class Helper {};

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
