#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_

#include <iostream>

#include "Span.hh"

namespace glosso::glossoc
{
enum class GlossocErrKind
{
    Ok = 0,
    ReadFileErr,
    IndentationErr,
};

std::ostream& operator<<(std::ostream& os, const GlossocErrKind& err);

class GlossocErr
{
  public:
    GlossocErr();
    GlossocErr(GlossocErrKind kind);
    GlossocErr(GlossocErrKind kind, Span span, const char* source,
               const char* msg);

    bool isOk() const;

    friend std::ostream& operator<<(std::ostream& os, const GlossocErr& err);

  private:
    GlossocErrKind mKind;
    bool mHasSpan;
    Span mSpan;
    const char* mSource;
    const char* mMsg;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_
