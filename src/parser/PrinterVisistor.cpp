// fmt
#include <fmt/core.h>

// vought
#include <parser/PrinterVisitor.hpp>

namespace Vought {

void PrinterVisitor::visitGroupingExpr(Grouping *expr) {
    expr->accept(this);
}

void PrinterVisitor::visitBinaryExpr(Binary *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Binary Operation {} =>", "",
                 mTabCount, expr->oper.getLexeme());
    mTabCount++;
    expr->left.get()->accept(this);
    expr->right.get()->accept(this);
    mTabCount--;
}
void PrinterVisitor::visitLiteralExpr(Literal *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Literal {} =>", "",
                 mTabCount, expr->value.toString());
}
void PrinterVisitor::visitVariableExpr(Variable *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Variable {}", "",
                 mTabCount, expr->name.getLexeme());
}
void PrinterVisitor::visitUnaryExpr(Unary *expr) {
    mNodeCount++;
    fmt::println("{:\t{}} Unary Operation {} =>", "",
                 mTabCount, expr->oper.getLexeme());
    mTabCount++;
    expr->expr.get()->accept(this);
    mTabCount--;
}

void PrinterVisitor::reset() {
    mNodeCount = 0;
    mTabCount = 0;
}

}  // namespace Vought
