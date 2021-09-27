#ifndef GLOSSO_LANG_TOOLCHAIN_OLFACTORY_COMPILER_HH_
#define GLOSSO_LANG_TOOLCHAIN_OLFACTORY_COMPILER_HH_

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <vector>

#include "Error.hh"
#include "Instruction.hh"
#include "Value.hh"

namespace glosso::olfactory
{
constexpr size_t LABEL_CAPACITY      = 1024;
constexpr size_t JUMP_CAPACITY       = 1024;
constexpr size_t GLOBAL_MEM_CAPACITY = 1048576;

struct LabelInfo
{
    std::string_view labelName;
    size_t progLine;
};

struct UninitLoop
{
    size_t inst;
    std::string_view labelName;
};

class Compiler
{
  public:
    explicit Compiler(const char* source) noexcept;
    ~Compiler();

    Compiler(const Compiler&) = delete;
    Compiler(Compiler&&)      = delete;
    Compiler& operator=(const Compiler&) = delete;
    Compiler& operator=(Compiler&&) = delete;

    OlfactoryErr compile(const char* outputFilename);

    size_t getCodeLine() const;

  private:
    OlfactoryErr parseGlasm();
    OlfactoryErr parseInst();
    OlfactoryErr parseOpcode();
    OlfactoryErr parseOperand(Value* output, const Opcode& opcode);
    OlfactoryErr parseLoopLabel();
    OlfactoryErr parseChar(Value* output, const Opcode& opcode);
    OlfactoryErr parseString(Value* output, const Opcode& opcode);
    void parseComment();

  private:
    void nextChar();
    void skipWhitespace();

  private:
    const char* mSource;
    const char* mStart;
    const char* mCurrent;
    size_t mCodeLine;
    size_t mProgLine;
    bool mIsFinished;

    std::vector<glosso::Instruction> mInst;

    LabelInfo mLabels[LABEL_CAPACITY];
    size_t mLabelsCount;

    UninitLoop mJumps[JUMP_CAPACITY];
    size_t mJumpsCount;

    uint8_t mGlobalMem[GLOBAL_MEM_CAPACITY];
    size_t mGlobalMemCount;
};
} // namespace glosso::olfactory

#endif // GLOSSO_LANG_TOOLCHAIN_OLFACTORY_COMPILER_HH_
