#pragma once

#include <string>

#include "PKB/pkb.h"
#include "tokens/procedure.h"
#include "tokens/stmt.h"
#include "tokenizer.h"

class Parser {
  public:
    Parser(const std::string& input);

    void Validate();
    void Populate(Pkb& pkb);

  private:
    Tokenizer tokenizer_;
    vector<Procedure> proc_lst_ = {};
    vector<vector<string>> called_procedures_set_ = {};
};
