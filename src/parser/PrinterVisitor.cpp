// fmt
#include <fmt/core.h>

// parl
#include <common/AST.hpp>
#include <parser/PrinterVisitor.hpp>

namespace PArL {

void PrinterVisitor::printWithTabs(std::string const &msg
) const {
    for (int i = 0; i < mTabCount; i++) {
        fmt::print("  ");
    }

    fmt::println(msg);
}

void PrinterVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);
}

void PrinterVisitor::visitBinary(Binary *expr) {
    mNodeCount++;
    printWithTabs(fmt::format(
        "Binary Operation {} =>", expr->oper.getLexeme()
    ));
    mTabCount++;
    expr->left->accept(this);
    expr->right->accept(this);
    mTabCount--;
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitLiteral(Literal *expr) {
    mNodeCount++;
    printWithTabs(
        fmt::format("Literal {}", expr->value.toString())
    );
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitVariable(Variable *expr) {
    mNodeCount++;
    printWithTabs(
        fmt::format("Variable {}", expr->name.getLexeme())
    );
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitUnary(Unary *expr) {
    mNodeCount++;
    printWithTabs(fmt::format(
        "Unary Operation {} =>", expr->oper.getLexeme()
    ));
    mTabCount++;
    expr->expr->accept(this);
    mTabCount--;
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitFunctionCall(FunctionCall *expr) {
    mNodeCount++;
    printWithTabs("Function Call =>");
    mTabCount++;
    printWithTabs(expr->identifier.getLexeme());
    for (auto &param : expr->params) {
        param->accept(this);
    }
    mTabCount--;
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitBuiltinWidth(BuiltinWidth *expr) {
    mNodeCount++;
    printWithTabs("__width");
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitBuiltinHeight(BuiltinHeight *expr
) {
    mNodeCount++;
    printWithTabs("__height");
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitBuiltinRead(BuiltinRead *expr) {
    mNodeCount++;
    printWithTabs("__read =>");
    mTabCount++;
    expr->x->accept(this);
    expr->y->accept(this);
    mTabCount--;
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr
) {
    mNodeCount++;
    printWithTabs("__random_int =>");
    mTabCount++;
    expr->max->accept(this);
    mTabCount--;
    if (expr->type.has_value()) {
        printWithTabs(
            fmt::format("as {}", expr->type->getLexeme())
        );
    }
}

void PrinterVisitor::visitPrintStmt(PrintStmt *stmt) {
    mNodeCount++;
    printWithTabs("__print =>");
    mTabCount++;
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitDelayStmt(DelayStmt *stmt) {
    mNodeCount++;
    printWithTabs("__delay =>");
    mTabCount++;
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitClearStmt(ClearStmt *stmt) {
    mNodeCount++;
    printWithTabs("__clear =>");
    mTabCount++;
    stmt->color->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitWriteBoxStmt(WriteBoxStmt *stmt) {
    mNodeCount++;
    printWithTabs("__write_box =>");
    mTabCount++;
    stmt->xCoor->accept(this);
    stmt->xOffset->accept(this);
    stmt->yCoor->accept(this);
    stmt->yOffset->accept(this);
    stmt->color->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitWriteStmt(WriteStmt *stmt) {
    mNodeCount++;
    printWithTabs("__write =>");
    mTabCount++;
    stmt->xCoor->accept(this);
    stmt->yCoor->accept(this);
    stmt->color->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitAssignment(Assignment *stmt) {
    mNodeCount++;
    printWithTabs("Assignment =>");
    mTabCount++;
    printWithTabs(stmt->identifier.getLexeme());
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitVariableDecl(VariableDecl *stmt) {
    mNodeCount++;
    printWithTabs("VariableDecl =>");
    mTabCount++;
    printWithTabs(stmt->identifier.getLexeme());
    printWithTabs(stmt->type.getLexeme());
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitBlock(Block *stmt) {
    mNodeCount++;
    printWithTabs("Block =>");
    mTabCount++;
    for (auto &innerStmt : stmt->stmts) {
        innerStmt->accept(this);
    }
    mTabCount--;
}

void PrinterVisitor::visitIfStmt(IfStmt *stmt) {
    mNodeCount++;
    printWithTabs("If =>");
    mTabCount++;
    stmt->expr->accept(this);
    stmt->ifThen->accept(this);
    mTabCount--;

    if (stmt->ifElse) {
        printWithTabs("Else =>");
        mTabCount++;
        stmt->ifElse->accept(this);
        mTabCount--;
    }
}

void PrinterVisitor::visitForStmt(ForStmt *stmt) {
    mNodeCount++;
    printWithTabs("For =>");
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
    printWithTabs("While =>");
    mTabCount++;
    stmt->expr->accept(this);
    stmt->block->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitReturnStmt(ReturnStmt *stmt) {
    mNodeCount++;
    printWithTabs("Return =>");
    mTabCount++;
    stmt->expr->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitFormalParam(FormalParam *param) {
    mNodeCount++;
    printWithTabs("Formal Param =>");
    mTabCount++;
    printWithTabs(param->identifier.getLexeme());
    printWithTabs(param->type.getLexeme());
    mTabCount--;
}

void PrinterVisitor::visitFunctionDecl(FunctionDecl *stmt) {
    mNodeCount++;
    printWithTabs("Func Decl =>");
    mTabCount++;
    printWithTabs(stmt->identifier.getLexeme());
    for (auto &param : stmt->params) {
        param->accept(this);
    }
    printWithTabs(stmt->type.getLexeme());
    stmt->block->accept(this);
    mTabCount--;
}

void PrinterVisitor::visitProgram(Program *prog) {
    mNodeCount++;
    printWithTabs("Program =>");
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

}  // namespace PArL
