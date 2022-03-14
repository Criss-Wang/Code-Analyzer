#pragma once

enum class CFGTokenType {
  kStart,
  kAssign,
  kRead,
  kPrint,
  kCall,
  kWhile,
  kIf,
  kThenEnd,
  kElseEnd,
  kWhileEnd,
  kEnd
};

class CFGToken {
  public:

    ~CFGToken() = default;

    CFGToken();

    CFGToken(CFGTokenType type, int stmt_num);

    void print();

  public:
    CFGTokenType type_;
    int stmt_num_;
};
