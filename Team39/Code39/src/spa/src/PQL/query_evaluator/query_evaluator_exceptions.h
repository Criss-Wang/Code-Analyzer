#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

namespace pql_exceptions {
  class TrueResultException : public exception {
    public:
      const char* what() const throw() {
        return "The selected boolean value is true";
      }
  };

  class EmptyResultException : public exception {
    public:
      const char* what() const throw() {
        return "The result string should be empty";
      }
  };

  class EmptyDomainException final : public EmptyResultException {
    public:
      const char* what() const throw() {
        return "The domain is empty";
      }
  };

  class FalseRelationException final : public EmptyResultException {
    public:
      const char* what() const throw() {
        return "The relation does not hold";
      }
  };

  class EmptyTableException final : public EmptyResultException {
    public:
      const char* what() const throw() {
        return "Intermediate table is empty";
      }
  };

  class ProcedureDoesNotExistException final : public EmptyResultException {
    public:
      const char* what() const throw() {
        return "The procedure entity given does not appear in the program";
      }
  };

  class VariableDoesNotExistException final : public EmptyResultException {
    public:
      const char* what() const throw() {
        return "The variable entity given does not appear in the program";
      }
  };

  class UnequalWithClauseException final : public EmptyResultException {
    public:
      const char* what() const throw() {
        return "Both side of with clause is unequal";
      }
  };
}
