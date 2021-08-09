#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_

#include <cstddef>
#include <vector>

#include "Error.hh"
#include "Location.hh"
#include "Token.hh"

namespace glosso::glossoc
{
class Lexer
{
  public:
    explicit Lexer(const char* source, size_t maxErrNum = 15);
    ~Lexer();

    bool isFinished() const;

    bool lexing(std::vector<Token>& tokens);

    std::vector<GlossocErr> takeErr() const;

  private:
    bool lexToken(std::vector<Token>& tokens);
    bool lexComment(std::vector<Token>& tokens);
    bool lexIdent(std::vector<Token>& tokens);
    bool lexNumber(std::vector<Token>& tokens);

  private:
    void nextChar();
    void skipWhitespace();
    void addErr(GlossocErrKind errKind, const char* additionalMessage);

  private:
    enum
    {
        NESTED_CAPACITY = 100
    };

    const char* mSource;
    const char* mStart;
    const char* mCurrent;
    const char* mPeek;

    bool mIsFinished;

    Location mLocation;

    size_t mNested[NESTED_CAPACITY];
    size_t mNestedCount;
    size_t mCurrentNested;

    const size_t mMaxErrorNum;
    GlossocErr* mErrs;
    size_t mErrNum;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_
