#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_

#include <iostream>
#include <vector>

#include "Error.hh"
#include "Lexer.hh"
#include "Span.hh"
#include "Token.hh"

using namespace glosso::glossoc;

#define TEST_LEXER(_prog, _expected)                               \
    Lexer lex{_prog};                                              \
    std::vector<Token> lexedTokens;                                \
    lex.lexing(lexedTokens);                                       \
                                                                   \
    ASSERT_EQ((_expected).size(), lexedTokens.size());             \
                                                                   \
    for (size_t i = 0; i < (_expected).size(); ++i)                \
    {                                                              \
        ASSERT_EQ((_expected)[i].tokType, lexedTokens[i].tokType); \
        ASSERT_EQ((_expected)[i].literal, lexedTokens[i].literal); \
    }

TEST(LexerTest, LexingSimpleSymbols)
{
    const char* prog = "+ - = : * & ! ^";
    std::vector<Token> expected{};

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingComments)
{
    const char* prog =
        R"prog(// This is a test version of a glosso language syntax
// It is like a python syntax but the comment is like C
int foo = 34127
)prog";
    std::vector<Token> expected{};

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingComma)
{
    const char* prog = R"prog(fn main():
    int bar = 2
    fn foo(x: int, y: int) -> int:
        int pug = 0
        for i in 0..x:
            pug += i * y
        return pug
    println(foo(bar, 7))
)prog";
    std::vector<Token> expected{};

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingNestedComma)
{
    const char* prog = R"prog(fn main():
    if this == 3:
        if that > 2:
            return
        elif that < 2:
            return
        else:
            return
)prog";

    std::vector<Token> expected = {};

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingStruct)
{
    const char* prog = R"prog(
struct Foo:
    bar1: int
    bar2: char
)prog";

    std::vector<Token> expected = {};

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingSimpleProgram)
{
    const char* prog = R"prog(
fn main():
    int y = 0
    for x in 0..10:
        y += x
    println(y)
    resetToZero(&y)

fn resetToZero(x: &int):
    *x = 0
)prog";

    std::vector<Token> expected = {};

    TEST_LEXER(prog, expected)
}

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_
