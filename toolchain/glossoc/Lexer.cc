#include <iostream>
#include <string_view>

#include "Lexer.hh"
#include "Utf8.hh"

using namespace glosso::glossoc;
using ErrKind = glosso::glossoc::GlossocErrKind;

// Macros ////
#define TOKENIZE(_type, _literal)                                           \
    do                                                                      \
    {                                                                       \
        nextChar();                                                         \
        output = {TokenType::_type, _literal, startLocation, mCurLocation}; \
    } while (false)

#define PEEK_CHAR(_n) (*(mCurrent + _n))
////////

// Static Functions ////
template <typename Char>
static bool isEitherChar(char c, Char toCmp);

template <typename Char, typename... Chars>
static bool isEitherChar(char c, Char toCmp, Chars... toCmps);

static bool isLetter(char chr);
static bool isDigit(char chr);
static bool isNameChar(char chr);
static bool isOperator(char chr);
////////

Lexer::Lexer(const char* source, size_t maxErrNum)
    : mSource(source)
    , mStart(source)
    , mCurrent(source)
    , mIsHalt(false)
    , mCurLocation(1, 1)
    , mRecordIndent()
    , mIsBeginBlockStmtLexed(false)
    , mMaxErrorNum(maxErrNum)
    , mErrs(new GlossocErr[maxErrNum])
    , mErrNum(0)
{
    mRecordIndent.reserve(20);
    mRecordIndent.push_back(0);
}

Lexer::~Lexer() { delete[] mErrs; }

bool Lexer::isHalt() const { return mIsHalt; }

bool Lexer::lexSucessed() const { return mErrNum == 0; }

std::vector<GlossocErr> Lexer::takeErr() const
{
    std::vector<GlossocErr> output;
    output.reserve(mErrNum);
    output.resize(mErrNum);

    memcpy((void*)output.data(), mErrs, mErrNum * sizeof(GlossocErr));

    return output;
}

Token Lexer::lexToken()
{
    Token output{mCurLocation, mCurLocation.goRight()};
    Location startLocation = mCurLocation;
    mStart                 = mCurrent;

    if (startLocation.isFirstCol())
        if (lexBlockStmt(&output, startLocation))
            return output;

    skipWhitespace();

    switch (*mCurrent)
    {
    case '(':
        TOKENIZE(Lparen, "(");
        break;

    case ')':
        TOKENIZE(Rparen, ")");
        break;

    case '{':
        TOKENIZE(Lbrace, "{");
        break;

    case '}':
        TOKENIZE(Rbrace, "}");
        break;

    case '[':
        TOKENIZE(Lsqbrace, "[");
        break;

    case ']':
        TOKENIZE(Rsqbrace, "]");
        break;

    case '\n':
        TOKENIZE(EndStmt, "");
        break;

    case '\0':
        if (mRecordIndent.back() != 0)
        {
            mRecordIndent.pop_back();
            output.type         = TokenType::EndBlockStmt;
            output.spanLocation = {startLocation, mCurLocation};
        }
        else if (mIsHalt)
            TOKENIZE(Eof, "");
        else
        {
            mIsHalt = true;
            TOKENIZE(EndStmt, "");
        }
        break;

    case ':':
        if (PEEK_CHAR(1) == '\n')
        {
            nextChar();
            mIsBeginBlockStmtLexed = true;
            TOKENIZE(BeginBlockStmt, "");
        }
        else if (isEitherChar(PEEK_CHAR(1), ' ', '\t'))
        {
            nextChar();
            skipWhitespace();
            if (*mCurrent == '\n')
            {
                mIsBeginBlockStmtLexed = true;
                TOKENIZE(BeginBlockStmt, "");
            }
            else
                output = {TokenType::Colon, ":", startLocation, mCurLocation};
        }
        else
            output = lexOperator();
        break;

    case '/':
        if (PEEK_CHAR(1) == '/' && !isOperator(PEEK_CHAR(2)))
        {
            while (!isEitherChar(*mCurrent, '\n', '\0'))
                nextChar();
            nextChar();
            return lexToken();
        }
        else
            output = lexOperator();
        break;

    default:
        if (isLetter(*mCurrent))
            output = lexIdentifier();
        else if (isOperator(*mCurrent))
            output = lexOperator();
        else if (isDigit(*mCurrent))
            output = lexNumber();
        else
        {
            nextChar();
            output.type         = TokenType::Illegal;
            output.literal      = {mStart, (size_t)(mCurrent - mStart)};
            output.spanLocation = {startLocation, mCurLocation};
        }
        break;
    }

    return output;
}

bool Lexer::lexBlockStmt(Token* output, const Location& startLocation)
{
    mStart = mCurrent;
    size_t currentIndentLen;

    for (currentIndentLen = 0; isEitherChar(*mCurrent, ' ', '\t');
         ++currentIndentLen)
        nextChar();

    if (isEitherChar(*mCurrent, '/', '\n'))
    {
        return false;
    }

    if (mIsBeginBlockStmtLexed)
    {
        if (mIsBeginBlockStmtLexed && currentIndentLen <= mRecordIndent.back())
        {
            mIsHalt = true;
            addErr(ErrKind::IndentationErr, startLocation, nullptr);
            return true;
        }
        else
            mRecordIndent.push_back(currentIndentLen);

        mIsBeginBlockStmtLexed = false;
        return false;
    }
    else if (currentIndentLen < mRecordIndent.back())
    {
        mRecordIndent.pop_back();
        output->type         = TokenType::EndBlockStmt;
        output->spanLocation = {startLocation, mCurLocation};
        return true;
    }
    else if (currentIndentLen > mRecordIndent.back())
    {
        addErr(ErrKind::IndentationErr, startLocation, nullptr);
        mIsHalt = true;
        return true;
    }

    return false;
}

Token Lexer::lexIdentifier()
{
    Token output{mCurLocation, mCurLocation.goRight()};
    Location startLocation = mCurLocation;
    mStart                 = mCurrent;

    while (isNameChar(*mCurrent))
        nextChar();

    output.type =
        glosso::glossoc::strToKeyword(mStart, (size_t)(mCurrent - mStart));
    output.literal      = {mStart, (size_t)(mCurrent - mStart)};
    output.spanLocation = {startLocation, mCurLocation};

    skipWhitespace();

    return output;
}

Token Lexer::lexOperator()
{
    Token output{mCurLocation, mCurLocation.goRight()};
    Location startLocation = mCurLocation;
    mStart                 = mCurrent;

    while (isOperator(*mCurrent))
        nextChar();

    output.type =
        glosso::glossoc::strToOperator(mStart, (size_t)(mCurrent - mStart));
    output.literal      = {mStart, (size_t)(mCurrent - mStart)};
    output.spanLocation = {startLocation, mCurLocation};

    skipWhitespace();

    return output;
}

// TODO(#12): Lexing hex, oct, bin integers and floating point numbers
// Currently, it only can lex decimal integers
Token Lexer::lexNumber()
{
    Token output{mCurLocation, mCurLocation.goRight()};
    Location startLocation = mCurLocation;
    mStart                 = mCurrent;

    while (isDigit(*mCurrent))
        nextChar();

    output.type         = TokenType::Integer;
    output.literal      = {mStart, (size_t)(mCurrent - mStart)};
    output.spanLocation = {startLocation, mCurLocation};

    skipWhitespace();

    return output;
}

void Lexer::addErr(GlossocErrKind errKind, Location startLocation,
                   const char* msg)
{
    if (mErrNum < mMaxErrorNum)
        mErrs[mErrNum++] = {
            errKind, {startLocation, mCurLocation}, mSource, msg};
}

void Lexer::nextChar()
{
    if (*mCurrent == '\n')
    {
        ++mCurrent;
        mCurLocation.newLine();
    }
    else
    {
        mCurrent += expectUtf8Len(*mCurrent);
        mCurLocation.goRightMut();
    }
}

void Lexer::skipWhitespace()
{
    while (isEitherChar(*mCurrent, ' ', '\t'))
        nextChar();
}

// Implementation of static functions
static bool isLetter(char chr)
{
    return ('A' <= chr && chr <= 'Z') || ('a' <= chr && chr <= 'z') ||
           chr == '_';
}

static bool isDigit(char chr) { return '0' <= chr && chr <= '9'; }

static bool isNameChar(char chr) { return isLetter(chr) || isDigit(chr); }

static bool isOperator(char chr)
{
    switch (chr)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '=':
    case '!':
    case '@':
    case '$':
    case '^':
    case '&':
    case '<':
    case '>':
    case '.':
    case ',':
    case ':':
    case ';':
    case '~':
    case '|':
        return true;
    default:
        return false;
    }
}

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
