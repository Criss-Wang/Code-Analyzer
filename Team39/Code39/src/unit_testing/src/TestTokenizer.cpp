#include "Tokenizer.h"
#include "Validator.h"
//#include "Parser.h"
#include "TNode.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

using namespace std;

void require(bool b) {
    REQUIRE(b);
}

TEST_CASE("1st Test") {

    TNode T;
  
  
  
    require(1 == 1);
}

TEST_CASE("Run tokenizer with file input") {
  ifstream myFile("SourceProgramInput.txt"); // Put input file in same folder as unit_testing.exe
  Tokenizer tokenizer;
  Validator validator;
  string sourceProg;
  vector<Token> actualTokensList;

  if (myFile.is_open()) {
    while (getline(myFile, sourceProg)) {
      vector<Token> actualTokensList = tokenizer.parse(sourceProg);

      for (Token currToken : actualTokensList) {
        currToken.print();
      }
      
      if (actualTokensList.size() != 0) {
        //validator.validate(actualTokensList);
      } else {
        cout << "No tokens";
      }
    }
    myFile.close();
  } else {
    cout << "Unable to open file";
  }
  require(1 == 1);
}



