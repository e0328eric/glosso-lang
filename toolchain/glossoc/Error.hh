#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_

#include <iostream>

#include "Location.hh"

namespace glosso::glossoc
{
enum class GlossocErrKind;

class GlossocErr
{
  public:
    GlossocErr();
    GlossocErr(GlossocErrKind kind, Location location, const char* sourceCode,
               const char* additionalMessage = nullptr);
    friend std::ostream& operator<<(std::ostream& os, const GlossocErr& err);

  private:
    GlossocErrKind mKind;
    Location mLocation;
    const char* mSourceCode;
    const char* mMsg;
};

enum class GlossocErrKind
{
    Ok = 0,
    ReadFileErr,
    UnknownTokenFoundErr,
};

std::ostream& operator<<(std::ostream& os, const GlossocErrKind& err);
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_ERROR_HH_
