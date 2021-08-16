#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include "Error.hh"
#include "FileIO.hh"
#include "Lexer.hh"
#include "Token.hh"

using Err = glosso::glossoc::GlossocErr;

const char* findExtension(const char* fileName)
{
    const char* output = fileName;

    while (*output != '\0')
        ++output;
    while (*output != '.' && output != fileName)
        --output;

    return output == fileName ? nullptr : output;
}

int main(int argc, char* argv[])
{
    Err err;
    char* sourceStr = nullptr;
    char buffer[100];

    // Check whether any argument is given
    if (argc < 2)
    {
        // TODO(#3): Integrate this error with GlossocErr print format
        std::cerr << "USAGE: glossoc <input-file> [-o <output-file>]\n";
        std::cerr << "ERROR: no input files" << std::endl;
        return 1;
    }

    // Take an input file name and extract an extension of given one
    const char* inputFilename = argv[1];
    const char* extension     = findExtension(inputFilename);

    if (extension == nullptr)
    {
        std::cerr << "ERROR: invalid input file" << std::endl;
        return 1;
    }

    memcpy((void*)buffer, inputFilename, (size_t)(extension - inputFilename));
    memcpy((void*)(buffer + (extension - inputFilename)), ".gsm", 4);
    buffer[extension - inputFilename + 4] = '\0';

    // Define a default output file name
    const char* outputFilename = buffer;

    switch (argc)
    {
    case 2:
        break;

    case 3:
        std::cerr << "USAGE: glossoc <input-file> [-o <output-file>]\n";
        std::cerr << "ERROR: specify the output file name" << std::endl;
        return 1;

    case 4:
        if (argv[3][0] == '-')
        {
            std::cerr << "USAGE: glossoc <input-file> [-o <output-file>]\n";
            std::cerr << "ERROR: specify the output file name" << std::endl;
            return 1;
        }
        outputFilename = argv[3];
        break;

    default:
        std::cerr << "USAGE: glossoc <input-file> [-o <output-file>]\n";
        std::cerr << "ERROR: too many input found" << std::endl;
        return 1;
    }

    if (!(err = glosso::glossoc::readFile(&sourceStr, inputFilename)).isOk())
    {
        std::cout << err << std::endl;
        return 1;
    }

    // Main part
    std::vector<glosso::glossoc::Token> foo;
    glosso::glossoc::Lexer lexer{sourceStr, 15};

    while (!lexer.isHalt())
        foo.emplace_back(lexer.lexToken());

    if (!lexer.lexSucessed())
    {
        auto errs = lexer.takeErr();
        for (auto& e : errs)
            std::cout << e << std::endl;
        delete[] sourceStr;
        return 1;
    }
    else
    {
        for (auto& token : foo)
            std::cout << token << std::endl;
        delete[] sourceStr;
        return 0;
    }
}
