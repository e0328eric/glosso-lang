#include "Utf8.hh"

using namespace glosso::glossoc;

utf8char glosso::glossoc::decodeUtf8(const char** lst)
{
    utf8char output;

    if ((unsigned char)**lst < 0x80)
    {
        output = **lst;
        *lst += 1;
    }
    else if ((**lst & 0xf0) == 0xf0)
    {
        output = ((*lst)[0] & 0x07) << 18 | ((*lst)[1] & 0x3f) << 12 |
                 ((*lst)[2] & 0x3f) << 6 | ((*lst)[3] & 0x3f);
        *lst += 4;
    }
    else if ((**lst & 0xe0) == 0xe0)
    {
        output = ((*lst)[0] & 0x0f) << 12 | ((*lst)[1] & 0x3f) << 6 |
                 ((*lst)[2] & 0x3f);
        *lst += 3;
    }
    else if ((**lst & 0xc0) == 0xc0)
    {
        output = ((*lst)[0] & 0x1f) << 6 | ((*lst)[1] & 0x3f);
        *lst += 2;
    }
    else
        return 0xfffd;
    return output;
}

bool glosso::glossoc::encodeUtf8(char** buffer, utf8char chr)
{
    if (chr < 0x007f)
        *(*buffer)++ = (char)chr;
    else if (chr < 0x07ff)
    {
        *(*buffer)++ = (char)((chr >> 6) | 0xd0);
        *(*buffer)++ = (char)((chr & 0x3f) | 0x80);
    }
    else if (chr < 0xffff)
    {
        *(*buffer)++ = (char)((chr >> 12) | 0xe0);
        *(*buffer)++ = (char)((chr >> 6 & 0x3f) | 0x80);
        *(*buffer)++ = (char)((chr & 0x3f) | 0x80);
    }
    else if (chr & 0x100000 != 0)
    {
        *(*buffer)++ = (char)((chr >> 18) | 0xf0);
        *(*buffer)++ = (char)((chr >> 12 & 0x3f) | 0x80);
        *(*buffer)++ = (char)((chr >> 6 & 0x3f) | 0x80);
        *(*buffer)++ = (char)((chr & 0x3f) | 0x80);
    }
    else
        return false; // Illegal unicode character found
    return true;
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
