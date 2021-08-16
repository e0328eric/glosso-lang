#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_UTF8_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_UTF8_HH_

#include <cstddef>
#include <cstdint>

namespace glosso::glossoc
{
typedef uint32_t utf8char;

utf8char decodeUtf8(char* lst);
bool encodeUtf8(char* buffer, utf8char chr);
size_t expectUtf8Len(char chr);
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_UTF8_HH_
