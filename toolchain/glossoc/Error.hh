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
};

std::ostream& operator<<(std::ostream& os, const GlossocErrKind& err);

class GlossocErr
{
  private:
    GlossocErrKind mKind;
    Span mSpan;
    const char* mSource;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_
