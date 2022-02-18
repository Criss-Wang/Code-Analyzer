#include "helper.h"

bool PatternHelper::IsOperator(const char c) {
  return (!isalpha(c) && !isdigit(c));
}

bool PatternHelper::IsExprSpec(const char c) {
  return c != ' ';
}

int PatternHelper::GetPriority(const char c) {
  if (c == '-' || c == '+')
    return 1;
  else if (c == '*' || c == '/' || c == '%')
    return 2;
  return 0;
}

string PatternHelper::PreprocessPattern(const string& pattern) {
  string res;
  for (const auto c : pattern) {
    if (IsExprSpec(c)) {
      res += c;
    }
  }
  return res;
}

string PatternHelper::GenerateSubPattern(stack<char>& operators, stack<string>& operands) {
  const char op = operators.top();
  operators.pop();

  const string op1 = operands.top();
  operands.pop();
  const string op2 = operands.top();
  operands.pop();
  const string curr_pattern = op2 + op1 + op;
  operands.push(curr_pattern);
  return curr_pattern;
}

unordered_set<string> PatternHelper::GetPatternSetPostfix(const string& input, const bool is_full) {
  // stack for operators.
  stack<char> operators;

  // stack for operands.
  stack<string> operands;

  // result set
  unordered_set<string> res;

  for (int i = 0; i < input.length(); i++) {

    // If current character is an opening bracket, then
    // push into the operators stack.
    if (input[i] == '(') {
      operators.push(input[i]);
    }

    // If current character is a closing bracket, then pop from both stacks and push result in operands stack until
    // matching opening bracket is found in operator stack.
    else if (input[i] == ')') {
      while (!operators.empty() && operators.top() != '(') {
        string curr_pattern = GenerateSubPattern(operators, operands);
        if (is_full) res.insert(curr_pattern);
      }

      // Pop opening bracket from stack.
      operators.pop();
    }

    // If current character is an operand then push it into operands stack.
    else if (!IsOperator(input[i])) {
      // operands.push(string(1, input[i]));
      operands.push(string(1, input[i]));
      if (is_full) res.insert(string(1, input[i]));
    }

    // If current character is an operator, then push it into operators stack after popping high priority operators from
    // operators stack and pushing result in operands stack.
    else {
      while (!operators.empty() && GetPriority(input[i]) <= GetPriority(operators.top())) {
        string curr_pattern = GenerateSubPattern(operators, operands);
        if (is_full) res.insert(curr_pattern);
      }
      operators.push(input[i]);
    }
  }

  if (operators.empty()) return { input };

  // Pop operators from operators stack until it is empty and add result of each pop operation in operands stack.
  string curr_pattern;
  while (!operators.empty()) {
    curr_pattern = GenerateSubPattern(operators, operands);
    if (is_full) res.insert(curr_pattern);
  }

  if (!is_full) res.insert(curr_pattern);

  // Final prefix expression is present in operands stack.
  // return operands.top();
  return res;
}