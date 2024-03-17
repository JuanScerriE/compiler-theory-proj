
// std
#include <algorithm>
#include <iostream>
#include <memory>

// lox
#include <common/AST.hpp>
#include <errors/ParsingError.hpp>
#include <parser/Parser.hpp>
#include <utility>

namespace Vought {

Parser::Parser(std::vector<Token> &tokens)
    : mTokens(tokens) {
}

void Parser::parse() {
    mAST = program();
}

std::unique_ptr<Program> Parser::getAST() {
    return std::move(mAST);
}

std::unique_ptr<Program> Parser::program() {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!isAtEnd()) {
        stmts.push_back(declaration());
    }

    return std::make_unique<Program>(std::move(stmts));
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({Token::Type::VAR})) {
            return variableDeclaration();
        }

        return statement();
    } catch (ParsingError &error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::variableDeclaration() {
    Token name = consume(Token::Type::IDENTIFIER,
                         "Expect a variable name.");

    std::unique_ptr<Expr> initializer = nullptr;
    if (match({Token::Type::EQUAL})) {
        initializer = expression();
    }

    consume(Token::Type::SEMICOLON,
            "Expect ';' after variable declaration");
    return std::make_unique<VarDecl>(
        name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({Token::Type::PRINT})) {
        return printStatement();
    }

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::printStatement() {
    std::unique_ptr<Expr> expr = expression();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::expression() {
    return equality();
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();

    while (match({Token::Type::BANG_EQUAL,
                  Token::Type::EQUAL_EQUAL})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = comparison();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match(
        {Token::Type::GREATER, Token::Type::GREATER_EQUAL,
         Token::Type::LESS, Token::Type::LESS_EQUAL})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = term();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({Token::Type::MINUS, Token::Type::PLUS})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = factor();

        expr = std::make_unique<Binary>(std::move(expr),
                                        std::move(oper),
                                        std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({Token::Type::SLASH, Token::Type::STAR})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = unary();

        expr = std::make_unique<Binary>(std::move(expr),
                                        std::move(oper),
                                        std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    while (match({Token::Type::BANG, Token::Type::MINUS})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = unary();

        return std::make_unique<Unary>(std::move(oper),
                                       std::move(right));
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({Token::Type::FALSE, Token::Type::TRUE,
               Token::Type::NUMBER, Token::Type::STRING})) {
        return std::make_unique<Literal>(
            previous().getLiteral());
    }

    if (match({Token::Type::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();

        consume(Token::Type::RIGHT_PAREN,
                "Expect ')' after expression.");

        return std::make_unique<Grouping>(std::move(expr));
    }

    if (match({Token::Type::IDENTIFIER})) {
        return std::make_unique<Variable>(previous());
    }

    throw ParsingError(peek(), "Expect expression.");
}

Token Parser::consume(Token::Type type,
                      std::string message) {
    if (check(type)) {
        return advance();
    }

    throw ParsingError(peek(), std::move(message));
}

bool Parser::match(const std::vector<Token::Type> &types) {
    for (Token::Type type : types) {
        if (check(type)) {
            advance();

            return true;
        }
    }

    return false;
}

bool Parser::check(Token::Type type) {
    if (isAtEnd()) {
        return false;
    }

    return peek().getType() == type;
}

Token Parser::advance() {
    if (!isAtEnd()) {
        mCurrent++;
    }

    return previous();
}

bool Parser::isAtEnd() {
    return peek().getType() == Token::Type::END_OF_FILE;
}

// TODO: For now this is fine but this is unlikely to be
// correct
Token &Parser::peek() {
    return mTokens[mCurrent];
}

Token Parser::previous() {
    return mTokens[mCurrent - 1];
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().getType() == Token::Type::SEMICOLON)
            return;

        switch (peek().getType()) {
            case Token::Type::CLASS:
            case Token::Type::FUN:
            case Token::Type::VAR:
            case Token::Type::FOR:
            case Token::Type::IF:
            case Token::Type::WHILE:
            case Token::Type::PRINT:
            case Token::Type::RETURN:
                return;
            default:;  // Do nothing
        }

        advance();
    }
}

}  // namespace Vought
