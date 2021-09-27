#include "Parser.hh"

using namespace glosso::glossoc;

#define EXPECTED_TOKEN(_tokenType)               \
    if (mCurrent->type != TokenType::_tokenType) \
        goto HANDLE_PARSE_FAILED;

Parser::Parser(const char* source, size_t maxErrNum)
    : mLexer(new Lexer{source, maxErrNum})
    , mCurrent(nullptr)
    , mPeek(nullptr)
{
    nextToken();
    nextToken();
}

Parser::~Parser()
{
    delete mLexer;
    delete mCurrent;
    delete mPeek;
}

Program* Parser::parseProgram()
{
    auto* output = new Program{};

    while (!mLexer->isHalt())
    {
        output->pushStmt(parseStatement());
        nextToken();
    }

    return output;
}

Statement* Parser::parseStatement()
{
    auto* output = new Statement{};

    switch (mCurrent->type)
    {
    case TokenType::IntegerType:
    case TokenType::I8Type:
    case TokenType::I16Type:
    case TokenType::I32Type:
    case TokenType::I64Type:
    case TokenType::UIntegerType:
    case TokenType::U8Type:
    case TokenType::U16Type:
    case TokenType::U32Type:
    case TokenType::U64Type:
    case TokenType::CharType:
        parseDeclareStmt(&output);
        break;

    case TokenType::Return:
        parseReturnStmt(&output);
        break;

    default:
        break;
    }

    return output;
}

// int x = 3
// Type Ident Assign Expression EndStmt
void Parser::parseDeclareStmt(Statement** stmt)
{
    auto* declareStmt = new DeclareStmt{};

    declareStmt->mTypeName = new TypeName{mCurrent->literal};
    nextToken();

    EXPECTED_TOKEN(Identifier);
    declareStmt->mIdentifier = new Identifier{mCurrent->literal};
    nextToken();

    EXPECTED_TOKEN(Assign);
    nextToken();

    // TODO: We yet implemented the expression parser
    declareStmt->mBody = parseExpression();

    // TODO: uncomment after implemeting the expression parser
    // if (!declareStmt->mBody)
    // goto HANDLE_PARSE_FAILED;
    // EXPECTED_TOKEN(EndStmt);
    // nextToken();

#define stmt (*stmt)
    stmt->mType              = StmtType::DeclareStmt;
    stmt->mInner.declareStmt = declareStmt;
#undef stmt
    return;

HANDLE_PARSE_FAILED:
    delete declareStmt;
    delete *stmt;
    *stmt = nullptr;
}

void Parser::parseReturnStmt(Statement** stmt)
{
    auto* returnStmt = new ReturnStmt{};

    // TODO: We yet implemented the expression parser
    returnStmt->mBody = parseExpression();

    // TODO: uncomment after implemeting the expression parser
    // if (!declareStmt->mBody)
    // goto HANDLE_PARSE_FAILED;
    // EXPECTED_TOKEN(EndStmt);
    // nextToken();

#define stmt (*stmt)
    stmt->mType             = StmtType::ReturnStmt;
    stmt->mInner.returnStmt = returnStmt;
#undef stmt
    return;

HANDLE_PARSE_FAILED:
    delete returnStmt;
    delete *stmt;
    *stmt = nullptr;
}

Expression* Parser::parseExpression()
{
    //
    return nullptr;
}

void Parser::nextToken()
{
    delete mCurrent;
    mCurrent = mPeek;
    mPeek    = new Token{mLexer->lexToken()};
}
