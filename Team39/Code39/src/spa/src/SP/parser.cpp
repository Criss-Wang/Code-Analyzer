#include <vector>
#include <stack>

#include "parser.h"
#include "tokens/procedure.h"
#include "sp_exceptions.h"
#include "design_extractor.h"
#include "Utility/CFG/control_flow_graph.h"

#define INDEX_OF_STMT_TYPE 0
#define INDEX_OF_EQUAL_OP 1


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

void populateNextRelationship(stack<int> previous, Pkb& pkb, int stmt_num) {
  if (!previous.empty() && stmt_num != previous.top()) {
    // Add previous stmt num and current stmt num to NextTable
    pkb.AddInfoToTable(TableIdentifier::kNext, previous.top(), vector<int>{ stmt_num });
  }
}

stack<vector<int>> populateParentRelationship(stack<int> parent, stack<vector<int>> children, int stmt_num) {
  if (!parent.empty()) {
    children.top().push_back(stmt_num);
  }

  return children;
}

stack<vector<pair<int, int>>> populateNextRelationshipForIf(stack<vector<pair<int, int>>> last_stmts_in_if, Pkb& pkb, int stmt_num) {
  // Add the last stmt num in then/else container and current stmt num to Next Table
  vector<pair<int, int>> last_stmts = last_stmts_in_if.top();
  for (int i = 0; i < last_stmts.size(); i++) {
    if (last_stmts.at(i).second == 0) {
      pkb.AddInfoToTable(TableIdentifier::kNext, last_stmts.at(i).first, vector<int>{ stmt_num });
    } else {
      vector<int> stmt_nums = { last_stmts.at(i).second, stmt_num };
      pkb.AddInfoToTable(TableIdentifier::kNext, last_stmts.at(i).first, stmt_nums);
    }
  }
  last_stmts_in_if.pop();
  return last_stmts_in_if;
}

Parser::Parser(const std::string& input, Pkb& pkb) {

  vector<Token> tokens_lst = tokenizer_.parse(input);

  vector<Token> proc_tokens = {};
  vector<shared_ptr<Stmt>> stmt_lst = {};

  int stmt_num = 0;

  // Stores parent/previous stmt's line number for Parent/Follows relationship
  stack<int> parent;
  stack<vector<int>> children;
  children.push({});
  stack<int> previous;

  // Stores vector of CFGTokens
  vector<CFGToken> cfg_tokens = { CFGToken(CFGTokenType::kStart, 0) };
  // Store "if"/"while" to determine which end token to add to cfg_tokens list
  stack<string> end_tokens;

  // Stores relevant stmt num for Next relationship
  stack<int> while_stmt_num;
  stack<int> if_stmt_num;
  stack<vector<pair<int, int>>> last_stmt_nums_in_if;
  bool is_prev_stmt_if = false;
  bool is_prev_stmt_while = false;

  // 1st round of sytax validation which involves curly brackets
  int curly_bracket_count = 0;
  int if_else_stmts = 0;
  bool is_new_container = true;

  for (auto token = begin(tokens_lst); token != end(tokens_lst); ++token) {
    if (token->text_ == "}") {

      if (is_new_container) {
        throw InvalidSyntaxException();
      }

      int previous_stmt_num = 0;
      if (!previous.empty()) {
        previous_stmt_num = previous.top();
        previous.pop();
      }

      bool has_two_more_tokens = token != end(tokens_lst) - 1 && token != end(tokens_lst) - 2;
      bool is_else_stmt = has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == TokenType::LEFT_CURLY;
      
      if (is_else_stmt) {
        if_else_stmts -= 1;
        token++;
        token++;
        is_new_container = true;

        int next_stmt_num_in_else = stmt_num + 1;
        previous.push(next_stmt_num_in_else);
        cfg_tokens.push_back(CFGToken(CFGTokenType::kThenEnd, 0));

        // Add the next stmt num in 'else' and 'then' container and stmt num of if to Next Table
        int next_stmt_num_in_then = if_stmt_num.top() + 1;
        pkb.AddInfoToTable(TableIdentifier::kNext, if_stmt_num.top(), vector<int>{next_stmt_num_in_then, next_stmt_num_in_else});

        if_stmt_num.pop();

        if (is_prev_stmt_while) {
          last_stmt_nums_in_if.top().push_back({ previous_stmt_num, previous_stmt_num + 1 });
          while_stmt_num.pop();
          is_prev_stmt_while = false;
        } else if (is_prev_stmt_if) {
          last_stmt_nums_in_if.top().push_back({ previous_stmt_num, 0 });
          is_prev_stmt_if = false;
        } else {
          last_stmt_nums_in_if.top().push_back({ previous_stmt_num, 0 });
        }

      } else {
        curly_bracket_count -= 1;

        if (!parent.empty()) {
          // Add parent stmt num and current stmt num to ParentTable
          if (!children.top().empty()) {
            pkb.AddInfoToTable(TableIdentifier::kParent, parent.top(), children.top());
          }
          parent.pop();
          children.pop();
        }

        if (end_tokens.empty() && is_prev_stmt_while) {
          // No stmt after while loop, add stmt num of while and first stmt num in container to Next Table
          pkb.AddInfoToTable(TableIdentifier::kNext, while_stmt_num.top(), vector<int>{while_stmt_num.top() + 1});
          while_stmt_num.pop();
          is_prev_stmt_while = false;
        }

        if (!end_tokens.empty()) {
          if (end_tokens.top() == "if") {
            cfg_tokens.push_back(CFGToken(CFGTokenType::kElseEnd, 0));

            if (is_prev_stmt_while) {
              last_stmt_nums_in_if.top().push_back({ previous_stmt_num, previous_stmt_num + 1 });
              while_stmt_num.pop();
              is_prev_stmt_while = false;
            } else if (is_prev_stmt_if) {
              last_stmt_nums_in_if.top().push_back({ previous_stmt_num, 0 });
              is_prev_stmt_if = false;
            } else {
              last_stmt_nums_in_if.top().push_back({ previous_stmt_num, 0 });
            }

            is_prev_stmt_if = true;

          } else if (end_tokens.top() == "while") {
            cfg_tokens.push_back(CFGToken(CFGTokenType::kWhileEnd, 0));

            if (is_prev_stmt_if) {
              // Add the last stmt num in then/else container and stmt num of while to Next Table
              last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, while_stmt_num.top());
              is_prev_stmt_if = false;
            } else {
              // Add the last stmt num in container and stmt num of while to Next Table
              pkb.AddInfoToTable(TableIdentifier::kNext, previous_stmt_num, vector<int>{ while_stmt_num.top() });
            }

            is_prev_stmt_while = true;
          }
          end_tokens.pop();

        }
      }
    } else if (token != end(tokens_lst) - 1 && (next(token, 1)->text_ == "=" || token->text_ == "read" 
      || token->text_ == "print" || token->text_ == "call")) {
      stmt_num += 1;
      is_new_container = false;
      vector<Token> tokens;
      while (token->type_ != TokenType::SEMICOLON && token != end(tokens_lst) - 1) {
        tokens.push_back(*token);
        token++;
      }

      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      } else if (is_prev_stmt_while) {
        vector<int> next_stmt_nums_for_while = { while_stmt_num.top() + 1, stmt_num };
        // Add stmt num of while and next stmt num to Next Table
        pkb.AddInfoToTable(TableIdentifier::kNext, while_stmt_num.top(), next_stmt_nums_for_while);
        while_stmt_num.pop();
        is_prev_stmt_while = false;
        next_stmt_nums_for_while = {};
      } else {
        populateNextRelationship(previous, pkb, stmt_num);
      }

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsRelationship(previous, pkb, stmt_num);

      if (tokens.at(INDEX_OF_EQUAL_OP).text_ == "=") {
        stmt_lst.push_back(make_shared<AssignStmt>(AssignStmt(tokens, stmt_num)));
        cfg_tokens.push_back(CFGToken(CFGTokenType::kAssign, stmt_num));

      } else if (tokens.at(INDEX_OF_STMT_TYPE).text_ == "read") {
        stmt_lst.push_back(make_shared<ReadStmt>(ReadStmt(tokens, stmt_num)));
        cfg_tokens.push_back(CFGToken(CFGTokenType::kRead, stmt_num));

      } else if (tokens.at(INDEX_OF_STMT_TYPE).text_ == "print") {
        stmt_lst.push_back(make_shared<PrintStmt>(PrintStmt(tokens, stmt_num)));
        cfg_tokens.push_back(CFGToken(CFGTokenType::kPrint, stmt_num));

      } else {
        stmt_lst.push_back(make_shared<CallStmt>(CallStmt(tokens, stmt_num)));
        cfg_tokens.push_back(CFGToken(CFGTokenType::kCall, stmt_num));

      }

    } else if (token->text_ == "procedure") {
      is_new_container = true;
      vector<Token> tokens;
      while (token->type_ != TokenType::LEFT_CURLY && token != end(tokens_lst) - 1) {
        tokens.push_back(*token);
        token++;
      }

      if (!proc_tokens.empty()) {
        proc_lst_.push_back(Procedure(proc_tokens, stmt_lst));
        proc_tokens = {};
        stmt_lst = {};

        cfg_tokens.push_back(CFGToken(CFGTokenType::kEnd, 0));

        // TODO: Remove debug statements
        for (int i = 0; i < cfg_tokens.size(); i++) {
          cfg_tokens.at(i).print();
        }

        cfg::CFG cfg = cfg::CFG::GenerateCfg(cfg_tokens);
        pkb.AddCfg(make_shared<cfg::CFG>(cfg));

      }
      
      proc_tokens = tokens;
      curly_bracket_count += 1;

      // reset values for next procedure
      cfg_tokens = { CFGToken(CFGTokenType::kStart, 0) };
      previous = {};
      parent = {};
      children = {};
      children.push({});
      while_stmt_num = {};
      if_stmt_num = {};
      last_stmt_nums_in_if = {};
      is_prev_stmt_if = false;
      is_prev_stmt_while = false;

    } else if (token->text_ == "while" || token->text_ == "if") {
      stmt_num += 1;
      is_new_container = true;
      vector<Token> tokens;
      while (token->type_ != TokenType::LEFT_CURLY && token != end(tokens_lst) - 1) {
        tokens.push_back(*token);
        token++;
      }

      if (is_prev_stmt_if) {
        last_stmt_nums_in_if = populateNextRelationshipForIf(last_stmt_nums_in_if, pkb, stmt_num);
        is_prev_stmt_if = false;
      } else if (is_prev_stmt_while) {
        vector<int> next_stmt_nums_for_while = { while_stmt_num.top() + 1, stmt_num };
        // Add stmt num of while and next stmt num to Next Table
        pkb.AddInfoToTable(TableIdentifier::kNext, while_stmt_num.top(), next_stmt_nums_for_while);
        while_stmt_num.pop();
        is_prev_stmt_while = false;
      } else {
        populateNextRelationship(previous, pkb, stmt_num);
      }

      children = populateParentRelationship(parent, children, stmt_num);
      previous = populateFollowsRelationship(previous, pkb, stmt_num);
      parent.push(stmt_num);
      children.push({});
      previous.push(stmt_num + 1);

      if (tokens.at(INDEX_OF_STMT_TYPE).text_ == "while") {
        stmt_lst.push_back(make_shared<WhileStmt>(WhileStmt(tokens, stmt_num)));
        cfg_tokens.push_back(CFGToken(CFGTokenType::kWhile, stmt_num));
        end_tokens.push("while");

        while_stmt_num.push(stmt_num);

      } else {
        stmt_lst.push_back(make_shared<IfStmt>(IfStmt(tokens, stmt_num)));
        if_else_stmts += 1;

        cfg_tokens.push_back(CFGToken(CFGTokenType::kIf, stmt_num));
        end_tokens.push("if");

        if_stmt_num.push(stmt_num);
        last_stmt_nums_in_if.push({});
      }

      curly_bracket_count += 1;

    } else {
      throw InvalidSyntaxException();
    }
  }

  proc_lst_.push_back(Procedure(proc_tokens, stmt_lst));
  cfg_tokens.push_back(CFGToken(CFGTokenType::kEnd, 0));

  // TODO: Remove debug statements
  for (int i = 0; i < cfg_tokens.size(); i++) {
    cfg_tokens.at(i).print();
  }

  cfg::CFG cfg = cfg::CFG::GenerateCfg(cfg_tokens);
  pkb.AddCfg(make_shared<cfg::CFG>(cfg));

  if (curly_bracket_count != 0 || if_else_stmts != 0) {
    throw InvalidSyntaxException();
  }
}

bool Dfs(const string& vertex, vector<string>& vertices, vector<vector<string>>& edges,
  unordered_set<string>& visited, unordered_set<string>& finished) {
  if (finished.find(vertex) != finished.end()) {
    return true;
  }

  if (visited.find(vertex) != visited.end()) {
    return false;
  }

  visited.insert(vertex);

  //find the index of vertex in vertices vector
  auto iter = find(vertices.begin(), vertices.end(), vertex);
  int idx = iter - vertices.begin();

  for (auto& neighbour : edges[idx]) {
    if (!Dfs(neighbour, vertices, edges, visited, finished)) {
      return false;
    }
  }

  finished.insert(vertex);
  return true;
}

void CheckCycle(vector<string>& vertices, vector<vector<string>>& edges) {
  unordered_set<string> visited({});
  unordered_set<string> finished({}); //Vertices in finished have no cyclic dependecies

  for (auto& vertex : vertices) {
    if (!(finished.find(vertex) != finished.end())) {
      //We only visit vertices that are not in finished
      if (!Dfs(vertex, vertices, edges, visited, finished)) {
        throw InvalidSemanticException();
      }
    }
  }
}

void ValidateProcedureCalls(vector<string>& procedures, vector<vector<string>>& calls) {

  int min_procedures = 1;
  if (procedures.size() < min_procedures) {
    throw InvalidSyntaxException();
  }

  //Check whether callee exist 
  for (auto& call : calls) {
    for (auto& callee : call) {
      if (find(procedures.begin(), procedures.end(), callee) == procedures.end()) {
        throw InvalidSemanticException();
      }
    }
  }

  CheckCycle(procedures, calls);
}

void Parser::Validate() {

  // keep track of procedures and called procedures
  vector<string> procedures = {};
  // Each set will store the procedure name called (might have multiple calls to the same procedure)
  vector<vector<string>> called_procedures_set = {};

  for (Procedure proc : proc_lst_) {
    proc.Validate();

    procedures.push_back(proc.GetProcName());
    called_procedures_set.push_back(proc.GetCalledProcedures());
  }

  ValidateProcedureCalls(procedures, called_procedures_set);
}

void Parser::Populate(Pkb& pkb) {
  for (Procedure proc : proc_lst_) {
    proc.PopulateEntities(pkb);
  }

  try {
    PopulateNestedRelationships(pkb);
  } catch (exception& e) {
    throw invalid_argument("PKB Population failed");
  }
}
