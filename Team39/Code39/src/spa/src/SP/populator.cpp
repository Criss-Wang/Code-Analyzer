#include <stdio.h>
#include <iostream>
#include <stack>

using namespace std;

#include "populator.h"
#include "SP/design_extractor.h"
#include "Utility/CFG/control_flow_graph.h"

const int kFirstIndex = 0;
const int kSecondIndex = 1;
const int kThirdIndex = 2;

// Returns procedure name
string populateProcedure(vector<Token> tokens, Pkb& pkb) {
  string proc_name = tokens.at(kSecondIndex).text_;

  // Add procedure name to procedure_set_
  pkb.AddEntityToSet(EntityIdentifier::kProc, proc_name);
  
  return proc_name;
}

// Returns variable being read
string populateReadStmt(vector<Token> tokens, Pkb& pkb, int stmt_num) {
  string read_var = tokens.at(kSecondIndex).text_;

  // Add stmt num to stmt_set_ and read_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kRead, stmt_num);

  // Add read_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, read_var);

  // Add stmt num and read_var to ReadTable
  pkb.AddInfoToTable(TableIdentifier::kRead, stmt_num, read_var);

  // Add stmt num and read_var to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num, vector<string>{read_var});

  return read_var;
}

// Returns variable being printed
string populatePrintStmt(vector<Token> tokens, Pkb& pkb, int stmt_num) {
  string print_var = tokens.at(kSecondIndex).text_;

  // Add stmt num to stmt_set_ and print_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kPrint, stmt_num);

  // Add print_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, print_var);

  // Add stmt num and print_var to PrintTable
  pkb.AddInfoToTable(TableIdentifier::kPrint, stmt_num, print_var);

  // Add stmt num and print_var to Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vector<string>{ print_var });
  
  return print_var;
}

// Returns a pair - first contains variable modified and second contains variables used
pair<string, vector<string>> populateAssignStmt(vector<Token> tokens, Pkb& pkb, int stmt_num) {
  string lhs_var = tokens.at(kFirstIndex).text_;
  vector<string> rhs_vars;
  vector<int> rhs_constants;
  string assignment_pattern = "";

  // Add stmt num to stmt_set_ and assign_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kAssign, stmt_num);

  // Add lhs_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, lhs_var);

  // Build assignment pattern
  const int kLastIndex = tokens.size() - 1;
  vector<Token>::const_iterator expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator expr_end = tokens.begin() + kLastIndex;
  vector<Token> expr(expr_start, expr_end);

  for (auto token = begin(expr); token != end(expr); ++token) {
    assignment_pattern += token->text_;

    if (token->type_ == TokenType::LETTER || token->type_ == TokenType::NAME) {
      // Add RHS variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text_);
      if (find(begin(rhs_vars), end(rhs_vars), token->text_) == end(rhs_vars)) {
        rhs_vars.push_back(token->text_);
      }

    } else if (token->type_ == TokenType::DIGIT || token->type_ == TokenType::INTEGER) {
      // Add RHS constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text_));
      if (find(begin(rhs_constants), end(rhs_constants), stoi(token->text_)) == end(rhs_constants)) {
        rhs_constants.push_back(stoi(token->text_));
      }
    }
  }

  // Add stmt num and assignment pattern to Assign Table
  pkb.AddInfoToTable(TableIdentifier::kAssign, stmt_num, assignment_pattern);
  pkb.AddInfoToTable(TableIdentifier::kAssignPattern, stmt_num, assignment_pattern);

  // Add stmt num and rhs constants to Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, rhs_constants);

  // Add stmt num and LHS variable to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num, vector<string>{ lhs_var });

  // Add stmt num and vector of variables into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, rhs_vars);

  return make_pair(lhs_var, rhs_vars);
}

// Returns a vector of variables used
vector<string> populateIfStmt(vector<Token> tokens, Pkb& pkb, int stmt_num) {
  vector<string> vars_in_cond_expr;
  vector<int> constants_in_cond_expr;
  string cond_expr_pattern = "";

  // Add stmt num to stmt_set_ and if_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kIf, stmt_num);

  const int kThirdLastIndex = tokens.size() - 3;
  vector<Token>::const_iterator cond_expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator cond_expr_end = tokens.begin() + kThirdLastIndex;
  vector<Token> cond_expr(cond_expr_start, cond_expr_end);

  for (auto token = begin(cond_expr); token != end(cond_expr); ++token) {
    if (token->type_ == TokenType::LETTER || token->type_ == TokenType::NAME) {
      // Add variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text_);
      if (find(begin(vars_in_cond_expr), end(vars_in_cond_expr), token->text_) == end(vars_in_cond_expr)) {
        vars_in_cond_expr.push_back(token->text_);
      }

    } else if (token->type_ == TokenType::DIGIT || token->type_ == TokenType::INTEGER) {
      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text_));
      if (find(begin(constants_in_cond_expr), end(constants_in_cond_expr), stoi(token->text_)) == end(constants_in_cond_expr)) {
        constants_in_cond_expr.push_back(stoi(token->text_));
      }
    }

    cond_expr_pattern += token->text_;
  }

  // Add stmt num and variables in cond expr into If Table
  pkb.AddInfoToTable(TableIdentifier::kIf, stmt_num, vars_in_cond_expr);

  // Add stmt num and constants in cond expr into Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_in_cond_expr);

  // Add stmt num and variables in cond expr into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_in_cond_expr);

  // Add stmt num and cond expr into if_pattern_to_stmt Table
  pkb.AddInfoToTable(TableIdentifier::kIfPattern, stmt_num, cond_expr_pattern);

  return vars_in_cond_expr;
}

// Returns a vector of variables used
vector<string> populateWhileStmt(vector<Token> tokens, Pkb& pkb, int stmt_num) {
  vector<string> vars_in_cond_expr;
  vector<int> constants_in_cond_expr;
  string cond_expr_pattern = "";

  // Add stmt num to stmt_set_ and while_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kWhile, stmt_num);

  const int kSecondLastIndex = tokens.size() - 2;
  vector<Token>::const_iterator cond_expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator cond_expr_end = tokens.begin() + kSecondLastIndex;
  vector<Token> cond_expr(cond_expr_start, cond_expr_end);

  for (auto token = begin(cond_expr); token != end(cond_expr); ++token) {
    if (token->type_ == TokenType::LETTER || token->type_ == TokenType::NAME) {
      // Add variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text_);
      if (find(begin(vars_in_cond_expr), end(vars_in_cond_expr), token->text_) == end(vars_in_cond_expr)) {
        vars_in_cond_expr.push_back(token->text_);
      }

    } else if (token->type_ == TokenType::DIGIT || token->type_ == TokenType::INTEGER) {
      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text_));
      if (find(begin(constants_in_cond_expr), end(constants_in_cond_expr), stoi(token->text_)) == end(constants_in_cond_expr)) {
        constants_in_cond_expr.push_back(stoi(token->text_));
      }
    }

    cond_expr_pattern += token->text_;
  }

  // Add stmt num and variables in cond expr into While Table
  pkb.AddInfoToTable(TableIdentifier::kWhile, stmt_num, vars_in_cond_expr);

  // Add stmt num and constants in cond expr into Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_in_cond_expr);

  // Add stmt num and variables in cond expr into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_in_cond_expr);

  // Add stmt num and cond expr into while_pattern_to_stmt Table
  pkb.AddInfoToTable(TableIdentifier::kWhilePattern, stmt_num, cond_expr_pattern);

  return vars_in_cond_expr;
}

// Returns the name of procedure called
string populateCallStmt(vector<Token> tokens, Pkb& pkb, int stmt_num) {
  string called_proc = tokens.at(kSecondIndex).text_;

  // Add stmt num to stmt_set_ and call_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kCall, stmt_num);

  // Add stmt num and call_proc to CallerTable
  pkb.AddInfoToTable(TableIdentifier::kCaller, stmt_num, called_proc);
  
  return called_proc;
}

stack<int> populateFollowsAndNextRelationship(stack<int> previous, Pkb& pkb, int stmt_num, bool is_prev_stmt_if) {

  if (!previous.empty()) {
    if (stmt_num == previous.top()) {
      return previous;
    } else {
      // Add previous stmt num and current stmt num to FollowsTable
      pkb.AddInfoToTable(TableIdentifier::kFollows, previous.top(), stmt_num);

      if (!is_prev_stmt_if) {
        // Add previous stmt num and current stmt num to NextTable
        pkb.AddInfoToTable(TableIdentifier::kNext, previous.top(), stmt_num);
        //cout << "Next: (" << previous.top() << " ," << stmt_num << ")" << endl;
      }

      previous.pop();
    }
  }

  previous.push(stmt_num);

  return previous;
}

stack<vector<int>> populateParentRelationship(stack<int> parent, stack<vector<int>> children, int stmt_num) {
  if (!parent.empty()) {
    children.top().push_back(stmt_num);
  }

  return children;
}

stack<int> populateNextRelationshipForIf(stack<int> last_stmt_nums_in_if, Pkb& pkb, int stmt_num) {
  // Add the last stmt num in then/else container and current stmt num to Next Table
  pkb.AddInfoToTable(TableIdentifier::kNext, last_stmt_nums_in_if.top(), stmt_num);
  //cout << "Next: (" << last_stmt_nums_in_if.top() << " ," << stmt_num << ")" << endl;

  last_stmt_nums_in_if.pop();

  pkb.AddInfoToTable(TableIdentifier::kNext, last_stmt_nums_in_if.top(), stmt_num);
  //cout << "Next: (" << last_stmt_nums_in_if.top() << " ," << stmt_num << ")" << endl;

  last_stmt_nums_in_if.pop();

  return last_stmt_nums_in_if;
}


vector<string> AppendToVector(vector<string> v, vector<string> vars) {
  for (auto var : vars) {
    if (find(begin(v), end(v), var) == end(v)) {
      v.push_back(var);
    }
  }
  return v;
}

void populate(vector<Token> input_tokens, Pkb& pkb) {

  int stmt_num = 1;

  // Stores parent/previous stmt's line number for Parent/Follows relationship
  stack<int> parent;
  stack<vector<int>> children;
  children.push({});
  stack<int> previous;
  
  //set<int> stmt_lst;

  // Stores procedure range
  string prev_proc = "";
  int start_stmt_num = 1;
  int end_stmt_num = 0;

  // Stores information for UsesP, ModifiesP and calls
  vector<string> uses_p = {};
  vector<string> modifies_p = {};
  vector<string> called_procedures = {};

  // Stores vector of CFGTokens
  vector<CFGToken> cfg_tokens = { CFGToken(CFGTokenType::kStart, 0) };
  // Store "if"/"while" to determine which end token to add to cfg_tokens list
  stack<string> end_tokens;

  // Stores relevant stmt num for Next relationship
  stack<int> while_stmt_num;
  stack<int> if_stmt_num;
  stack<int> last_stmt_nums_in_if;
  bool is_prev_stmt_if = false;

  for (auto token = begin(input_tokens); token != end(input_tokens); ++token) {
    if (token->type_ == TokenType::RIGHT_CURLY) {

      int previous_stmt_num = 0;
      if (!previous.empty()) {
        previous_stmt_num = previous.top();
        previous.pop();
      }

      bool has_two_more_tokens = token != end(input_tokens) - 1 && token != end(input_tokens) - 2;
      bool is_else_stmt = has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == TokenType::LEFT_CURLY;

      if (is_else_stmt) {
        previous.push(stmt_num);
        cfg_tokens.push_back(CFGToken(CFGTokenType::kThenEnd, 0));

        // Add the next stmt num in container and stmt num of if to Next Table
        pkb.AddInfoToTable(TableIdentifier::kNext, if_stmt_num.top(), stmt_num);
        //cout << "Next: (" << if_stmt_num.top() << " ," << stmt_num << ")" << endl;
        if_stmt_num.pop();

        last_stmt_nums_in_if.push(previous_stmt_num);

      } else {
        if (!parent.empty()) {
          // Add parent stmt num and current stmt num to ParentTable
          if (!children.top().empty()) {
            pkb.AddInfoToTable(TableIdentifier::kParent, parent.top(), children.top());
          }
          parent.pop();
          children.pop();
        }

        if (!end_tokens.empty()) {
          if (end_tokens.top() == "if") {
            cfg_tokens.push_back(CFGToken(CFGTokenType::kElseEnd, 0));
            last_stmt_nums_in_if.push(previous_stmt_num);
            is_prev_stmt_if = true;

          } else if (end_tokens.top() == "while") {
            cfg_tokens.push_back(CFGToken(CFGTokenType::kWhileEnd, 0));

            // Add the last stmt num in container and stmt num of while to Next Table
            pkb.AddInfoToTable(TableIdentifier::kNext, previous_stmt_num, while_stmt_num.top());
            //cout << "Next: (" << previous_stmt_num << " ," << while_stmt_num.top() << ")" << endl;

            while_stmt_num.pop();
          }
          end_tokens.pop();
        }
      }

    } else if (next(token, 1)->text_ == "=") {
      vector<Token> tokens;
      while (token->type_ != TokenType::SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      pair<string, vector<string>> variables = populateAssignStmt(tokens, pkb, stmt_num);

      modifies_p = AppendToVector(modifies_p, { variables.first });
      uses_p = AppendToVector(uses_p, { variables.second });

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsAndNextRelationship(previous, pkb, stmt_num, is_prev_stmt_if);
      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      }

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kAssign, stmt_num));

      stmt_num += 1;

    } else if (token->text_ == "procedure") {
      vector<Token> tokens;
      while (token->type_ != TokenType::LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      //cout << endl;
      //cout << tokens.at(1).text_ << endl;
      if (prev_proc != "") {
        // Add procedure name and range to ProcRangeTable
        pkb.AddInfoToTable(TableIdentifier::kProcedure, prev_proc, pair<int, int>(start_stmt_num, end_stmt_num));

        // Add procedure name and callees to CallsTable
        pkb.AddInfoToTable(TableIdentifier::kCalls, prev_proc, called_procedures);

        // Add procedure name and modified variables to ModifiesProcToVariablesTable
        pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, prev_proc, modifies_p);

        // Add procedure name and used variables to UsesProcToVariablesTable
        pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, prev_proc, uses_p);

        cfg_tokens.push_back(CFGToken(CFGTokenType::kEnd, 0));
        // CFG::GenerateCfg(cfg_tokens);
      }

      // reset values for next procedure
      start_stmt_num = end_stmt_num + 1;
      prev_proc = populateProcedure(tokens, pkb);
      called_procedures = {};
      modifies_p = {};
      uses_p = {};
      cfg_tokens = { CFGToken(CFGTokenType::kStart, 0) };

    } else if (token->text_ == "read") {
      vector<Token> tokens;
      while (token->type_ != TokenType::SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      string read_var = populateReadStmt(tokens, pkb, stmt_num);
      modifies_p = AppendToVector(modifies_p, { read_var });

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsAndNextRelationship(previous, pkb, stmt_num, is_prev_stmt_if);
      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      }

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kRead, stmt_num));

      stmt_num += 1;

    } else if (token->text_ == "print") {
      vector<Token> tokens;
      while (token->type_ != TokenType::SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      string print_var = populatePrintStmt(tokens, pkb, stmt_num);
      uses_p = AppendToVector(uses_p, { print_var });
      
      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsAndNextRelationship(previous, pkb, stmt_num, is_prev_stmt_if);
      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      }

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kPrint, stmt_num));

      stmt_num += 1;

    } else if (token->text_ == "if") {
      vector<Token> tokens;
      while (token->type_ != TokenType::LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      vector<string> used_vars = populateIfStmt(tokens, pkb, stmt_num);
      uses_p = AppendToVector(uses_p, used_vars);

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsAndNextRelationship(previous, pkb, stmt_num, is_prev_stmt_if);
      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      }
      parent.push(stmt_num);
      children.push({});
      previous.push(stmt_num + 1);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kIf, stmt_num));
      end_tokens.push("if");

      if_stmt_num.push(stmt_num);
      // Add the stmt num of if and next stmt num to Next Table
      pkb.AddInfoToTable(TableIdentifier::kNext, stmt_num, stmt_num + 1);
      //cout << "Next: (" << stmt_num << " ," << stmt_num + 1 << ")" << endl;

      stmt_num += 1;

    } else if (token->text_ == "while") {
      vector<Token> tokens;
      while (token->type_ != TokenType::LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      
      vector<string> used_vars = populateWhileStmt(tokens, pkb, stmt_num);
      uses_p = AppendToVector(uses_p, used_vars);

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsAndNextRelationship(previous, pkb, stmt_num, is_prev_stmt_if);
      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      }
      parent.push(stmt_num);
      children.push({});
      previous.push(stmt_num + 1);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kWhile, stmt_num));
      end_tokens.push("while");

      while_stmt_num.push(stmt_num);
      // Add the stmt num of while and next stmt num to Next Table
      pkb.AddInfoToTable(TableIdentifier::kNext, stmt_num, stmt_num + 1);
      //cout << "Next: (" << stmt_num << " ," << stmt_num + 1 << ")" << endl;

      stmt_num += 1;

    } else if (token->text_ == "call") {
      vector<Token> tokens;
      while (token->type_ != TokenType::SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      string called_proc = populateCallStmt(tokens, pkb, stmt_num);

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsAndNextRelationship(previous, pkb, stmt_num, is_prev_stmt_if);
      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      }

      if (find(begin(called_procedures), end(called_procedures), called_proc) == end(called_procedures)) {
        called_procedures.push_back(called_proc);
      }
      end_stmt_num += 1;
      
      cfg_tokens.push_back(CFGToken(CFGTokenType::kCall, stmt_num));

      stmt_num += 1;

    }
  }

  // Add stmt_lst to stmt_list_set_
  // pkb.AddEntityToSet(EntityIdentifier::kStmtLst, stmt_lst);

  // Populate ProcRangeTable, CallsTable, ModifiesProcToVariablesTable, UsesProcToVariablesTable for the last procedure
  pkb.AddInfoToTable(TableIdentifier::kProcedure, prev_proc, pair<int, int>(start_stmt_num, end_stmt_num));
  pkb.AddInfoToTable(TableIdentifier::kCalls, prev_proc, called_procedures);
  pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, prev_proc, modifies_p);
  pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, prev_proc, uses_p);
  
  // Generate CFG for last procedure
  cfg_tokens.push_back(CFGToken(CFGTokenType::kEnd, 0));
  //CFG::GenerateCfg(cfg_tokens);

  try {
    PopulateNestedRelationships(pkb);
  } catch (exception& e) {
    throw invalid_argument("PKB Population failed");
  }
}
