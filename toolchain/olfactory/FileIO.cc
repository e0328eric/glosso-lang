#include <cassert>
#include <cstring>
#include <cerrno>

#include "FileIO.hh"

using Err = glosso::olfactory::OlfactoryErr;

Err glosso::olfactory::readFile(char** output, const char* inputFilename)
{
    assert(*output == nullptr);

    FILE* inputFile = nullptr;
    ssize_t sourceLen;

    if ((inputFile = fopen(inputFilename, "r")) == nullptr)
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

    *output = new char[(size_t)sourceLen + 1];
    fread((void*)(*output), 1, (size_t)sourceLen, inputFile);
    (*output)[sourceLen] = '\0';

    if (ferror(inputFile))
    {
        std::cerr << "ERROR: cannot read the file" << inputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        goto EXCEPTION_HANDLE;
    }

    fclose(inputFile);

    return Err::Ok;

EXCEPTION_HANDLE:
    delete[](*output);
    if (inputFile != nullptr)
        fclose(inputFile);

    return Err::ReadFailedErr;
}
