#ifndef GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_HH_
#define GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_HH_

namespace glosso::glossoc
{
enum class StmtKind
{
    IfStmt,
    WhileStmt,
    ForStmt,
};

class Statement
{
  public:
    explicit Statement(StmtKind kind);
    virtual ~Statement() = 0;

    virtual StmtKind getKind() const = 0;

  private:
    StmtKind mKind;
};

class IfStmt : public Statement
{
  public:
    IfStmt() = default;
    ~IfStmt() override;
};
} // namespace glosso::glossoc
#endif // GLOSSO_LANG_TOOLCHAIN_GLOSSOC_AST_HH_
