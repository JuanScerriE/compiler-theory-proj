// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// parl
#include <ir_gen/GenVisitor.hpp>
#include <parl/AST.hpp>
#include <parl/Core.hpp>

// std
#include <initializer_list>
#include <memory>

namespace PArL {

GenVisitor::GenVisitor(Environment *global) {
    mStack.init(global);
}

void GenVisitor::visit(core::Type *) {
    core::abort("unimplemented");
}

void GenVisitor::visit(core::Expr *expr) {
    core::abort("unimplemented");
}

void GenVisitor::visit(core::PadWidth *expr) {
    emit_line("width");
}

void GenVisitor::visit(core::PadHeight *expr) {
    emit_line("height");
}

void GenVisitor::visit(core::PadRead *expr) {
    expr->y->accept(this);
    expr->x->accept(this);
    emit_line("read");
}

void GenVisitor::visit(core::PadRandomInt *expr) {
    expr->max->accept(this);
    emit_line("irnd");
}

void GenVisitor::visit(core::BooleanLiteral *expr) {
    emit_line("push {}", expr->value ? 1 : 0);
}

void GenVisitor::visit(core::IntegerLiteral *expr) {
    emit_line("push {}", expr->value);
}

void GenVisitor::visit(core::FloatLiteral *expr) {
    emit_line("push {}", expr->value);
}

void GenVisitor::visit(core::ColorLiteral *expr) {
    emit_line(
        "push #{:x}{:x}{:x}",
        expr->value.r(),
        expr->value.g(),
        expr->value.b()
    );
}

void GenVisitor::visit(core::ArrayLiteral *expr) {
    for (auto itr = expr->exprs.rbegin();
         itr != expr->exprs.rend();
         itr++) {
        (*itr)->accept(this);
    }
}

void GenVisitor::visit(core::Variable *expr) {
    size_t level = 0;

    Environment *frame = mStack.currentFrame();

    std::optional<Symbol> info{};

    for (;;) {
        info = frame->findSymbol(expr->identifier);

        if (info.has_value())
            break;

        level++;

        frame = frame->getEnclosing();

        core::abort_if(
            frame == nullptr,
            "undefined variable"
        );
    }

    VariableSymbol symbol = *info->as<VariableSymbol>();

    if (symbol.type.is<core::Base>()) {
        emit_line("push [{}:{}]", symbol.idx, level);

        return;
    }

    if (symbol.type.is<core::Array>()) {
        size_t arraySize =
            symbol.type.as<core::Array>().size;
        emit_line("push {}", arraySize);
        emit_line("pusha [{}:{}]", symbol.idx, level);
        emit_line("push {} // START HACK", arraySize);
        emit_line("oframe");
        emit_line("push {}", arraySize);
        emit_line("push 0");
        emit_line("push 0");
        emit_line("sta");
        emit_line("push {}", arraySize);
        emit_line("pusha [0:0]");
        emit_line("cframe // END HACK");

        return;
    }

    core::abort("unknown type");
}

void GenVisitor::visit(core::ArrayAccess *expr) {
    size_t level = 0;

    Environment *scope = mStack.currentFrame();

    Environment *terminatingScope = scope;

    while (terminatingScope->getType() !=
               Environment::Type::GLOBAL &&
           terminatingScope->getType() !=
               Environment::Type::FUNCTION) {
        terminatingScope = terminatingScope->getEnclosing();
    }

    std::optional<Symbol> info{};

    for (;;) {
        info = scope->findSymbol(expr->identifier);

        if (info.has_value() || scope == terminatingScope)
            break;

        level++;

        scope = scope->getEnclosing();
    }

    core::abort_if(
        !info.has_value(),
        "symbol is undefined"
    );

    expr->index->accept(this);

    VariableSymbol symbol = *info->as<VariableSymbol>();

    emit_line("push +[{}:{}]", symbol.idx, level);
}

void GenVisitor::visit(core::FunctionCall *expr) {
    for (auto &param : expr->params) {
        param->accept(this);
    }

    size_t size{0};

    for (auto &param : expr->params) {
        core::Primitive paramType = mType.getType(
            param.get(),
            mStack.currentFrame()
        );

        if (paramType.is<core::Array>()) {
            size += paramType.as<core::Array>().size;
        } else {
            size++;
        }
    }

    emit_line("push {}", size);

    emit_line("push .{}", expr->identifier);

    emit_line("call");
}

void GenVisitor::visit(core::SubExpr *expr) {
    expr->subExpr->accept(this);
}

void GenVisitor::visit(core::Binary *expr) {
    // NOTE: the type of both left and right expressions are
    // the same

    switch (expr->op) {
        case core::Operation::AND:
            // NOTE: we are evaluating in the operations
            // due to the fact that the operations
            // in the VM expect left-most operand as
            // being at the top of th stack e.g. mod
            // 3 % 4, 3 will be at the top
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("and");
            // mReturn remains the same
            break;
        case core::Operation::OR:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("or");
            // mReturn remains the same
            break;
        case core::Operation::LT:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("lt");
            break;
        case core::Operation::GT:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("gt");
            break;
        case core::Operation::EQ:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("eq");
            break;
        case core::Operation::NEQ:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("neq");
            break;
        case core::Operation::LE:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("le");
            break;
        case core::Operation::GE:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("ge");
            break;
        case core::Operation::ADD: {
            core::Primitive type = mType.getType(
                expr->right.get(),
                mStack.currentFrame()
            );

            if (type ==
                core::Primitive{core::Base::COLOR}) {
                emit_line("push 16777216");  // #ffffff + 1
                expr->right->accept(this);
                expr->left->accept(this);
                emit_line("add");
                emit_line("mod");
            } else {
                expr->right->accept(this);
                expr->left->accept(this);
                emit_line("add");
            }

        } break;
        case core::Operation::SUB: {
            core::Primitive type = mType.getType(
                expr->right.get(),
                mStack.currentFrame()
            );
            if (type ==
                core::Primitive{core::Base::COLOR}) {
                emit_line("push 16777216");
                expr->right->accept(this);
                expr->left->accept(this);
                emit_line("sub");
                emit_line("mod");
            } else {
                expr->right->accept(this);
                expr->left->accept(this);
                emit_line("sub");
            }
        } break;
        case core::Operation::MUL:
            expr->right->accept(this);
            expr->left->accept(this);
            emit_line("mul");
            break;
        case core::Operation::DIV: {
            core::Primitive type = mType.getType(
                expr->right.get(),
                mStack.currentFrame()
            );

            if (type == core::Primitive{core::Base::INT}) {
                expr->right->accept(this);
                expr->right->accept(this);
                expr->left->accept(this);
                emit_line("mod");
                expr->left->accept(this);
                emit_line("sub");
                emit_line("div");
            } else {
                expr->right->accept(this);
                expr->left->accept(this);
                emit_line("div");
            }
        } break;
        default:
            core::abort("unreachable");
    }
}

void GenVisitor::visit(core::Unary *expr) {
    expr->expr->accept(this);

    switch (expr->op) {
        case core::Operation::NOT:
            emit_line("not");
            // mReturn remains the same
            break;
        case core::Operation::SUB: {
            core::Primitive type = mType.getType(
                expr->expr.get(),
                mStack.currentFrame()
            );
            if (type ==
                core::Primitive{core::Base::COLOR}) {
                emit_line("push #ffffff");
                emit_line("sub");
            } else {
                emit_line("push -1");
                emit_line("mul");
            }
            // mReturn remains the same
        } break;
        default:
            core::abort("unreachable");
    }
}

void GenVisitor::visit(core::Assignment *stmt) {
    stmt->expr->accept(this);

    size_t level = 0;

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

        level++;

        scope = scope->getEnclosing();
    }

    core::abort_if(
        !left.has_value(),
        "symbol is undefined"
    );

    bool isArrayAccess = false;

    if (stmt->index) {
        stmt->index->accept(this);

        isArrayAccess = true;
    }

    emit_line("push {}", left->as<VariableSymbol>()->idx);

    if (isArrayAccess) {
        emit_line("add");
    }

    emit_line("push {}", level);

    emit_line("st");
}

void GenVisitor::visit(core::VariableDecl *stmt) {
    stmt->expr->accept(this);

    size_t level = 0;

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

        level++;

        scope = scope->getEnclosing();
    }

    core::abort_if(
        !left.has_value(),
        "symbol is undefined"
    );

    size_t idx = scope->getIdx();

    VariableSymbol symbol = *left->as<VariableSymbol>();

    if (symbol.type.is<core::Base>()) {
        scope->incIdx();
    } else if (symbol.type.is<core::Array>()) {
        scope->incIdx(symbol.type.as<core::Array>().size);
    } else {
        core::abort("unknown type");
    }

    // NOTE: make sure this is actually a reference.
    scope->getSymbolAsRef(stmt->identifier)
        .asRef<VariableSymbol>()
        .idx = idx;

    if (symbol.type.is<core::Array>()) {
        emit_line(
            "push {}",
            symbol.type.as<core::Array>().size
        );
    }
    emit_line("push {}", idx);
    emit_line("push 0");
    if (symbol.type.is<core::Array>()) {
        emit_line("sta");
    } else {
        emit_line("st");
    }
}

void GenVisitor::visit(core::PrintStmt *stmt) {
    stmt->expr->accept(this);

    core::Primitive type = mType.getType(
        stmt->expr.get(),
        mStack.currentFrame()
    );

    if (type.is<core::Base>()) {
        emit_line("print");

        return;
    }

    if (type.is<core::Array>()) {
        emit_line("push {}", type.as<core::Array>().size);
        emit_line("printa");

        return;
    }

    core::abort("unknown type");
}

void GenVisitor::visit(core::DelayStmt *stmt) {
    stmt->expr->accept(this);

    emit_line("delay");
}

void GenVisitor::visit(core::WriteBoxStmt *stmt) {
    stmt->color->accept(this);
    stmt->h->accept(this);
    stmt->w->accept(this);
    stmt->y->accept(this);
    stmt->x->accept(this);

    emit_line("writebox");
}

void GenVisitor::visit(core::WriteStmt *stmt) {
    stmt->color->accept(this);
    stmt->y->accept(this);
    stmt->x->accept(this);

    emit_line("write");
}

void GenVisitor::visit(core::ClearStmt *stmt) {
    stmt->color->accept(this);

    emit_line("clear");
}

void GenVisitor::visit(core::Block *block) {
    Environment *nextFrame = mStack.peekNextFrame();

    size_t count = 0;

    for (auto &stmt : block->stmts) {
        count += mDeclCounter.count(stmt.get(), nextFrame);
    }

    emit_line("push {}", count);
    emit_line("oframe");

    mFrameDepth++;

    mStack.pushFrame(count);

    for (auto &stmt : block->stmts) {
        stmt->accept(this);
    }

    mStack.popFrame();

    mFrameDepth--;

    emit_line("cframe");
}

void GenVisitor::visit(core::FormalParam *param) {
    Environment *scope = mStack.currentFrame();

    std::optional<Symbol> symbol =
        scope->findSymbol(param->identifier);

    core::abort_if(
        !symbol.has_value(),
        "symbol is undefined"
    );

    size_t idx = scope->getIdx();

    VariableSymbol variable = *symbol->as<VariableSymbol>();

    if (variable.type.is<core::Base>()) {
        scope->incIdx();
    } else if (variable.type.is<core::Array>()) {
        scope->incIdx(variable.type.as<core::Array>().size);
    } else {
        core::abort("unknown type");
    }

    // NOTE: make sure this is actually a reference.
    scope->getSymbolAsRef(param->identifier)
        .asRef<VariableSymbol>()
        .idx = idx;
}

void GenVisitor::visit(core::FunctionDecl *stmt) {
    Environment *nextFrame = mStack.peekNextFrame();

    size_t aritySize{0};

    for (auto &stmt : stmt->params) {
        aritySize +=
            mDeclCounter.count(stmt.get(), nextFrame);
    }

    size_t count{0};

    for (auto &stmt : stmt->block->stmts) {
        count += mDeclCounter.count(stmt.get(), nextFrame);
    }

    emit_line(".{}", stmt->identifier);

    mStack.pushFrame(aritySize + count);

    for (auto &param : stmt->params) {
        param->accept(this);
    }

    emit_line("push {}", count);
    emit_line("alloc");

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    mStack.popFrame();
}

void GenVisitor::visit(core::IfStmt *stmt) {
    stmt->cond->accept(this);

    emit_line("not");

    size_t thenSize =
        mIRCounter.count(stmt->thenBlock.get(), mStack);

    // 1 for the offset, 1 for the cjmp, 1 for else if exits
    emit_line(
        "push #PC+{}",
        1 + 1 + (stmt->elseBlock ? 2 : 0) + thenSize
    );
    emit_line("cjmp");

    stmt->thenBlock->accept(this);

    if (stmt->elseBlock) {
        size_t elseSize =
            mIRCounter.count(stmt->elseBlock.get(), mStack);

        emit_line(
            fmt::format("push #PC+{}", 1 + 1 + elseSize)
        );
        emit_line("jmp");
        stmt->elseBlock->accept(this);
    }
}

void GenVisitor::visit(core::ForStmt *stmt) {
    size_t count = 0;

    Environment *nextFrame = mStack.peekNextFrame();

    count = mDeclCounter.count(stmt->decl.get(), nextFrame);

    for (auto &stmt : stmt->block->stmts) {
        count += mDeclCounter.count(stmt.get(), nextFrame);
    }

    emit_line("push {}", count);
    emit_line("oframe");

    mFrameDepth++;

    RefStack copy = mStack;

    mStack.pushFrame(count);

    if (stmt->decl) {
        stmt->decl->accept(this);
    }

    size_t condSize =
        mIRCounter.count(stmt->cond.get(), mStack);

    stmt->cond->accept(this);

    size_t forSize = 0;

    forSize += mIRCounter.count(stmt.get(), mStack);

    size_t assignSize =
        mIRCounter.count(stmt->assignment.get(), mStack);

    emit_line("not");
    emit_line(
        "push #PC+{}",
        1 + forSize + assignSize + 2 + 1
    );
    emit_line("cjmp");

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    stmt->assignment->accept(this);

    emit_line(
        "push #PC-{}",
        assignSize + forSize + 3 + condSize
    );
    emit_line("jmp");

    mStack.popFrame();

    mFrameDepth--;

    emit_line("cframe");
}

void GenVisitor::visit(core::WhileStmt *stmt) {
    size_t condSize =
        mIRCounter.count(stmt->cond.get(), mStack);

    stmt->cond->accept(this);

    size_t whileSize =
        mIRCounter.count(stmt->block.get(), mStack);

    emit_line("not");
    emit_line("push #PC+{}", 1 + whileSize + 2 + 1);
    emit_line("cjmp");

    stmt->block->accept(this);

    emit_line("push #PC-{}", whileSize + 3 + condSize);
    emit_line("jmp");
}

void GenVisitor::visit(core::ReturnStmt *stmt) {
    stmt->expr->accept(this);

    for (size_t i = 0; i < mFrameDepth; i++) {
        emit_line("cframe");
    }

    // CHECK, if you need to add reta
    emit_line("ret");
}

void GenVisitor::visit(core::Program *prog) {
    auto itr = prog->stmts.begin();

    for (; itr != prog->stmts.end(); itr++) {
        if (isFunction.check(itr->get())) {
            (*itr)->accept(this);
        } else {
            break;
        }
    }

    size_t count = 0;

    auto itr_ = itr;

    for (; itr_ != prog->stmts.end(); itr_++) {
        count += mDeclCounter.count(
            itr_->get(),
            mStack.currentFrame()
        );
    }

    emit_line(".main");
    emit_line("push {}", count);
    emit_line("oframe");

    mFrameDepth++;

    mStack.currentFrame()->setSize(count);

    for (; itr != prog->stmts.end(); itr++) {
        core::abort_if(
            isFunction.check(itr->get()),
            "no function declaration allowed in .main"
        );

        (*itr)->accept(this);
    }

    mFrameDepth--;

    emit_line("cframe");
    emit_line("halt");
}

// the below is an example of program which does not
// properly close its scopes .test push 1 oframe push 44
// push 0
// push 0
// st
// push 1
// oframe
// push 20
// push 0
// push 0
// st
// push 10
// ret
// cframe
// push 20
// cframe
// ret
// .main
// push 0
// push .test
// call
// print
// push [0:0]
// print
// halt

void GenVisitor::print() {
    for (auto &line : mCode) {
        fmt::println(line);
    }
}

void GenVisitor::reset() {
    isFunction.reset();
    mIRCounter.reset();
    mDeclCounter.reset();
    mStack.reset();
    mCode.clear();
    mPC = 0;
    mFrameDepth = 0;
}

}  // namespace PArL
