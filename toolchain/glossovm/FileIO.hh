#ifndef GLOSSO_TOOLCHAIN_GLOSSOVM_FILEIO_HH_
#define GLOSSO_TOOLCHAIN_GLOSSOVM_FILEIO_HH_

#include "Error.hh"

namespace glosso::glossovm
{
GlossoVmErr readFile(char** output, const char* inputFilename);
}

#endif // GLOSSO_TOOLCHAIN_GLOSSOVM_FILEIO_HH_
