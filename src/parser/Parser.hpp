#pragma once

// std
#include <exception>
#include <memory>
#include <stdexcept>
#include <vector>

// lox
#include <common/AST.hpp>
#include <common/Token.hpp>

namespace Vought {

class Parser {
   public:
    explicit Parser(std::vector<Token>& tokens);

    void parse();
    std::unique_ptr<Program> getAST();

   private:
    std::unique_ptr<Program> program();

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> variableDeclaration();

    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> printStatement();

    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();

    bool match(const std::vector<Token::Type>& types);
    Token consume(Token::Type type, std::string message);
    bool check(Token::Type type);
    Token advance();
    bool isAtEnd();
    Token& peek();
    Token previous();
    void synchronize();

    std::vector<Token>& mTokens;

    std::unique_ptr<Program> mAST;

    int mCurrent = 0;
};

}  // namespace Vought
