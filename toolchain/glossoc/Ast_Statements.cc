#include "Ast_Statements.hh"

using namespace glosso::glossoc;

DeclareStmt::DeclareStmt()
    : mTypeName(nullptr)
    , mIdentifier(nullptr)
    , mBody(nullptr)
{
}

DeclareStmt::~DeclareStmt()
{
    delete mTypeName;
    delete mIdentifier;
    delete mBody;
}

ReturnStmt::ReturnStmt()
    : mBody(nullptr)
{
}

ReturnStmt::~ReturnStmt() { delete mBody; }

IfStmt::IfStmt()
    : mCondition(nullptr)
    , mConsequence(nullptr)
    , mAlternative(nullptr)
{
}

IfStmt::~IfStmt()
{
    delete mCondition;
    delete mConsequence;
    delete mAlternative;
}

WhileStmt::WhileStmt()
    : mCondition(nullptr)
    , mBody(nullptr)
{
}

WhileStmt::~WhileStmt()
{
    delete mCondition;
    delete mBody;
}

BlockStmt::BlockStmt()
    : mHead(new BlockStmtInner{nullptr, nullptr, nullptr})
    , mTail(new BlockStmtInner{nullptr, nullptr, nullptr})
{
    mHead->next     = mTail;
    mTail->previous = mHead;
}

BlockStmt::~BlockStmt()
{
    auto* blockStmt = mTail;
    while (blockStmt)
    {
        auto* tmp = blockStmt->previous;
        delete blockStmt->stmt;
        delete blockStmt;
        blockStmt = tmp;
    }
}

void BlockStmt::pushStmt(Statement* stmtPtr)
{
    auto* blockStmt = new BlockStmtInner{nullptr, stmtPtr, nullptr};

    blockStmt->previous   = mHead;
    blockStmt->next       = mHead->next;
    mHead->next->previous = blockStmt;
    mHead->next           = blockStmt;
}

void BlockStmt::pushOwnedStmt(Statement** stmtPtr)
{
    auto* blockStmt = new BlockStmtInner{nullptr, *stmtPtr, nullptr};

    blockStmt->previous   = mHead;
    blockStmt->next       = mHead->next;
    mHead->next->previous = blockStmt;
    mHead->next           = blockStmt;

    *stmtPtr = nullptr;
}

Statement* BlockStmt::popStmt()
{
    auto* output = mTail->previous->stmt;
    auto* tmp    = mTail->previous;

    tmp->previous->next = mTail;
    mTail->previous     = tmp->previous;

    delete tmp;

    return output;
}

ExprStmt::ExprStmt()
    : mExpr(nullptr)
{
}

ExprStmt::~ExprStmt() { delete mExpr; }

Statement::Statement()
    : mType(static_cast<StmtType>(0))
    , mInner({nullptr})
{
}

Statement::~Statement()
{
    switch (mType)
    {
    case StmtType::DeclareStmt:
        delete mInner.declareStmt;
        break;

    case StmtType::ReturnStmt:
        delete mInner.returnStmt;
        break;

    case StmtType::IfStmt:
        delete mInner.ifStmt;
        break;

    case StmtType::WhileStmt:
        delete mInner.whileStmt;
        break;

    case StmtType::BlockStmt:
        delete mInner.blockStmt;
        break;

    case StmtType::ExprStmt:
        delete mInner.exprStmt;
        break;
    }
}

StmtType Statement::getType() const { return mType; }
