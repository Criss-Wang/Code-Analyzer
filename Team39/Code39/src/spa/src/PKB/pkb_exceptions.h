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

class UpdateKeyException final : public exception {
  public:
    const char* what() const throw() {
      return "Could not update key with new value in table";
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

class UpdateIndexTableException final : public exception {
  public:
    const char* what() const throw() {
      return "Failed to update index table";
    }
};

class AddKeyValuePairException final : public exception {
  private:
    string message;

  public:
    AddKeyValuePairException(int table_identifier, string error_message)
      : message("Failed to add key value pair to table " + to_string(table_identifier) + ". " + error_message) {}

    const char* what() const throw() {
      return message.c_str();
    }
};

class AddEntityException final : public exception {
  private:
    string message;

  public:
    AddEntityException(int entity_identifier, string error_message)
      : message("Failed to add entity to set " + to_string(entity_identifier) + ". " + error_message) {}

    const char* what() const throw() {
      return message.c_str();
    }
};
