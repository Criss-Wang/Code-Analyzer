#include <stdio.h>
#include <iostream>
#include <stack>

using namespace std;

#include "populator.h"
#include "SP/design_extractor.h"

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
string populateReadStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(kFirstIndex).stmt_num_;
  string read_var = tokens.at(kSecondIndex).text_;

  // Add stmt num to stmt_set_ and read_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kRead, stmt_num);

  // Add stmt num and read_var to ReadTable
  pkb.AddInfoToTable(TableIdentifier::kRead, stmt_num, read_var);

  // Add stmt num and read_var to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num, vector<string>{read_var});

  // Add read_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, read_var);

  return read_var;
}

// Returns variable being printed
string populatePrintStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(kFirstIndex).stmt_num_;
  string print_var = tokens.at(kSecondIndex).text_;

  // Add stmt num to stmt_set_ and print_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kPrint, stmt_num);

  // Add stmt num and print_var to PrintTable
  pkb.AddInfoToTable(TableIdentifier::kPrint, stmt_num, print_var);

  // Add stmt num and print_var to Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vector<string>{ print_var });

  // Add read_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, print_var);

  return print_var;
}

// Returns a pair - first contains variable modified and second contains variables used
pair<string, vector<string>> populateAssignStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(kFirstIndex).stmt_num_;
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

    if (token->type_ == LETTER || token->type_ == NAME) {
      // Add RHS variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text_);
      if (find(begin(rhs_vars), end(rhs_vars), token->text_) == end(rhs_vars)) {
        rhs_vars.push_back(token->text_);
      }

    } else if (token->type_ == DIGIT || token->type_ == INTEGER) {
      // Add RHS constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text_));
      if (find(begin(rhs_constants), end(rhs_constants), stoi(token->text_)) == end(rhs_constants)) {
        rhs_constants.push_back(stoi(token->text_));
      }
    }
  }

  // Add stmt num and assignment pattern to Assign Table
  pkb.AddInfoToTable(TableIdentifier::kAssign, stmt_num, assignment_pattern);
  pkb.AddInfoToTable(TableIdentifier::kPattern, stmt_num, assignment_pattern);

  // Add stmt num and rhs constants to Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, rhs_constants);

  // Add stmt num and LHS variable to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num, vector<string>{ lhs_var });

  // Add stmt num and vector of variables into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, rhs_vars);

  return make_pair(lhs_var, rhs_vars);
}

// Returns a vector of variables used
vector<string> populateIfStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(kFirstIndex).stmt_num_;
  vector<string> vars_in_cond_expr;
  vector<int> constants_in_cond_expr;

  // Add stmt num to stmt_set_ and if_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kIf, stmt_num);

  const int kThirdLastIndex = tokens.size() - 3;
  vector<Token>::const_iterator cond_expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator cond_expr_end = tokens.begin() + kThirdLastIndex;
  vector<Token> cond_expr(cond_expr_start, cond_expr_end);

  for (auto token = begin(cond_expr); token != end(cond_expr); ++token) {
    if (token->type_ == LETTER || token->type_ == NAME) {
      // Add variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text_);
      if (find(begin(vars_in_cond_expr), end(vars_in_cond_expr), token->text_) == end(vars_in_cond_expr)) {
        vars_in_cond_expr.push_back(token->text_);
      }

    } else if (token->type_ == DIGIT || token->type_ == INTEGER) {
      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text_));
      if (find(begin(constants_in_cond_expr), end(constants_in_cond_expr), stoi(token->text_)) == end(constants_in_cond_expr)) {
        constants_in_cond_expr.push_back(stoi(token->text_));
      }
    }
  }

  // Add stmt num and variables in cond expr into If Table
  pkb.AddInfoToTable(TableIdentifier::kIf, stmt_num, vars_in_cond_expr);

  // Add stmt num and constants in cond expr into Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_in_cond_expr);

  // Add stmt num and variables in cond expr into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_in_cond_expr);

  return vars_in_cond_expr;
}

// Returns a vector of variables used
vector<string> populateWhileStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(kFirstIndex).stmt_num_;
  vector<string> vars_in_cond_expr;
  vector<int> constants_in_cond_expr;

  // Add stmt num to stmt_set_ and while_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kWhile, stmt_num);

  const int kSecondLastIndex = tokens.size() - 2;
  vector<Token>::const_iterator cond_expr_start = tokens.begin() + kThirdIndex;
  vector<Token>::const_iterator cond_expr_end = tokens.begin() + kSecondLastIndex;
  vector<Token> cond_expr(cond_expr_start, cond_expr_end);

  for (auto token = begin(cond_expr); token != end(cond_expr); ++token) {
    if (token->type_ == LETTER || token->type_ == NAME) {
      // Add variable to variable_set_
      pkb.AddEntityToSet(EntityIdentifier::kVariable, token->text_);
      if (find(begin(vars_in_cond_expr), end(vars_in_cond_expr), token->text_) == end(vars_in_cond_expr)) {
        vars_in_cond_expr.push_back(token->text_);
      }

    } else if (token->type_ == DIGIT || token->type_ == INTEGER) {
      // Add constant to constant_set_
      pkb.AddEntityToSet(EntityIdentifier::kConstant, stoi(token->text_));
      if (find(begin(constants_in_cond_expr), end(constants_in_cond_expr), stoi(token->text_)) == end(constants_in_cond_expr)) {
        constants_in_cond_expr.push_back(stoi(token->text_));
      }
    }
  }

  // Add stmt num and variables in cond expr into While Table
  pkb.AddInfoToTable(TableIdentifier::kWhile, stmt_num, vars_in_cond_expr);

  // Add stmt num and constants in cond expr into Constant Table
  pkb.AddInfoToTable(TableIdentifier::kConstant, stmt_num, constants_in_cond_expr);

  // Add stmt num and variables in cond expr into Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num, vars_in_cond_expr);

  return vars_in_cond_expr;
}

// Returns the name of procedure called
string populateCallStmt(vector<Token> tokens, Pkb& pkb) {
  int stmt_num = tokens.at(kFirstIndex).stmt_num_;
  string called_proc = tokens.at(kSecondIndex).text_;

  // Add stmt num to stmt_set_ and call_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num);
  pkb.AddEntityToSet(EntityIdentifier::kCall, stmt_num);

  // Add stmt num and call_proc to CallerTable
  pkb.AddInfoToTable(TableIdentifier::kCaller, stmt_num, called_proc);
  
  return called_proc;
}

stack<int> populateFollowsRelationship(stack<int> previous, Pkb& pkb, int stmt_num) {
  if (!previous.empty()) {
    if (stmt_num == previous.top()) {
      return previous;
    } else {
      // Add previous stmt num and current stmt num to FollowsTable
      pkb.AddInfoToTable(TableIdentifier::kFollows, previous.top(), stmt_num);
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

vector<string> AppendToVector(vector<string> v, vector<string> vars) {
  for (auto var : vars) {
    if (find(begin(v), end(v), var) == end(v)) {
      v.push_back(var);
    }
  }
  return v;
}

void populate(vector<Token> input_tokens, Pkb& pkb) {

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

  for (auto token = begin(input_tokens); token != end(input_tokens); ++token) {
    if (token->type_ == RIGHT_CURLY) {

      int previous_stmt_num = 0;
      if (!previous.empty()) {
        previous_stmt_num = previous.top();
        previous.pop();
      }

      bool has_two_more_tokens = token != end(input_tokens) - 1 && token != end(input_tokens) - 2;
      bool is_else_stmt = has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == LEFT_CURLY;

      if (is_else_stmt) {
        previous.push(previous_stmt_num + 1);
        cfg_tokens.push_back(CFGToken(CFGTokenType::kIfElse, 0));
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
            cfg_tokens.push_back(CFGToken(CFGTokenType::kIfEnd, 0));
          } else if (end_tokens.top() == "while") {
            cfg_tokens.push_back(CFGToken(CFGTokenType::kWhileEnd, 0));
          }
          end_tokens.pop();
        }
      }

    } else if (next(token, 1)->text_ == "=") {
      vector<Token> tokens;
      while (token->type_ != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      pair<string, vector<string>> variables = populateAssignStmt(tokens, pkb);

      modifies_p = AppendToVector(modifies_p, { variables.first });
      uses_p = AppendToVector(uses_p, { variables.second });

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kAssign, token->stmt_num_));

    } else if (token->text_ == "procedure") {
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      if (prev_proc != "") {
        // Add procedure name and range to ProcRangeTable
        pkb.AddInfoToTable(TableIdentifier::kProcedure, prev_proc, pair<int, int>(start_stmt_num, end_stmt_num));

        // Add procedure name and callees to CallsTable
        pkb.AddInfoToTable(TableIdentifier::kCalls, prev_proc, called_procedures);

        // Add procedure name and modified variables to ModifiesProcToVariablesTable
        //pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, prev_proc, modifies_p);

        // Add procedure name and used variables to UsesProcToVariablesTable
        //pkb.AddInfoToTable(TableIdentifier::KUsesProcToVar, prev_proc, uses_p);

        cfg_tokens.push_back(CFGToken(CFGTokenType::kEnd, 0));
        // buildCFG(cfg_tokens);
        cout << prev_proc << endl;
        for (CFGToken t : cfg_tokens) {
          t.print();
        }
        cout << endl;
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
      while (token->type_ != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      string read_var = populateReadStmt(tokens, pkb);
      modifies_p = AppendToVector(modifies_p, { read_var });

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kRead, token->stmt_num_));

    } else if (token->text_ == "print") {
      vector<Token> tokens;
      while (token->type_ != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      string print_var = populatePrintStmt(tokens, pkb);
      uses_p = AppendToVector(uses_p, { print_var });
      
      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kPrint, token->stmt_num_));

    } else if (token->text_ == "if") {
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      vector<string> used_vars = populateIfStmt(tokens, pkb);
      uses_p = AppendToVector(uses_p, used_vars);

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);
      parent.push(token->stmt_num_);
      children.push({});
      previous.push(token->stmt_num_ + 1);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kIfStart, token->stmt_num_));
      end_tokens.push("if");

    } else if (token->text_ == "while") {
      vector<Token> tokens;
      while (token->type_ != LEFT_CURLY) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);
      
      vector<string> used_vars = populateWhileStmt(tokens, pkb);
      uses_p = AppendToVector(uses_p, used_vars);

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);
      parent.push(token->stmt_num_);
      children.push({});
      previous.push(token->stmt_num_ + 1);

      end_stmt_num += 1;

      cfg_tokens.push_back(CFGToken(CFGTokenType::kWhileStart, token->stmt_num_));
      end_tokens.push("while");

    } else if (token->text_ == "call") {
      vector<Token> tokens;
      while (token->type_ != SEMICOLON) {
        tokens.push_back(*token);
        token++;
      }
      tokens.push_back(*token);

      string called_proc = populateCallStmt(tokens, pkb);

      children = populateParentRelationship(parent, children, token->stmt_num_);
      previous = populateFollowsRelationship(previous, pkb, token->stmt_num_);

      if (find(begin(called_procedures), end(called_procedures), called_proc) == end(called_procedures)) {
        called_procedures.push_back(called_proc);
      }
      end_stmt_num += 1;
      
      cfg_tokens.push_back(CFGToken(CFGTokenType::kCall, token->stmt_num_));

    }
  }

  // Add stmt_lst to stmt_list_set_
  // pkb.AddEntityToSet(EntityIdentifier::kStmtLst, stmt_lst);

  // Populate ProcRangeTable, CallsTable, ModifiesProcToVariablesTable, UsesProcToVariablesTable for the last procedure
  pkb.AddInfoToTable(TableIdentifier::kProcedure, prev_proc, pair<int, int>(start_stmt_num, end_stmt_num));
  pkb.AddInfoToTable(TableIdentifier::kCalls, prev_proc, called_procedures);
  //pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, prev_proc, modifies_p);
  //pkb.AddInfoToTable(TableIdentifier::KUsesProcToVar, prev_proc, uses_p);
  
  // Generate CFG for last procedure
  cfg_tokens.push_back(CFGToken(CFGTokenType::kEnd, 0));
  // buildCFG(cfg_tokens);
  cout << prev_proc << endl;
  for (CFGToken t : cfg_tokens) {
    t.print();
  }
  cout << endl;

  if (PopulateNestedRelationships(pkb) == 0) {
    throw invalid_argument("PKB Population failed");
  }
}

/*
vector<CFG> buildCFG(vector<Token> input_tokens) {
  vector<CFG> cfgs = {};
  CFG cfg = CFG();
  GraphNode* current_node = nullptr;
  stack<GraphNode*> previous_nodes = {};

  // Keep track of nodes which are waiting for alternative paths to be created
  stack<GraphNode*> waiting_nodes = {};
  stack<GraphNode*> waiting_nodes_in_if = {};
  bool link_two_nodes = false;

  for (auto token = begin(input_tokens); token != end(input_tokens); ++token) {

    if (token->type_ == RIGHT_CURLY) {
      bool has_two_more_tokens = token != end(input_tokens) - 1 && token != end(input_tokens) - 2;
      bool is_else_stmt = has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == LEFT_CURLY;

      if (previous_nodes.empty()) { // end of procedure
        current_node->InsertNextNode(new GraphNode(NodeType::END));
        current_node = current_node->GetNextNode();

        if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::WHILE) {
          waiting_nodes.top()->InsertAlternativeNode(current_node);
          waiting_nodes.pop();

        } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::IF) {
          waiting_nodes.top()->InsertAlternativeNode(current_node);
          waiting_nodes.pop();

        } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::STMT) {
          waiting_nodes.top()->InsertNextNode(current_node);
          waiting_nodes.pop();
          waiting_nodes_in_if.top()->InsertNextNode(current_node);
          waiting_nodes_in_if.pop();
        }

        cfgs.push_back(cfg);
        cfg = CFG();

      } else if (is_else_stmt) {
        waiting_nodes.push(waiting_nodes_in_if.top());
        waiting_nodes_in_if.pop();
        waiting_nodes_in_if.push(current_node);
        current_node = new GraphNode(NodeType::DUMMY);

      } else if (previous_nodes.top()->GetNodeType() == NodeType::WHILE) {
        current_node->InsertNextNode(previous_nodes.top());
        waiting_nodes.push(previous_nodes.top());
        previous_nodes.pop();
        current_node = new GraphNode(NodeType::DUMMY);

      } else if (previous_nodes.top()->GetNodeType() == NodeType::IF) {
        waiting_nodes.push(current_node);
        previous_nodes.pop();
        current_node = new GraphNode(NodeType::DUMMY);

      }

    } else if (next(token, 1)->text_ == "=" || token->text_ == "read" || token->text_ == "print" || token->text_ == "call") {
      if (current_node->GetNodeType() != NodeType::STMT) {
        current_node->InsertNextNode(new GraphNode(NodeType::STMT));
        current_node = current_node->GetNextNode();
      }

      current_node->AddStmtNum(token->stmt_num_);

      // the stmtLst inside the loop will be set as alternative Node instead of Next
      if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::WHILE) {
        waiting_nodes.top()->InsertAlternativeNode(current_node);
        waiting_nodes.pop();

      } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::IF) {
        waiting_nodes.top()->InsertAlternativeNode(current_node);
        waiting_nodes.pop();

      } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::STMT) {
        waiting_nodes.top()->InsertNextNode(current_node);
        waiting_nodes.pop();
        waiting_nodes_in_if.top()->InsertNextNode(current_node);
        waiting_nodes_in_if.pop();
      }

    } else if (token->text_ == "procedure") {

      // Initialize CFG for new procedure
      cfg = CFG(next(token, 1)->text_);
      current_node = cfg.GetStartNode();

    } else if (token->text_ == "if") {
      current_node->InsertNextNode(new GraphNode(NodeType::IF));
      current_node = current_node->GetNextNode();
      current_node->AddStmtNum(token->stmt_num_);

      if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::WHILE) {
        waiting_nodes.top()->InsertAlternativeNode(current_node);
        waiting_nodes.pop();

      } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::IF) {
        waiting_nodes.top()->InsertAlternativeNode(current_node);
        waiting_nodes.pop();

      } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::STMT) {
        waiting_nodes.top()->InsertNextNode(current_node);
        waiting_nodes.pop();
        waiting_nodes_in_if.top()->InsertNextNode(current_node);
        waiting_nodes_in_if.pop();
      }

      waiting_nodes_in_if.push(current_node);
      previous_nodes.push(current_node);

    } else if (token->text_ == "while") {
      current_node->InsertNextNode(new GraphNode(NodeType::WHILE));
      current_node = current_node->GetNextNode();
      current_node->AddStmtNum(token->stmt_num_);

      if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::WHILE) {
        waiting_nodes.top()->InsertAlternativeNode(current_node);
        waiting_nodes.pop();

      } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::IF) {
        waiting_nodes.top()->InsertNextNode(current_node);
        waiting_nodes.pop();

      } else if (!waiting_nodes.empty() && waiting_nodes.top()->GetNodeType() == NodeType::STMT) {
        waiting_nodes.top()->InsertNextNode(current_node);
        waiting_nodes.pop();
        waiting_nodes_in_if.top()->InsertNextNode(current_node);
        waiting_nodes_in_if.pop();
      }

      waiting_nodes_in_if.push(current_node);
      previous_nodes.push(current_node);

    }
  }

  return cfgs;
}
*/
