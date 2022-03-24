#include "procedure.h"
#include "SP/sp_exceptions.h"
#include "Utility/CFG/control_flow_graph.h"

#include<stack>

#define kIndexZero 0
#define kIndexOne 1
#define kIndexTwo 2

vector<string> AppendToVector(vector<string> v, vector<string> vars) {
  for (auto var : vars) {
    if (find(begin(v), end(v), var) == end(v)) {
      v.push_back(var);
    }
  }
  return v;
}

Procedure::Procedure(vector<Token>& tokens, vector<shared_ptr<Stmt>>& stmt_lst) {
  int expected_size = 2;

  if (tokens.size() != expected_size) {

    throw InvalidSyntaxException();
  }
  proc_name_ = tokens.at(kIndexOne).text_;
  stmt_lst_ = stmt_lst;

  for (shared_ptr<Stmt> stmt : stmt_lst_) {
    if (typeid(*stmt) == typeid(ReadStmt)) {
      modifies_p_ = AppendToVector(modifies_p_, { (*stmt).GetVar() });
    } else if (typeid(*stmt) == typeid(PrintStmt) || typeid(*stmt) == typeid(IfStmt) || typeid(*stmt) == typeid(WhileStmt)) {
      uses_p_ = AppendToVector(uses_p_, { (*stmt).GetVar() });
    } else if (typeid(*stmt) == typeid(CallStmt)) {
      called_procedures_ = AppendToVector(called_procedures_, { (*stmt).GetVar() });
    } else if (typeid(*stmt) == typeid(AssignStmt)) {
      vector<string> vars = (*stmt).GetVar();
      string lhs_var = vars.at(0);
      vector<string> rhs_var(vars.begin() + kIndexOne, vars.end());
      modifies_p_ = AppendToVector(modifies_p_, { lhs_var });
      uses_p_ = AppendToVector(uses_p_, rhs_var);
    }
  }
}

std::string Procedure::GetProcName() {
  return proc_name_;
}

vector<string> Procedure::GetCalledProcedures() {
  return called_procedures_;
}

void Procedure::Validate() {
  bool is_valid = isalpha(proc_name_[0]);
  if (!is_valid) {
    throw InvalidSyntaxException();
  }

  for (shared_ptr<Stmt> stmt : stmt_lst_) {
    (*stmt).Validate();
  }
}

void Procedure::PopulateEntities(Pkb& pkb) {

  for (shared_ptr<Stmt> stmt : stmt_lst_) {
    (*stmt).PopulateEntities(pkb);
  }

  // Add procedure name to procedure_set_
  pkb.AddEntityToSet(EntityIdentifier::kProc, proc_name_);

  // Populate ProcRangeTable
  int start_stmt_num = (*stmt_lst_.at(kIndexZero)).GetStmtNum();
  int end_stmt_num = (*stmt_lst_.at(stmt_lst_.size() - 1)).GetStmtNum();
  pkb.AddInfoToTable(TableIdentifier::kProcedure, proc_name_, pair<int, int>(start_stmt_num, end_stmt_num));

  // Populate CallsTable, ModifiesProcToVariablesTable, UsesProcToVariablesTable
  pkb.AddInfoToTable(TableIdentifier::kCalls, proc_name_, called_procedures_);
  pkb.AddInfoToTable(TableIdentifier::KModifiesProcToVar, proc_name_, modifies_p_);
  pkb.AddInfoToTable(TableIdentifier::kUsesProcToVar, proc_name_, uses_p_);

}
