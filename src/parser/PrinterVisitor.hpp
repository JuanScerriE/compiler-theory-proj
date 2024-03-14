#pragma once

// vought
#include <common/Visitor.hpp>

namespace Vought {

class PrinterVisitor : public Visitor {
public:
    void visitGroupingExpr(Grouping *expr) override;
    void visitBinaryExpr(Binary *expr) override;
    void visitLiteralExpr(Literal *expr) override;
    void visitVariableExpr(Variable *expr) override;
    void visitUnaryExpr(Unary *expr) override;

    // void visitPrintStmt(PrintStmt const *expr) override;
    // void visitExprStmt(ExprStmt const *expr) override;
    // void visitVarDecl(VarDecl const *expr) override;

    void reset() override;

private:
    int mNodeCount = 0;
    int mTabCount = 0;
};

} // namespace Vought
