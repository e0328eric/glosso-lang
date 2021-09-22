#include <cstdio>
#include <cstdlib>

#include "Metadata.hh"

using namespace glosso;

Metadata glosso::parseMetadata(const char* data)
{
    Metadata metadata = *(Metadata*)data;

    if (metadata.magic != MAGIC_NUMBER)
    {
        fprintf(stderr, "ERROR: invalid magic number. got %lld\n",
                metadata.magic);
        exit(1);
    }

    if (metadata.version != GLOSSO_VM_VERSION)
    {
        fprintf(stderr,
                "ERROR: invalid version number. expected %lld\n, got %lld\n",
                GLOSSO_VM_VERSION, metadata.version);
        exit(1);
    }

    return metadata;
}
