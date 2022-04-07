#pragma once

#include <string>

#include "PKB/pkb.h"
#include "tokens/procedure.h"
#include "tokens/stmt.h"
#include "tokenizer.h"

class Parser {
  public:
    Parser(const std::string& input, Pkb& pkb);

    void Validate();
    void Populate(Pkb& pkb);

  private:
    Tokenizer tokenizer_ = Tokenizer("");
    vector<Procedure> proc_lst_ = {};
};
