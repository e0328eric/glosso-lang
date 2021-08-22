#define USE_TOKEN_TYPE_MACRO
#include "Token.hh"

using namespace glosso::glossoc;

static constexpr size_t strLen(const char* str)
{
    size_t output;
    for (output = 0; str[output]; ++output)
        ;
    return output;
    // return *str ? 1 + strLen(str + 1) : 0;
}

static constexpr uint64_t fnv1Hash(const char* str, size_t len)
{
    size_t strlen   = len > 0 ? len : strLen(str);
    uint64_t output = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < strlen; ++i)
    {
        output *= 0x100000001b3ULL;
        output ^= (uint64_t)str[i];
    }

    return output;
}

#define T(_ts, _t)         \
    case fnv1Hash(_ts, 0): \
        return glosso::glossoc::TokenType::_t;

TokenType glosso::glossoc::strToKeyword(const char* start, size_t len)
{
    switch (fnv1Hash(start, len))
    {
        KEYWORD_TOKEN(T)
        TYPE_TOKEN(T)
    default:
        return TokenType::Identifier;
    }
}

TokenType glosso::glossoc::strToOperator(const char* start, size_t len)
{
    switch (fnv1Hash(start, len))
    {
        SPECIAL_OP(T)
    default:
        return TokenType::Operator;
    }
}
#undef T

#define T(_ts, _t)                       \
    case glosso::glossoc::TokenType::_t: \
        os << (#_t);                     \
        break;

std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const TokenType& tokType)
{
    switch (tokType)
    {
        BASIC_TOKEN(T)
        DELIMITER_TOKEN(T) SPECIAL_OP(T) KEYWORD_TOKEN(T) TYPE_TOKEN(T);
    default:
        os << "(Illegal Opcode)";
        break;
    }
    return os;
}
#undef T

Token::Token(Location start, Location end)
    : type(TokenType::Illegal)
    , literal("")
    , spanLocation(start, end)
{
}

Token::Token(TokenType type, const char* literal, Location start, Location end)
    : type(type)
    , literal(literal)
    , spanLocation(start, end)
{
}

Token::Token(TokenType type, const char* literal, size_t litLen, Location start,
             Location end)
    : type(type)
    , literal(literal, litLen)
    , spanLocation(start, end)
{
}

std::ostream& glosso::glossoc::operator<<(std::ostream& os, const Token& tok)
{
    os << "{ TokenType: " << tok.type << ", Literal: " << tok.literal
       << ", Span: " << tok.spanLocation << " }";
    return os;
}
