#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

class InvalidProgramException final : public exception {
  public:
    const char* what() const throw() {
      return "The program is invalid";
    }
};
