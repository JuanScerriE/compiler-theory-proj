// vought
#include <common/AST.hpp>

namespace Vought {

void Grouping::accept(Visitor *visitor) {
    visitor->visitGroupingExpr(this);
}

void Binary::accept(Visitor *visitor) {
    visitor->visitBinaryExpr(this);
}

void Unary::accept(Visitor *visitor) {
    visitor->visitUnaryExpr(this);
}

void Literal::accept(Visitor *visitor) {
    visitor->visitLiteralExpr(this);
}

void Variable::accept(Visitor *visitor) {
    visitor->visitVariableExpr(this);
}

void VarDecl::accept(Visitor *visitor) {
    visitor->visitVarDecl(this);
}

void ExprStmt::accept(Visitor *visitor) {
    visitor->visitExprStmt(this);
}

void PrintStmt::accept(Visitor *visitor) {
    visitor->visitPrintStmt(this);
}

} // namespace Vought
