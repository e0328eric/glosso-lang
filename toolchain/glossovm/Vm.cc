#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "FileIO.hh"
#include "Metadata.hh"
#include "Vm.hh"

using namespace glosso;
using namespace glosso::glossovm;
using Err = glosso::glossovm::GlossoVmErr;

Vm::Vm(const char* source)
    : mSource(source), mCurrent(nullptr), mPeek(nullptr),
      mIsParseFinished(false), mGlobalData(nullptr), mGlobalDataLen(0),
      mInst(nullptr), mInstLen(0), mIp(0), mStack(), mSp(0), mIsHalt(false)
{
    size_t sourceLen = strlen(source);

    Metadata metadata = parseMetadata(mSource);

    mCurrent = mSource + metadata.procLocation;
    mPeek = mSource + metadata.procLocation;

    mGlobalData = mSource + GLOBAL_MEMORY_LOCATION;
    mGlobalDataLen = metadata.globalMemLength;

    mInst = new Instruction[sourceLen];

    nextChar();
}

Vm::~Vm()
{
    delete[] mInst;
    delete[] mSource;
}

Err Vm::run()
{
    Err err = parse();

    if (err != Err::Ok)
        return err;

    while (!mIsHalt && err == Err::Ok)
        err = runInst();

    return err;
}

Err Vm::runInst()
{
#define CHECK_STACK_OVERFLOW(_n)                                               \
    if (mSp + (_n) >= STACK_CAPACITY)                                          \
    {                                                                          \
        err = Err::StackOverflowErr;                                           \
        break;                                                                 \
    }

#define CHECK_STACK_UNDERFLOW(_n)                                              \
    if (mSp < (_n))                                                            \
    {                                                                          \
        err = Err::StackUnderflowErr;                                          \
        break;                                                                 \
    }

#define CHECK_IS_VALID_JUMP(_n)                                                \
    if ((_n) >= mInstLen)                                                      \
    {                                                                          \
        err = Err::InvalidJumpErr;                                             \
        break;                                                                 \
    }

#define CHECK_IS_VALID_RELATIVE_JUMP(_n)                                       \
    if ((uint64_t)((int64_t)mIp + (_n)) >= mInstLen ||                         \
        ((int64_t)mIp - (_n)) < 0)                                             \
    {                                                                          \
        err = Err::InvalidJumpErr;                                             \
        break;                                                                 \
    }

#define CONVERT_OPERAND(_name, _ty, _fnt)                                      \
    _ty _name = 0;                                                             \
    if (!operand._fnt(_name))                                                  \
    {                                                                          \
        err = Err::InvalidOperandErr;                                          \
        break;                                                                 \
    }

#define PUSH_VALUE(_value)                                                     \
    CHECK_STACK_OVERFLOW(1);                                                   \
    mStack[mSp++] = Value{_value};                                             \
    ++mIp;                                                                     \
    break

#define SCAN_VALUE(_type)                                                      \
    do                                                                         \
    {                                                                          \
        CHECK_STACK_OVERFLOW(1);                                               \
        _type value;                                                           \
        std::cin >> value;                                                     \
        mStack[mSp++] = Value{value};                                          \
        ++mIp;                                                                 \
    } while (false);                                                           \
    break

#define READ_VALUE(_readType)                                                  \
    do                                                                         \
    {                                                                          \
        CHECK_STACK_UNDERFLOW(1);                                              \
        CHECK_STACK_OVERFLOW(1);                                               \
                                                                               \
        void* ptr = nullptr;                                                   \
        auto value = mStack[mSp - 1];                                          \
                                                                               \
        if ((ptr = value.getHeapPtrVal()) == nullptr)                          \
            return Err::ReadWithNonPtrValueErr;                                \
                                                                               \
        mStack[mSp++] = Value{*(_readType*)ptr};                               \
        ++mIp;                                                                 \
    } while (false);                                                           \
    break

#define WRITE_VALUE(_writeType, _convFnt)                                      \
    do                                                                         \
    {                                                                          \
        CHECK_STACK_UNDERFLOW(2);                                              \
                                                                               \
        _writeType writeV;                                                     \
        auto writeValue = mStack[--mSp];                                       \
        auto ptrValue = mStack[mSp - 1];                                       \
        void* ptr = nullptr;                                                   \
                                                                               \
        if ((ptr = ptrValue.getHeapPtrVal()) == nullptr)                       \
            return Err::WriteWithNonPtrValueErr;                               \
                                                                               \
        if (!writeValue._convFnt(writeV))                                      \
            return Err::WriteValueErr;                                         \
                                                                               \
        *(_writeType*)ptr = writeV;                                            \
        ++mIp;                                                                 \
    } while (false);                                                           \
    break;

#define CAST_VALUE(_from, _fromFnt, _to)                                       \
    do                                                                         \
    {                                                                          \
        CHECK_STACK_UNDERFLOW(1);                                              \
        _from v;                                                               \
        auto value = mStack[--mSp];                                            \
        if (!value._fromFnt(v))                                                \
            mStack[mSp++] = Value{};                                           \
        else                                                                   \
            mStack[mSp++] = Value{(_to)v};                                     \
        ++mIp;                                                                 \
    } while (false);                                                           \
    break

    ////////////////////////////////////////////////
    if (mIp >= mInstLen)
        return Err::InvalidAccessInstErr;

    Err err = Err::Ok;
    const auto& opcode = mInst[mIp].opcode;
    const auto& operand = mInst[mIp].operand;

    switch (opcode)
    {
    case Opcode::Nop:
        ++mIp;
        break;

    case Opcode::Push:
        CHECK_STACK_OVERFLOW(1);
        mStack[mSp++] = operand;
        ++mIp;
        break;

    case Opcode::PushFalse:
        PUSH_VALUE(false);

    case Opcode::PushTrue:
        PUSH_VALUE(true);

    case Opcode::PushZero:
        PUSH_VALUE(0);

    case Opcode::PushOne:
        PUSH_VALUE(1);

    case Opcode::PushNegOne:
        PUSH_VALUE(-1);

    case Opcode::PushUZero:
        PUSH_VALUE(0u);

    case Opcode::PushUOne:
        PUSH_VALUE(1u);

    case Opcode::PushFZero:
        PUSH_VALUE(0.0);

    case Opcode::PushFOne:
        PUSH_VALUE(1.0);

    case Opcode::PushFNegOne:
        PUSH_VALUE(-1.0);

    case Opcode::Pop:
        CHECK_STACK_UNDERFLOW(1);
        --mSp;
        ++mIp;
        break;

    case Opcode::Dup: {
        CONVERT_OPERAND(value, uint64_t, getUIntVal);
        CHECK_STACK_UNDERFLOW(value + 1);
        CHECK_STACK_OVERFLOW(1);

        mStack[mSp] = mStack[mSp - 1 - value];
        ++mSp;
        ++mIp;
        break;
    }

    case Opcode::Sdup: {
        CHECK_STACK_UNDERFLOW(1);
        uint64_t value;
        if (!mStack[--mSp].getUIntVal(value))
        {
            mStack[mSp++] = Value{};
            break;
        }

        CHECK_STACK_UNDERFLOW(value + 1);
        CHECK_STACK_OVERFLOW(1);

        mStack[mSp] = mStack[mSp - 1 - value];
        ++mSp;
        ++mIp;
        break;
    }

    case Opcode::Jmp: {
        CONVERT_OPERAND(value, uint64_t, getUIntVal);
        CHECK_IS_VALID_JUMP(value);
        mIp = value;
        break;
    }

    case Opcode::JmpTrue: {
        CONVERT_OPERAND(goInto, uint64_t, getUIntVal);
        CHECK_IS_VALID_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[--mSp];

        if (!value.isFalseVal())
            mIp = goInto;
        else
            ++mIp;

        break;
    }

    case Opcode::JmpFalse: {
        CONVERT_OPERAND(goInto, uint64_t, getUIntVal);
        CHECK_IS_VALID_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[--mSp];

        if (value.isFalseVal())
            mIp = goInto;
        else
            ++mIp;

        break;
    }

    case Opcode::JmpEq: {
        CONVERT_OPERAND(goInto, uint64_t, getUIntVal);
        CHECK_IS_VALID_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(2);

        auto val1 = mStack[--mSp];
        auto val2 = mStack[--mSp];

        if (val1 == val2)
            mIp = goInto;
        else
            ++mIp;

        break;
    }

    case Opcode::JmpNeq: {
        CONVERT_OPERAND(goInto, uint64_t, getUIntVal);
        CHECK_IS_VALID_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(2);

        auto val1 = mStack[--mSp];
        auto val2 = mStack[--mSp];

        if (val1 != val2)
            mIp = goInto;
        else
            ++mIp;

        break;
    }

    case Opcode::RelativeJmp: {
        CONVERT_OPERAND(value, int64_t, getIntVal);
        CHECK_IS_VALID_RELATIVE_JUMP(value);
        mIp = (uint64_t)((int64_t)mIp + value);
        break;
    }

    case Opcode::RelativeJmpTrue: {
        CONVERT_OPERAND(goInto, int64_t, getIntVal);
        CHECK_IS_VALID_RELATIVE_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[--mSp];

        if (!value.isFalseVal())
            mIp = (uint64_t)((int64_t)mIp + goInto);
        else
            ++mIp;

        break;
    }

    case Opcode::RelativeJmpFalse: {
        CONVERT_OPERAND(goInto, int64_t, getIntVal);
        CHECK_IS_VALID_RELATIVE_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[--mSp];

        if (value.isFalseVal())
            mIp = (uint64_t)((int64_t)mIp + goInto);
        else
            ++mIp;

        break;
    }

    case Opcode::RelativeJmpEq: {
        CONVERT_OPERAND(goInto, int64_t, getIntVal);
        CHECK_IS_VALID_RELATIVE_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(2);

        auto val1 = mStack[--mSp];
        auto val2 = mStack[--mSp];

        if (val1 == val2)
            mIp = (uint64_t)((int64_t)mIp + goInto);
        else
            ++mIp;

        break;
    }

    case Opcode::RelativeJmpNeq: {
        CONVERT_OPERAND(goInto, int64_t, getIntVal);
        CHECK_IS_VALID_RELATIVE_JUMP(goInto);
        CHECK_STACK_UNDERFLOW(2);

        auto val1 = mStack[--mSp];
        auto val2 = mStack[--mSp];

        if (val1 != val2)
            mIp = (uint64_t)((int64_t)mIp + goInto);
        else
            ++mIp;

        break;
    }

    case Opcode::Call: {
        CONVERT_OPERAND(goInto, uint64_t, getUIntVal);
        CHECK_IS_VALID_JUMP(goInto);
        CHECK_STACK_OVERFLOW(1);

        mStack[mSp++] = Value{(uint64_t)mIp + 1};
        mIp = goInto;

        break;
    }

    case Opcode::Return: {
        CHECK_STACK_UNDERFLOW(1);

        uint64_t addr = 0ULL;
        auto value = mStack[--mSp];

        if (!value.getUIntVal(addr))
            return Err::IllegalReturnAddressErr;

        mIp = (size_t)addr;

        break;
    }

    case Opcode::Swap: {
        CONVERT_OPERAND(value, uint64_t, getUIntVal);
        CHECK_STACK_UNDERFLOW(value + 1);

        auto tmp = mStack[mSp - 1];
        mStack[mSp - 1] = mStack[mSp - 1 - value];
        mStack[mSp - 1 - value] = tmp;
        ++mIp;

        break;
    }

    case Opcode::Sswap: {
        CHECK_STACK_UNDERFLOW(1);
        uint64_t value;
        if (!mStack[--mSp].getUIntVal(value))
        {
            mStack[mSp++] = Value{};
            break;
        }

        CHECK_STACK_UNDERFLOW(value + 1);

        auto tmp = mStack[mSp - 1];
        mStack[mSp - 1] = mStack[mSp - 1 - value];
        mStack[mSp - 1 - value] = tmp;
        ++mIp;

        break;
    }

    case Opcode::Not: {
        CHECK_STACK_UNDERFLOW(1);
        auto value = mStack[--mSp];
        mStack[mSp++] = !value;
        ++mIp;

        break;
    }

	case Opcode::And: {
        CHECK_STACK_UNDERFLOW(2);
        auto val1 = mStack[--mSp];
		auto val2 = mStack[--mSp];
        mStack[mSp++] = val1 & val2;
        ++mIp;

        break;
    }

	case Opcode::Or: {
        CHECK_STACK_UNDERFLOW(2);
       	auto val1 = mStack[--mSp];
		auto val2 = mStack[--mSp];
        mStack[mSp++] = val1 | val2;
        ++mIp;

        break;
    }

	case Opcode::Xor: {
        CHECK_STACK_UNDERFLOW(2);
		auto val1 = mStack[--mSp];
		auto val2 = mStack[--mSp];
        mStack[mSp++] = val1 ^ val2;
        ++mIp;

        break;
    }

    case Opcode::Add: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = val1 + val2;
        ++mIp;

        break;
    }

    case Opcode::Sub: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = val1 - val2;
        ++mIp;

        break;
    }

    case Opcode::Mul: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = val1 * val2;
        ++mIp;

        break;
    }

    case Opcode::Div: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = val1 / val2;
        ++mIp;

        break;
    }

    case Opcode::Negate: {
        CHECK_STACK_UNDERFLOW(1);
        auto value = mStack[--mSp];
        mStack[mSp++] = -value;
        ++mIp;

        break;
    }

    case Opcode::Equal: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = Value{val1 == val2};
        ++mIp;

        break;
    }

    case Opcode::Neq: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = Value{val1 != val2};
        ++mIp;

        break;
    }

    case Opcode::Lt: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = Value{val1 < val2};
        ++mIp;

        break;
    }

    case Opcode::LtEq: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = Value{val1 <= val2};
        ++mIp;

        break;
    }

    case Opcode::Gt: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = Value{val1 > val2};
        ++mIp;

        break;
    }

    case Opcode::GtEq: {
        CHECK_STACK_UNDERFLOW(2);
        auto val2 = mStack[--mSp];
        auto val1 = mStack[--mSp];
        mStack[mSp++] = Value{val1 >= val2};
        ++mIp;

        break;
    }

    case Opcode::Inc: {
        CHECK_STACK_UNDERFLOW(1);
        ++mStack[mSp - 1];
        ++mIp;

        break;
    }

    case Opcode::Dec: {
        CHECK_STACK_UNDERFLOW(1);
        --mStack[mSp - 1];
        ++mIp;

        break;
    }

    case Opcode::ScanI:
        SCAN_VALUE(int64_t);

    case Opcode::ScanU:
        SCAN_VALUE(uint64_t);

    case Opcode::ScanF:
        SCAN_VALUE(double);

    case Opcode::ScanC:
        SCAN_VALUE(char);

    case Opcode::ScanB:
        SCAN_VALUE(bool);

    case Opcode::ScanS: {
        CHECK_STACK_OVERFLOW(1);

        std::string getInput;
        std::getline(std::cin, getInput);

        size_t strLen = getInput.size();

        char* buffer = (char*)malloc(strLen + 1);
        memcpy((void*)buffer, getInput.c_str(), strLen);
        buffer[strLen] = '\0';

        mStack[mSp++] = Value{(void*)buffer};
        ++mIp;
        break;
    }

    case Opcode::Print: {
        CHECK_STACK_UNDERFLOW(1);
        auto value = mStack[--mSp];
        value.printValue(mGlobalData);
        ++mIp;

        break;
    }

    case Opcode::PrintLn: {
        CHECK_STACK_UNDERFLOW(1);
        auto value = mStack[--mSp];
        value.printValue(mGlobalData);
        std::cout << std::endl;
        ++mIp;

        break;
    }

    case Opcode::PrintS: {
        CHECK_STACK_UNDERFLOW(1);
        void* ptr;
        auto value = mStack[mSp - 1];

        if ((ptr = value.getHeapPtrVal()) == nullptr)
        {
            ++mIp;
            break;
        }

        std::cout << (char*)ptr;
        ++mIp;

        break;
    }

    case Opcode::PrintSLn: {
        CHECK_STACK_UNDERFLOW(1);
        void* ptr;
        auto value = mStack[mSp - 1];

        if ((ptr = value.getHeapPtrVal()) == nullptr)
        {
            ++mIp;
            break;
        }

        std::cout << (char*)ptr << std::endl;
        ++mIp;

        break;
    }

    case Opcode::Alloc: {
        CONVERT_OPERAND(amount, uint64_t, getUIntVal);
        CHECK_STACK_OVERFLOW(1);

        void* ptr = malloc((size_t)amount);

        if (ptr == nullptr)
            return Err::AllocFailedErr;

        mStack[mSp++] = Value{ptr};
        ++mIp;
        break;
    }

    case Opcode::ReAlloc: {
        CONVERT_OPERAND(amount, uint64_t, getUIntVal);
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[mSp - 1];
        void* ptr;

        if ((ptr = value.getHeapPtrVal()) == nullptr)
            return Err::ReallocWithNonPtrValueErr;

        free(ptr);

        ptr = malloc((size_t)amount);

        if (ptr == nullptr)
            return Err::AllocFailedErr;

        mStack[mSp - 1] = Value{ptr};
        ++mIp;
        break;
    }

    case Opcode::Free: {
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[--mSp];
        void* ptr;

        if ((ptr = value.getHeapPtrVal()) == nullptr)
            return Err::FreeWithNonPtrValueErr;

        free(ptr);
        ++mIp;
        break;
    }

    case Opcode::Read: {
        CONVERT_OPERAND(prefilename, size_t, getGlobalPtrVal);
        CHECK_STACK_OVERFLOW(2);

        Err err = Err::Ok;
        auto filename = mGlobalData + prefilename;
        char* output = nullptr;
		uint64_t len;

        if ((err = readFile(&output, filename, AtorType::CStyle, &len)) != Err::Ok)
        {
            return err;
        }

        mStack[mSp++] = Value{output};
        mStack[mSp++] = Value{len};

        ++mIp;
        break;
    }

    case Opcode::ReadI:
        READ_VALUE(int64_t);

    case Opcode::ReadU:
        READ_VALUE(uint64_t);

    case Opcode::ReadF:
        READ_VALUE(double);

    case Opcode::ReadC:
        READ_VALUE(char);

    case Opcode::ReadB:
        READ_VALUE(bool);

    case Opcode::Write: {
        // TODO(#15): yet this does nothing
        ++mIp;
        break;
    }

    case Opcode::WriteI:
        WRITE_VALUE(int64_t, getIntVal);

    case Opcode::WriteU:
        WRITE_VALUE(uint64_t, getUIntVal);

    case Opcode::WriteF:
        WRITE_VALUE(double, getFloatVal);

    case Opcode::WriteC:
        WRITE_VALUE(char, getCharVal);

    case Opcode::WriteB:
        WRITE_VALUE(bool, getBoolVal);

    case Opcode::I2U:
        CAST_VALUE(int64_t, getIntVal, uint64_t);

    case Opcode::I2F:
        CAST_VALUE(int64_t, getIntVal, double);

    case Opcode::I2C:
        CAST_VALUE(int64_t, getIntVal, char);

    case Opcode::U2I:
        CAST_VALUE(uint64_t, getUIntVal, int64_t);

    case Opcode::U2F:
        CAST_VALUE(uint64_t, getUIntVal, double);

    case Opcode::U2C:
        CAST_VALUE(uint64_t, getUIntVal, char);

    case Opcode::U2B:
        CAST_VALUE(uint64_t, getUIntVal, bool);

    case Opcode::F2I:
        CAST_VALUE(double, getFloatVal, int64_t);

    case Opcode::F2U:
        CAST_VALUE(double, getFloatVal, uint64_t);

    case Opcode::F2C:
        CAST_VALUE(double, getFloatVal, char);

    case Opcode::C2I:
        CAST_VALUE(char, getCharVal, int64_t);

    case Opcode::C2U:
        CAST_VALUE(char, getCharVal, uint64_t);

    case Opcode::C2F:
        CAST_VALUE(char, getCharVal, double);

    case Opcode::B2U: {
        CHECK_STACK_UNDERFLOW(1);

        bool v = false;
        auto value = mStack[--mSp];

        if (!value.getBoolVal(v))
            mStack[mSp++] = Value{};
        else
            mStack[mSp++] = Value{v ? 1ULL : 0ULL};

        ++mIp;

        break;
    }

    case Opcode::N2B: {
        CHECK_STACK_UNDERFLOW(1);

        auto value = mStack[--mSp];

        if (!value.isType(ValueType::Null))
            mStack[mSp++] = Value{};
        else
            mStack[mSp++] = Value{false};

        ++mIp;

        break;
    }

    case Opcode::Halt:
        mIsHalt = true;
        ++mIp;
        break;

    default:
        err = Err::InvalidOpcodeExecuteErr;
        break;
    }

    return err;

#undef CAST_VALUE
#undef WRITE_VALUE
#undef READ_VALUE
#undef SCAN_VALUE
#undef PUSH_VALUE
#undef CONVERT_OPERAND
#undef CHECK_IS_VALID_RELATIVE_JUMP
#undef CHECK_IS_VALID_JUMP
#undef CHECK_STACK_UNDERFLOW
#undef CHECK_STACK_OVERFLOW
}

/* Parser Implementations */
Err Vm::parse()
{
    Err err = Err::Ok;

    while (!mIsParseFinished && err == Err::Ok)
        err = parseInst();

    return err;
}

Err Vm::parseInst()
{
    if (*mCurrent == '\xe0' && *mPeek == '\xf0')
    {
        mIsParseFinished = true;
        return Err::Ok;
    }

    // First, parse the opcode
    auto opcode = *(Opcode*)mCurrent;
    if (opcode >= Opcode::Illegal)
        return Err::ParseOpcodeErr;

    nextChar();

    switch (hasOperand(opcode))
    {
    case OperandType::NoOperand:
        mInst[mInstLen++] = {opcode, Value{}};
        break;

    case OperandType::HasOperand:
    case OperandType::LoopOperand: {
        Value value{};

        const char* result = value.readValue(mCurrent);
        if (result == nullptr)
            return Err::ParseOperandErr;

        mCurrent = result;
        mPeek = mCurrent + 1;

        mInst[mInstLen++] = {opcode, value};
        break;
    }

    default:
        assert(0 && "Unreachable! (Vm::ParseInst)");
    }

    return Err::Ok;
}

const Instruction& Vm::getCurrentInst() const
{
    return mInst[mIp];
}

const Value* Vm::getStack() const
{
    return mStack;
}

size_t Vm::getSp() const
{
    return mSp;
}

bool Vm::isHalt() const
{
    return mIsHalt;
}

/* Private member implementations */
void Vm::nextChar()
{
    mCurrent = mPeek;
    ++mPeek;
}
