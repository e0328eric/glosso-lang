#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_

#define IS_TEST_

#include <iostream>
#include <vector>

#include "Error.hh"
#include "Lexer.hh"
#include "Span.hh"
#include "Token.hh"

using namespace glosso::glossoc;

#define TEST_LEXER(_prog, _expected)                               \
    Lexer lex{_prog, 15};                                          \
    std::vector<Token> lexedTokens;                                \
    while (!lex.isHalt())                                          \
        lexedTokens.emplace_back(lex.lexToken());                  \
                                                                   \
    ASSERT_EQ((_expected).size(), lexedTokens.size());             \
                                                                   \
    for (size_t i = 0; i < (_expected).size(); ++i)                \
    {                                                              \
        ASSERT_EQ((_expected)[i].type, lexedTokens[i].type);       \
        ASSERT_EQ((_expected)[i].literal, lexedTokens[i].literal); \
    }

TEST(LexerTest, LexingSimpleSymbols)
{
    const char* prog = "+ -= : * & ! ^ >> <<= <->";
    std::vector<Token> expected{
        {TokenType::Plus, "+"},      {TokenType::MinusAssign, "-="},
        {TokenType::Colon, ":"},     {TokenType::Star, "*"},
        {TokenType::Ampersand, "&"}, {TokenType::Bang, "!"},
        {TokenType::Hat, "^"},       {TokenType::Shr, ">>"},
        {TokenType::ShlEq, "<<="},   {TokenType::Operator, "<->"},
        {TokenType::EndStmt, ""}};

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingComments)
{
    const char* prog =
        R"prog(// This is a test version of a glosso language syntax
// It is like a python syntax but the comment is like C
int foo = 34127
)prog";
    std::vector<Token> expected{
        {TokenType::IntegerType, "int"}, {TokenType::Identifier, "foo"},
        {TokenType::Assign, "="},        {TokenType::Integer, "34127"},
        {TokenType::EndStmt, ""},        {TokenType::EndStmt, ""},
    };

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
    std::vector<Token> expected{
        {TokenType::Function, "fn"},     {TokenType::Identifier, "main"},
        {TokenType::Lparen, "("},        {TokenType::Rparen, ")"},
        {TokenType::BeginBlockStmt, ""}, {TokenType::IntegerType, "int"},
        {TokenType::Identifier, "bar"},  {TokenType::Assign, "="},
        {TokenType::Integer, "2"},       {TokenType::EndStmt, ""},
        {TokenType::Function, "fn"},     {TokenType::Identifier, "foo"},
        {TokenType::Lparen, "("},        {TokenType::Identifier, "x"},
        {TokenType::Colon, ":"},         {TokenType::IntegerType, "int"},
        {TokenType::Comma, ","},         {TokenType::Identifier, "y"},
        {TokenType::Colon, ":"},         {TokenType::IntegerType, "int"},
        {TokenType::Rparen, ")"},        {TokenType::RightArrow, "->"},
        {TokenType::IntegerType, "int"}, {TokenType::BeginBlockStmt, ""},
        {TokenType::IntegerType, "int"}, {TokenType::Identifier, "pug"},
        {TokenType::Assign, "="},        {TokenType::Integer, "0"},
        {TokenType::EndStmt, ""},        {TokenType::For, "for"},
        {TokenType::Identifier, "i"},    {TokenType::In, "in"},
        {TokenType::Integer, "0"},       {TokenType::RangeExcluded, ".."},
        {TokenType::Identifier, "x"},    {TokenType::BeginBlockStmt, ""},
        {TokenType::Identifier, "pug"},  {TokenType::PlusAssign, "+="},
        {TokenType::Identifier, "i"},    {TokenType::Star, "*"},
        {TokenType::Identifier, "y"},    {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},   {TokenType::Return, "return"},
        {TokenType::Identifier, "pug"},  {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},   {TokenType::Identifier, "println"},
        {TokenType::Lparen, "("},        {TokenType::Identifier, "foo"},
        {TokenType::Lparen, "("},        {TokenType::Identifier, "bar"},
        {TokenType::Comma, ","},         {TokenType::Integer, "7"},
        {TokenType::Rparen, ")"},        {TokenType::Rparen, ")"},
        {TokenType::EndStmt, ""},        {TokenType::EndBlockStmt, ""},
        {TokenType::EndStmt, ""},
    };

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

    std::vector<Token> expected = {
        {TokenType::Function, "fn"},
        {TokenType::Identifier, "main"},
        {TokenType::Lparen, "("},
        {TokenType::Rparen, ")"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::If, "if"},
        {TokenType::Identifier, "this"},
        {TokenType::Equal, "=="},
        {TokenType::Integer, "3"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::If, "if"},
        {TokenType::Identifier, "that"},
        {TokenType::Gt, ">"},
        {TokenType::Integer, "2"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::Return, "return"},
        {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::Elif, "elif"},
        {TokenType::Identifier, "that"},
        {TokenType::Lt, "<"},
        {TokenType::Integer, "2"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::Return, "return"},
        {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::Else, "else"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::Return, "return"},
        {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::EndStmt, ""},
    };

    TEST_LEXER(prog, expected)
}

TEST(LexerTest, LexingStruct)
{
    const char* prog = R"prog(
struct Foo:
    bar1: int
    bar2: char
)prog";

    std::vector<Token> expected = {
        {TokenType::EndStmt, ""},        {TokenType::Struct, "struct"},
        {TokenType::Identifier, "Foo"},  {TokenType::BeginBlockStmt, ""},
        {TokenType::Identifier, "bar1"}, {TokenType::Colon, ":"},
        {TokenType::IntegerType, "int"}, {TokenType::EndStmt, ""},
        {TokenType::Identifier, "bar2"}, {TokenType::Colon, ":"},
        {TokenType::CharType, "char"},   {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},   {TokenType::EndStmt, ""},
    };

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

    std::vector<Token> expected = {
        {TokenType::EndStmt, ""},
        {TokenType::Function, "fn"},
        {TokenType::Identifier, "main"},
        {TokenType::Lparen, "("},
        {TokenType::Rparen, ")"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::IntegerType, "int"},
        {TokenType::Identifier, "y"},
        {TokenType::Assign, "="},
        {TokenType::Integer, "0"},
        {TokenType::EndStmt, ""},
        {TokenType::For, "for"},
        {TokenType::Identifier, "x"},
        {TokenType::In, "in"},
        {TokenType::Integer, "0"},
        {TokenType::RangeExcluded, ".."},
        {TokenType::Integer, "10"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::Identifier, "y"},
        {TokenType::PlusAssign, "+="},
        {TokenType::Identifier, "x"},
        {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::Identifier, "println"},
        {TokenType::Lparen, "("},
        {TokenType::Identifier, "y"},
        {TokenType::Rparen, ")"},
        {TokenType::EndStmt, ""},
        {TokenType::Identifier, "resetToZero"},
        {TokenType::Lparen, "("},
        {TokenType::Ampersand, "&"},
        {TokenType::Identifier, "y"},
        {TokenType::Rparen, ")"},
        {TokenType::EndStmt, ""},
        {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::Function, "fn"},
        {TokenType::Identifier, "resetToZero"},
        {TokenType::Lparen, "("},
        {TokenType::Identifier, "x"},
        {TokenType::Colon, ":"},
        {TokenType::Ampersand, "&"},
        {TokenType::IntegerType, "int"},
        {TokenType::Rparen, ")"},
        {TokenType::BeginBlockStmt, ""},
        {TokenType::Star, "*"},
        {TokenType::Identifier, "x"},
        {TokenType::Assign, "="},
        {TokenType::Integer, "0"},
        {TokenType::EndStmt, ""},
        {TokenType::EndBlockStmt, ""},
        {TokenType::EndStmt, ""},
    };

    TEST_LEXER(prog, expected)
}

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_
