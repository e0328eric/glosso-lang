#include <cassert>
#include <string_view>

#include "Error.hh"

#define FMT_RED    "\x1b[1;31m"
#define FMT_CYAN   "\x1b[1;36m"
#define FMT_NORMAL "\x1b[0m"

using namespace glosso::glossoc;

//// Static Functions ////////
static size_t intAsString(char* buffer, size_t num);
const char* jumpAlongLineskip(const char* source, size_t nth);
////////

GlossocErr::GlossocErr()
    : mKind(GlossocErrKind::Ok)
    , mLocation()
    , mSourceCode(nullptr)
    , mMsg(nullptr)
{
}

GlossocErr::GlossocErr(GlossocErrKind kind, Location location,
                       const char* source, const char* additionalMessage)
    : mKind(kind)
    , mLocation(location)
    , mSourceCode(source)
    , mMsg(additionalMessage)
{
}

std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const GlossocErrKind& err)
{
    switch (err)
    {
    case GlossocErrKind::Ok:
        os << "";
        break;
    case GlossocErrKind::ReadFileErr:
        os << "Failed to read a file";
        break;
    case GlossocErrKind::UnknownTokenFoundErr:
        os << "Unknown token is found";
        break;
    }

    return os;
}

std::ostream& glosso::glossoc::operator<<(std::ostream& os,
                                          const GlossocErr& err)
{
    // Formatting Error Message
    os << FMT_RED "ERROR: " FMT_NORMAL << err.mKind << "\n";

    if (err.mSourceCode != nullptr)
    {
        char lineNumBuffer[20];
        lineNumBuffer[0] = ' ';
        auto lineNumberLen =
            intAsString(lineNumBuffer + 1, err.mLocation.getRow());

        const char* currentRawLine =
            jumpAlongLineskip(err.mSourceCode, err.mLocation.getRow());
        const char* lineEnd = strchr(currentRawLine, '\n');
        lineEnd = lineEnd == nullptr ? strchr(currentRawLine, '\0') : lineEnd;

        std::string_view currentLine{currentRawLine,
                                     (size_t)(lineEnd - currentRawLine)};

        os << FMT_CYAN;
        for (size_t i = 0; i < lineNumberLen + 1; ++i)
            os << " ";
        os << " --> Line: " << err.mLocation << "\n";

        os << FMT_CYAN;
        for (size_t i = 0; i < lineNumberLen + 2; ++i)
            os << " ";
        os << " |\n" FMT_NORMAL;

        os << FMT_CYAN << lineNumBuffer << " | " FMT_NORMAL << currentLine
           << "\n";

        os << FMT_CYAN;
        for (size_t i = 0; i < lineNumberLen + 2; ++i)
            os << " ";
        os << " |";
        os << FMT_RED;
        for (size_t i = 0; i < err.mLocation.getColumn(); ++i)
            os << " ";
        // TODO: Yet GlossocErr does not know where the token ends
        // For this moment, assume that every token has length 1
        os << "^\n" FMT_NORMAL;
    }

    if (err.mMsg)
        os << "    note: " << err.mMsg << "\n";
    return os;
}

static size_t intAsString(char* buffer, size_t num)
{
    assert(buffer != nullptr);

    char* begining = buffer;
    ssize_t strLen = -1;

    for (size_t i = num; i > 0; i /= 10, ++buffer, ++strLen)
        *buffer = (char)(i % 10 + '0');

    for (size_t i = 0; i < ((size_t)strLen + 1) >> 1; ++i)
    {
        auto tmp                     = begining[i];
        begining[i]                  = begining[(size_t)strLen - i];
        begining[(size_t)strLen - i] = tmp;
    }
    *buffer = '\0';

    return (size_t)strLen;
}

const char* jumpAlongLineskip(const char* source, size_t nth)
{
    const char* output = source;
    for (; nth > 1; --nth)
    {
        output = strchr(output, '\n');
        ++output;
    }

    return output;
}
