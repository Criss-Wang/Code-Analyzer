#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "PKB/pkb.h"
#include "SP/tokenizer.h"
#include "assignment_pattern.h"
#include "conditional_expr.h"

class Stmt {
  public:
    virtual ~Stmt() = default;

    int GetStmtNum();
    virtual string GetVar() { return ""; };
    virtual unordered_set<string> GetVars() { return {}; };

    virtual void PopulateEntities(Pkb& pkb) { };

  protected:
    int stmt_num_ = -1;
};

class AssignStmt : public Stmt {
  public:
    ~AssignStmt() = default;
    AssignStmt(std::vector<Token>& tokens, int stmt_num);

    string GetVar();
    unordered_set<string> GetVars();

    void PopulateEntities(Pkb& pkb);

  private:
    std::string lhs_var_;
    AssignmentPattern rhs_pattern_;
};

class ReadStmt : public Stmt {
  public:
    ~ReadStmt() = default;
    ReadStmt(std::vector<Token>& tokens, int stmt_num);

    string GetVar();

    void PopulateEntities(Pkb& pkb);

  private:
    std::string read_var_;
};

class PrintStmt : public Stmt {
  public:
    ~PrintStmt() = default;
    PrintStmt(std::vector<Token>& tokens, int stmt_num);

    string GetVar();

    void PopulateEntities(Pkb& pkb);

  private:
    std::string print_var_;
};

class IfStmt : public Stmt {
  public:
    ~IfStmt() = default;
    IfStmt(std::vector<Token>& tokens, int stmt_num);

    unordered_set<string> GetVars();

    void PopulateEntities(Pkb& pkb);

  private:
    ConditionalExpression cond_expr_;
};

class WhileStmt : public Stmt {
  public:
    ~WhileStmt() = default;
    WhileStmt(std::vector<Token>& tokens, int stmt_num);

    unordered_set<string> GetVars();

    void PopulateEntities(Pkb& pkb);

  private:
    ConditionalExpression cond_expr_;
};

class CallStmt : public Stmt {
  public:
    ~CallStmt() = default;
    CallStmt(std::vector<Token>& tokens, int stmt_num);

    string GetVar();

    void PopulateEntities(Pkb& pkb);

  private:
    std::string called_proc_;
};
