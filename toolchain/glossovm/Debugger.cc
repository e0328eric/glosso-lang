#include <cassert>
#include <iostream>

#include "Debugger.hh"

using namespace glosso::glossovm;
using Err = glosso::glossovm::GlossoVmErr;

Debugger::Debugger(const char* source)
    : Vm(source)
{
}

Err Debugger::run()
{
    char keyChar  = '\0';
    char saveChar = '\0';
    bool isQuit   = false;
    Err err       = Vm::parse();

    if (err != Err::Ok)
        return err;

    while (!Vm::isHalt() && err == Err::Ok && !isQuit)
    {
        std::cout << "> ";

        // TODO(#2): scans opcode is ignored
        std::cin >> keyChar;
        if (std::cin.fail())
            std::cin.clear();

        std::cout << std::endl;

        switch (keyChar)
        {
        case 'r':
        case 'c':
        case 's':
        case 'h':
        case 'q':
            saveChar = keyChar;
            break;

        default:
            break;
        }

        err = runArgument(&isQuit, saveChar);
    }

    return err;
}

Err Debugger::runArgument(bool* isQuit, char ch)
{
    assert(isQuit != nullptr);
    Err err = Err::Ok;

    switch (ch)
    {
    case 'r':
        err = runInst();
        break;

    case 'c':
        while (!Vm::isHalt() && err == Err::Ok)
        {
            err = runInst();
            std::cout << "<Instruction>\n";
            std::cout << Vm::getCurrentInst() << "\n";
            std::cout << "<Stack>\n";
            std::cout << "[ ";
            for (size_t i = 0; i < Vm::getSp(); ++i)
                std::cout << Vm::getStack()[i] << " ";
            std::cout << "]\n" << std::endl;
        }
        break;

    case 's':
        std::cout << "<Instruction>\n";
        std::cout << Vm::getCurrentInst() << "\n";
        std::cout << "<Stack>\n";
        std::cout << "[ ";
        for (size_t i = 0; i < Vm::getSp(); ++i)
            std::cout << Vm::getStack()[i] << " ";
        std::cout << "]\n" << std::endl;
        break;

    case 'h':
        std::cout << "<Help>\n";
        std::cout << "r : run the program once\n";
        std::cout << "s : show the current stack\n";
        std::cout << "h : show the help message\n";
        std::cout << "q : quit\n" << std::endl;
        break;

    case 'q':
        *isQuit = true;
        break;

    default:
        break;
    }

    return err;
}
