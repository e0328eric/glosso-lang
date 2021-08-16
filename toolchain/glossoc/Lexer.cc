#include <iostream>
#include <string_view>

#include "Lexer.hh"
#include "Utf8.hh"

using namespace glosso::glossoc;
using ErrKind = glosso::glossoc::GlossocErrKind;

// Macros
#define TOKENIZE(_type, _literal)                                           \
    do                                                                      \
    {                                                                       \
        nextChar();                                                         \
        output = {TokenType::_type, _literal, startLocation, mCurLocation}; \
    } while (false)

#define PEEK_CHAR(_n) (*(mCurrent + _n))

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
    , mIsBlockStmtBegin(false)
    , mRecordIndent()
    , mNested(0)
    , mCurIndent(0)
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
    Token output;
    Location startLocation = mCurLocation;
    mStart                 = mCurrent;

    if (mIsBlockStmtBegin && !isEitherChar(*mCurrent, '\n', '\0'))
        if (lexEndBlockStmt(&output))
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
        else
        {
            mIsHalt = true;
            TOKENIZE(Eof, "");
        }
        break;
    case ':':
        if (PEEK_CHAR(1) == '\n')
        {
            nextChar();
            output = lexBeginBlockStmt(startLocation);
        }
        else if (isEitherChar(PEEK_CHAR(1), ' ', '\t'))
        {
            nextChar();
            skipWhitespace();
            if (*mCurrent == '\n')
                output = lexBeginBlockStmt(startLocation);
            else
                TOKENIZE(Colon, ":");
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

Token Lexer::lexBeginBlockStmt(const Location& startLocation)
{
    Token output;
    size_t indentWidth = 0;

    nextChar();

    // TODO: tab and space has same length
    // after implementing glossoc config file, change the
    // default tab size by 4
    while (isEitherChar(*mCurrent, ' ', '\t'))
    {
        ++indentWidth;
        nextChar();
    }
    if (mRecordIndent.back() >= indentWidth)
    {
        addErr(ErrKind::IndentationErr, startLocation, nullptr);
        mIsHalt = true;
        return output;
    }
    mRecordIndent.push_back(indentWidth);
    mIsBlockStmtBegin   = true;
    output.type         = TokenType::BeginBlockStmt;
    output.spanLocation = {startLocation, mCurLocation};

    return output;
}

bool Lexer::lexEndBlockStmt(Token* output)
{
    Location startLocation = mCurLocation;

    if (mNested > 0)
    {
        if (mRecordIndent.back() < mCurIndent)
        {
            addErr(ErrKind::IndentationErr, startLocation, nullptr);
            mIsHalt = true;
            return output;
        }

        if (mNested-- <= 0)
            mIsBlockStmtBegin = false;
        output->type         = TokenType::EndBlockStmt;
        output->spanLocation = {startLocation, mCurLocation};
        return true;
    }
    else
    {
        switch (*mCurrent)
        {
        case ' ':
        case '\t':
        {
            size_t recordedInitLen = mRecordIndent.size();
            size_t recordedLen     = recordedInitLen;

            mCurIndent = 0;
            // TODO: tab and space has same length
            // after implementing glossoc config file, change the default tab
            // size by 4
            while (isEitherChar(*mCurrent, ' ', '\t'))
            {
                ++mCurIndent;
                nextChar();
            }

            for (; mRecordIndent.back() > mCurIndent; --recordedLen)
            {
                ++mNested;
                mRecordIndent.pop_back();
            }

            if (mRecordIndent.back() < mCurIndent)
            {
                addErr(ErrKind::IndentationErr, startLocation, nullptr);
                mIsHalt = true;
                return output;
            }

            if (recordedLen < recordedInitLen)
            {
                if (mNested-- <= 0)
                    mIsBlockStmtBegin = false;
                output->type         = TokenType::EndBlockStmt;
                output->spanLocation = {startLocation, mCurLocation};
                return true;
            }
            else
                return false;
        }
        default:
            if (mCurLocation.getColumn() == 1)
            {
                mRecordIndent.pop_back();
                output->type         = TokenType::EndBlockStmt;
                output->spanLocation = {startLocation, mCurLocation};
                if (mRecordIndent.empty())
                    mIsBlockStmtBegin = false;
                return true;
            }
            return false;
        }
    }
}

Token Lexer::lexIdentifier()
{
    Token output;
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
    Token output;
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

// TODO: Lexing hex, oct, bin integers and floating point numbers
// Currently, it only can lex decimal integers
Token Lexer::lexNumber()
{
    Token output;
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
        mCurLocation.goRight();
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
