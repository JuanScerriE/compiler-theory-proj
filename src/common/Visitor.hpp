#pragma once

// vought
#include <common/AST.hpp>

namespace Vought {

class Visitor {
   public:
    virtual void visitGroupingExpr(Grouping *expr) = 0;
    virtual void visitBinaryExpr(Binary *expr) = 0;
    virtual void visitLiteralExpr(Literal *expr) = 0;
    virtual void visitVariableExpr(Variable *expr) = 0;
    virtual void visitUnaryExpr(Unary *expr) = 0;

    // virtual void visitPrintStmt(PrintStmt const *expr) = 0;
    // virtual void visitExprStmt(ExprStmt const *expr) = 0;
    // virtual void visitVarDecl(VarDecl const *expr) = 0;

    virtual void reset() = 0;
};

} // namespace Vought
