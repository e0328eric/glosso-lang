#include "Utf8.hh"

using namespace glosso::glossoc;

utf8char glosso::glossoc::decodeUtf8(char* lst)
{
    //
    (void)lst;
    return 0;
}

bool glosso::glossoc::encodeUtf8(char* buffer, utf8char chr)
{
    (void)buffer;
    (void)chr;
    return 0;
}

size_t glosso::glossoc::expectUtf8Len(char chr)
{
    if ((unsigned char)chr < 0x80)
        return 1;
    else if ((chr & 0xf0) == 0xf0)
        return 4;
    else if ((chr & 0xe0) == 0xe0)
        return 3;
    else if ((chr & 0xc0) == 0xc0)
        return 2;
    else
        return 1; // to ignore current character
}
