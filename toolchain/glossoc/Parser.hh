#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_PARSER_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_PARSER_HH_

#include "Ast.hh"
#include "Lexer.hh"
#include "Token.hh"

namespace glosso::glossoc
{
class Parser
{
  public:
    explicit Parser(const char* source, size_t maxErrNum);
    ~Parser();

    Parser(const Parser&) = delete;
    Parser(Parser&&)      = delete;
    Parser& operator=(const Parser&) = delete;
    Parser& operator=(Parser&&) = delete;

    Program* parseProgram();

  private:
    void nextToken();
    Statement* parseStatement();
    void parseDeclareStmt(Statement** stmt);
    void parseReturnStmt(Statement** stmt);

    Expression* parseExpression();

  private:
    Lexer* mLexer;
    Token* mCurrent;
    Token* mPeek;
};
} // namespace glosso::glossoc

#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_TESTS_PARSER_HH_
