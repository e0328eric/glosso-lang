#include "Error.hh"

using namespace glosso::glossovm;

std::ostream& glosso::glossovm::operator<<(std::ostream& os,
                                           const GlossoVmErr& err)
{
    switch (err)
    {
    case GlossoVmErr::Ok:
        os << "";
        break;
    case GlossoVmErr::ReadFileErr:
        os << "ERROR: failed to read a file";
        break;
    case GlossoVmErr::ParseOpcodeErr:
        os << "ERROR: error occured while parsing an opcode";
        break;
    case GlossoVmErr::ParseOperandErr:
        os << "ERROR: error occured while parsing an operand";
        break;
    case GlossoVmErr::StackOverflowErr:
        os << "ERROR: stack overflow";
        break;
    case GlossoVmErr::StackUnderflowErr:
        os << "ERROR: stack underflow";
        break;
    case GlossoVmErr::InvalidOpcodeExecuteErr:
        os << "ERROR: invalid opcode found to execute";
        break;
    case GlossoVmErr::InvalidOperandErr:
        os << "ERROR: invalid operand is given";
        break;
    case GlossoVmErr::InvalidAccessInstErr:
        os << "ERROR: segmentation fault";
        break;
    case GlossoVmErr::InvalidJumpErr:
        os << "ERROR: segmentation fault (Jump Err)";
        break;
    case GlossoVmErr::IllegalReturnAddressErr:
        os << "ERROR: segmentation fault (Return Err)";
        break;
    case GlossoVmErr::ReadStringFailedErr:
        os << "ERROR: failed to read a string";
        break;
    case GlossoVmErr::AllocFailedErr:
        os << "ERROR: allocate memory failed";
        break;
    case GlossoVmErr::ReallocWithNonPtrValueErr:
        os << "ERROR: realloc called on the non pointer value";
        break;
    case GlossoVmErr::FreeWithNonPtrValueErr:
        os << "ERROR: free called on the non pointer value";
        break;
    case GlossoVmErr::ReadWithNonPtrValueErr:
        os << "ERROR: read called on the non pointer value";
        break;
    case GlossoVmErr::WriteWithNonPtrValueErr:
        os << "ERROR: write called on the non pointer value";
        break;
    case GlossoVmErr::WriteValueErr:
        os << "ERROR: failed to write a value on the given pointer";
        break;
	case GlossoVmErr::DebuggerLinenoiseErr:
		os << "ERROR: linenoise error occurs";
		break;
    }

    return os;
}
