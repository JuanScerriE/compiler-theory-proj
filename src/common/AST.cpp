// vought
#include <common/AST.hpp>

namespace Vought {

void SubExpr::accept(Visitor *visitor) {
    visitor->visitSubExpr(this);
}

void Binary::accept(Visitor *visitor) {
    visitor->visitBinary(this);
}

void Literal::accept(Visitor *visitor) {
    visitor->visitLiteral(this);
}

void Variable::accept(Visitor *visitor) {
    visitor->visitVariable(this);
}

void FunctionCall::accept(Visitor *visitor) {
    visitor->visitFunctionCall(this);
}

void BuiltinWidth::accept(Visitor *visitor) {
    visitor->visitBuiltinWidth(this);
}

void BuiltinHeight::accept(Visitor *visitor) {
    visitor->visitBuiltinHeight(this);
}

void BuiltinRead::accept(Visitor *visitor) {
    visitor->visitBuiltinRead(this);
}

void BuiltinRandomInt::accept(Visitor *visitor) {
    visitor->visitBuiltinRandomInt(this);
}

void PrintStmt::accept(Visitor *visitor) {
    visitor->visitPrintStmt(this);
}

void DelayStmt::accept(Visitor *visitor) {
    visitor->visitDelayStmt(this);
}

void WriteBoxStmt::accept(Visitor *visitor) {
    visitor->visitWriteBoxStmt(this);
}

void WriteStmt::accept(Visitor *visitor) {
    visitor->visitWriteStmt(this);
}

void Assignment::accept(Visitor *visitor) {
    visitor->visitAssignment(this);
}

void VariableDecl::accept(Visitor *visitor) {
    visitor->visitVariableDecl(this);
}

void Block::accept(Visitor *visitor) {
    visitor->visitBlock(this);
}


void IfStmt::accept(Visitor *visitor) {
    visitor->visitIfStmt(this);
}


void ForStmt::accept(Visitor *visitor) {
    visitor->visitForStmt(this);
}


void WhileStmt::accept(Visitor *visitor) {
    visitor->visitWhileStmt(this);
}


void ReturnStmt::accept(Visitor *visitor) {
    visitor->visitReturnStmt(this);
}

void FormalParam::accept(Visitor *visitor) {
    visitor->visitFormalParam(this);
}

void FunctionDecl::accept(Visitor *visitor) {
    visitor->visitFunctionDecl(this);
}

void Program::accept(Visitor *visitor) {
    visitor->visitProgram(this);
}

} // namespace Vought
