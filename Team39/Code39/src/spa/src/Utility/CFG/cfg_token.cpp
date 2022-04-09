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
