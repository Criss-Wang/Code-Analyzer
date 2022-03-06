#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

namespace pql_exceptions {
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
}
