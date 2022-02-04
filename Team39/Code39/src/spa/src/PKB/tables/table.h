#pragma once

#include <unordered_map>

#include "../pkb_exceptions.h"

/**
 * Template class for tables to record key value pairs
 * Apparently, the implementation must be defined in this file for templates and cannot be defined in .cpp files
 * https://stackoverflow.com/questions/1353973/c-template-linking-error
 */
template <typename T1, typename T2>
class Table {
  protected:
    unordered_map<T1, T2> table_;

  private:
    bool keyExistsInTable(const T1 key) {
      return table_.find(key) != table_.end();
    }

  public:
    ~Table() = default;

    int GetTableSize() {
      return static_cast<int>(table_.size());
    }

    bool AddKeyValuePair(const T1 key, const T2 value) {
      try {
        if (this->keyExistsInTable(key)) {
          throw new NonEmptyKeyException();
        }
        table_[key] = value;
        bool insert_success = table_[key] == value;
        if (insert_success) {
          return true;
        } else {
          return false;
        }
      }
      catch (exception e) {
        throw e;
      }
    }

    T2 GetValueByKey(T1 key) {
      try {
        if (this->keyExistsInTable(key)) {
          return table_[key];
        } else {
          throw new InvalidKeyException();
        }
      }
      catch (exception e) {
        throw e;
      }
    }
};
