// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <common/AST.hpp>
#include <analysis/AnalysisVisitor.hpp>
#include <analysis/Environment.hpp>

// std
#include <memory>

namespace Vought {

void AnalysisVisitor::analyse(Program *prog) {
    prog->accept(this);
}

void AnalysisVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);
}

void AnalysisVisitor::visitBinary(Binary *expr) {
}

void AnalysisVisitor::visitLiteral(Literal *expr) {
}

void AnalysisVisitor::visitVariable(Variable *expr) {
}

void AnalysisVisitor::visitUnary(Unary *expr) {
}

void AnalysisVisitor::visitFunctionCall(FunctionCall *expr) {
}

void AnalysisVisitor::visitBuiltinWidth(BuiltinWidth *expr) {
}

void AnalysisVisitor::visitBuiltinHeight(
    BuiltinHeight *expr) {
}

void AnalysisVisitor::visitBuiltinRead(BuiltinRead *expr) {
}

void AnalysisVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr) {
}

void AnalysisVisitor::visitPrintStmt(PrintStmt *stmt) {
}

void AnalysisVisitor::visitDelayStmt(DelayStmt *stmt) {
}

void AnalysisVisitor::visitWriteBoxStmt(WriteBoxStmt *stmt) {
}

void AnalysisVisitor::visitWriteStmt(WriteStmt *stmt) {
}

void AnalysisVisitor::visitAssignment(Assignment *stmt) {
}

void AnalysisVisitor::visitVariableDecl(VariableDecl *stmt) {
}

void AnalysisVisitor::visitBlock(Block *stmt) {
}

void AnalysisVisitor::visitIfStmt(IfStmt *stmt) {
}

void AnalysisVisitor::visitForStmt(ForStmt *stmt) {
}

void AnalysisVisitor::visitWhileStmt(WhileStmt *stmt) {
}

void AnalysisVisitor::visitReturnStmt(ReturnStmt *stmt) {
}

void AnalysisVisitor::visitFormalParam(FormalParam *param) {
}

void AnalysisVisitor::visitFunctionDecl(FunctionDecl *stmt) {
}

void AnalysisVisitor::visitProgram(Program *prog) {
}

void AnalysisVisitor::reset() {
}

}  // namespace Vought
