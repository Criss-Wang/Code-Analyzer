#include <vector>

#include "parser.h"
#include "tokens/procedure.h"
#include "sp_exceptions.h"
#include "design_extractor.h"

#define kIndexZero 0
#define kIndexOne 1
#define kIndexTwo 2

Parser::Parser(const std::string& input) {

  vector<Token> tokens_lst = tokenizer_.parse(input);

  vector<Token> proc_tokens = {};
  vector<shared_ptr<Stmt>> stmt_lst = {};

  int stmt_num = 1;

  // 1st round of sytax validation which involves curly brackets
  int curly_bracket_count = 0;
  int if_else_stmts = 0;
  bool is_new_container = true;

  for (auto token = begin(tokens_lst); token != end(tokens_lst); ++token) {
    if (token->text_ == "}") {

      if (is_new_container) {
        throw InvalidSyntaxException();
      }

      bool has_two_more_tokens = token != end(tokens_lst) - 1 && token != end(tokens_lst) - 2;
      if (has_two_more_tokens && next(token, 1)->text_ == "else" && next(token, 2)->type_ == TokenType::LEFT_CURLY) {
        if_else_stmts -= 1;
        token++;
        token++;
        is_new_container = true;

      } else {
        curly_bracket_count -= 1;
      }

    } else if (token != end(tokens_lst) - 1 && (next(token, 1)->text_ == "=" || token->text_ == "read" 
      || token->text_ == "print" || token->text_ == "call")) {
      is_new_container = false;
      vector<Token> tokens;
      while (token->type_ != TokenType::SEMICOLON && token != end(tokens_lst) - 1) {
        tokens.push_back(*token);
        token++;
      }

      if (tokens.at(kIndexOne).text_ == "=") {
        stmt_lst.push_back(make_shared<AssignStmt>(AssignStmt(tokens, stmt_num)));
      } else if (tokens.at(kIndexZero).text_ == "read") {
        stmt_lst.push_back(make_shared<ReadStmt>(ReadStmt(tokens, stmt_num)));
      } else if (tokens.at(kIndexZero).text_ == "print") {
        stmt_lst.push_back(make_shared<PrintStmt>(PrintStmt(tokens, stmt_num)));
      } else {
        stmt_lst.push_back(make_shared<CallStmt>(CallStmt(tokens, stmt_num)));
      }
      stmt_num += 1;

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
      }
      
      proc_tokens = tokens;
      curly_bracket_count += 1;

    } else if (token->text_ == "while" || token->text_ == "if") {
      is_new_container = true;
      vector<Token> tokens;
      while (token->type_ != TokenType::LEFT_CURLY && token != end(tokens_lst) - 1) {
        tokens.push_back(*token);
        token++;
      }

      if (tokens.at(kIndexZero).text_ == "while") {
        stmt_lst.push_back(make_shared<WhileStmt>(WhileStmt(tokens, stmt_num)));
      } else {
        stmt_lst.push_back(make_shared<IfStmt>(IfStmt(tokens, stmt_num)));
        if_else_stmts += 1;
      }
      stmt_num += 1;
      curly_bracket_count += 1;

    } else {
      throw InvalidSyntaxException();
    }
  }

  proc_lst_.push_back(Procedure(proc_tokens, stmt_lst));

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
