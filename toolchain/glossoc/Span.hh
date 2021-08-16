#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LOCATION_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LOCATION_HH_

#include <cstddef>
#include <iostream>

namespace glosso::glossoc
{
class Location
{
  public:
    Location();
    Location(size_t row, size_t col);

    size_t getRow() const;
    size_t getColumn() const;

    void goRight();
    void newLine();

    friend std::ostream& operator<<(std::ostream& os, const Location& location);

  private:
    size_t mRow;
    size_t mColumn;
};

class Span
{
  public:
    Span(Location start, Location end);

    const Location& getStart() const;
    const Location& getEnd() const;

    friend std::ostream& operator<<(std::ostream& os, const Span& span);

  private:
    Location mStart;
    Location mEnd;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_LOCATION_HH_
