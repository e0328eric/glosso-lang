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

// TODO(#10): Move column with amount of the unicode width
// In present, every character has length 1 in the terminal.
// expected: a, ß, 가, 家
//           ^  ^  ^^  ^^
// actual: a, ß, 가, 家
//         ^  ^  ^   ^
void Location::goRight() { ++mColumn; }

void Location::newLine()
{
    ++mRow;
    mColumn = 1;
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

const Location& Span::getStart() const { return mStart; }
const Location& Span::getEnd() const { return mEnd; }

std::ostream& glosso::glossoc::operator<<(std::ostream& os, const Span& span)
{
    os << "( " << span.mStart << " " << span.mEnd << " )";
    return os;
}
