// fmt
#include <fmt/core.h>

// vought
#include <common/AST.hpp>
#include <parser/PrinterVisitor.hpp>

namespace Vought {

void PrinterVisitor::visitSubExpr(SubExpr *expr) {
    expr->accept(this);
}

void PrinterVisitor::visitBinary(Binary *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Binary Operation {} =>", "",
                 mTabCount, expr->oper.getLexeme());
    mTabCount++;
    expr->left.get()->accept(this);
    expr->right.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitLiteral(Literal *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Literal {} =>", "", mTabCount,
                 expr->value.toString(true));
}

void PrinterVisitor::visitVariable(Variable *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Variable {}", "", mTabCount,
                 expr->name.getLexeme());
}

void PrinterVisitor::visitUnary(Unary *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Unary Operation {} =>", "",
                 mTabCount, expr->oper.getLexeme());
    mTabCount++;
    expr->expr.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitFunctionCall(FunctionCall *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Function Call =>", "", mTabCount);
    mTabCount++;
    fmt::println("{:\t{}} {}", "", mTabCount,
                 expr->identifier.getLexeme());
    for (auto &param : *expr->params) {
        expr->accept(this);
    }
    mTabCount--;
}

void PrinterVisitor::visitBuiltinWidth(BuiltinWidth *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} __width", "", mTabCount);
}

void PrinterVisitor::visitBuiltinHeight(
    BuiltinHeight *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} __height", "", mTabCount);
}

void PrinterVisitor::visitBuiltinRead(BuiltinRead *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} __read =>", "", mTabCount);
    mTabCount++;
    expr->x.get()->accept(this);
    expr->y.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} __random_int =>", "", mTabCount);
    mTabCount++;
    expr->max.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitPrintStmt(PrintStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} __print =>", "", mTabCount);
    mTabCount++;
    stmt->expr.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitDelayStmt(DelayStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} __delay =>", "", mTabCount);
    mTabCount++;
    stmt->expr.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitWriteBoxStmt(WriteBoxStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} __write_box =>", "", mTabCount);
    mTabCount++;
    stmt->xCoor.get()->accept(this);
    stmt->xOffset.get()->accept(this);
    stmt->yCoor.get()->accept(this);
    stmt->yOffset.get()->accept(this);
    stmt->color.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitWriteStmt(WriteStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} __write =>", "", mTabCount);
    mTabCount++;
    stmt->xCoor.get()->accept(this);
    stmt->yCoor.get()->accept(this);
    stmt->color.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitAssignment(Assignment *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} Assignment =>", "", mTabCount);
    mTabCount++;
    fmt::println("{:\t{}} {}", "", mTabCount,
                 stmt->identifier.getLexeme());
    stmt->expr.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitVariableDecl(VariableDecl *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} VariableDecl =>", "", mTabCount);
    mTabCount++;
    fmt::println("{:\t{}} {}", "", mTabCount,
                 stmt->identifier.getLexeme());
    fmt::println("{:\t{}} {}", "", mTabCount,
                 stmt->type.getLexeme());
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitBlock(Block *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} Block =>", "", mTabCount);
    mTabCount++;
    for (auto &innerStmt : stmt->stmts) {
        innerStmt->accept(this);
    }
    mTabCount--;
}

void PrinterVisitor::visitIfStmt(IfStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} If =>", "", mTabCount);
    mTabCount++;
    stmt->expr->accept(this);
    stmt->ifThen->accept(this);
    mTabCount--;

    if (stmt->ifElse) {
        fmt::println("{:\t{}} Else =>", "", mTabCount);
        mTabCount++;
        stmt->ifElse->accept(this);
        mTabCount--;
    }
}

void PrinterVisitor::visitForStmt(ForStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} For =>", "", mTabCount);
    mTabCount++;
    if (stmt->varDecl) {
        stmt->varDecl->accept(this);
    }
    stmt->expr->accept(this);
    if (stmt->assignment) {
        stmt->assignment->accept(this);
    }
    stmt->block->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitWhileStmt(WhileStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} While =>", "", mTabCount);
    mTabCount++;
    stmt->expr->accept(this);
    stmt->block->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitReturnStmt(ReturnStmt *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} Return =>", "", mTabCount);
    mTabCount++;
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitFormalParam(FormalParam *param) {
    mNodeCount++;
    fmt::println("{:\t{}} Formal Param =>", "", mTabCount);
    mTabCount++;
    fmt::println("{:\t{}} {}", "", mTabCount,
                 param->identifier.getLexeme());
    fmt::println("{:\t{}} {}", "", mTabCount,
                 param->type.getLexeme());
    mTabCount--;
}

void PrinterVisitor::visitFunctionDecl(FunctionDecl *stmt) {
    mNodeCount++;
    fmt::println("{:\t{}} Func Decl =>", "", mTabCount);
    mTabCount++;
    fmt::println("{:\t{}} {}", "", mTabCount,
                 stmt->identifier.getLexeme());
    for (auto &param : *stmt->params) {
        param->accept(this);
    }
    fmt::println("{:\t{}} {}", "", mTabCount,
                 stmt->type.getLexeme());
    stmt->block->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitProgram(Program *prog) {
    mNodeCount++;
    fmt::println("{:\t{}} Program =>", "", mTabCount);
    mTabCount++;
    for (auto &stmt : prog->stmts) {
        stmt->accept(this);
    }
    mTabCount--;
}

void PrinterVisitor::reset() {
    mTabCount = 0;
    mNodeCount = 0;
}

}  // namespace Vought
