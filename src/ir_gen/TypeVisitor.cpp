#include <ir_gen/TypeVisitor.hpp>

namespace PArL {

void TypeVisitor::visit(core::Type *type) {
    if (!type->isArray) {
        mReturn = type->base;

        return;
    }

    core::abort_if(
        !type->size,
        "emtpy array index cannot be present"
    );

    int value = type->size->value;

    core::abort_if(
        value < 1,
        "array size must be positive"
    );

    mReturn = core::Array{
        static_cast<size_t>(value),
        core::box{core::Primitive{type->base}}
    };
}

void TypeVisitor::visit(core::Expr *expr) {
    if (expr->type.has_value()) {
        (*expr->type)->accept(this);
    }
}

void TypeVisitor::visit(core::PadWidth *expr) {
    mReturn = core::Base::INT;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::PadHeight *expr) {
    mReturn = core::Base::INT;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::PadRead *expr) {
    mReturn = core::Base::INT;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::PadRandomInt *expr) {
    mReturn = core::Base::INT;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::BooleanLiteral *expr) {
    mReturn = core::Base::BOOL;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::IntegerLiteral *expr) {
    mReturn = core::Base::INT;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::FloatLiteral *expr) {
    mReturn = core::Base::FLOAT;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::ColorLiteral *expr) {
    mReturn = core::Base::COLOR;
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::ArrayLiteral *expr) {
    // NOTE: we only need to check the first element
    // since the rest are guaranteed to be all the
    // same type due to semantic analysis
    expr->exprs[0]->accept(this);

    mReturn =
        core::Array{expr->exprs.size(), core::box{mReturn}};
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::Variable *expr) {
    Environment *scope = mStack.currentFrame();

    Environment *terminatingScope = scope;

    while (terminatingScope->getType() !=
               Environment::Type::GLOBAL &&
           terminatingScope->getType() !=
               Environment::Type::FUNCTION) {
        terminatingScope = terminatingScope->getEnclosing();
    }

    std::optional<Symbol> symbol{};

    for (;;) {
        symbol = scope->findSymbol(expr->identifier);

        if (symbol.has_value() || scope == terminatingScope)
            break;

        scope = scope->getEnclosing();
    }

    core::abort_if(
        !symbol.has_value(),
        "symbol is undefined"
    );

    mReturn = symbol->as<VariableSymbol>()->type;

    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::ArrayAccess *expr) {
    Environment *scope = mStack.currentFrame();

    Environment *terminatingScope = scope;

    while (terminatingScope->getType() !=
               Environment::Type::GLOBAL &&
           terminatingScope->getType() !=
               Environment::Type::FUNCTION) {
        terminatingScope = terminatingScope->getEnclosing();
    }

    std::optional<Symbol> symbol{};

    for (;;) {
        symbol = scope->findSymbol(expr->identifier);

        if (symbol.has_value() || scope == terminatingScope)
            break;

        scope = scope->getEnclosing();
    }

    core::abort_if(
        !symbol.has_value(),
        "symbol is undefined"
    );

    mReturn = *symbol->as<VariableSymbol>()
                   ->type.as<core::Array>()
                   .type;

    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::FunctionCall *expr) {
    Environment *scope = mStack.currentFrame();

    std::optional<Symbol> symbol{};

    for (;;) {
        symbol = scope->findSymbol(expr->identifier);

        scope = scope->getEnclosing();

        if (symbol.has_value() || scope == nullptr)
            break;
    }

    core::abort_if(
        !symbol.has_value(),
        "symbol is undefined"
    );

    mReturn = symbol->as<FunctionSymbol>()->returnType;

    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::SubExpr *expr) {
    expr->subExpr->accept(this);
    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::Binary *expr) {
    // NOTE: the type of both left and right expressions are
    // the same
    expr->right->accept(this);
    expr->left->accept(this);

    switch (expr->op) {
        case core::Operation::AND:
        case core::Operation::OR:
            // mReturn remains the same
            break;
        case core::Operation::LT:
        case core::Operation::GT:
        case core::Operation::EQ:
        case core::Operation::NEQ:
        case core::Operation::LE:
        case core::Operation::GE:
            mReturn = core::Base::BOOL;
            break;
        case core::Operation::ADD:
        case core::Operation::SUB:
        case core::Operation::MUL:
        case core::Operation::DIV:
            // mReturn remains the same
            break;
        default:
            core::abort("unreachable");
    }

    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::Unary *expr) {
    expr->expr->accept(this);

    switch (expr->op) {
        case core::Operation::NOT:
            // mReturn remains the same
            break;
        case core::Operation::SUB:
            // mReturn remains the same
            break;
        default:
            core::abort("unreachable");
    }

    expr->core::Expr::accept(this);
}

void TypeVisitor::visit(core::Assignment *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::VariableDecl *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::PrintStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::DelayStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::WriteBoxStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::WriteStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::ClearStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::Block *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::FormalParam *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::FunctionDecl *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::IfStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::ForStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::WhileStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::ReturnStmt *) {
    core::abort("unimplemented");
}
void TypeVisitor::visit(core::Program *) {
    core::abort("unimplemented");
}

void TypeVisitor::reset() {
    mStack.reset();

    mReturn = std::monostate{};
}

core::Primitive
TypeVisitor::getType(core::Node *node, Environment *env) {
    mStack.init(nullptr, env);

    node->accept(this);

    auto result = mReturn;

    reset();

    return result;
}

}  // namespace PArL