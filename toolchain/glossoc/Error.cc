#include "Error.hh"

using namespace glosso::glossoc;
typedef GlossocErrKind ErrKind;

std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const GlossocErrKind& err)
{
    switch (err)
    {
    case ErrKind::Ok:
        os << "";
        break;

    case ErrKind::ReadFileErr:
        os << "ERROR: cannot read a file";
        break;

    default:
        break;
    }

    return os;
}
