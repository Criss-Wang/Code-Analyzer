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

  public:
    static string PreprocessPattern(const string& pattern);
    static unordered_set<string> GetPattenSet(string input);
};


