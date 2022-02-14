#include <vector>

using namespace std;

#include "Parser.h"
#include "SP/tokenizer.h"
#include "SP/validator.h"
#include "SP/populator.h"

void Parse(string input, Pkb& pkb) {
  
  Tokenizer tokenizer;
  vector<Token> input_tokens = tokenizer.parse(input);

  if (Validate(input_tokens)) {
    populate(input_tokens, pkb);
  }

}
