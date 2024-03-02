#pragma once

// std
#include <algorithm>
#include <any>
#include <memory>
#include <utility>

// lox
#include <common/Token.hpp>

namespace Lox {

class Binary;
class Grouping;
class Literal;
class Variable;
class Unary;

class ExprVisitor {
   public:
    virtual void visitGroupingExpr(
        Grouping const *expr) = 0;
    virtual void visitBinaryExpr(Binary const *expr) = 0;
    virtual void visitLiteralExpr(Literal const *expr) = 0;
    virtual void visitVariableExpr(
        Variable const *expr) = 0;
    virtual void visitUnaryExpr(Unary const *expr) = 0;
};

class Expr {
   public:
    virtual void accept(ExprVisitor *visitor) = 0;

    virtual ~Expr() = default;
};

class Grouping : public Expr {
   public:
    explicit Grouping(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    void accept(ExprVisitor *visitor) override {
        visitor->visitGroupingExpr(this);
    }

    std::unique_ptr<Expr> expr;
};

class Binary : public Expr {
   public:
    Binary(std::unique_ptr<Expr> left, const Token &oper,
           std::unique_ptr<Expr> right)
        : left(std::move(left)),
          oper(oper),
          right(std::move(right)) {
    }

    void accept(ExprVisitor *visitor) override {
        visitor->visitBinaryExpr(this);
    }

    std::unique_ptr<Expr> left;
    Token oper;
    std::unique_ptr<Expr> right;
};

class Unary : public Expr {
   public:
    Unary(const Token &oper, std::unique_ptr<Expr> expr)
        : oper(oper), expr(std::move(expr)) {
    }

    void accept(ExprVisitor *visitor) override {
        visitor->visitUnaryExpr(this);
    }

    Token oper;
    std::unique_ptr<Expr> expr;
};

class Literal : public Expr {
   public:
    explicit Literal(Value value)
        : value(std::move(value)) {
    }

    void accept(ExprVisitor *visitor) override {
        visitor->visitLiteralExpr(this);
    }

    Value value;
};

class Variable : public Expr {
   public:
    explicit Variable(Token const &name) : name(name) {
    }

    void accept(ExprVisitor *visitor) override {
        visitor->visitVariableExpr(this);
    }

    Token name;
};

class PrintStmt;
class ExprStmt;
class VarDecl;

class StmtVisitor {
   public:
    virtual void visitPrintStmt(PrintStmt const *expr) = 0;
    virtual void visitExprStmt(ExprStmt const *expr) = 0;
    virtual void visitVarDecl(VarDecl const *expr) = 0;
};

class Stmt {
   public:
    virtual void accept(StmtVisitor *visitor) = 0;
    virtual ~Stmt() = default;
};

class VarDecl : public Stmt {
   public:
    explicit VarDecl(const Token &name,
                     std::unique_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer)) {
    }

    void accept(StmtVisitor *visitor) override {
        visitor->visitVarDecl(this);
    }

    Token name;
    std::unique_ptr<Expr> initializer;
};

class PrintStmt : public Stmt {
   public:
    explicit PrintStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    void accept(StmtVisitor *visitor) override {
        visitor->visitPrintStmt(this);
    }

    std::unique_ptr<Expr> expr;
};

class ExprStmt : public Stmt {
   public:
    explicit ExprStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    void accept(StmtVisitor *visitor) override {
        visitor->visitExprStmt(this);
    }

    std::unique_ptr<Expr> expr;
};

class Program {
   public:
    explicit Program(
        std::vector<std::unique_ptr<Stmt>> stmts)
        : stmts(std::move(stmts)) {
    }

    std::vector<std::unique_ptr<Stmt>> stmts;
};

}  // namespace Lox
