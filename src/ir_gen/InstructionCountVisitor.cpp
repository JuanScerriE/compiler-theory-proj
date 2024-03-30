// parl
#include <cstring>
#include <ir_gen/InstructionCountVisitor.hpp>

namespace PArL {

void InstructionCountVisitor::visit(core::Type *type) {
    core::abort("unimplemented");
}

void InstructionCountVisitor::visit(core::Expr *expr) {
    core::abort("unimplemented");
}

void InstructionCountVisitor::visit(core::PadWidth *expr) {
    mCount++;
}

void InstructionCountVisitor::visit(core::PadHeight *expr) {
    mCount++;
}

void InstructionCountVisitor::visit(core::PadRead *expr) {
    expr->y->accept(this);
    expr->x->accept(this);
    mCount++;
}

void InstructionCountVisitor::visit(core::PadRandomInt *expr
) {
    expr->max->accept(this);
    mCount++;
}

void InstructionCountVisitor::visit(core::BooleanLiteral
                                        *) {
    mCount++;
}

void InstructionCountVisitor::visit(core::IntegerLiteral
                                        *) {
    mCount++;
}

void InstructionCountVisitor::visit(core::FloatLiteral *) {
    mCount++;
}

void InstructionCountVisitor::visit(core::ColorLiteral *) {
    mCount++;
}

void InstructionCountVisitor::visit(core::ArrayLiteral *expr
) {
    for (auto itr = expr->exprs.rbegin();
         itr != expr->exprs.rend();
         itr++) {
        (*itr)->accept(this);
    }
}

void InstructionCountVisitor::visit(core::Variable *expr) {
    Environment *frame = mStack.currentFrame();

    std::optional<Symbol> info{};

    for (;;) {
        info = frame->findSymbol(expr->identifier);

        if (info.has_value())
            break;

        frame = frame->getEnclosing();

        core::abort_if(
            frame == nullptr,
            "undefined variable"
        );
    }

    VariableSymbol symbol = *info->as<VariableSymbol>();

    if (symbol.type.is<core::Base>()) {
        mCount++;

        return;
    }

    if (symbol.type.is<core::Array>()) {
        mCount += 11;

        return;
    }

    core::abort("unknown type");
}

void InstructionCountVisitor::visit(core::ArrayAccess *expr
) {
    expr->index->accept(this);

    mCount++;
}

void InstructionCountVisitor::visit(core::FunctionCall *expr
) {
    for (auto &param : expr->params) {
        param->accept(this);
    }

    mCount += 3;
}

void InstructionCountVisitor::visit(core::SubExpr *expr) {
    expr->subExpr->accept(this);
}

void InstructionCountVisitor::visit(core::Binary *expr) {
    // NOTE: the type of both left and right expressions are
    // the same

    switch (expr->op) {
        case core::Operation::AND:
        case core::Operation::OR:
        case core::Operation::LT:
        case core::Operation::GT:
        case core::Operation::EQ:
        case core::Operation::NEQ:
        case core::Operation::LE:
        case core::Operation::GE:
            expr->right->accept(this);
            expr->left->accept(this);
            mCount++;
            break;
        case core::Operation::ADD: {
            core::Primitive type = mType.getType(
                expr->right.get(),
                mStack.currentFrame()
            );

            if (type ==
                core::Primitive{core::Base::COLOR}) {
                mCount++;
                expr->right->accept(this);
                expr->left->accept(this);
                mCount += 2;
            } else {
                expr->right->accept(this);
                expr->left->accept(this);
                mCount++;
            }

        } break;
        case core::Operation::SUB: {
            core::Primitive type = mType.getType(
                expr->right.get(),
                mStack.currentFrame()
            );
            if (type ==
                core::Primitive{core::Base::COLOR}) {
                mCount++;
                expr->right->accept(this);
                expr->left->accept(this);
                mCount += 2;
            } else {
                expr->right->accept(this);
                expr->left->accept(this);
                mCount++;
            }
        } break;
        case core::Operation::MUL:
            expr->right->accept(this);
            expr->left->accept(this);
            mCount++;
        case core::Operation::DIV: {
            core::Primitive type = mType.getType(
                expr->right.get(),
                mStack.currentFrame()
            );
            if (type == core::Primitive{core::Base::INT}) {
                expr->right->accept(this);
                expr->right->accept(this);
                expr->left->accept(this);
                mCount++;
                expr->left->accept(this);
                mCount += 2;
            } else {
                expr->right->accept(this);
                expr->left->accept(this);
                mCount++;
            }
        } break;
        default:
            core::abort("unreachable");
    }
}

void InstructionCountVisitor::visit(core::Unary *expr) {
    expr->expr->accept(this);

    switch (expr->op) {
        case core::Operation::NOT:
            mCount++;
            // mReturn remains the same
            break;
        case core::Operation::SUB:
            mCount += 2;
            // mReturn remains the same
            break;
        default:
            core::abort("unreachable");
    }
}

void InstructionCountVisitor::visit(core::Assignment *stmt
) {
    stmt->expr->accept(this);

    bool isArrayAccess = false;

    if (stmt->index) {
        stmt->index->accept(this);

        isArrayAccess = true;
    }

    if (isArrayAccess) {
        mCount++;
    }

    mCount += 3;
}

void InstructionCountVisitor::visit(core::VariableDecl *stmt
) {
    stmt->expr->accept(this);

    Environment *scope = mStack.currentFrame();

    Environment *terminatingScope = scope;

    while (terminatingScope->getType() !=
               Environment::Type::GLOBAL &&
           terminatingScope->getType() !=
               Environment::Type::FUNCTION) {
        terminatingScope = terminatingScope->getEnclosing();
    }

    std::optional<Symbol> left{};

    for (;;) {
        left = scope->findSymbol(stmt->identifier);

        if (left.has_value() || scope == terminatingScope)
            break;

        scope = scope->getEnclosing();
    }

    core::abort_if(
        !left.has_value(),
        "symbol is undefined"
    );

    VariableSymbol symbol = *left->as<VariableSymbol>();

    if (symbol.type.is<core::Array>()) {
        mCount++;
    }

    mCount += 3;
}

void InstructionCountVisitor::visit(core::PrintStmt *stmt) {
    stmt->expr->accept(this);

    core::Primitive type = mType.getType(
        stmt->expr.get(),
        mStack.currentFrame()
    );

    if (type.is<core::Base>()) {
        mCount++;

        return;
    }

    if (type.is<core::Array>()) {
        mCount += 2;

        return;
    }

    core::abort("unknown type");
}

void InstructionCountVisitor::visit(core::DelayStmt *stmt) {
    stmt->expr->accept(this);

    mCount++;
}

void InstructionCountVisitor::visit(core::WriteBoxStmt *stmt
) {
    stmt->color->accept(this);
    stmt->h->accept(this);
    stmt->w->accept(this);
    stmt->y->accept(this);
    stmt->x->accept(this);

    mCount++;
}

void InstructionCountVisitor::visit(core::WriteStmt *stmt) {
    stmt->color->accept(this);
    stmt->y->accept(this);
    stmt->x->accept(this);

    mCount++;
}

void InstructionCountVisitor::visit(core::ClearStmt *stmt) {
    stmt->color->accept(this);

    mCount++;
}

void InstructionCountVisitor::visit(core::Block *block) {
    mCount += 2;

    mFrameDepth++;

    mStack.pushFrame();

    for (auto &stmt : block->stmts) {
        stmt->accept(this);
    }

    mStack.popFrame();

    mFrameDepth--;

    mCount++;
}

void InstructionCountVisitor::visit(core::FormalParam *) {
}

void InstructionCountVisitor::visit(core::FunctionDecl *stmt
) {
    mCount++;

    mCount += 2;

    mStack.pushFrame();

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    mStack.popFrame();
}

void InstructionCountVisitor::visit(core::IfStmt *stmt) {
    stmt->cond->accept(this);

    mCount += 3;

    stmt->thenBlock->accept(this);

    if (stmt->elseBlock) {
        mCount += 2;

        stmt->elseBlock->accept(this);
    }
}

void InstructionCountVisitor::visit(core::ForStmt *stmt) {
    mCount += 2;

    mFrameDepth++;

    mStack.pushFrame();

    if (stmt->decl) {
        stmt->decl->accept(this);
    }

    stmt->cond->accept(this);

    mCount += 3;

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    stmt->assignment->accept(this);

    mCount += 2;

    mStack.popFrame();

    mFrameDepth--;

    mCount++;
}

void InstructionCountVisitor::visit(core::WhileStmt *stmt) {
    stmt->cond->accept(this);

    mCount += 3;

    stmt->block->accept(this);

    mCount += 2;
}

void InstructionCountVisitor::visit(core::ReturnStmt *stmt
) {
    stmt->expr->accept(this);

    mCount += mFrameDepth;

    mCount++;
}

void InstructionCountVisitor::visit(core::Program *prog) {
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
        core::abort_if(
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
    mStack.reset();
    mFrameDepth = 0;
    mCount = 0;
}

size_t InstructionCountVisitor::count(
    core::Node *node,
    RefStack stack
) {
    mStack = stack;

    node->accept(this);

    size_t result = mCount;

    reset();

    return result;
}

}  // namespace PArL
