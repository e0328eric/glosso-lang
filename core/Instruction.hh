#ifndef _GLOSSO_CORE_LIBRARY_INSTRUCTION_HH_
#define _GLOSSO_CORE_LIBRARY_INSTRUCTION_HH_

#include <iostream>
#include <string_view>
#include <unordered_map>

#include "Value.hh"

#define OPCODE(O)              \
    O("nop", Nop)              \
    O("push", Push)            \
    O("pushf", PushFalse)      \
    O("pusht", PushTrue)       \
    O("push0", PushZero)       \
    O("push1", PushOne)        \
    O("pushn1", PushNegOne)    \
    O("pushu0", PushUZero)     \
    O("pushu1", PushUOne)      \
    O("pushf0", PushFZero)     \
    O("pushf1", PushFOne)      \
    O("pushfn1", PushFNegOne)  \
    O("pop", Pop)              \
    O("dup", Dup)              \
    O("jmp", Jmp)              \
    O("jt", JmpTrue)           \
    O("jf", JmpFalse)          \
    O("je", JmpEq)             \
    O("jne", JmpNeq)           \
    O("rjmp", RelativeJmp)     \
    O("rjt", RelativeJmpTrue)  \
    O("rjf", RelativeJmpFalse) \
    O("rje", RelativeJmpEq)    \
    O("rjne", RelativeJmpNeq)  \
    O("call", Call)            \
    O("ret", Return)           \
    O("swap", Swap)            \
    O("not", Not)              \
    O("add", Add)              \
    O("sub", Sub)              \
    O("mul", Mul)              \
    O("div", Div)              \
    O("neg", Negate)           \
    O("eq", Equal)             \
    O("neq", Neq)              \
    O("lt", Lt)                \
    O("lte", LtEq)             \
    O("gt", Gt)                \
    O("gte", GtEq)             \
    O("inc", Inc)              \
    O("dec", Dec)              \
    O("scani", ScanI)          \
    O("scanu", ScanU)          \
    O("scanf", ScanF)          \
    O("scanc", ScanC)          \
    O("scanb", ScanB)          \
    O("scans", ScanS)          \
    O("print", Print)          \
    O("println", PrintLn)      \
    O("prints", PrintS)        \
    O("printsln", PrintSLn)    \
    O("alloc", Alloc)          \
    O("realloc", ReAlloc)      \
    O("free", Free)            \
    O("readi", ReadI)          \
    O("readu", ReadU)          \
    O("readf", ReadF)          \
    O("readc", ReadC)          \
    O("readb", ReadB)          \
    O("writei", WriteI)        \
    O("writeu", WriteU)        \
    O("writef", WriteF)        \
    O("writec", WriteC)        \
    O("writeb", WriteB)        \
    O("i2u", I2U)              \
    O("i2f", I2F)              \
    O("i2c", I2C)              \
    O("u2i", U2I)              \
    O("u2f", U2F)              \
    O("u2c", U2C)              \
    O("u2b", U2B)              \
    O("f2i", F2I)              \
    O("f2u", F2U)              \
    O("f2c", F2C)              \
    O("c2i", C2I)              \
    O("c2u", C2U)              \
    O("c2f", C2F)              \
    O("b2u", B2U)              \
    O("n2b", N2B)              \
    O("halt", Halt)

namespace glosso
{
#define O(_os, _o) _o,
enum class Opcode : uint8_t
{
    OPCODE(O) Illegal,
};
#undef O

enum class OperandType
{
    NoOperand,
    HasOperand,
    LoopOperand,
};

struct Instruction
{
    Opcode opcode;
    Value operand;
};

inline OperandType hasOperand(const Opcode& opcode)
{
    switch (opcode)
    {
    case Opcode::Push:
    case Opcode::Dup:
    case Opcode::RelativeJmp:
    case Opcode::RelativeJmpTrue:
    case Opcode::RelativeJmpFalse:
    case Opcode::RelativeJmpEq:
    case Opcode::RelativeJmpNeq:
    case Opcode::Swap:
    case Opcode::Alloc:
    case Opcode::ReAlloc:
        return OperandType::HasOperand;
    case Opcode::Jmp:
    case Opcode::JmpTrue:
    case Opcode::JmpFalse:
    case Opcode::JmpEq:
    case Opcode::JmpNeq:
    case Opcode::Call:
        return OperandType::LoopOperand;
    default:
        return OperandType::NoOperand;
    }
}

Opcode strToOpcode(const char* start, size_t len);
std::ostream& operator<<(std::ostream& os, const Opcode& opcode);
std::ostream& operator<<(std::ostream& os, const Instruction& inst);
} // namespace glosso

#ifndef USE_OPCODE_MACRO
#undef OPCODE
#endif // USE_OPCODE_MACRO

#endif // _GLOSSO_CORE_LIBRARY_INSTRUCTION_HH_
