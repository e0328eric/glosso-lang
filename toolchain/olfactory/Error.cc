#include "Error.hh"

using namespace glosso::olfactory;

std::ostream& glosso::olfactory::operator<<(std::ostream& os,
                                            const OlfactoryErr& err)
{
    switch (err)
    {
    case OlfactoryErr::Ok:
        os << "";
        break;
    case OlfactoryErr::ReadFailedErr:
        os << "ERROR: reading failed";
        break;
    case OlfactoryErr::WriteFailedErr:
        os << "ERROR: writting failed";
        break;
    case OlfactoryErr::InstVectorIsNullErr:
        os << "ERROR: internal bug occurs (InstVectorIsNullErr)";
        break;
    case OlfactoryErr::InstVectorAccessErr:
        os << "ERROR: internal bug occurs (InstVectorAccessErr)";
        break;
    case OlfactoryErr::OperandTypesCollideErr:
        os << "ERROR: internal bug occurs (OperandTypesCollideErr)";
        break;
    case OlfactoryErr::JumpsListOverflowErr:
        os << "ERROR: too many jumps found";
        break;
    case OlfactoryErr::GlobalMemoryOverflowErr:
        os << "ERROR: too many memories were stored";
        break;
    case OlfactoryErr::IllegalJumpLabelNameErr:
        os << "ERROR: illegal jump label name is found";
        break;
    case OlfactoryErr::ParseIntegerErr:
        os << "ERROR: parsing integer error";
        break;
    case OlfactoryErr::ParseUIntegerErr:
        os << "ERROR: parsing unsigned integer error";
        break;
    case OlfactoryErr::ParseFloatErr:
        os << "ERROR: parsing float error";
        break;
    case OlfactoryErr::ParseCharErr:
        os << "ERROR: parsing character error";
        break;
    case OlfactoryErr::ParseStringErr:
        os << "ERROR: parsing string error";
        break;
	case OlfactoryErr::IllFormedInclude:
		os << "ERROR: invalid include format found";
		break;
	case OlfactoryErr::IllFormedDefine:
		os << "ERROR: invalid define format found";
		break;
    }

    return os;
}
