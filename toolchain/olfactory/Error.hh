#ifndef GLOSSO_LANG_TOOLCHAIN_OLFACTORY_ERROR_HH_
#define GLOSSO_LANG_TOOLCHAIN_OLFACTORY_ERROR_HH_

#include <iostream>

namespace glosso::olfactory
{
enum class OlfactoryErr
{
    Ok = 0,
    ReadFailedErr,
    WriteFailedErr,
    InstVectorIsNullErr,
    InstVectorAccessErr,
    OperandTypesCollideErr,
    JumpsListOverflowErr,
    GlobalMemoryOverflowErr,
    IllegalJumpLabelNameErr,
    ParseIntegerErr,
    ParseUIntegerErr,
    ParseFloatErr,
    ParseCharErr,
    ParseStringErr,
	IllFormedInclude,
	IllFormedDefine,
};

std::ostream& operator<<(std::ostream& os, const OlfactoryErr& err);
} // namespace glosso::olfactory

#endif // GLOSSO_LANG_TOOLCHAIN_OLFACTORY_ERROR_HH_
