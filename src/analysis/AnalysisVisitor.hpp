#pragma once

// vought
#include <analysis/SymbolStack.hpp>
#include <common/Visitor.hpp>

namespace PArL {

class SyncAnalysis : public std::exception {};

class AnalysisVisitor : public Visitor {
   public:
    void visitSubExpr(SubExpr *expr) override;
    void visitBinary(Binary *expr) override;
    void visitLiteral(Literal *expr) override;
    void visitVariable(Variable *expr) override;
    void visitUnary(Unary *expr) override;
    void visitFunctionCall(FunctionCall *expr) override;
    void visitBuiltinWidth(BuiltinWidth *expr) override;
    void visitBuiltinHeight(BuiltinHeight *expr) override;
    void visitBuiltinRead(BuiltinRead *expr) override;
    void visitBuiltinRandomInt(
        BuiltinRandomInt *expr) override;

    void visitPrintStmt(PrintStmt *stmt) override;
    void visitDelayStmt(DelayStmt *stmt) override;
    void visitWriteBoxStmt(WriteBoxStmt *stmt) override;
    void visitWriteStmt(WriteStmt *stmt) override;
    void visitClearStmt(ClearStmt *stmt) override;
    void visitAssignment(Assignment *stmt) override;
    void visitVariableDecl(VariableDecl *stmt) override;
    void visitBlock(Block *stmt) override;
    void visitIfStmt(IfStmt *stmt) override;
    void visitForStmt(ForStmt *stmt) override;
    void visitWhileStmt(WhileStmt *stmt) override;
    void visitReturnStmt(ReturnStmt *stmt) override;
    void visitFormalParam(FormalParam *param) override;
    void visitFunctionDecl(FunctionDecl *stmt) override;
    void visitProgram(Program *prog) override;

    void unscopedBlock(Block *block);
    void optionalCast(Expr *expr);

    void error(Token const &token, const std::string &msg);
    [[nodiscard]] bool hasError() const;

    void reset() override;

   private:
    bool mHasError{false};
    bool mBranchReturns{false};
    Signature mReturn{};
    SymbolStack mSymbolStack{};
};

}  // namespace PArL
