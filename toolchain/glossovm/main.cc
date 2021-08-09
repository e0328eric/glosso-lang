#include <cstring>
#include <iostream>

#include "Debugger.hh"
#include "Error.hh"
#include "FileIO.hh"
#include "Vm.hh"

using Err = glosso::glossovm::GlossoVmErr;

int main(int argc, char* argv[])
{
    Err err         = Err::Ok;
    char* sourceStr = nullptr;

    // TODO: implement better command line parser
    switch (argc)
    {
    case 0:
    case 1:
        std::cerr << "USAGE: glossovm [--debug] <input-binary>\n";
        std::cerr << "ERROR: no input files" << std::endl;
        return 1;

    case 2:
    {
        if (argv[1][0] == '-')
        {
            std::cerr << "USAGE: glossovm [--debug] <input-binary>\n";
            std::cerr << "ERROR: no input files" << std::endl;
            return 1;
        }

        if ((err = glosso::glossovm::readFile(&sourceStr, argv[1])) != Err::Ok)
        {
            std::cerr << err << std::endl;
            return 1;
        }

        glosso::glossovm::Vm vm{sourceStr};
        err = vm.run();

        if (err != Err::Ok)
        {
            std::cerr << err << std::endl;
            return 1;
        }
        break;
    }

    case 3:
    {
        if (strcmp(argv[1], "--debug") != 0)
        {
            std::cerr << "USAGE: glossovm [--debug] <input-binary>\n";
            std::cerr << "ERROR: given option is not `--debug`" << std::endl;
            return 1;
        }

        if ((err = glosso::glossovm::readFile(&sourceStr, argv[2])) != Err::Ok)
        {
            std::cerr << err << std::endl;
            return 1;
        }

        glosso::glossovm::Debugger debugger{sourceStr};
        err = debugger.run();

        if (err != Err::Ok)
        {
            std::cerr << err << std::endl;
            return 1;
        }
        break;
    }

    default:
        std::cerr << "USAGE: glossovm [--debug] <input-binary>\n";
        std::cerr << "ERROR: too many input found" << std::endl;
        return 1;
    }

    return 0;
}
