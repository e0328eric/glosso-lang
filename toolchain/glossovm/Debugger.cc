#include <cassert>
#include <cstdio>

#include <linenoise.h>

#include "Debugger.hh"

using namespace glosso::glossovm;
using Err = glosso::glossovm::GlossoVmErr;

constexpr const char* PROMPT = "> ";

Debugger::Debugger(const char* source) : Vm(source)
{
}

Err Debugger::run()
{
    bool isQuit = false;
    char* line = nullptr;
    char saveChar = 's';
    Err err = Vm::parse();

    linenoiseHistorySetMaxLen(15);

    if (err != Err::Ok)
        return err;

    while (!Vm::isHalt() && err == Err::Ok && !isQuit)
    {
        line = linenoise(PROMPT);

        if (!line)
        {
            linenoiseFree(line);
            return GlossoVmErr::DebuggerLinenoiseErr;
        }

        // This debugger needs only first character of the given command.
        // Thus, save and sound are the same argument and valid one
        saveChar = line[0] != 0 ? line[0] : saveChar;

        err = runArgument(&isQuit, saveChar);

        linenoiseFree(line);
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
        err = runInst();
        std::cout << "<Next Instruction>\n";
        std::cout << Vm::getCurrentInst() << "\n";
        std::cout << "<Stack>\n";
        std::cout << "[ ";
        for (size_t i = 0; i < Vm::getSp(); ++i)
            std::cout << Vm::getStack()[i] << " ";
        std::cout << "]\n" << std::endl;
        break;

    case 's':
        std::cout << "<Next Instruction>\n";
        std::cout << Vm::getCurrentInst() << "\n";
        std::cout << "<Stack>\n";
        std::cout << "[ ";
        for (size_t i = 0; i < Vm::getSp(); ++i)
            std::cout << Vm::getStack()[i] << " ";
        std::cout << "]\n" << std::endl;
        break;

    case 'h':
        fprintf(stdout, "<Help>\n");
        fprintf(stdout, "r : run the program once\n");
        fprintf(stdout, "s : show the current stack\n");
        fprintf(stdout, "c : execute both r and s command\n");
        fprintf(stdout, "h : show the help message\n");
        fprintf(stdout, "q : quit\n");
        break;

    case 'q':
        *isQuit = true;
        break;

    default:
        break;
    }

    return err;
}
