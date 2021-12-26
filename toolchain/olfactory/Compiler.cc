#include <cassert>
#include <cerrno>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Compiler.hh"
#include "Metadata.hh"
#include "SizeInt.hh"

using namespace glosso;
using namespace glosso::olfactory;
using Err = glosso::olfactory::OlfactoryErr;

//// Forward Declarations ////////
template <typename Char>
static bool isEitherChar(char c, Char toCmp)
{
    return c == toCmp;
}

template <typename Char, typename... Chars>
static bool isEitherChar(char c, Char toCmp, Chars... toCmps)
{
    return (c == toCmp) || isEitherChar(c, toCmps...);
}

static inline bool isLabelNameLetter(char chr);

static Err parseInteger(Value* output, const char* start, const char* end,
                        int base);
static Err parseUInteger(Value* output, const char* start, const char* end,
                         int base);
static Err parseFloat(Value* output, const char* start, const char* end);
static bool parseUIntOpcode(const Opcode& opcode);
static uint8_t hexToInt(const char& ch);
////////

Compiler::Compiler(const char* source) noexcept
    : mSource(source), mStart(nullptr), mCurrent(nullptr), mCodeLine(1),
      mProgLine(0), mIsFinished(false), mInst(), mLabels(), mLabelsCount(0),
      mJumps(), mJumpsCount(0), mGlobalMem(), mGlobalMemCount(0)
{
    memset((void*)mGlobalMem, 0, GLOBAL_MEM_CAPACITY);
    mStart = mSource;
    mCurrent = mSource;
}

Compiler::~Compiler()
{
    delete[] mSource;
}

Err Compiler::compile(const char* outputFilename)
{
#define WRITE_FILE(_ptr, _nitems)                                              \
    do                                                                         \
    {                                                                          \
        if ((fwrite(_ptr, 1, _nitems, outputFile)) < _nitems)                  \
        {                                                                      \
            std::cerr << "ERROR: failed writing into " << outputFilename       \
                      << "\n\t";                                               \
            std::cerr << strerror(errno) << std::endl;                         \
            return Err::WriteFailedErr;                                        \
        }                                                                      \
    } while (false)

    // First run the main function of the olfactory assembly compiler
    Err err = Err::Ok;
    if ((err = parseGlasm()) != Err::Ok)
        return err;

    FILE* outputFile = nullptr;

    if ((outputFile = fopen(outputFilename, "wb")) == nullptr)
    {
        std::cerr << "ERROR: cannot open the file " << outputFilename << "\n\t";
        std::cerr << strerror(errno) << std::endl;
        return Err::WriteFailedErr;
    }

    // This specific number is just for writing zero bytes into a file
    uint64_t zero = 0ULL;

    size_t padding = 8 - mGlobalMemCount % 8;
    padding = padding == 8 ? 0 : padding;

    Metadata metaToWrite{MAGIC_NUMBER, GLOSSO_VM_VERSION,
                         (uint64_t)(32 + mGlobalMemCount + padding),
                         (uint64_t)mGlobalMemCount};

    // Write Metadata
    WRITE_FILE((void*)&metaToWrite, sizeof(Metadata));
    WRITE_FILE((void*)mGlobalMem, mGlobalMemCount);
    WRITE_FILE((void*)&zero, padding);

    // Write Instructions
    for (auto& inst : mInst)
    {
        WRITE_FILE((void*)&inst.opcode, sizeof(Opcode));
        if (hasOperand(inst.opcode) != OperandType::NoOperand)
            inst.operand.writeValue(outputFile);
    }

    fprintf(outputFile, "%c", '\xe0');
    fprintf(outputFile, "%c", '\xf0');
    fclose(outputFile);
    return Err::Ok;
#undef WRITE_FILE
}

// TODO(#13): This prints the line number of the string
// which is made from the preprocessor.
// That means that the line number almost differ to the original one.
size_t Compiler::getCodeLine() const
{
    return mCodeLine;
}

// Compiler functions
Err Compiler::parseGlasm()
{
    size_t labelIdx = 0;
    Err err = Err::Ok;

    // Parse whole glasm file
    while (!mIsFinished && err == Err::Ok)
        err = parseInst();

    if (err != Err::Ok)
        return err;

    // Linking addresses of jumps
    for (size_t jumpIdx = 0; jumpIdx < mJumpsCount; ++jumpIdx)
    {
        for (labelIdx = 0; labelIdx < mLabelsCount; ++labelIdx)
            if (mJumps[jumpIdx].labelName == mLabels[labelIdx].labelName)
                break;

        if (mJumps[jumpIdx].inst >= mInst.size())
            return Err::InstVectorAccessErr;

        mInst[mJumps[jumpIdx].inst].operand =
            Value{(uint64_t)mLabels[labelIdx].progLine};
    }

    return Err::Ok;
}

Err Compiler::parseInst()
{
    Err err = Err::Ok;
    skipWhitespace();
    mStart = mCurrent;

    while (!isEitherChar(*mCurrent, ' ', '\n', ';', ':', '\0'))
        nextChar();

    switch (*mCurrent)
    {
    case ';':
        parseComment();
        return Err::Ok;

    case ':':
        return parseLoopLabel();

    case '\0':
        mIsFinished = true;
        return Err::Ok;

    default:
        if ((err = parseOpcode()) != Err::Ok)
            return err;
    }

    while (!isEitherChar(*mCurrent, '\n', '\0'))
        nextChar();

    if (*mCurrent == '\n')
        ++mProgLine;

    return Err::Ok;
}

Err Compiler::parseOpcode()
{
    Err err = Err::Ok;
    auto opcode = strToOpcode(mStart, (size_t)(mCurrent - mStart));
    auto opType = hasOperand(opcode);
    Value operand;

    if (opType == OperandType::HasOperand)
        if ((err = parseOperand(&operand, opcode)) != Err::Ok)
            return err;

    mInst.emplace_back(Instruction{opcode, operand});

    if (opType == OperandType::LoopOperand)
    {
        if (mJumpsCount >= JUMP_CAPACITY)
            return Err::JumpsListOverflowErr;

        skipWhitespace();
        mStart = mCurrent;

        bool loopState = true;
        while (loopState)
        {
            switch (*mCurrent)
            {
            case ' ':
            case '\t':
            case '\n':
            case '\0':
                loopState = false;
                break;

            default:
                if (isLabelNameLetter(*mCurrent))
                    nextChar();
                else
                    return Err::IllegalJumpLabelNameErr;
                break;
            }
        }

        mJumps[mJumpsCount].inst = mInst.size() - 1;
        mJumps[mJumpsCount].labelName =
            std::string_view{mStart, (size_t)(mCurrent - mStart)};
        ++mJumpsCount;
    }

    return Err::Ok;
}

Err Compiler::parseOperand(Value* output, const Opcode& opcode)
{
    Err err = Err::Ok;
    assert(output != nullptr);

    skipWhitespace();
    mStart = mCurrent;

    bool isFirstChar = true;
    struct
    {
        bool isUInt;
        bool isFloat;
        int radix;
    } operandTy = {false, false, 10};

    bool loopState = true;
    while (loopState)
    {
        switch (*mCurrent)
        {
        case '\n':
        case '\0':
            loopState = false;
            break;

        case '0':
            if (!isFirstChar)
            {
                nextChar();
                break;
            }

            if (isEitherChar(*(mCurrent + 1), 'b', 'o', 'x'))
            {
                nextChar();
                switch (*mCurrent)
                {
                case 'b':
                    operandTy.radix = 2;
                    break;
                case 'o':
                    operandTy.radix = 8;
                    break;
                case 'x':
                    operandTy.radix = 16;
                    break;
                default:
                    assert(0 && "Unreachable code (parseOperand)");
                }
                mStart += 2;
            }
            nextChar();
            break;

        case 'u':
            operandTy.isUInt = true;
            nextChar();
            break;

        case '.':
            operandTy.isFloat = true;
            nextChar();
            break;

        case '\'':
            return parseChar(output, opcode);

        case '"':
            return parseString(output, opcode);

        default:
            nextChar();
            break;
        }

        isFirstChar = false;
    }

    if (operandTy.isUInt && operandTy.isFloat)
        return Err::OperandTypesCollideErr;

    std::string_view operandStr{mStart, (size_t)(mCurrent - mStart)};

    if (operandStr == "null")
        *output = Value{};
    else if (operandStr == "true")
        *output = Value{true};
    else if (operandStr == "false")
        *output = Value{false};
    else if (operandTy.isUInt || parseUIntOpcode(opcode))
        err = parseUInteger(output, mStart, mCurrent, operandTy.radix);
    else if (operandTy.isFloat)
        err = parseFloat(output, mStart, mCurrent);
    else
        err = parseInteger(output, mStart, mCurrent, operandTy.radix);

    return err;
}

Err Compiler::parseLoopLabel()
{
    const char* i = mStart;
    while (i != mCurrent && isLabelNameLetter(*i))
        ++i;

    if (i != mCurrent)
        return Err::IllegalJumpLabelNameErr;

    std::string_view loopLabel{mStart, (size_t)(mCurrent - mStart)};
    mLabels[mLabelsCount].labelName = loopLabel;
    mLabels[mLabelsCount].progLine = mProgLine;
    ++mLabelsCount;

    while (!isEitherChar(*mCurrent, ' ', ';', '\n', '\0'))
        nextChar();

    return Err::Ok;
}

Err Compiler::parseChar(Value* output, const Opcode& opcode)
{
    // TODO(#7): Implement more opcodes whose operand is char
    // The only opcode whose operand is char is push in this moment
    if (*mCurrent != '\'' || opcode != Opcode::Push)
        return Err::ParseCharErr;

    char buffer[5] = {0};
    char value;

    nextChar();
    mStart = mCurrent;

    while (!isEitherChar(*mCurrent, '\'', '\0'))
        nextChar();

    if (*mCurrent == '\0')
        return Err::ParseCharErr;

    if ((size_t)(mCurrent - mStart) >= 5)
        return Err::ParseCharErr;

    memcpy((void*)buffer, mStart, (size_t)(mCurrent - mStart));
    buffer[mCurrent - mStart] = '\0';

    if (buffer[0] == '\\')
    {
        switch (buffer[1])
        {
        case '\\':
            value = '\\';
            break;
        case '0':
            value = '\0';
            break;
        case 't':
            value = '\t';
            break;
        case 'r':
            value = '\r';
            break;
        case 'n':
            value = '\n';
            break;
        case 'x':
            value = (char)(hexToInt(buffer[2]) << 4 | hexToInt(buffer[3]));
            break;
        default:
            return Err::ParseCharErr;
        }
    }
    else
        value = buffer[0];

    *output = Value{value};
    return Err::Ok;
}

// TODO(#8): olfactory cannot parse escape character in the proper way
Err Compiler::parseString(Value* output, const Opcode& opcode)
{
    if (*mCurrent != '"' || !hasStringOperand(opcode))
        return Err::ParseStringErr;

    mStart = mCurrent;
    nextChar();

    while (!isEitherChar(*mCurrent, '"', '\0'))
        nextChar();

    if (*mCurrent == '\0')
        return Err::ParseStringErr;

    if ((size_t)(mCurrent - mStart) >= GLOBAL_MEM_CAPACITY)
        return Err::GlobalMemoryOverflowErr;

    size_t memStart = mGlobalMemCount;
    memcpy((void*)(mGlobalMem + mGlobalMemCount), mStart + 1,
           (size_t)(mCurrent - mStart - 1));
    mGlobalMemCount += (size_t)(mCurrent - mStart - 1);
    mGlobalMem[mGlobalMemCount++] = '\0';

    *output = Value{static_cast<SizeInt>(memStart)};
    return Err::Ok;
}

void Compiler::parseComment()
{
    if (*mCurrent != ';')
        return;

    while (!isEitherChar(*mCurrent, '\n', '\0'))
        nextChar();
}

// Compiler utilities
void Compiler::nextChar()
{
    if (*mCurrent == '\n')
        ++mCodeLine;
    ++mCurrent;
}

void Compiler::skipWhitespace()
{
    while (isEitherChar(*mCurrent, ' ', '\n', '\t', '\r'))
        nextChar();
}

static inline bool isLabelNameLetter(char chr)
{
    return (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') ||
           (chr >= '0' && chr <= '9') || chr == '_';
}

static Err parseInteger(Value* output, const char* start, const char* end,
                        int base)
{
    assert(output != nullptr);

    int64_t value = 0;
    if (auto [result, ec] = std::from_chars(start, end, value, base);
        ec == std::errc())
    {
        *output = Value{value};
        return Err::Ok;
    }

    return Err::ParseIntegerErr;
}

static Err parseUInteger(Value* output, const char* start, const char* end,
                         int base)
{
    assert(output != nullptr);

    uint64_t value = 0;
    if (auto [result, ec] = std::from_chars(start, end, value, base);
        ec == std::errc())
    {
        *output = Value{value};
        return Err::Ok;
    }

    return Err::ParseUIntegerErr;
}

static Err parseFloat(Value* output, const char* start, const char* end)
{
    double value;
    assert(output != nullptr);

    if (value = strtod(start, (char**)&end); errno == ERANGE || start == end)
        return Err::ParseFloatErr;

    *output = Value{value};

    return Err::Ok;
}

static bool parseUIntOpcode(const Opcode& opcode)
{
    return opcode == Opcode::Dup || opcode == Opcode::Swap ||
           opcode == Opcode::Alloc || opcode == Opcode::ReAlloc;
}

static uint8_t hexToInt(const char& ch)
{
    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return (uint8_t)(ch - '0');
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return (uint8_t)(ch - 'A' + 10);
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return (uint8_t)(ch - 'a' + 10);
    default:
        return '\0';
    }
}
