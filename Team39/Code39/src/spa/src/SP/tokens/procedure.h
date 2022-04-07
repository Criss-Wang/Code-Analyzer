#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "stmt.h"
#include "SP/tokenizer.h"

class Procedure {
  public:
    Procedure(vector<Token>& tokens, vector<shared_ptr<Stmt>>& stmt_lst_);

    string GetProcName();
    unordered_set<string> GetCalledProcedures();

    void PopulateEntities(Pkb& pkb);

  private:
    string proc_name_ = "";
    vector<shared_ptr<Stmt>> stmt_lst_ = {};
    unordered_set<string> called_procedures_ = {};
    unordered_set<string> modifies_p_ = {};
    unordered_set<string> uses_p_ = {};
};
