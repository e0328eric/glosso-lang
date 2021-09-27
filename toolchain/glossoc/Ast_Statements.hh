#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_STATEMENTS_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_STATEMENTS_HH_

#include "Ast_Expressions.hh"

namespace glosso::glossoc
{
enum class StmtType
{
    DeclareStmt,
    ReturnStmt,
    IfStmt,
    WhileStmt,
    BlockStmt,
    ExprStmt,
};
class Statement;

class DeclareStmt
{
  public:
    DeclareStmt();
    ~DeclareStmt();

  public:
    TypeName* mTypeName;
    Identifier* mIdentifier;
    Expression* mBody;
};

class ReturnStmt
{
  public:
    ReturnStmt();
    ~ReturnStmt();

  public:
    Expression* mBody;
};

class IfStmt
{
  public:
    IfStmt();
    ~IfStmt();

  public:
    Expression* mCondition;
    Statement* mConsequence;
    Statement* mAlternative;
};

class WhileStmt
{
  public:
    WhileStmt();
    ~WhileStmt();

  public:
    Expression* mCondition;
    Statement* mBody;
};

struct BlockStmtInner
{
    BlockStmtInner* previous;
    Statement* stmt;
    BlockStmtInner* next;
};

class BlockStmt
{
  public:
    BlockStmt();
    ~BlockStmt();

    // Moves a value and change it into nullptr
    void pushStmt(Statement* stmtPtr);
    void pushOwnedStmt(Statement** stmtPtr);
    Statement* popStmt();

  private:
    BlockStmtInner* mHead;
    BlockStmtInner* mTail;
};

class ExprStmt
{
  public:
    ExprStmt();
    ~ExprStmt();

  public:
    Expression* mExpr;
};

class Statement
{
  public:
    Statement();
    ~Statement();

    StmtType getType() const;

  public:
    StmtType mType;
    union
    {
        DeclareStmt* declareStmt;
        ReturnStmt* returnStmt;
        IfStmt* ifStmt;
        WhileStmt* whileStmt;
        BlockStmt* blockStmt;
        ExprStmt* exprStmt;
    } mInner;
};

typedef BlockStmt Program;

} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_STATEMENTS_HH_
