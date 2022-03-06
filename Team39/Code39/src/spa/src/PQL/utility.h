#pragma once

#include <string>
#include <sstream>

namespace pql {
  bool IsLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
  }

  bool IsDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool IsIdent(const std::string& ident) {
    return ident[0] == '\"' && ident[ident.length() - 1] == '\"';
  }

  bool IsInteger(const std::string& s) {
    std::stringstream ssm;
    ssm << s;
    if (!pql::IsDigit(ssm.get())) {
      return false;
    }
    return true;
  }
}
