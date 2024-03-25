#pragma once

namespace PArL {

class Node;
class Expr;
class Stmt;
class SubExpr;
class Binary;
class Literal;
class Variable;
class Unary;
class FunctionCall;
class BuiltinWidth;
class BuiltinHeight;
class BuiltinRead;
class BuiltinRandomInt;
class PrintStmt;
class DelayStmt;
class WriteBoxStmt;
class WriteStmt;
class ClearStmt;
class Assignment;
class VariableDecl;
class Block;
class IfStmt;
class ForStmt;
class WhileStmt;
class ReturnStmt;
class FormalParam;
class FunctionDecl;
class Program;

class Visitor {
   public:
    virtual void visitSubExpr(SubExpr *expr) = 0;
    virtual void visitBinary(Binary *expr) = 0;
    virtual void visitLiteral(Literal *expr) = 0;
    virtual void visitVariable(Variable *expr) = 0;
    virtual void visitUnary(Unary *expr) = 0;
    virtual void visitFunctionCall(FunctionCall *expr) = 0;
    virtual void visitBuiltinWidth(BuiltinWidth *expr) = 0;
    virtual void visitBuiltinHeight(BuiltinHeight *expr
    ) = 0;
    virtual void visitBuiltinRead(BuiltinRead *expr) = 0;
    virtual void visitBuiltinRandomInt(
        BuiltinRandomInt *expr
    ) = 0;
    virtual void visitPrintStmt(PrintStmt *stmt) = 0;
    virtual void visitDelayStmt(DelayStmt *stmt) = 0;
    virtual void visitWriteBoxStmt(WriteBoxStmt *stmt) = 0;
    virtual void visitWriteStmt(WriteStmt *stmt) = 0;
    virtual void visitClearStmt(ClearStmt *stmt) = 0;
    virtual void visitAssignment(Assignment *stmt) = 0;
    virtual void visitVariableDecl(VariableDecl *stmt) = 0;
    virtual void visitBlock(Block *stmt) = 0;
    virtual void visitIfStmt(IfStmt *stmt) = 0;
    virtual void visitForStmt(ForStmt *stmt) = 0;
    virtual void visitWhileStmt(WhileStmt *stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt *stmt) = 0;

    // TODO: think about Formal Param might not be ideal
    // here or anywhere
    virtual void visitFormalParam(FormalParam *param) = 0;
    virtual void visitFunctionDecl(FunctionDecl *stmt) = 0;

    virtual void visitProgram(Program *prog) = 0;

    virtual void reset() = 0;

    virtual ~Visitor() = default;
};

}  // namespace PArL
