#pragma once
#include <string>
#include "PKB/pkb.h"

using namespace std;


// TODO (Yuxuan): [Integration] Use actual vector<Token> returned by Tokenizer to populate
// To be deleted after tokenizer is completed.
struct Token {
	string type;
	string text;
	int line_num;
};

// change back to parse(string source_program_input) after tokenizer is done.
void parse(vector<Token> input);
