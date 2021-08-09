#include <iostream>

#include "Metadata.hh"

using namespace glosso;

Metadata glosso::parseMetadata(const char* data)
{
    Metadata metadata = *(Metadata*)data;

    if (metadata.magic != MAGIC_NUMBER)
    {
        std::cerr << "ERROR: invalid magic number. got " << metadata.magic
                  << std::endl;
        exit(1);
    }

    if (metadata.version != GLOSSO_VM_VERSION)
    {
        std::cerr << "ERROR: invalid version number. expected "
                  << GLOSSO_VM_VERSION << ", got " << metadata.version
                  << std::endl;
        exit(1);
    }

    return metadata;
}
