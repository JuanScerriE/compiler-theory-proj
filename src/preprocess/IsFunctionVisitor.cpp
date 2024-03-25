// parl
#include <preprocess/IsFunctionVisitor.hpp>

namespace PArL {

void IsFunctionVisitor::visitSubExpr(SubExpr *) {
}

void IsFunctionVisitor::visitBinary(Binary *) {
}

void IsFunctionVisitor::visitLiteral(Literal *) {
}

void IsFunctionVisitor::visitVariable(Variable *) {
}

void IsFunctionVisitor::visitUnary(Unary *) {
}

void IsFunctionVisitor::visitFunctionCall(FunctionCall *) {
}

void IsFunctionVisitor::visitBuiltinWidth(BuiltinWidth *) {
}

void IsFunctionVisitor::visitBuiltinHeight(BuiltinHeight
                                               *) {
}

void IsFunctionVisitor::visitBuiltinRead(BuiltinRead *) {
}

void IsFunctionVisitor::
    visitBuiltinRandomInt(BuiltinRandomInt *) {
}

void IsFunctionVisitor::visitPrintStmt(PrintStmt *) {
}

void IsFunctionVisitor::visitDelayStmt(DelayStmt *) {
}

void IsFunctionVisitor::visitWriteBoxStmt(WriteBoxStmt *) {
}

void IsFunctionVisitor::visitWriteStmt(WriteStmt *) {
}

void IsFunctionVisitor::visitClearStmt(ClearStmt *) {
}

void IsFunctionVisitor::visitAssignment(Assignment *) {
}

void IsFunctionVisitor::visitVariableDecl(VariableDecl *) {
}

void IsFunctionVisitor::visitBlock(Block *) {
}

void IsFunctionVisitor::visitIfStmt(IfStmt *) {
}

void IsFunctionVisitor::visitForStmt(ForStmt *) {
}

void IsFunctionVisitor::visitWhileStmt(WhileStmt *) {
}

void IsFunctionVisitor::visitReturnStmt(ReturnStmt *) {
}

void IsFunctionVisitor::visitFormalParam(FormalParam *) {
}

void IsFunctionVisitor::visitFunctionDecl(FunctionDecl *) {
    mReturn = true;
}

void IsFunctionVisitor::visitProgram(Program *) {
}

void IsFunctionVisitor::reset() {
    mReturn = false;
}

bool IsFunctionVisitor::check(Node *node) {
    node->accept(this);

    bool result = mReturn;

    reset();

    return result;
}

}  // namespace PArL
