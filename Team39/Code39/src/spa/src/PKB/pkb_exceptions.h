#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

class KeyInUseException final : public exception {
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

class BadResultException final : public exception {
  public:
    const char* what() const throw() {
      return "The result given is not meeting the requirement";
    }
};

class ReversePopulationException final : public exception {
  public:
    const char* what() const throw() {
      return "Failed to populate reverse relation";
    }
};
