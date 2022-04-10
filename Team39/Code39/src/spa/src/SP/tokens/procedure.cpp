#include "procedure.h"
#include "SP/sp_exceptions.h"
#include "Utility/CFG/control_flow_graph.h"

#include<stack>

#define PROC_TOKENS_SIZE 2
#define INDEX_OF_PROC_NAME 1

Procedure::Procedure(vector<Token>& tokens, vector<shared_ptr<Stmt>>& stmt_lst) {

  if (tokens.size() != PROC_TOKENS_SIZE) {
    throw InvalidSyntaxException();
  }

  if (tokens.at(INDEX_OF_PROC_NAME).type_ != TokenType::NAME && tokens.at(INDEX_OF_PROC_NAME).type_ != TokenType::LETTER) {
    throw InvalidSyntaxException();
  }
  proc_name_ = tokens.at(INDEX_OF_PROC_NAME).text_;
  stmt_lst_ = move(stmt_lst);

  for (shared_ptr<Stmt> stmt : stmt_lst_) {
    stmt->PopulateRelationships(uses_p_, modifies_p_, called_procedures_);
  }
}

std::string Procedure::GetProcName() {
  return proc_name_;
}

unordered_set<string> Procedure::GetCalledProcedures() {
  return called_procedures_;
}

void Procedure::PopulateEntities(Pkb& pkb) {

  for (shared_ptr<Stmt> stmt : stmt_lst_) {
    (*stmt).PopulateEntities(pkb);
  }

  // Add procedure name to procedure_set_
  pkb.AddEntityToSet(EntityIdentifier::kProc, proc_name_);

  const int kIndexOfFirstStmtNum = 0;
  const int kIndexOfLastStmtNum = stmt_lst_.size() - 1;
  // Populate ProcRangeTable
  int start_stmt_num = (*stmt_lst_.at(kIndexOfFirstStmtNum)).GetStmtNum();
  int end_stmt_num = (*stmt_lst_.at(kIndexOfLastStmtNum)).GetStmtNum();
  pkb.AddInfoToTable(TableIdentifier::kProcedure, proc_name_, make_pair(start_stmt_num, end_stmt_num));

  // Populate CallsTable, ModifiesProcToVariablesTable, UsesProcToVariablesTable
  if (!called_procedures_.empty()) {
    vector<string> called_procedures;
    for (string p : called_procedures_) {
      called_procedures.push_back(p);
    }
    pkb.AddInfoToTable(TableIdentifier::kCalls, proc_name_, called_procedures);
  }

  if (!modifies_p_.empty()) {
    vector<string> modifies_p;
    for (string v : modifies_p_) {
      modifies_p.push_back(v);
    }
    pkb.AddInfoToTable(TableIdentifier::kModifiesProcToVar, proc_name_, modifies_p);
  }

  if (!uses_p_.empty()) {
    vector<string> uses_p;
    for (string v : uses_p_) {
      uses_p.push_back(v);
    }
    pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, proc_name_, uses_p);
  }
}
