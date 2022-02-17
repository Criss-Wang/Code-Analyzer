#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

namespace pql_exceptions {
  class EmptyDomainException final : public exception {
    public:
      const char* what() const throw() {
        return "The domain is empty";
      }
  };
}
