#pragma once

#include <unordered_map>

#include "../pkb_exceptions.h"

using namespace std;

/**
 * Template class for tables to record key value pairs
 * Apparently, the implementation must be defined in this file for templates and cannot be defined in .cpp files
 * https://stackoverflow.com/questions/1353973/c-template-linking-error
 */
template <typename T1, typename T2>
class Table {
  protected:
    unordered_map<T1, T2> table_;

  public:
    ~Table() = default;

    unordered_map<T1, T2> GetInternalMap() {
      return this->table_;
    }

    int GetTableSize() {
      return static_cast<int>(table_.size());
    }

    virtual bool KeyExistsInTable(const T1 key) {
      return table_.find(key) != table_.end();
    }

    virtual bool AddKeyValuePair(T1 key, T2 value) {
      if (this->KeyExistsInTable(key)) {
        throw KeyInUseException();
      }
      table_[key] = value;
      if (table_[key] == value) {
        return true;
      }

      return false;
    }

    virtual T2 GetValueByKey(T1 key) {
      if (this->KeyExistsInTable(key)) {
        return table_[key];
      }
      throw InvalidKeyException();
    }

    virtual vector<T1> GetKeyLst() {
      vector<T1> key_set;
      for (const auto& [key, value]: table_) {
        key_set.push_back(key);
      }
      return key_set;
    }

    virtual vector<pair<T1, T2>> GetKeyValueLst() {
      vector<pair<T1, T2>> val_set;
      for (const auto& [key, value] : table_) {
        val_set.push_back(make_pair(key, value));
      }
      return val_set;
    }

    // If key does not exist, add the key into the table. Otherwise, update the key with the new value
    virtual bool UpdateKeyWithNewValue(T1 key, T2 value) {
      try {
        if (!this->KeyExistsInTable(key)) {
          return AddKeyValuePair(key, value);
        }
        table_[key] = move(value);
        return true;
      } catch (InvalidKeyException& e) {
        throw UpdateKeyException();
      }
    }
};

class RelTable : public Table<int, int> {};
class RelListTable : public Table<int, vector<int>> {};
class RelListReverseTable : public Table<int, vector<int>> {
  public:
    virtual bool UpdateKeyValuePair(int value_to_update, const vector<int>& keys);
};

// Tables for Entity Attributes
class EntityVarsTable : public Table<int, string> {};
class EntityIntListTable : public Table<int, vector<int>> {};
class EntityVarsListTable : public Table<int, vector<string>> {};
class IndexToEntityTable : public Table<int, string> {};
class EntityToIndexTable : public Table<string, int> {};

enum class TableType {
  kRelSimple,
  kRelList,
  kRelReverse,
  kRelListOrReverse
};

enum class IndexTableType {
  kVar,
  kVarIndex,
  kProc,
  kProcIndex
};

enum class TableIdentifier {
  kAssign, kRead, kPrint, kConstant, kIf, kWhile, kProcedure,
  kAssignPattern, kIfPattern, kWhilePattern,
  kFollows, kFollowsStar,
  kParent, kParentStar,
  kUsesStmtToVar, kUsesProcToVar, kModifiesStmtToVar, kModifiesProcToVar,
  kCaller, kCalls, kCallsStar,
  kNext
};
