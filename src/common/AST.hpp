#pragma once

// std
#include <memory>
#include <utility>

// lox
#include <common/Token.hpp>

namespace Vought {

class Visitor;

class Node {
   public:
    virtual void accept(Visitor *visitor) = 0;

    virtual ~Node() = default;
};

class Expr : Node {};

class Grouping : public Expr {
   public:
    explicit Grouping(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    std::unique_ptr<Expr> expr;
};

class Binary : public Expr {
   public:
    Binary(std::unique_ptr<Expr> left, Token const &oper,
           std::unique_ptr<Expr> right)
        : left(std::move(left)),
          oper(oper),
          right(std::move(right)) {
    }

    std::unique_ptr<Expr> left;
    Token oper;
    std::unique_ptr<Expr> right;
};

class Unary : public Expr {
   public:
    Unary(Token const &oper, std::unique_ptr<Expr> expr)
        : oper(oper), expr(std::move(expr)) {
    }

    Token oper;
    std::unique_ptr<Expr> expr;
};

class Literal : public Expr {
   public:
    explicit Literal(Value value)
        : value(std::move(value)) {
    }

    Value value;
};

class Variable : public Expr {
   public:
    explicit Variable(Token const &name) : name(name) {
    }

    Token name;
};

class Stmt : Node {};

class VarDecl : public Stmt {
   public:
    explicit VarDecl(const Token &name,
                     std::unique_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer)) {
    }

    Token name;
    std::unique_ptr<Expr> initializer;
};

class PrintStmt : public Stmt {
   public:
    explicit PrintStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    std::unique_ptr<Expr> expr;
};

class ExprStmt : public Stmt {
   public:
    explicit ExprStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
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

}  // namespace Vought
