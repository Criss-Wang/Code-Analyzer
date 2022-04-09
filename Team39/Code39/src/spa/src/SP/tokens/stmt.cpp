#include "stmt.h"
#include "SP/sp_exceptions.h"

#define STMT_TOKENS_SIZE 2
#define INDEX_OF_VAR 1
#define INDEX_OF_LHS_VAR 0
#define INDEX_OF_RHS_VAR 2
#define MIN_SIZE_OF_ASSIGN 3
#define MIN_SIZE_OF_IF 7
#define MIN_SIZE_OF_WHILE 6

int Stmt::GetStmtNum() {
  return stmt_num_;
}

AssignStmt::AssignStmt(std::vector<Token>& tokens, int stmt_num) {
  if (tokens.size() < MIN_SIZE_OF_ASSIGN) {
    throw InvalidSyntaxException();
  }

  stmt_num_ = stmt_num;

  if (tokens.at(INDEX_OF_LHS_VAR).type_ != TokenType::NAME && tokens.at(INDEX_OF_LHS_VAR).type_ != TokenType::LETTER) {
    throw InvalidSyntaxException();
  }
  lhs_var_ = tokens.at(INDEX_OF_LHS_VAR).text_;

  vector<Token>::const_iterator pattern_start = tokens.begin() + INDEX_OF_RHS_VAR;
  vector<Token>::const_iterator pattern_end = tokens.end();
  vector<Token> pattern(pattern_start, pattern_end);
  rhs_pattern_ = AssignmentPattern(pattern);
}

void AssignStmt::PopulateRelationships(unordered_set<string>& uses_p, unordered_set<string>& modifies_p, unordered_set<string>& called_procedures) {
  uses_p.merge(rhs_pattern_.GetVars());
  modifies_p.insert(lhs_var_);
}

void AssignStmt::PopulateEntities(Pkb& pkb) {
  // Add stmt num to stmt_set_ and assign_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num_);
  pkb.AddEntityToSet(EntityIdentifier::kAssign, stmt_num_);

  // Add lhs_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, lhs_var_);

  // Add stmt num and LHS variable to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num_, vector<string>{ lhs_var_ });

  rhs_pattern_.PopulateEntities(pkb, stmt_num_);
}

ReadStmt::ReadStmt(std::vector<Token>& tokens, int stmt_num) {

  if (tokens.size() != STMT_TOKENS_SIZE) {
    throw InvalidSyntaxException();
  }
  stmt_num_ = stmt_num;

  if (tokens.at(INDEX_OF_VAR).type_ != TokenType::NAME && tokens.at(INDEX_OF_VAR).type_ != TokenType::LETTER) {
    throw InvalidSyntaxException();
  }
  read_var_ = tokens.at(INDEX_OF_VAR).text_;
}

void ReadStmt::PopulateRelationships(unordered_set<string>& uses_p, unordered_set<string>& modifies_p, unordered_set<string>& called_procedures) {
  modifies_p.insert(read_var_);
}

void ReadStmt::PopulateEntities(Pkb& pkb) {
  // Add stmt num to stmt_set_ and read_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num_);
  pkb.AddEntityToSet(EntityIdentifier::kRead, stmt_num_);

  // Add read_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, read_var_);

  // Add stmt num and read_var to ReadTable
  pkb.AddInfoToTable(TableIdentifier::kRead, stmt_num_, read_var_);

  // Add stmt num and read_var to Modifies Table
  pkb.AddInfoToTable(TableIdentifier::kModifiesStmtToVar, stmt_num_, vector<string>{read_var_});
}

PrintStmt::PrintStmt(std::vector<Token>& tokens, int stmt_num) {

  if (tokens.size() != STMT_TOKENS_SIZE) {
    throw InvalidSyntaxException();
  }
  stmt_num_ = stmt_num;

  if (tokens.at(INDEX_OF_VAR).type_ != TokenType::NAME && tokens.at(INDEX_OF_VAR).type_ != TokenType::LETTER) {
    throw InvalidSyntaxException();
  }
  print_var_ = tokens.at(INDEX_OF_VAR).text_;
}

void PrintStmt::PopulateRelationships(unordered_set<string>& uses_p, unordered_set<string>& modifies_p, unordered_set<string>& called_procedures) {
  uses_p.insert(print_var_);
}

void PrintStmt::PopulateEntities(Pkb& pkb) {
  // Add stmt num to stmt_set_ and print_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num_);
  pkb.AddEntityToSet(EntityIdentifier::kPrint, stmt_num_);

  // Add print_var to variable_set_
  pkb.AddEntityToSet(EntityIdentifier::kVariable, print_var_);

  // Add stmt num and print_var to PrintTable
  pkb.AddInfoToTable(TableIdentifier::kPrint, stmt_num_, print_var_);

  // Add stmt num and print_var to Uses Table
  pkb.AddInfoToTable(TableIdentifier::kUsesStmtToVar, stmt_num_, vector<string>{ print_var_ });
}

IfStmt::IfStmt(std::vector<Token>& tokens, int stmt_num) {
  stmt_num_ = stmt_num;

  // "if", "(", "1", "==", "1", ")", "then"
  if (tokens.size() < MIN_SIZE_OF_IF) {
    throw InvalidSyntaxException();
  }

  const int kIndexOfLeftParen = 1;
  const int kIndexOfFirstVar = 2;
  const int kIndexOfThen = tokens.size() - 1;
  const int kIndexOfRightParen = kIndexOfThen - 1;

  bool check_left_paren = tokens.at(kIndexOfLeftParen).type_ == TokenType::LEFT_PAREN;
  bool check_right_paren = tokens.at(kIndexOfRightParen).type_ == TokenType::RIGHT_PAREN;
  bool check_then_keyword = tokens.at(kIndexOfThen).text_ == "then";

  bool is_valid = check_left_paren && check_right_paren && check_then_keyword;

  if (!is_valid) {
    throw InvalidSyntaxException();
  }

  vector<Token>::const_iterator first = tokens.begin() + kIndexOfFirstVar;
  vector<Token>::const_iterator last = tokens.begin() + kIndexOfRightParen;
  vector<Token> cond_expr(first, last);

  cond_expr_ = ConditionalExpression(cond_expr);

}

void IfStmt::PopulateRelationships(unordered_set<string>& uses_p, unordered_set<string>& modifies_p, unordered_set<string>& called_procedures) {
  uses_p.merge(cond_expr_.GetVars());
}

void IfStmt::PopulateEntities(Pkb& pkb) {
  // Add stmt num to stmt_set_ and if_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num_);
  pkb.AddEntityToSet(EntityIdentifier::kIf, stmt_num_);

  cond_expr_.PopulateEntities(pkb, stmt_num_);

  if (!cond_expr_.GetVars().empty()) {
    vector<string> vars;
    for (string v : cond_expr_.GetVars()) {
      vars.push_back(v);
    }
    // Add stmt num and variables in cond expr into If Table
    pkb.AddInfoToTable(TableIdentifier::kIf, stmt_num_, vars);

    // Add stmt num and cond expr into if_pattern_to_stmt Table
    pkb.AddInfoToTable(TableIdentifier::kIfPattern, stmt_num_, vars);
  }
}

WhileStmt::WhileStmt(std::vector<Token>& tokens, int stmt_num) {
  stmt_num_ = stmt_num;

  // "while", "(", "1", "==", "1", ")"
  if (tokens.size() < MIN_SIZE_OF_WHILE) {
    throw InvalidSyntaxException();
  }

  const int kIndexOfLeftParen = 1;
  const int kIndexOfFirstVar = 2;
  const int kIndexOfRightParen = tokens.size() - 1;

  bool check_left_paren = tokens.at(kIndexOfLeftParen).type_ == TokenType::LEFT_PAREN;
  bool check_right_paren = tokens.at(kIndexOfRightParen).type_ == TokenType::RIGHT_PAREN;

  bool is_valid = check_left_paren && check_right_paren;

  if (!is_valid) {
    throw InvalidSyntaxException();
  }

  vector<Token>::const_iterator cond_expr_start = tokens.begin() + kIndexOfFirstVar;
  vector<Token>::const_iterator cond_expr_end = tokens.begin() + kIndexOfRightParen;

  vector<Token> cond_expr(cond_expr_start, cond_expr_end);

  cond_expr_ = ConditionalExpression(cond_expr);

}

void WhileStmt::PopulateRelationships(unordered_set<string>& uses_p, unordered_set<string>& modifies_p, unordered_set<string>& called_procedures) {
  uses_p.merge(cond_expr_.GetVars());
}

void WhileStmt::PopulateEntities(Pkb& pkb) {
  // Add stmt num to stmt_set_ and while_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num_);
  pkb.AddEntityToSet(EntityIdentifier::kWhile, stmt_num_);

  cond_expr_.PopulateEntities(pkb, stmt_num_);

  if (!cond_expr_.GetVars().empty()) {
    vector<string> vars;
    for (string v : cond_expr_.GetVars()) {
      vars.push_back(v);
    }
    // Add stmt num and variables in cond expr into While Table
    pkb.AddInfoToTable(TableIdentifier::kWhile, stmt_num_, vars);

    // Add stmt num and cond expr into while_pattern_to_stmt Table
    pkb.AddInfoToTable(TableIdentifier::kWhilePattern, stmt_num_, vars);
  }

}

CallStmt::CallStmt(std::vector<Token>& tokens, int stmt_num) {
  int expected_size = 2;
  if (tokens.size() != expected_size) {
    throw InvalidSyntaxException();
  }
  stmt_num_ = stmt_num;

  if (tokens.at(INDEX_OF_VAR).type_ != TokenType::NAME && tokens.at(INDEX_OF_VAR).type_ != TokenType::LETTER) {
    throw InvalidSyntaxException();
  }
  called_proc_ = tokens.at(INDEX_OF_VAR).text_;
}

void CallStmt::PopulateRelationships(unordered_set<string>& uses_p, unordered_set<string>& modifies_p, unordered_set<string>& called_procedures) {
  called_procedures.insert(called_proc_);
}

void CallStmt::PopulateEntities(Pkb& pkb) {
  // Add stmt num to stmt_set_ and call_set_
  pkb.AddEntityToSet(EntityIdentifier::kStmt, stmt_num_);
  pkb.AddEntityToSet(EntityIdentifier::kCall, stmt_num_);

  // Add stmt num and call_proc to CallerTable
  pkb.AddInfoToTable(TableIdentifier::kCaller, stmt_num_, called_proc_);
}
