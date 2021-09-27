#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_VM_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_VM_HH_

#include "Error.hh"
#include "Instruction.hh"
#include "Value.hh"

namespace glosso::glossovm
{
constexpr size_t STACK_CAPACITY = 1024;

class Vm
{
  public:
    explicit Vm(const char* inputFilename);
    virtual ~Vm();

    Vm(const Vm&) = delete;
    Vm(Vm&&)      = delete;
    Vm& operator=(const Vm&) = delete;
    Vm& operator=(Vm&&) = delete;

    GlossoVmErr run();

  protected:
    GlossoVmErr runInst();

    GlossoVmErr parse();
    GlossoVmErr parseInst();

    const glosso::Instruction& getCurrentInst() const;
    const glosso::Value* getStack() const;
    size_t getSp() const;
    bool isHalt() const;

  private:
    void nextChar();

  private:
    const char* mSource;
    const char* mCurrent;
    const char* mPeek;
    bool mIsParseFinished;

    const char* mGlobalData;
    size_t mGlobalDataLen;

    glosso::Instruction* mInst;
    size_t mInstLen;
    size_t mIp;

    glosso::Value mStack[STACK_CAPACITY];
    size_t mSp;
    bool mIsHalt;
};
} // namespace glosso::glossovm

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOVM_VM_HH_
