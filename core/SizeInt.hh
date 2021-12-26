#ifndef GLOSSO_LANG_CORE_SIZEINT_HH_
#define GLOSSO_LANG_CORE_SIZEINT_HH_

#include <compare>
#include <cstddef>
#include <iostream>

namespace glosso
{
class SizeInt
{
  public:
    SizeInt();
    SizeInt(size_t num);

    std::partial_ordering operator<=>(const SizeInt& rhs) const;
    operator size_t() const;

    friend std::ostream& operator<<(std::ostream& os, const SizeInt& si);

  private:
    size_t mNum;
};

std::ostream& operator<<(std::ostream& os, const SizeInt& si);
} // namespace glosso

#endif // GLOSSO_LANG_CORE_SIZEINT_HH_
