#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_EXPRESSIONS_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_EXPRESSIONS_HH_

#include <string_view>

namespace glosso::glossoc
{
enum class ExprType
{
    Identifier,
    TypeName,
};

class Identifier
{
  public:
    explicit Identifier(std::string_view name);
    ~Identifier() = default;

    const std::string_view& getName() const;

  public:
    std::string_view mName;
};

class TypeName
{
  public:
    explicit TypeName(std::string_view name);
    ~TypeName() = default;

    const std::string_view& getName() const;

  public:
    std::string_view mName;
};

class Expression
{
  public:
    explicit Expression(ExprType type);
    Expression(ExprType type, void* inner);
    ~Expression();

    ExprType getType() const;

  public:
    ExprType mType;
    union
    {
        Identifier* identExpr;
        TypeName* typenameExpr;
    } mInner;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_EXPRESSIONS_HH_
