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
    virtual bool KeyExistsInTable(const T1 key) {
      return table_.find(key) != table_.end();
    }

  public:
    ~Table() = default;

    int GetTableSize() {
      return static_cast<int>(table_.size());
    }

    virtual bool AddKeyValuePair(T1 key, T2 value) {
      try {
        if (this->KeyExistsInTable(key)) {
          throw NonEmptyKeyException();
        }
        table_[key] = value;
        if (table_[key] == value) {
          return true;
        }

        return false;
      }
      catch (exception& e) {
        throw e;
      }
    }

    virtual T2 GetValueByKey(T1 key) {
      try {
        if (this->KeyExistsInTable(key)) {
          return table_[key];
        }
        throw InvalidKeyException();
      }
      catch (exception& e) {
        throw e;
      }
    }

    virtual vector<T1> GetKeyLst() {
      vector<T1> key_set;
      for (const auto& [key, value]: table_) {
        key_set.push_back(key);
      }
      return key_set;
    }

    virtual vector<T2> GetValueLst() {
      vector<T2> val_set;
      for (const auto& [key, value] : table_) {
        val_set.push_back(value);
      }
      return val_set;
    }
};