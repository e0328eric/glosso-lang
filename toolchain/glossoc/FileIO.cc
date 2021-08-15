#include <cassert>
#include <iostream>

#include "FileIO.hh"

using ErrKind = glosso::glossoc::GlossocErrKind;

ErrKind glosso::glossoc::readFile(char** output, const char* inputFilename)
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

    (*output) = new char[(size_t)sourceLen + 1];
    fread((void*)(*output), 1, (size_t)sourceLen, inputFile);
    (*output)[sourceLen] = '\0';

    if (ferror(inputFile))
    {
        std::cerr << "ERROR: cannot read the file" << inputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        goto EXCEPTION_HANDLE;
    }

    fclose(inputFile);
    return ErrKind::Ok;

EXCEPTION_HANDLE:
    delete[](*output);
    if (inputFile != nullptr)
        fclose(inputFile);

    return ErrKind::ReadFileErr;
}
