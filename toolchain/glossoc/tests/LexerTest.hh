#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_

#include <iostream>
#include <vector>

#include "Error.hh"
#include "Lexer.hh"
#include "Location.hh"
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
    const char* prog = "+-=:*&!^";
    std::vector<Token> expected{
        {TokenType::Plus, "+"},      {TokenType::SubAssign, "-="},
        {TokenType::Colon, ":"},     {TokenType::Star, "*"},
        {TokenType::Ampersand, "&"}, {TokenType::Bang, "!"},
        {TokenType::Hat, "^"},       {TokenType::EndStmt, ""},
    };

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
        {TokenType::IntType, "int"}, {TokenType::Ident, "foo"},
        {TokenType::Assign, "="},    {TokenType::Integer, "34127"},
        {TokenType::EndStmt, ""},    {TokenType::EndStmt, ""},
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
        {TokenType::Function, "fn"},     {TokenType::Ident, "main"},
        {TokenType::Lparen, "("},        {TokenType::Rparen, ")"},
        {TokenType::StartStmtBlock, ""}, {TokenType::IntType, "int"},
        {TokenType::Ident, "bar"},       {TokenType::Assign, "="},
        {TokenType::Integer, "2"},       {TokenType::EndStmt, ""},
        {TokenType::Function, "fn"},     {TokenType::Ident, "foo"},
        {TokenType::Lparen, "("},        {TokenType::Ident, "x"},
        {TokenType::Colon, ":"},         {TokenType::IntType, "int"},
        {TokenType::Comma, ","},         {TokenType::Ident, "y"},
        {TokenType::Colon, ":"},         {TokenType::IntType, "int"},
        {TokenType::Rparen, ")"},        {TokenType::RightArrow, "->"},
        {TokenType::IntType, "int"},     {TokenType::StartStmtBlock, ""},
        {TokenType::IntType, "int"},     {TokenType::Ident, "pug"},
        {TokenType::Assign, "="},        {TokenType::Integer, "0"},
        {TokenType::EndStmt, ""},        {TokenType::For, "for"},
        {TokenType::Ident, "i"},         {TokenType::In, "in"},
        {TokenType::Integer, "0"},       {TokenType::RangeExcluded, ".."},
        {TokenType::Ident, "x"},         {TokenType::StartStmtBlock, ""},
        {TokenType::Ident, "pug"},       {TokenType::AddAssign, "+="},
        {TokenType::Ident, "i"},         {TokenType::Star, "*"},
        {TokenType::Ident, "y"},         {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},   {TokenType::Return, "return"},
        {TokenType::Ident, "pug"},       {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},   {TokenType::Ident, "println"},
        {TokenType::Lparen, "("},        {TokenType::Ident, "foo"},
        {TokenType::Lparen, "("},        {TokenType::Ident, "bar"},
        {TokenType::Comma, ","},         {TokenType::Integer, "7"},
        {TokenType::Rparen, ")"},        {TokenType::Rparen, ")"},
        {TokenType::EndStmt, ""},        {TokenType::EndStmtBlock, ""},
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
        {TokenType::Ident, "main"},
        {TokenType::Lparen, "("},
        {TokenType::Rparen, ")"},
        {TokenType::StartStmtBlock, ""},
        {TokenType::If, "if"},
        {TokenType::Ident, "this"},
        {TokenType::Eq, "=="},
        {TokenType::Integer, "3"},
        {TokenType::StartStmtBlock, ""},
        {TokenType::If, "if"},
        {TokenType::Ident, "that"},
        {TokenType::Gt, ">"},
        {TokenType::Integer, "2"},
        {TokenType::StartStmtBlock, ""},
        {TokenType::Return, "return"},
        {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},
        {TokenType::Elif, "elif"},
        {TokenType::Ident, "that"},
        {TokenType::Lt, "<"},
        {TokenType::Integer, "2"},
        {TokenType::StartStmtBlock, ""},
        {TokenType::Return, "return"},
        {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},
        {TokenType::Else, "else"},
        {TokenType::StartStmtBlock, ""},
        {TokenType::Return, "return"},
        {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},
        {TokenType::EndStmtBlock, ""},
        {TokenType::EndStmtBlock, ""},
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
        {TokenType::EndStmt, ""},      {TokenType::Struct, "struct"},
        {TokenType::Ident, "Foo"},     {TokenType::StartStmtBlock, ""},
        {TokenType::Ident, "bar1"},    {TokenType::Colon, ":"},
        {TokenType::IntType, "int"},   {TokenType::EndStmt, ""},
        {TokenType::Ident, "bar2"},    {TokenType::Colon, ":"},
        {TokenType::CharType, "char"}, {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""}, {TokenType::EndStmt, ""},
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
        {TokenType::EndStmt, ""},          {TokenType::Function, "fn"},
        {TokenType::Ident, "main"},        {TokenType::Lparen, "("},
        {TokenType::Rparen, ")"},          {TokenType::StartStmtBlock, ""},
        {TokenType::IntType, "int"},       {TokenType::Ident, "y"},
        {TokenType::Assign, "="},          {TokenType::Integer, "0"},
        {TokenType::EndStmt, ""},          {TokenType::For, "for"},
        {TokenType::Ident, "x"},           {TokenType::In, "in"},
        {TokenType::Integer, "0"},         {TokenType::RangeExcluded, ".."},
        {TokenType::Integer, "10"},        {TokenType::StartStmtBlock, ""},
        {TokenType::Ident, "y"},           {TokenType::AddAssign, "+="},
        {TokenType::Ident, "x"},           {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},     {TokenType::Ident, "println"},
        {TokenType::Lparen, "("},          {TokenType::Ident, "y"},
        {TokenType::Rparen, ")"},          {TokenType::EndStmt, ""},
        {TokenType::Ident, "resetToZero"}, {TokenType::Lparen, "("},
        {TokenType::Ampersand, "&"},       {TokenType::Ident, "y"},
        {TokenType::Rparen, ")"},          {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},     {TokenType::EndStmt, ""},
        {TokenType::Function, "fn"},       {TokenType::Ident, "resetToZero"},
        {TokenType::Lparen, "("},          {TokenType::Ident, "x"},
        {TokenType::Colon, ":"},           {TokenType::Ampersand, "&"},
        {TokenType::IntType, "int"},       {TokenType::Rparen, ")"},
        {TokenType::StartStmtBlock, ""},   {TokenType::Star, "*"},
        {TokenType::Ident, "x"},           {TokenType::Assign, "="},
        {TokenType::Integer, "0"},         {TokenType::EndStmt, ""},
        {TokenType::EndStmtBlock, ""},     {TokenType::EndStmt, ""},
    };

    TEST_LEXER(prog, expected)
}

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_LEXERTEST_HH_
