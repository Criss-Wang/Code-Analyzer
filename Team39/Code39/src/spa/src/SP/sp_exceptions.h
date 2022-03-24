#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

class InvalidSyntaxException final : public exception {
public:
  const char* what() const throw() {
    return "The program is syntactically invalid";
  }
};

class InvalidSemanticException final : public exception {
public:
  const char* what() const throw() {
    return "The program is semantically invalid";
  }
};
