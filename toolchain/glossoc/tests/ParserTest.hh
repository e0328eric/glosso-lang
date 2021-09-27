#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_PARSERTEST_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_PARSERTEST_HH_

#define IS_TEST_

#include "Ast.hh"
#include "Error.hh"
#include "Parser.hh"
#include "Span.hh"
#include "Token.hh"

using namespace glosso::glossoc;

TEST(ParserTest, ParseDeclareStatement)
{
    const char* prog = "int foo = 3";

    Parser parser{prog, 15};
    Program* program = parser.parseProgram();

    auto* gotStmt = program->popStmt();

    ASSERT_TRUE(gotStmt != nullptr);

    ASSERT_EQ(gotStmt->mType, StmtType::DeclareStmt);
#define declareStmt (gotStmt->mInner.declareStmt)
    ASSERT_EQ(declareStmt->mTypeName->mName, "int");
    ASSERT_EQ(declareStmt->mIdentifier->mName, "foo");

    delete program;
}

TEST(ParserTest, ParseReturnStatement)
{
    const char* prog = "return 3";

    Parser parser{prog, 15};
    Program* program = parser.parseProgram();

    auto* gotStmt = program->popStmt();

    ASSERT_TRUE(gotStmt != nullptr);

    ASSERT_EQ(gotStmt->mType, StmtType::ReturnStmt);
#define returnStmt (gotStmt->mInner.returnStmt)

    delete program;
}

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_PARSERTEST_HH_
