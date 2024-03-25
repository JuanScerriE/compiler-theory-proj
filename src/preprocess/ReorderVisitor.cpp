// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <common/AST.hpp>
#include <common/Abort.hpp>
#include <common/Token.hpp>
#include <preprocess/ReorderVisitor.hpp>

// std
#include <memory>

namespace PArL {

void ReorderVisitor::visitSubExpr(SubExpr *) {
}

void ReorderVisitor::visitBinary(Binary *) {
}

void ReorderVisitor::visitLiteral(Literal *) {
}

void ReorderVisitor::visitVariable(Variable *) {
}

void ReorderVisitor::visitUnary(Unary *) {
}

void ReorderVisitor::visitFunctionCall(FunctionCall *) {
}

void ReorderVisitor::visitBuiltinWidth(BuiltinWidth *) {
}

void ReorderVisitor::visitBuiltinHeight(BuiltinHeight *) {
}

void ReorderVisitor::visitBuiltinRead(BuiltinRead *) {
}

void ReorderVisitor::visitBuiltinRandomInt(BuiltinRandomInt
                                               *) {
}

void ReorderVisitor::visitPrintStmt(PrintStmt *) {
}

void ReorderVisitor::visitDelayStmt(DelayStmt *) {
}

void ReorderVisitor::visitWriteBoxStmt(WriteBoxStmt *) {
}

void ReorderVisitor::visitWriteStmt(WriteStmt *) {
}

void ReorderVisitor::visitClearStmt(ClearStmt *) {
}

void ReorderVisitor::visitAssignment(Assignment *) {
}

void ReorderVisitor::visitVariableDecl(VariableDecl *) {
}

void ReorderVisitor::visitBlock(Block *stmt) {
    reorder(stmt->stmts);

    for (auto &stmt : stmt->stmts) {
        stmt->accept(this);
    }
}

void ReorderVisitor::visitIfStmt(IfStmt *stmt) {
    stmt->ifThen->accept(this);

    if (stmt->ifElse) {
        stmt->ifElse->accept(this);
    }
}

void ReorderVisitor::visitForStmt(ForStmt *stmt) {
    stmt->block->accept(this);
}

void ReorderVisitor::visitWhileStmt(WhileStmt *stmt) {
    stmt->block->accept(this);
}

void ReorderVisitor::visitReturnStmt(ReturnStmt *) {
}

void ReorderVisitor::visitFormalParam(FormalParam *) {
}

void ReorderVisitor::visitFunctionDecl(FunctionDecl *stmt) {
    stmt->block->accept(this);
}

void ReorderVisitor::visitProgram(Program *prog) {
    reorder(prog->stmts);

    for (auto &stmt : prog->stmts) {
        stmt->accept(this);
    }
}

void ReorderVisitor::reorder(
    std::vector<std::unique_ptr<Stmt>> &stmts
) {
    for (auto &stmt : stmts) {
        if (isFunction.check(stmt.get())) {
            mFuncQueue.push_back(std::move(stmt));
        } else {
            mStmtQueue.push_back(std::move(stmt));
        }
    }

    stmts.clear();

    for (auto &stmt : mFuncQueue) {
        stmts.push_back(std::move(stmt));
    }

    for (auto &stmt : mStmtQueue) {
        stmts.push_back(std::move(stmt));
    }

    reset();
}

void ReorderVisitor::reset() {
    mFuncQueue.clear();
    mStmtQueue.clear();
}

}  // namespace PArL
