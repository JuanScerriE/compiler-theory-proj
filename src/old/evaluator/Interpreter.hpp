#pragma once

#include <common/AST.hpp>
#include <common/Value.hpp>

namespace Lox {

class Interpreter : ExprVisitor, StmtVisitor {
   public:
    void interpret(std::unique_ptr<Program> &program);

   private:
    void execute(Stmt *stmt);
    void eval(Expr *expr);
    void visitVarDecl(VarDecl const *expr) override;
    void visitExprStmt(ExprStmt const *expr) override;
    void visitPrintStmt(PrintStmt const *expr) override;

    void visitVariableExpr(Variable const *expr) override;
    void visitLiteralExpr(Literal const *expr) override;
    void visitGroupingExpr(Grouping const *expr) override;
    void visitBinaryExpr(Binary const *expr) override;
    void visitUnaryExpr(Unary const *expr) override;

    static bool isTruthy(Value &value);
    static bool isEqual(Value &value1, Value &value2);
    static void checkNumberOperand(
        Token const &exprOperator, Value const &operand);
    static void checkNumberOperands(
        Token const &exprOperator, Value const &left,
        Value const &right);

    Value mExprResult;
};

}  // namespace Lox
