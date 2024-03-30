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
    Environment *env = mRefStack.currentEnv();

    std::optional<Symbol> info{};

    for (;;) {
        info = env->findSymbol(expr->identifier);

        if (info.has_value() || env == nullptr)
            break;

        env = env->getEnclosing();
    }

    core::abort_if(
        !info.has_value(),
        "symbol is undefined"
    );

    auto symbol = info->as<VariableSymbol>();

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
                mRefStack.currentEnv(),
                mRefStack.getGlobal()
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
                mRefStack.currentEnv(),
                mRefStack.getGlobal()
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
                mRefStack.currentEnv(),
                mRefStack.getGlobal()
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
            break;
        case core::Operation::SUB:
            mCount += 2;
            break;
        default:
            core::abort("unreachable");
    }
}

void InstructionCountVisitor::visit(core::Assignment *stmt
) {
    stmt->expr->accept(this);

    if (stmt->index) {
        stmt->index->accept(this);
    }

    mCount++;

    if (stmt->index) {
        mCount++;
    }

    mCount += 2;
}

void InstructionCountVisitor::visit(core::VariableDecl *stmt
) {
    stmt->expr->accept(this);

    Environment *env = mRefStack.currentEnv();

    Environment *stoppingEnv = env;

    while (stoppingEnv->getType() !=
               Environment::Type::GLOBAL &&
           stoppingEnv->getType() !=
               Environment::Type::FUNCTION) {
        stoppingEnv = stoppingEnv->getEnclosing();
    }

    std::optional<Symbol> left{};

    for (;;) {
        left = env->findSymbol(stmt->identifier);

        if (left.has_value() || env == stoppingEnv)
            break;

        env = env->getEnclosing();
    }

    core::abort_if(
        !left.has_value(),
        "symbol is undefined"
    );

    auto symbol = left->as<VariableSymbol>();

    if (symbol.type.is<core::Array>()) {
        mCount++;
    }

    mCount += 3;
}

void InstructionCountVisitor::visit(core::PrintStmt *stmt) {
    stmt->expr->accept(this);

    core::Primitive type = mType.getType(
        stmt->expr.get(),
        mRefStack.currentEnv(),
        mRefStack.getGlobal()
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

    mRefStack.pushEnv();

    for (auto &stmt : block->stmts) {
        stmt->accept(this);
    }

    mRefStack.popEnv();

    mFrameDepth--;

    mCount++;
}

void InstructionCountVisitor::visit(core::FormalParam *) {
}

void InstructionCountVisitor::visit(core::FunctionDecl *stmt
) {
    mCount++;

    mRefStack.pushEnv();

    stmt->block->accept(this);

    mRefStack.popEnv();
}

void InstructionCountVisitor::visit(core::IfStmt *stmt) {
    mRefStack.pushEnv();

    stmt->cond->accept(this);

    mCount += 3;

    stmt->thenBlock->accept(this);

    mRefStack.popEnv();

    if (stmt->elseBlock) {
        mRefStack.pushEnv();

        mCount += 2;

        stmt->elseBlock->accept(this);

        mRefStack.popEnv();
    }
}

void InstructionCountVisitor::visit(core::ForStmt *stmt) {
    mCount += 2;

    mFrameDepth++;

    mRefStack.pushEnv();

    if (stmt->decl) {
        stmt->decl->accept(this);
    }

    stmt->cond->accept(this);

    mCount += 3;

    stmt->block->accept(this);

    stmt->assignment->accept(this);

    mCount += 2;

    mRefStack.popEnv();

    mFrameDepth--;

    mCount++;
}

void InstructionCountVisitor::visit(core::WhileStmt *stmt) {
    mRefStack.pushEnv();

    stmt->cond->accept(this);

    mCount += 3;

    stmt->block->accept(this);

    mCount += 2;

    mRefStack.popEnv();
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
    mRefStack.reset();
    mFrameDepth = 0;
    mCount = 0;
}

size_t InstructionCountVisitor::count(
    core::Node *node,
    RefStack stack
) {
    mRefStack = stack;

    node->accept(this);

    size_t result = mCount;

    reset();

    return result;
}

}  // namespace PArL
