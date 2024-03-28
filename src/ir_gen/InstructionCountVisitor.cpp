// parl
#include <cstring>
#include <ir_gen/InstructionCountVisitor.hpp>

namespace PArL {

void InstructionCountVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);
}

void InstructionCountVisitor::visitBinary(Binary *expr) {
    expr->right->accept(this);
    expr->left->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitLiteral(Literal *) {
    mCount++;
}

void InstructionCountVisitor::visitVariable(Variable *) {
    mCount++;
}

void InstructionCountVisitor::visitUnary(Unary *expr) {
    expr->expr->accept(this);

    switch (expr->oper.getType()) {
        case Token::Type::NOT:
            mCount++;
            break;
        case Token::Type::MINUS:
            mCount += 2;
            break;
        default:
            abort("unreachable");
    }
}

void InstructionCountVisitor::visitFunctionCall(
    FunctionCall *expr
) {
    for (auto &param : expr->params) {
        param->accept(this);
    }

    mCount += 3;
}

void InstructionCountVisitor::visitBuiltinWidth(PadWidth
                                                    *) {
    mCount++;
}

void InstructionCountVisitor::visitBuiltinHeight(PadHeight
                                                     *) {
    mCount++;
}

void InstructionCountVisitor::visitBuiltinRead(PadRead *expr
) {
    expr->y->accept(this);
    expr->x->accept(this);
    mCount++;
}

void InstructionCountVisitor::visitBuiltinRandomInt(
    PadRandomInt *expr
) {
    expr->max->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitPrintStmt(PrintStmt *stmt
) {
    stmt->expr->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitDelayStmt(DelayStmt *stmt
) {
    stmt->expr->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitWriteBoxStmt(
    WriteBoxStmt *stmt
) {
    stmt->color->accept(this);
    stmt->yOffset->accept(this);
    stmt->xOffset->accept(this);
    stmt->yCoor->accept(this);
    stmt->xCoor->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitWriteStmt(WriteStmt *stmt
) {
    stmt->color->accept(this);
    stmt->yCoor->accept(this);
    stmt->xCoor->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitClearStmt(ClearStmt *stmt
) {
    stmt->color->accept(this);

    mCount++;
}

void InstructionCountVisitor::visitAssignment(
    Assignment *stmt
) {
    stmt->expr->accept(this);
    mCount += 3;
}

void InstructionCountVisitor::visitVariableDecl(
    VariableDecl *stmt
) {
    stmt->expr->accept(this);
    mCount += 3;
}

void InstructionCountVisitor::visitBlock(Block *stmt) {
    mCount += 2;

    mFrameDepth++;

    for (auto &stmt : stmt->stmts) {
        stmt->accept(this);
    }

    mFrameDepth--;

    mCount++;
}

void InstructionCountVisitor::visitIfStmt(IfStmt *stmt) {
    stmt->expr->accept(this);

    mCount += 3;

    stmt->ifThen->accept(this);

    if (stmt->ifElse) {
        mCount += 2;

        stmt->ifThen->accept(this);
    }
}

void InstructionCountVisitor::visitForStmt(ForStmt *stmt) {
    mCount += 2;

    mFrameDepth++;

    if (stmt->varDecl) {
        stmt->varDecl->accept(this);
    }

    stmt->expr->accept(this);

    mCount += 3;

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    stmt->assignment->accept(this);

    mCount += 2;

    mFrameDepth--;

    mCount++;
}

void InstructionCountVisitor::visitWhileStmt(WhileStmt *stmt
) {
    stmt->expr->accept(this);

    mCount += 3;

    stmt->block->accept(this);

    mCount += 2;
}

void InstructionCountVisitor::visitReturnStmt(
    ReturnStmt *stmt
) {
    stmt->expr->accept(this);

    mCount += mFrameDepth;

    mCount++;
}

void InstructionCountVisitor::visitFormalParam(FormalParam
                                                   *) {
}

void InstructionCountVisitor::visitFunctionDecl(
    FunctionDecl *stmt
) {
    mCount++;

    mCount += 2;

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }
}

void InstructionCountVisitor::visitProgram(Program *prog) {
    auto itr = prog->stmts.begin();

    for (; itr != prog->stmts.end(); itr++) {
        if (isFunction.check(itr->get())) {
            (*itr)->accept(this);
        } else {
            break;
        }
    }

    mCount += 3;

    mFrameDepth++;

    for (; itr != prog->stmts.end(); itr++) {
        abortIf(
            isFunction.check(itr->get()),
            "no function declaration allowed in .main"
        );

        (*itr)->accept(this);
    }

    mFrameDepth--;

    mCount += 2;
}

void InstructionCountVisitor::reset() {
    isFunction.reset();
    mFrameDepth = 0;
    mCount = 0;
}

size_t InstructionCountVisitor::count(Node *node) {
    node->accept(this);

    size_t result = mCount;

    reset();

    return result;
}

}  // namespace PArL
