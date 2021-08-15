#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TOKEN_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TOKEN_HH_

#include <cstddef>
#include <iostream>
#include <string_view>

#include "Span.hh"

#define BASIC_TOKEN(T)    \
    T("", BeginStmt)      \
    T("", EndStmt)        \
    T("", BeginStmtBlock) \
    T("", EndStmtBlock)   \
    T("", Identifier)     \
    T("", Operator)       \
    T("", Integer)        \
    T("", UInteger)       \
    T("", Char)

#define DELIMITER_TOKEN(T) \
    T("(", Lparen)         \
    T(")", Rparen)         \
    T("{", Lbrace)         \
    T("}", Rbrace)         \
    T("[", Lsqbrace)       \
    T("]", Rsqbrace)

#define SPECIAL_OP(T)    \
    T("=", Assign)       \
    T("+", Plus)         \
    T("-", Minus)        \
    T("*", Star)         \
    T("/", Slash)        \
    T("+=", PlusAssign)  \
    T("-=", MinusAssign) \
    T("*=", StarAssign)  \
    T("/=", SlashAssign) \
    T("==", Equal)       \
    T("!=", Neq)         \
    T("<", Lt)           \
    T("<=", LtEq)        \
    T(">", Gt)           \
    T(">=", GtEq)        \
    T("!", Bang)         \
    T(":", Colon)        \
    T(";", Semicolon)    \
    T("&", Ampersand)    \
    T("->", RightArrow)

#define KEYWORD_TOKEN(T) \
    T("fn", Function)    \
    T("return", Return)  \
    T("opdef", Opdef)    \
    T("if", If)          \
    T("elif", Elif)      \
    T("else", Else)      \
    T("while", While)    \
    T("for", For)        \
    T("struct", Struct)  \
    T("enum", Enum)      \
    T("union", Union)

#define TYPE_TOKEN(T)       \
    T("int", IntegerType)   \
    T("i8", I8Type)         \
    T("i16", I16Type)       \
    T("i32", I32Type)       \
    T("i64", I64Type)       \
    T("uint", UIntegerType) \
    T("u8", U8Type)         \
    T("u16", U16Type)       \
    T("u32", U32Type)       \
    T("u64", U64Type)       \
    T("char", CharType)

namespace glosso::glossoc
{
#define T(_ts, _t) _t,
enum class TokenType
{
    BASIC_TOKEN(T) DELIMITER_TOKEN(T) SPECIAL_OP(T) KEYWORD_TOKEN(T)
        TYPE_TOKEN(T) Illegal,
};
#undef T

TokenType strToKeyword(const char* start, size_t len);
TokenType strToOperator(const char* start, size_t len);
std::ostream& operator<<(std::ostream& os, const TokenType& tokType);

class Token
{
  public:
    Token(TokenType type, const char* literal, size_t litLen, Location start,
          Location end);
    ~Token() = default;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);

  public:
    TokenType type;
    std::string_view literal;
    Span spanLocation;
};
} // namespace glosso::glossoc

#ifndef USE_TOKEN_TYPE_MACRO
#undef BASIC_TOKEN
#undef DELIMITER_TOKEN
#undef SPECIAL_OP
#undef KEYWORD_TOKEN
#endif // USE_TOKEN_TYPE_MACRO

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TOKEN_HH_
