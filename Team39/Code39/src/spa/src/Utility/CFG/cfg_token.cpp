#pragma once

#include<iostream>

#include "cfg_token.h"

using namespace std;

CFGToken::CFGToken() {
  this->type_ = CFGTokenType::kStart;
  this->stmt_num_ = 0;
}

CFGToken::CFGToken(CFGTokenType type, int stmt_num) {
  this->type_ = type;
  this->stmt_num_ = stmt_num;
}

void CFGToken::print() {
  string type = "";
  switch (this->type_) {
    case CFGTokenType::kStart:
      type = "start";
      break;
    case CFGTokenType::kAssign:
      type = "assign";
      break;
    case CFGTokenType::kRead:
      type = "read";
      break;
    case CFGTokenType::kPrint:
      type = "print";
      break;
    case CFGTokenType::kCall:
      type = "call";
      break;
    case CFGTokenType::kWhile:
      type = "while";
      break;
    case CFGTokenType::kIf:
      type = "if";
      break;
    case CFGTokenType::kThenEnd:
      type = "thenEnd";
      break;
    case CFGTokenType::kElseEnd:
      type = "elseEnd";
      break;
    case CFGTokenType::kWhileEnd:
      type = "whileEnd";
      break;
    case CFGTokenType::kEnd:
      type = "end";
      break;
    default:
      break;
  }

  cout << "Token type: " << type << " Line: " << this->stmt_num_ << endl;
}
