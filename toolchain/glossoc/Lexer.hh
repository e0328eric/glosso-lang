#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_

#include "Token.hh"

namespace glosso::glossoc
{
class Lexer
{
  public:
    Lexer(const char* source);
    ~Lexer() = default;

    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&)      = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer& operator=(Lexer&&) = delete;

    Token lexToken();

  private:
    const char* mSource;
    const char* mStart;
    const char* mCurrent;
    bool isHalt;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LEXER_HH_
