#pragma once

// std
#include <memory>
#include <optional>
#include <utility>
#include <vector>

// parl
#include <common/Token.hpp>
#include <common/Visitor.hpp>

namespace PArL {

class Node {
   public:
    virtual void accept(Visitor* visitor) = 0;

    virtual ~Node() = default;
};

class Expr : public Node {
   public:
    void accept(Visitor* visitor) override = 0;

    std::optional<Token> type{};
};

class SubExpr : public Expr {
   public:
    explicit SubExpr(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    std::unique_ptr<Expr> expr;
};

class Binary : public Expr {
   public:
    Binary(std::unique_ptr<Expr> left, Token oper,
           std::unique_ptr<Expr> right)
        : left(std::move(left)),
          oper(std::move(oper)),
          right(std::move(right)) {
    }

    void accept(Visitor* visitor) override;

    std::unique_ptr<Expr> left;
    Token oper;
    std::unique_ptr<Expr> right;
};

class Unary : public Expr {
   public:
    Unary(Token oper, std::unique_ptr<Expr> expr)
        : oper(std::move(oper)), expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    Token oper;
    std::unique_ptr<Expr> expr;
};

class FunctionCall : public Expr {
   public:
    FunctionCall(Token identifier,
                 std::vector<std::unique_ptr<Expr>> params)
        : identifier(std::move(identifier)),
          params(std::move(params)) {
    }

    void accept(Visitor* visitor) override;

    Token identifier;
    std::vector<std::unique_ptr<Expr>> params;
};

class Literal : public Expr {
   public:
    explicit Literal(Token value)
        : value(std::move(value)) {
    }

    void accept(Visitor* visitor) override;

    Token value;
};

class Variable : public Expr {
   public:
    explicit Variable(Token name) : name(std::move(name)) {
    }

    void accept(Visitor* visitor) override;

    Token name;
};

class BuiltinWidth : public Expr {
   public:
    explicit BuiltinWidth(Token token)
        : token(std::move(token)) {
    }

    Token token;
    void accept(Visitor* visitor) override;
};

class BuiltinHeight : public Expr {
   public:
    explicit BuiltinHeight(Token token)
        : token(std::move(token)) {
    }

    Token token;
    void accept(Visitor* visitor) override;
};

class BuiltinRead : public Expr {
   public:
    explicit BuiltinRead(Token token,
                         std::unique_ptr<Expr> x,
                         std::unique_ptr<Expr> y)
        : token(std::move(token)),
          x(std::move(x)),
          y(std::move(y)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
};

class BuiltinRandomInt : public Expr {
   public:
    explicit BuiltinRandomInt(Token token,
                              std::unique_ptr<Expr> max)
        : token(std::move(token)), max(std::move(max)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> max;
};

class Stmt : public Node {};

class VariableDecl : public Stmt {
   public:
    explicit VariableDecl(Token identifier, Token type,
                          std::unique_ptr<Expr> expr)
        : identifier(std::move(identifier)),
          type(std::move(type)),
          expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    Token identifier;
    Token type;
    std::unique_ptr<Expr> expr;
};

class Assignment : public Stmt {
   public:
    explicit Assignment(Token identifier,
                        std::unique_ptr<Expr> expr)
        : identifier(std::move(identifier)),
          expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    Token identifier;
    std::unique_ptr<Expr> expr;
};

class PrintStmt : public Stmt {
   public:
    explicit PrintStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    std::unique_ptr<Expr> expr;
};

class DelayStmt : public Stmt {
   public:
    explicit DelayStmt(Token token,
                       std::unique_ptr<Expr> expr)
        : token(std::move(token)), expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> expr;
};

class WriteBoxStmt : public Stmt {
   public:
    explicit WriteBoxStmt(Token token,
                          std::unique_ptr<Expr> xCoor,
                          std::unique_ptr<Expr> yCoor,
                          std::unique_ptr<Expr> xOffset,
                          std::unique_ptr<Expr> yOffset,
                          std::unique_ptr<Expr> color)
        : token(std::move(token)),
          xCoor(std::move(xCoor)),
          yCoor(std::move(yCoor)),
          xOffset(std::move(xOffset)),
          yOffset(std::move(yOffset)),
          color(std::move(color)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> xCoor;
    std::unique_ptr<Expr> yCoor;
    std::unique_ptr<Expr> xOffset;
    std::unique_ptr<Expr> yOffset;
    std::unique_ptr<Expr> color;
};

class WriteStmt : public Stmt {
   public:
    explicit WriteStmt(Token token,
                       std::unique_ptr<Expr> xCoor,
                       std::unique_ptr<Expr> yCoor,
                       std::unique_ptr<Expr> color)
        : token(std::move(token)),
          xCoor(std::move(xCoor)),
          yCoor(std::move(yCoor)),
          color(std::move(color)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> xCoor;
    std::unique_ptr<Expr> yCoor;
    std::unique_ptr<Expr> color;
};

class ClearStmt : public Stmt {
   public:
    explicit ClearStmt(Token token,
                       std::unique_ptr<Expr> color)
        : token(std::move(token)), color(std::move(color)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> color;
};

class Block : public Stmt {
   public:
    explicit Block(std::vector<std::unique_ptr<Stmt>> stmts)
        : stmts(std::move(stmts)) {
    }

    void accept(Visitor* visitor) override;

    std::vector<std::unique_ptr<Stmt>> stmts;
};

class FormalParam : public Node {
   public:
    explicit FormalParam(Token identifier, Token type)
        : identifier(std::move(identifier)),
          type(std::move(type)) {
    }

    void accept(Visitor* visitor) override;

    Token identifier;
    Token type;
};

class FunctionDecl : public Stmt {
   public:
    explicit FunctionDecl(
        Token identifier,
        std::vector<std::unique_ptr<FormalParam>> params,
        Token type, std::unique_ptr<Block> block)
        : identifier(std::move(identifier)),
          params(std::move(params)),
          type(std::move(type)),
          block(std::move(block)) {
    }

    void accept(Visitor* visitor) override;

    Token identifier;
    std::vector<std::unique_ptr<FormalParam>> params;
    Token type;
    std::unique_ptr<Block> block;
};

class IfStmt : public Stmt {
   public:
    explicit IfStmt(Token token, std::unique_ptr<Expr> expr,
                    std::unique_ptr<Block> ifThen,
                    std::unique_ptr<Block> ifElse)
        : token(std::move(token)),
          expr(std::move(expr)),
          ifThen(std::move(ifThen)),
          ifElse(std::move(ifElse)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> expr;
    std::unique_ptr<Block> ifThen;
    std::unique_ptr<Block> ifElse;
};

class ForStmt : public Stmt {
   public:
    explicit ForStmt(Token token,
                     std::unique_ptr<VariableDecl> varDecl,
                     std::unique_ptr<Expr> expr,
                     std::unique_ptr<Assignment> assignment,
                     std::unique_ptr<Block> block)
        : token(std::move(token)),
          varDecl(std::move(varDecl)),
          expr(std::move(expr)),
          assignment(std::move(assignment)),
          block(std::move(block)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<VariableDecl> varDecl;
    std::unique_ptr<Expr> expr;
    std::unique_ptr<Assignment> assignment;
    std::unique_ptr<Block> block;
};

class WhileStmt : public Stmt {
   public:
    explicit WhileStmt(Token token,
                       std::unique_ptr<Expr> expr,
                       std::unique_ptr<Block> block)
        : token(std::move(token)),
          expr(std::move(expr)),
          block(std::move(block)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> expr;
    std::unique_ptr<Block> block;
};

class ReturnStmt : public Stmt {
   public:
    explicit ReturnStmt(Token token,
                        std::unique_ptr<Expr> expr)
        : token(std::move(token)), expr(std::move(expr)) {
    }

    void accept(Visitor* visitor) override;

    Token token;
    std::unique_ptr<Expr> expr;
};

class Program : public Node {
   public:
    explicit Program(
        std::vector<std::unique_ptr<Stmt>> stmts)
        : stmts(std::move(stmts)) {
    }

    void accept(Visitor* visitor) override;

    std::vector<std::unique_ptr<Stmt>> stmts;
};

}  // namespace PArL
