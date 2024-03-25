#pragma once

// parl
#include <common/AST.hpp>
#include <common/Visitor.hpp>

namespace PArL {

class VarDeclCountVisitor : public Visitor {
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
    void visitBuiltinRandomInt(BuiltinRandomInt *expr
    ) override;

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

    void reset() override;

    size_t count(Node *node);

   private:
    size_t mCount{0};
};

}  // namespace PArL
