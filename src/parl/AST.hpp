#pragma once

// parl
#include <parl/Core.hpp>
#include <parl/Visitor.hpp>

// std
#include <memory>
#include <optional>
#include <vector>

namespace PArL::core {

struct Node {
    virtual void accept(Visitor*) = 0;

    virtual ~Node() = default;
};

struct Type : public Node {
    explicit Type(const Position&, const Base&, bool, std::unique_ptr<IntegerLiteral>);

    void accept(Visitor*) override;

    const Position position;
    const Base base;
    const bool isArray;
    std::unique_ptr<IntegerLiteral> size;
};

struct Expr : public Node {
    void accept(Visitor*) override;

    std::optional<std::unique_ptr<Type>> type{};
};

struct Literal : public Expr {};

struct PadWidth : public Literal {
    explicit PadWidth(const Position&);

    void accept(Visitor*) override;

    const Position position;
};

struct PadHeight : public Literal {
    explicit PadHeight(const Position&);

    void accept(Visitor*) override;

    const Position position;
};

struct PadRead : public Literal {
    explicit PadRead(const Position&, std::unique_ptr<Expr>, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
};

struct PadRandomInt : public Literal {
    explicit PadRandomInt(const Position&, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> max;
};

struct BooleanLiteral : public Literal {
    explicit BooleanLiteral(const Position&, bool);

    void accept(Visitor*) override;

    const Position position;
    const bool value;
};

struct IntegerLiteral : public Literal {
    explicit IntegerLiteral(const Position&, int);

    void accept(Visitor*) override;

    const Position position;
    const int value;
};

struct FloatLiteral : public Literal {
    explicit FloatLiteral(const Position&, float);

    void accept(Visitor*) override;

    const Position position;
    const float value;
};

struct ColorLiteral : public Literal {
    explicit ColorLiteral(const Position&, const Color&);

    void accept(Visitor*) override;

    const Position position;
    const Color value;
};

struct ArrayLiteral : public Literal {
    explicit ArrayLiteral(const Position&, std::vector<std::unique_ptr<Expr>>);

    void accept(Visitor*) override;

    const Position position;
    std::vector<std::unique_ptr<Expr>> exprs;
};

struct Reference : public Expr {};

struct Variable : public Reference {
    explicit Variable(const Position&, std::string);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
};

struct ArrayAccess : public Reference {
    explicit ArrayAccess(const Position&, std::string, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
    std::unique_ptr<Expr> index;
};

struct FunctionCall : public Reference {
    explicit FunctionCall(const Position&, std::string, std::vector<std::unique_ptr<Expr>>);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
    std::vector<std::unique_ptr<Expr>> params;
};

struct SubExpr : public Expr {
    explicit SubExpr(std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    std::unique_ptr<Expr> subExpr;
};

struct Binary : public Expr {
    explicit Binary(const Position&, std::unique_ptr<Expr>, Operation, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> left;
    const Operation op;
    std::unique_ptr<Expr> right;
};

struct Unary : public Expr {
    explicit Unary(const Position&, Operation, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    const Operation op;
    std::unique_ptr<Expr> expr;
};

struct Stmt : public Node {};

struct Assignment : public Stmt {
    explicit Assignment(const Position&, std::string, std::unique_ptr<Expr>, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
    std::unique_ptr<Expr> index;
    std::unique_ptr<Expr> expr;
};

struct VariableDecl : public Stmt {
    explicit VariableDecl(const Position&, std::string, std::unique_ptr<Type>, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expr> expr;
};

struct PrintStmt : public Stmt {
    explicit PrintStmt(std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    std::unique_ptr<Expr> expr;
};

struct DelayStmt : public Stmt {
    explicit DelayStmt(const Position&, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> expr;
};

struct WriteBoxStmt : public Stmt {
    explicit WriteBoxStmt(const Position&, std::unique_ptr<Expr>, std::unique_ptr<Expr>, std::unique_ptr<Expr>, std::unique_ptr<Expr>, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
    std::unique_ptr<Expr> w;
    std::unique_ptr<Expr> h;
    std::unique_ptr<Expr> color;
};

struct WriteStmt : public Stmt {
    explicit WriteStmt(const Position&, std::unique_ptr<Expr>, std::unique_ptr<Expr>, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> x;
    std::unique_ptr<Expr> y;
    std::unique_ptr<Expr> color;
};

struct ClearStmt : public Stmt {
    explicit ClearStmt(const Position&, std::unique_ptr<Expr>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> color;
};

struct Block : public Stmt {
    explicit Block(std::vector<std::unique_ptr<Stmt>>);

    void accept(Visitor*) override;

    std::vector<std::unique_ptr<Stmt>> stmts;
};

struct FormalParam : public Node {
    explicit FormalParam(const Position&, std::string, std::unique_ptr<Type>);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
    std::unique_ptr<Type> type;
};

struct FunctionDecl : public Stmt {
    explicit FunctionDecl(const Position&, std::string, std::vector<std::unique_ptr<FormalParam>>, std::unique_ptr<Type>, std::unique_ptr<Block>);

    void accept(Visitor*) override;

    const Position position;
    const std::string identifier;
    std::vector<std::unique_ptr<FormalParam>> params;
    std::unique_ptr<Type> type;
    std::unique_ptr<Block> block;
};

struct IfStmt : public Stmt {
    explicit IfStmt(const Position&, std::unique_ptr<Expr>, std::unique_ptr<Block>, std::unique_ptr<Block>);

    void accept(Visitor*) override;

    const Position position;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Block> thenBlock;
    std::unique_ptr<Block> elseBlock;
};

struct ForStmt : public Stmt {
    explicit ForStmt(const Position&, std::unique_ptr<VariableDecl>, std::unique_ptr<Expr>, std::unique_ptr<Assignment>, std::unique_ptr<Block>);

    void accept(Visitor* visitor) override;

    const Position position;
    std::unique_ptr<VariableDecl> decl;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Assignment> assignment;
    std::unique_ptr<Block> block;
};

struct WhileStmt : public Stmt {
    explicit WhileStmt(const Position&, std::unique_ptr<Expr>, std::unique_ptr<Block>);

    void accept(Visitor* visitor) override;

    const Position position;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Block> block;
};

struct ReturnStmt : public Stmt {
    explicit ReturnStmt(const Position&, std::unique_ptr<Expr>);

    void accept(Visitor* visitor) override;

    const Position position;
    std::unique_ptr<Expr> expr;
};

struct Program : public Node {
    explicit Program(std::vector<std::unique_ptr<Stmt>>);

    void accept(Visitor* visitor) override;

    std::vector<std::unique_ptr<Stmt>> stmts;
};

}  // namespace PArL::core
