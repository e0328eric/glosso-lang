#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_

#include <vector>

#include "Error.hh"
#include "Token.hh"
#include "Span.hh"

namespace glosso::glossoc
{
class Lexer
{
  public:
    Lexer(const char* source, size_t maxErrNum);
    ~Lexer();

    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&)      = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer& operator=(Lexer&&) = delete;

    Token lexToken();
    bool isHalt() const;
    bool lexSucessed() const;
    std::vector<GlossocErr> takeErr() const;

  private:
    Token lexBeginBlockStmt(const Location& startLocation);
    bool lexEndBlockStmt(Token* output);
    Token lexIdentifier();
    Token lexOperator();
    Token lexNumber();
    void addErr(GlossocErrKind errKind, Location startLocation,
                const char* msg);
    void nextChar();
    void skipWhitespace();

  private:
    const char* mSource;
    const char* mStart;
    const char* mCurrent;
    bool mIsHalt;
    Location mCurLocation;
    bool mIsBlockStmtBegin;

    std::vector<size_t> mRecordIndent;
    size_t mNested;
    size_t mCurIndent;

    const size_t mMaxErrorNum;
    GlossocErr* mErrs;
    size_t mErrNum;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_
