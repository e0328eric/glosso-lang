#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_DEBUGGER_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_DEBUGGER_HH_

#include "Error.hh"
#include "Vm.hh"

namespace glosso::glossovm
{
class Debugger : public Vm
{
  public:
    explicit Debugger(const char* source);
    ~Debugger() override = default;

    Debugger(const Debugger&) = delete;
    Debugger(Debugger&&)      = delete;
    Debugger& operator=(const Debugger&&) = delete;
    Debugger& operator=(Debugger&&) = delete;

    GlossoVmErr run();

  private:
    GlossoVmErr runArgument(bool* isQuit, char ch);
};
} // namespace glosso::glossovm

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_DEBUGGER_HH_
