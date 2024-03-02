#pragma once

// std
#include <iostream>
#include <memory>

// lox
#include <common/AST.hpp>

namespace Lox {

class ASTPrinter : ExprVisitor, StmtVisitor {
   public:
    void print(std::unique_ptr<Program>& program) {
        for (auto& stmt : program->stmts) {
            stringify(*stmt);
            std::cout << mStr << std::endl;
            mStr.clear();
        }
    }

   private:
    void visitBinaryExpr(Binary const* expr) override {
        mStr += "(" + expr->oper.getLexeme() + " ";
        stringify(*expr->left);
        mStr += " ";
        stringify(*expr->right);
        mStr += ")";
    }

    void visitUnaryExpr(Unary const* expr) override {
        mStr += "(" + expr->oper.getLexeme() + " ";
        stringify(*expr->expr);
        mStr += ")";
    }

    void visitLiteralExpr(Literal const* expr) override {
        mStr += expr->value.toString();
    }

    void visitVariableExpr(Variable const* expr) override {
        mStr += expr->name.getLexeme();
    }

    void visitGroupingExpr(Grouping const* expr) override {
        mStr += "(grouping ";
        stringify(*expr->expr);
        mStr += ")";
    }

    void visitPrintStmt(PrintStmt const* stmt) override {
        mStr += "(print ";
        stringify(*stmt->expr);
        mStr += ")";
    }

    void visitExprStmt(ExprStmt const* stmt) override {
        stringify(*stmt->expr);
    }

    void visitVarDecl(VarDecl const* stmt) override {
        mStr += "(let " + stmt->name.getLexeme() + " ";
        if (stmt->initializer == nullptr) {
            mStr += "nil";
        } else {
            stringify(*stmt->initializer);
        }
        mStr += ")";
    }

    void stringify(Stmt& stmt) {
        stmt.accept(this);
    }

    void stringify(Expr& expr) {
        expr.accept(this);
    }

    std::string mStr;
};

}  // namespace Lox
