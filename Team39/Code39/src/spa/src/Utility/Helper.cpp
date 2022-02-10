#include "Helper.h"

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
  for (auto c : pattern) {
    if (IsExprSpec(c)) {
      res += c;
    }
  }
  return res;
}


unordered_set<string> PatternHelper::GetPatternSet(string input) {
  // stack for operators.
  stack<char> operators;

  // stack for operands.
  stack<string> operands;
  stack<string> operands2;
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

        // operator
        char op = operators.top();
        operators.pop();

        string op3 = operands2.top();
        operands2.pop();
        string op4 = operands2.top();
        operands2.pop();
        string tmp2 = op4 + op + op3;
        operands2.push(tmp2);
        res.insert(tmp2);
      }

      // Pop opening bracket from stack.
      operators.pop();
    }

    // If current character is an operand then push it into operands stack.
    else if (!IsOperator(input[i])) {
      // operands.push(string(1, input[i]));
      operands2.push(string(1, input[i]));
      res.insert(string(1, input[i]));
    }

    // If current character is an operator, then push it into operators stack after popping high priority operators from
    // operators stack and pushing result in operands stack.
    else {
      while (!operators.empty() && GetPriority(input[i]) <= GetPriority(operators.top())) {
        char op = operators.top();
        operators.pop();

        string op3 = operands2.top();
        operands2.pop();
        string op4 = operands2.top();
        operands2.pop();

        string tmp2 = op4 + op + op3;
        operands2.push(tmp2);
        res.insert(tmp2);
      }
      operators.push(input[i]);
    }
  }

  // Pop operators from operators stack until it is empty and add result of each pop operation in operands stack.
  while (!operators.empty()) {
    char op = operators.top();
    operators.pop();

    string op3 = operands2.top();
    operands2.pop();
    string op4 = operands2.top();
    operands2.pop();

    string tmp2 = op4 + op + op3;
    operands2.push(tmp2);
    res.insert(tmp2);
  }

  // Final prefix expression is present in operands stack.
  // return operands.top();
  return res;
}