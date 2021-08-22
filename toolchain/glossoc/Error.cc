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
        os << "cannot read a file";
        break;

    case ErrKind::IndentationErr:
        os << "indentation parse error";
        break;

    default:
        break;
    }

    return os;
}

GlossocErr::GlossocErr()
    : mKind(ErrKind::Ok)
    , mHasSpan(false)
    , mSpan({0, 0}, {0, 0})
    , mSource(nullptr)
    , mMsg(nullptr)
{
}

GlossocErr::GlossocErr(ErrKind kind)
    : mKind(kind)
    , mHasSpan(false)
    , mSpan({0, 0}, {0, 0})
    , mSource(nullptr)
    , mMsg(nullptr)
{
}

GlossocErr::GlossocErr(GlossocErrKind kind, Span span, const char* source,
                       const char* msg)
    : mKind(kind)
    , mHasSpan(true)
    , mSpan(span)
    , mSource(source)
    , mMsg(msg)
{
}

bool GlossocErr::isOk() const { return mKind == ErrKind::Ok; }

// Pretty Print
std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const GlossocErr& err)
{
    if (err.mHasSpan)
        os << "ERROR: " << err.mKind << " at " << err.mSpan;
    else
        os << "ERROR: " << err.mKind;
    return os;
}
