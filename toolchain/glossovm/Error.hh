#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_ERROR_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_ERROR_HH_

#include <iostream>

namespace glosso::glossovm
{
enum class GlossoVmErr
{
    Ok,
    ReadFileErr,
    ParseOpcodeErr,
    ParseOperandErr,
    StackOverflowErr,
    StackUnderflowErr,
    InvalidOpcodeExecuteErr,
    InvalidOperandErr,
    InvalidAccessInstErr,
    InvalidJumpErr,
    IllegalReturnAddressErr,
    ReadStringFailedErr,
    AllocFailedErr,
    ReallocWithNonPtrValueErr,
    FreeWithNonPtrValueErr,
    ReadWithNonPtrValueErr,
    WriteWithNonPtrValueErr,
    WriteValueErr,
};

std::ostream& operator<<(std::ostream& os, const GlossoVmErr& err);
} // namespace glosso::glossovm

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_ERROR_HH_
