#ifndef GLOSSO_LANG_TOOLCHAIN_OLFACTORY_FILEIO_HH_
#define GLOSSO_LANG_TOOLCHAIN_OLFACTORY_FILEIO_HH_

#include "Error.hh"

namespace glosso::olfactory
{
OlfactoryErr readFile(char** output, const char* inputFilename);
} // namespace glosso::olfactory

#endif // GLOSSO_LANG_TOOLCHAIN_OLFACTORY_FILEIO_HH_
