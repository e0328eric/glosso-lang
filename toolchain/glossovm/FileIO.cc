#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "FileIO.hh"

using Err = glosso::glossovm::GlossoVmErr;
using AtorType = glosso::glossovm::AtorType;

Err glosso::glossovm::readFile(char** output, const char* inputFilename,
                               AtorType ator, uint64_t* lenOutput)
{
    assert(*output == nullptr);

    ssize_t sourceLen;
    FILE* inputFile = nullptr;

    if ((inputFile = fopen(inputFilename, "rb")) == nullptr)
    {
        std::cerr << "ERROR: cannot open the file " << inputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        goto EXCEPTION_HANDLE;
    }

    if (fseek(inputFile, 0, SEEK_END) < 0)
    {
        std::cerr << "ERROR: cannot read the file" << inputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        goto EXCEPTION_HANDLE;
    }

    if ((sourceLen = ftell(inputFile)) < 0)
    {
        std::cerr << "ERROR: cannot read the file" << inputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        goto EXCEPTION_HANDLE;
    }

    rewind(inputFile);

    switch (ator)
    {
    case AtorType::CppStyle:
        (*output) = new char[(size_t)sourceLen + 1];
        break;
    case AtorType::CStyle:
        (*output) = (char*)malloc((size_t)sourceLen + 1);
        break;
    }

    fread((void*)(*output), 1, (size_t)sourceLen, inputFile);
    (*output)[sourceLen] = '\0';

    if (ferror(inputFile))
    {
        std::cerr << "ERROR: cannot read the file" << inputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        goto EXCEPTION_HANDLE;
    }

    if (lenOutput != nullptr)
    {
        *lenOutput = static_cast<uint64_t>(sourceLen);
    }

    fclose(inputFile);
    return Err::Ok;

EXCEPTION_HANDLE:
    switch (ator)
    {
    case AtorType::CppStyle:
        delete[](*output);
        break;
    case AtorType::CStyle:
        free((void*)(*output));
        break;
    }
    (*output) = nullptr;

    if (inputFile != nullptr)
        fclose(inputFile);

    return Err::ReadFileErr;
}
