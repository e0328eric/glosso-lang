#include "Span.hh"

using namespace glosso::glossoc;

Location::Location()
    : mRow(1)
    , mColumn(1)
{
}

Location::Location(size_t row, size_t column)
    : mRow(row)
    , mColumn(column)
{
}

size_t Location::getRow() const { return mRow; }

size_t Location::getColumn() const { return mColumn; }

void Location::goRight() { ++mColumn; }

void Location::newLine()
{
    ++mRow;
    mColumn = 0;
}

std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const Location& location)
{
    os << location.mRow << ":" << location.mColumn;
    return os;
}

Span::Span(Location start, Location end)
    : mStart(start)
    , mEnd(end)
{
}

Location& Span::getStart() const { return mStart; }
Location& Span::getEnd() const { return mEnd; }
