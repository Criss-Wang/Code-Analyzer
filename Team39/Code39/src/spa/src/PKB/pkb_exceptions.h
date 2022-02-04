#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

class NonEmptyKeyException : public exception {
  public:
    const char * what() const throw() {
      return "Key is already in use";
    }
};

class InvalidKeyException : public exception {
  public:
    const char * what() const throw() {
      return "Key does not exist in table";
    }
};

class InvalidIdentifierException : public exception {
public:
  const char* what() const throw() {
    return "Invalid identifier given";
  }
};
