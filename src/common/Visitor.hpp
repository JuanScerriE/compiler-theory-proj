#pragma once

// vought

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

template<typename T>
class Visitor {
   public:
    virtual T visitSubExpr(SubExpr *expr) = 0;
    virtual T visitBinary(Binary *expr) = 0;
    virtual T visitLiteral(Literal *expr) = 0;
    virtual T visitVariable(Variable *expr) = 0;
    virtual T visitUnary(Unary *expr) = 0;
    virtual T visitFunctionCall(FunctionCall *expr) = 0;
    virtual T visitBuiltinWidth(BuiltinWidth *expr) = 0;
    virtual T visitBuiltinHeight(
        BuiltinHeight *expr) = 0;
    virtual T visitBuiltinRead(BuiltinRead *expr) = 0;
    virtual T visitBuiltinRandomInt(
        BuiltinRandomInt *expr) = 0;

    virtual T visitPrintStmt(PrintStmt *stmt) = 0;
    virtual T visitDelayStmt(DelayStmt *stmt) = 0;
    virtual T visitWriteBoxStmt(WriteBoxStmt *stmt) = 0;
    virtual T visitWriteStmt(WriteStmt *stmt) = 0;
    virtual T visitClearStmt(ClearStmt *stmt) = 0;
    virtual T visitAssignment(Assignment *stmt) = 0;
    virtual T visitVariableDecl(VariableDecl *stmt) = 0;
    virtual T visitBlock(Block *stmt) = 0;
    virtual T visitIfStmt(IfStmt *stmt) = 0;
    virtual T visitForStmt(ForStmt *stmt) = 0;
    virtual T visitWhileStmt(WhileStmt *stmt) = 0;
    virtual T visitReturnStmt(ReturnStmt *stmt) = 0;

    // TODO: think about Formal Param might not be ideal
    // here or anywhere
    virtual T visitFormalParam(FormalParam *param) = 0;
    virtual T visitFunctionDecl(FunctionDecl *stmt) = 0;

    virtual T visitProgram(Program *prog) = 0;

    virtual void reset() = 0;

    virtual ~Visitor() = default;
};

}  // namespace PArL
