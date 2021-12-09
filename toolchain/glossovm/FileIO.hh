#ifndef GLOSSO_TOOLCHAIN_GLOSSOVM_FILEIO_HH_
#define GLOSSO_TOOLCHAIN_GLOSSOVM_FILEIO_HH_

#include "Error.hh"

namespace glosso::glossovm
{
enum class AtorType
{
    CppStyle = 0,
    CStyle,
};

GlossoVmErr readFile(char** output, const char* inputFilename,
                     AtorType ator = AtorType::CppStyle);
} // namespace glosso::glossovm

#endif // GLOSSO_TOOLCHAIN_GLOSSOVM_FILEIO_HH_
