#include "Lexer.hh"

using namespace glosso::glossoc;

// Static Functions ////
static inline char peekChar(const char* str, size_t n) { return *(str + n); }
////////

Lexer::Lexer(const char* source)
    : mSource(source)
    , mStart(source)
    , mCurrent(source)
    , isHalt(false)
{
}
