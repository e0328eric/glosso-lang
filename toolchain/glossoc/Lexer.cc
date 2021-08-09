#include <cassert>
#include <cctype>
#include <cstdio>

#include "Lexer.hh"

using namespace glosso::glossoc;
using Err = glosso::glossoc::GlossocErr;

#define APPEND_TOKEN(_tokType, _literal) \
    tokens.emplace_back(TokenType::_tokType, _literal);

//// Static Functions ////////
template <typename Char>
static bool isEitherChar(char c, Char toCmp)
{
    return c == toCmp;
}

template <typename Char, typename... Chars>
static bool isEitherChar(char c, Char toCmp, Chars... toCmps)
{
    return (c == toCmp) || isEitherChar(c, toCmps...);
}
static bool isLetter(const char& c);
////////

Lexer::Lexer(const char* source, size_t maxErrNum)
    : mSource(source)
    , mStart(source)
    , mCurrent(source)
    , mPeek(source + 1)
    , mIsFinished(false)
    , mLocation(1, 1)
    , mNested()
    , mNestedCount(0)
    , mCurrentNested(0)
    , mMaxErrorNum(maxErrNum)
    , mErrs(new GlossocErr[maxErrNum])
    , mErrNum(0)
{
}

Lexer::~Lexer() { delete[] mErrs; }

bool Lexer::isFinished() const { return mIsFinished; }

std::vector<GlossocErr> Lexer::takeErr() const
{
    std::vector<GlossocErr> output;
    output.reserve(mErrNum);
    output.resize(mErrNum);

    memcpy((void*)output.data(), mErrs, mErrNum * sizeof(GlossocErr));

    return output;
}

bool Lexer::lexing(std::vector<Token>& tokens)
{
    bool result = true;
    while (!mIsFinished && result)
        result = lexToken(tokens);

    return result;
}

bool Lexer::lexToken(std::vector<Token>& tokens)
{
    mStart = mCurrent;

    switch (*mCurrent)
    {
    case '+':
        if (*mPeek == '=')
        {
            nextChar();
            APPEND_TOKEN(AddAssign, "+=");
        }
        else
            APPEND_TOKEN(Plus, "+");
        nextChar();
        skipWhitespace();
        break;

    case '-':
        switch (*mPeek)
        {
        case '=':
            nextChar();
            APPEND_TOKEN(SubAssign, "-=");
            break;
        case '>':
            nextChar();
            APPEND_TOKEN(RightArrow, "->");
            break;
        default:
            APPEND_TOKEN(Minus, "-");
            break;
        }
        nextChar();
        skipWhitespace();
        break;

    case '*':
        if (*mPeek == '=')
        {
            nextChar();
            APPEND_TOKEN(StarAssign, "*=");
        }
        else
            APPEND_TOKEN(Star, "*");
        nextChar();
        skipWhitespace();
        break;

    case '/':
        switch (*mPeek)
        {
        case '=':
            nextChar();
            APPEND_TOKEN(SlashAssign, "/=");
            break;

        case '/':
        case '*':
            nextChar();
            return lexComment(tokens);

        default:
            APPEND_TOKEN(Slash, "/");
            break;
        }
        nextChar();
        skipWhitespace();
        break;

    case '=':
        if (*mPeek == '=')
        {
            nextChar();
            APPEND_TOKEN(Eq, "==");
        }
        else
            APPEND_TOKEN(Assign, "=");
        nextChar();
        skipWhitespace();
        break;

    case '<':
        if (*mPeek == '=')
        {
            nextChar();
            APPEND_TOKEN(LtEq, "<=");
        }
        else
            APPEND_TOKEN(Lt, "<");
        nextChar();
        skipWhitespace();
        break;

    case '>':
        if (*mPeek == '=')
        {
            nextChar();
            APPEND_TOKEN(GtEq, ">=");
        }
        else
            APPEND_TOKEN(Gt, ">");
        nextChar();
        skipWhitespace();
        break;

    case ',':
        APPEND_TOKEN(Comma, ",");
        nextChar();
        skipWhitespace();
        break;

    case '.':
        if (*mPeek == '.')
        {
            nextChar();
            if (*mPeek == '=')
            {
                nextChar();
                APPEND_TOKEN(RangeIncluded, "..=");
            }
            else
                APPEND_TOKEN(RangeExcluded, "..");
        }
        else
            APPEND_TOKEN(Period, ".");
        nextChar();
        skipWhitespace();
        break;

    case ':':
        if (*mPeek == '\n')
        {
            nextChar();
            nextChar();
            ++mNestedCount;
            while (isEitherChar(*mCurrent, ' ', '\t'))
            {
                ++mNested[mNestedCount];
                nextChar();
            }
            APPEND_TOKEN(StartStmtBlock, "");
        }
        else
        {
            APPEND_TOKEN(Colon, ":");
            nextChar();
            skipWhitespace();
        }
        break;

    case '&':
        APPEND_TOKEN(Ampersand, "&");
        nextChar();
        skipWhitespace();
        break;

    case '!':
        if (*mPeek == '=')
        {
            nextChar();
            APPEND_TOKEN(Neq, "!=");
        }
        else
            APPEND_TOKEN(Bang, "!");
        nextChar();
        skipWhitespace();
        break;

    case '^':
        APPEND_TOKEN(Hat, "^");
        nextChar();
        skipWhitespace();
        break;

    case '(':
        APPEND_TOKEN(Lparen, "(");
        nextChar();
        skipWhitespace();
        break;

    case ')':
        APPEND_TOKEN(Rparen, ")");
        nextChar();
        skipWhitespace();
        break;

    case '\n':
    {
        nextChar();
        APPEND_TOKEN(EndStmt, "");

        mCurrentNested = 0;
        while (isEitherChar(*mCurrent, ' ', '\t'))
        {
            ++mCurrentNested;
            nextChar();
        }

        for (; mNested[mNestedCount] > mCurrentNested && mNestedCount > 0;
             --mNestedCount)
            APPEND_TOKEN(EndStmtBlock, "");
        break;
    }

    case '\0':
        while (mNestedCount-- > 0)
            APPEND_TOKEN(EndStmtBlock, "");

        mIsFinished = true;
        APPEND_TOKEN(EndStmt, "");
        break;

    default:
        if (std::isalpha(*mCurrent) || *mCurrent == '_')
            return lexIdent(tokens);
        else if (std::isdigit(*mCurrent))
            return lexNumber(tokens);
        addErr(GlossocErrKind::UnknownTokenFoundErr, nullptr);
        return false;
    }

    return true;
}

bool Lexer::lexComment(std::vector<Token>& tokens)
{
    switch (*mCurrent)
    {
    case '/':
        while (!isEitherChar(*mCurrent, '\n', '\0'))
            nextChar();
        if (*mCurrent == '\n')
            nextChar();
        break;

    case '*':
        while (!(*mCurrent == '*' && *mPeek == '/') || *mCurrent == '\0')
            nextChar();
        if (*mCurrent == '\n')
        {
            nextChar();
            nextChar();
        }
        break;

    default:
        assert(0 && "Internal Error: Unreachable (lexComment)");
    }

    return lexToken(tokens);
}

bool Lexer::lexIdent(std::vector<Token>& tokens)
{
    Token tok;
    while (isLetter(*mCurrent))
        nextChar();

    auto strLen = (size_t)(mCurrent - mStart);
    tok.literal = {mStart, strLen};
    tok.tokType = glosso::glossoc::takeKeywords(mStart, strLen);
    tokens.push_back(tok);

    if (isEitherChar(*mCurrent, ' ', '\t'))
        nextChar();

    return true;
}

// TODO(#5): Implement parsing floating number, number prefix and number
// postfix
bool Lexer::lexNumber(std::vector<Token>& tokens)
{
    Token tok;
    while (std::isdigit(*mCurrent))
        nextChar();

    auto strLen = (size_t)(mCurrent - mStart);
    tok.literal = {mStart, strLen};
    tok.tokType = TokenType::Integer;
    tokens.push_back(tok);

    if (isEitherChar(*mCurrent, ' ', '\t'))
        nextChar();

    return true;
}

void Lexer::nextChar()
{
    mCurrent = mPeek;
    switch (*mPeek)
    {
    case '\0':
        break;

    case '\n':
        ++mPeek;
        mLocation.newLine();
        break;

    default:
        ++mPeek;
        mLocation.goRight();
        break;
    }
}

void Lexer::skipWhitespace()
{
    while (isEitherChar(*mCurrent, ' ', '\t'))
        nextChar();
}

void Lexer::addErr(GlossocErrKind errKind, const char* msg)
{
    if (mErrNum < mMaxErrorNum)
        mErrs[mErrNum++] = {errKind, mLocation, mSource, msg};
}

static bool isLetter(const char& c) { return std::isalnum(c) || c == '_'; }
