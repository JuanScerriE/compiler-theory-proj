#pragma once

// parl
#include <common/Visitor.hpp>
#include <cstddef>
#include <ir_gen/RefStack.hpp>

// std
#include <string>

#include "ir_gen/InstructionCountVisitor.hpp"
#include "ir_gen/VarDeclCountVisitor.hpp"
#include "preprocess/IsFunctionVisitor.hpp"

namespace PArL {

class GenVisitor : public Visitor {
   public:
    void visitSubExpr(SubExpr *expr) override;
    void visitBinary(Binary *expr) override;
    void visitLiteral(Literal *expr) override;
    void visitVariable(Variable *expr) override;
    void visitUnary(Unary *expr) override;
    void visitFunctionCall(FunctionCall *expr) override;
    void visitBuiltinWidth(PadWidth *expr) override;
    void visitBuiltinHeight(PadHeight *expr) override;
    void visitBuiltinRead(PadRead *expr) override;
    void visitBuiltinRandomInt(PadRandomInt *expr) override;

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

    void emitLines(std::initializer_list<std::string> lines
    );
    void emitLine(std::string const &line);

    void print();

    void reset() override;

   private:
    IsFunctionVisitor isFunction{};

    VarDeclCountVisitor mCounter{};
    InstructionCountVisitor mIRCounter{};

    RefStack mStack{};

    std::vector<std::string> mCode{};

    size_t mPC{0};

    size_t mFrameDepth{0};
};

}  // namespace PArL
