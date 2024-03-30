// parl
#include <ir_gen/VarDeclCountVisitor.hpp>

namespace PArL {

void VarDeclCountVisitor::visit(core::Type *) {
}

void VarDeclCountVisitor::visit(core::Expr *) {
}

void VarDeclCountVisitor::visit(core::PadWidth *) {
}

void VarDeclCountVisitor::visit(core::PadHeight *) {
}

void VarDeclCountVisitor::visit(core::PadRead *) {
}

void VarDeclCountVisitor::visit(core::PadRandomInt *) {
}

void VarDeclCountVisitor::visit(core::BooleanLiteral *) {
}

void VarDeclCountVisitor::visit(core::IntegerLiteral *) {
}

void VarDeclCountVisitor::visit(core::FloatLiteral *) {
}

void VarDeclCountVisitor::visit(core::ColorLiteral *) {
}

void VarDeclCountVisitor::visit(core::ArrayLiteral *) {
}

void VarDeclCountVisitor::visit(core::Variable *) {
}

void VarDeclCountVisitor::visit(core::ArrayAccess *) {
}

void VarDeclCountVisitor::visit(core::FunctionCall *) {
}

void VarDeclCountVisitor::visit(core::SubExpr *) {
}

void VarDeclCountVisitor::visit(core::Binary *) {
}

void VarDeclCountVisitor::visit(core::Unary *) {
}

void VarDeclCountVisitor::visit(core::Assignment *) {
}

void VarDeclCountVisitor::visit(core::VariableDecl *stmt) {
    std::optional<Symbol> symbol =
        mEnv->findSymbol(stmt->identifier);

    core::abort_if(
        !symbol.has_value(),
        "symbol is undefined"
    );

    auto &variable = symbol->asRef<VariableSymbol>();

    if (variable.type.is<core::Array>()) {
        mCount += variable.type.as<core::Array>().size;
    } else {
        mCount++;
    }
}

void VarDeclCountVisitor::visit(core::PrintStmt *) {
}

void VarDeclCountVisitor::visit(core::DelayStmt *) {
}

void VarDeclCountVisitor::visit(core::WriteBoxStmt *) {
}

void VarDeclCountVisitor::visit(core::WriteStmt *) {
}

void VarDeclCountVisitor::visit(core::ClearStmt *) {
}

void VarDeclCountVisitor::visit(core::Block *) {
}

void VarDeclCountVisitor::visit(core::FormalParam *param) {
    std::optional<Symbol> symbol =
        mEnv->findSymbol(param->identifier);

    core::abort_if(
        !symbol.has_value(),
        "symbol is undefined"
    );

    auto &variable = symbol->asRef<VariableSymbol>();

    if (variable.type.is<core::Array>()) {
        mCount += variable.type.as<core::Array>().size;
    } else {
        mCount++;
    }
}

void VarDeclCountVisitor::visit(core::FunctionDecl *) {
}

void VarDeclCountVisitor::visit(core::IfStmt *) {
}

void VarDeclCountVisitor::visit(core::ForStmt *) {
}

void VarDeclCountVisitor::visit(core::WhileStmt *) {
}

void VarDeclCountVisitor::visit(core::ReturnStmt *) {
}

void VarDeclCountVisitor::visit(core::Program *) {
}

void VarDeclCountVisitor::reset() {
    mCount = 0;
}

size_t VarDeclCountVisitor::count(
    core::Node *node,
    Environment *env
) {
    mEnv = env;

    node->accept(this);

    size_t result = mCount;

    reset();

    return result;
}

}  // namespace PArL
