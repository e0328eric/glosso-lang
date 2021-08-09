#ifndef GLOSSO_LANG_CORE_METADATA_HH_
#define GLOSSO_LANG_CORE_METADATA_HH_

#include <cstddef>
#include <cstdint>

namespace glosso
{
constexpr uint64_t MAGIC_NUMBER         = 0x01a46f73736f6c67ULL;
constexpr uint64_t GLOSSO_VM_VERSION    = 0x4148504c41ULL;
constexpr size_t GLOBAL_MEMORY_LOCATION = 0x20;

struct Metadata
{
    uint64_t magic;
    uint64_t version;
    uint64_t procLocation;
    uint64_t globalMemLength;
};

Metadata parseMetadata(const char* data);
} // namespace glosso

#endif // GLOSSO_LANG_CORE_METADATA_HH_
