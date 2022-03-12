#pragma once

using namespace std; 

enum class CFGTokenType {
  kStart,
  kAssign,
  kRead,
  kPrint,
  kCall,
  kIfStart,
  kIfElse,
  kIfEnd,
  kWhileStart,
  kWhileEnd,
  kEnd
};

class CFGToken {
  public:

    ~CFGToken() = default;

    CFGToken();

    CFGToken(CFGTokenType type, int stmt_num);

    void print();

  private:
    CFGTokenType type_;
    int stmt_num_;
};
