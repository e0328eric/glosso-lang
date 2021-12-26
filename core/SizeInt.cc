#include "SizeInt.hh"

using namespace glosso;

SizeInt::SizeInt() : mNum(0)
{
}
SizeInt::SizeInt(size_t num) : mNum(num)
{
}

std::partial_ordering SizeInt::operator<=>(const SizeInt& rhs) const
{
    return mNum <=> rhs.mNum;
}

SizeInt::operator size_t() const
{
    return mNum;
}

std::ostream& glosso::operator<<(std::ostream& os, const SizeInt& si)
{
    os << si.mNum;
    return os;
}
