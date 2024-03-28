// parl
#include <ir_gen/VarDeclCountVisitor.hpp>

namespace PArL {

void VarDeclCountVisitor::visitSubExpr(SubExpr *) {
}

void VarDeclCountVisitor::visitBinary(Binary *) {
}

void VarDeclCountVisitor::visitLiteral(Literal *) {
}

void VarDeclCountVisitor::visitVariable(Variable *) {
}

void VarDeclCountVisitor::visitUnary(Unary *) {
}

void VarDeclCountVisitor::visitFunctionCall(FunctionCall
                                                *) {
}

void VarDeclCountVisitor::visitBuiltinWidth(PadWidth *) {
}

void VarDeclCountVisitor::visitBuiltinHeight(PadHeight *) {
}

void VarDeclCountVisitor::visitBuiltinRead(PadRead *) {
}

void VarDeclCountVisitor::visitBuiltinRandomInt(PadRandomInt
                                                    *) {
}

void VarDeclCountVisitor::visitPrintStmt(PrintStmt *) {
}

void VarDeclCountVisitor::visitDelayStmt(DelayStmt *) {
}

void VarDeclCountVisitor::visitWriteBoxStmt(WriteBoxStmt
                                                *) {
}

void VarDeclCountVisitor::visitWriteStmt(WriteStmt *) {
}

void VarDeclCountVisitor::visitClearStmt(ClearStmt *) {
}

void VarDeclCountVisitor::visitAssignment(Assignment *) {
}

void VarDeclCountVisitor::visitVariableDecl(VariableDecl
                                                *) {
    mCount++;
}

void VarDeclCountVisitor::visitBlock(Block *) {
}

void VarDeclCountVisitor::visitIfStmt(IfStmt *) {
}

void VarDeclCountVisitor::visitForStmt(ForStmt *) {
}

void VarDeclCountVisitor::visitWhileStmt(WhileStmt *) {
}

void VarDeclCountVisitor::visitReturnStmt(ReturnStmt *) {
}

void VarDeclCountVisitor::visitFormalParam(FormalParam *) {
    mCount++;
}

void VarDeclCountVisitor::visitFunctionDecl(FunctionDecl
                                                *) {
}

void VarDeclCountVisitor::visitProgram(Program *) {
}

void VarDeclCountVisitor::reset() {
    mCount = 0;
}

size_t VarDeclCountVisitor::count(Node *node) {
    node->accept(this);

    bool result = mCount;

    reset();

    return result;
}

}  // namespace PArL
