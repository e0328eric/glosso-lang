#include "Ast_Expressions.hh"

using namespace glosso::glossoc;

Identifier::Identifier(std::string_view name)
    : mName(name)
{
}

const std::string_view& Identifier::getName() const { return mName; }

TypeName::TypeName(std::string_view name)
    : mName(name)
{
}

const std::string_view& TypeName::getName() const { return mName; }

Expression::Expression(ExprType type)
    : mType(type)
    , mInner({nullptr})
{
}

Expression::Expression(ExprType type, void* inner)
    : mType(type)
    , mInner({nullptr})
{
    switch (mType)
    {
    case ExprType::Identifier:
        mInner.identExpr = static_cast<Identifier*>(inner);
        break;

    case ExprType::TypeName:
        mInner.typenameExpr = static_cast<TypeName*>(inner);
        break;
    }
}

Expression::~Expression()
{
    switch (mType)
    {
    case ExprType::Identifier:
        delete mInner.identExpr;
        break;

    case ExprType::TypeName:
        delete mInner.typenameExpr;
        break;
    }
}
