#pragma once

#include <string>
#include <vector>

#include "stmt.h"
#include "SP/tokenizer.h"

class Procedure {
  public:
    Procedure(vector<Token>& tokens, vector<shared_ptr<Stmt>>& stmt_lst_);

    std::string GetProcName();
    vector<string> GetCalledProcedures();

    void Validate();
    void PopulateEntities(Pkb& pkb);

  private:
    std::string proc_name_ = "";
    vector<shared_ptr<Stmt>> stmt_lst_ = {};
    vector<string> called_procedures_ = {};
    vector<string> modifies_p_ = {};
    vector<string> uses_p_ = {};
    vector<int> right_curly_brackets_ = {};
    vector<int> else_stmt_ = {};
};
