#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TOKEN_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TOKEN_HH_

#include <cstddef>
#include <iostream>
#include <string_view>

#define TOKEN_TYPES(T)         \
    T(StartStmtBlock, nullptr) \
    T(EndStmtBlock, nullptr)   \
    T(EndStmt, nullptr)        \
    T(Ident, nullptr)          \
    T(Integer, nullptr)        \
    T(Plus, nullptr)           \
    T(Minus, nullptr)          \
    T(Star, nullptr)           \
    T(Slash, nullptr)          \
    T(Assign, nullptr)         \
    T(AddAssign, nullptr)      \
    T(SubAssign, nullptr)      \
    T(StarAssign, nullptr)     \
    T(SlashAssign, nullptr)    \
    T(Ampersand, nullptr)      \
    T(BackSlash, nullptr)      \
    T(Eq, nullptr)             \
    T(Neq, nullptr)            \
    T(Lt, nullptr)             \
    T(Gt, nullptr)             \
    T(LtEq, nullptr)           \
    T(GtEq, nullptr)           \
    T(Bang, nullptr)           \
    T(Comma, nullptr)          \
    T(Period, nullptr)         \
    T(Colon, nullptr)          \
    T(SemiColon, nullptr)      \
    T(Hat, nullptr)            \
    T(Percent, nullptr)        \
    T(RangeIncluded, nullptr)  \
    T(RangeExcluded, nullptr)  \
    T(RightArrow, nullptr)     \
    T(Lparen, nullptr)         \
    T(Rparen, nullptr)

#define TOKEN_KEYWORDS(T) \
    T(Elif, "elif")       \
    T(Else, "else")       \
    T(For, "for")         \
    T(Function, "fn")     \
    T(If, "if")           \
    T(In, "in")           \
    T(Return, "return")   \
    T(Struct, "struct")   \
    T(Enum, "enum")       \
    T(Union, "union")     \
    T(While, "while")     \
    T(Const, "const")     \
    T(CharType, "char")   \
    T(IntType, "int")

namespace glosso::glossoc
{

#define T(_t, _tlit) _t,
enum class TokenType
{
    TOKEN_TYPES(T) TOKEN_KEYWORDS(T)
};
#undef T

class Token
{
  public:
    Token() = default;
    Token(TokenType tokType, const char* literal);
    ~Token() = default;
    friend std::ostream& operator<<(std::ostream& os, const Token& token);

  public:
    TokenType tokType;
    std::string_view literal;
};

TokenType takeKeywords(const char* str, size_t len);

std::ostream& operator<<(std::ostream& os, const TokenType& tokKind);
} // namespace glosso::glossoc

#ifndef USE_TOKEN_TYPES_MACRO
#undef TOKEN_TYPES
#undef TOKEN_KEYWORDS
#endif // USE_TOKEN_TYPES_MACRO

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TOKEN_HH_
