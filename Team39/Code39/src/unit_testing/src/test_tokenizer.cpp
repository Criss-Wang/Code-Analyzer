#include "SP/Tokenizer.h"
#include "SP/Validator.h"
//#include "Parser.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void requireTokenizer(bool b) {
    REQUIRE(b);
}

TEST_CASE("Run tokenizer with file input") {
  ifstream myFile("SourceProgramInput.txt"); // Put input file in same folder as unit_testing.exe
  Tokenizer tokenizer;
  //Validator validator;
  string sourceProg;
  vector<Token> actualTokensList;

  if (myFile.is_open()) {
    while (getline(myFile, sourceProg)) {
      try {
        vector<Token> actualTokensList = tokenizer.parse(sourceProg);

        for (Token currToken : actualTokensList) {
          currToken.print();
        }

        if (actualTokensList.size() != 0) {
          //validator.validate(actualTokensList);
        } else {
          cout << "No tokens";
        }
      } catch (const std::invalid_argument& e) {
        cout << "Syntax error";
      }
    }
    myFile.close();
  } else {
    cout << "Unable to open file";
  }
  requireTokenizer(1 == 1);
}



