#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

class NonEmptyKeyException final : public exception {
  public:
    const char * what() const throw() {
      return "Key is already in use";
    }
};

class InvalidKeyException final : public exception {
  public:
    const char * what() const throw() {
      return "Key does not exist in table";
    }
};

class EmptyValueException final : public exception {
  public:
    const char* what() const throw() {
      return "Value should not be empty";
    }
};

// Thrown when wrong TableIdentifier or EntityIdentifier is provided
class InvalidIdentifierException final : public exception {
  public:
    const char* what() const throw() {
      return "Invalid identifier given";
    }
};
