#include <cstdio>
#include <cstring>
#include <iostream>

#include "Compiler.hh"
#include "Error.hh"
#include "FileIO.hh"
#include "Preprocessor.hh"

using Err = glosso::olfactory::OlfactoryErr;

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
    Err err         = Err::Ok;
    char* sourceStr = nullptr;
    char buffer[100];

    // Check whether any argument is given
    if (argc < 2)
    {
        std::cerr << "USAGE: olfactory <input-file> [-o <output-file>]\n";
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
        std::cerr << "USAGE: olfactory <input-file> [-o <output-file>]\n";
        std::cerr << "ERROR: specify the output file name" << std::endl;
        return 1;

    case 4:
        if (argv[3][0] == '-')
        {
            std::cerr << "USAGE: olfactory <input-file> [-o <output-file>]\n";
            std::cerr << "ERROR: specify the output file name" << std::endl;
            return 1;
        }
        outputFilename = argv[3];
        break;

    default:
        std::cerr << "USAGE: olfactory <input-file> [-o <output-file>]\n";
        std::cerr << "ERROR: too many input found" << std::endl;
        return 1;
    }

    if ((err = glosso::olfactory::readFile(&sourceStr, inputFilename)) !=
        Err::Ok)
    {
        std::cout << err << std::endl;
        return 1;
    }

	char* output;
	glosso::olfactory::Preprocessor prep{inputFilename, sourceStr};
	if ((err = prep.preprocess(&output)) != Err::Ok)
	{
		std::cerr << err << " while preprocessing the code" << std::endl;
		return 1;
	}

    // Main part
    glosso::olfactory::Compiler compiler{output};
    if ((err = compiler.compile(outputFilename)) != Err::Ok)
    {
        std::cerr << err << " at line " << compiler.getCodeLine() << std::endl;
        return 1;
    }

    return 0;
}
