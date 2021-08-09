#define USE_TOKEN_TYPES_MACRO
#include "Token.hh"

using namespace glosso::glossoc;

Token::Token(TokenType tokType, const char* literal)
    : tokType(tokType)
    , literal(literal)
{
}

static constexpr size_t strLen(const char* str)
{
    return *str ? 1 + strLen(str + 1) : 0;
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

#define T(_t, _tlit)         \
    case fnv1Hash(_tlit, 0): \
        return TokenType::_t;

TokenType glosso::glossoc::takeKeywords(const char* str, size_t len)
{
    switch (fnv1Hash(str, len))
    {
        TOKEN_KEYWORDS(T)
    default:
        return TokenType::Ident;
    }
}

#undef T

#define T(_t, _tlit)    \
    case TokenType::_t: \
        os << (#_t);    \
        break;

std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const TokenType& tokKind)
{
    switch (tokKind)
    {
        TOKEN_TYPES(T)
        TOKEN_KEYWORDS(T)
    }

    return os;
}

#undef T

std::ostream& glosso::glossoc::operator<<(std::ostream& os, const Token& token)
{
    os << "{ Token: " << token.tokType << ", Literal: " << token.literal
       << " }";
    return os;
}
