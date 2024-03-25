#pragma once

// std
#include <initializer_list>
#include <memory>

// parl
#include <common/AST.hpp>
#include <common/Token.hpp>
#include <lexer/Lexer.hpp>

// definitions
#define LOOKAHEAD (2)

namespace PArL {

class SyncParser : public std::exception {};

class Parser {
   public:
    explicit Parser(Lexer& lexer);

    [[nodiscard]] bool hasError() const;

    void parse(std::string const& source);

    std::unique_ptr<Program> getAst();

    void reset();

   private:
    std::unique_ptr<Expr> expr();
    std::unique_ptr<Expr> logicOr();
    std::unique_ptr<Expr> logicAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> primary();
    std::unique_ptr<Expr> subExpr();
    std::unique_ptr<Expr> functionCall();
    std::unique_ptr<BuiltinWidth> padWidth();
    std::unique_ptr<BuiltinHeight> padHeight();
    std::unique_ptr<BuiltinRead> padRead();
    std::unique_ptr<BuiltinRandomInt> padRandI();

    std::unique_ptr<Program> program();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Block> block();
    std::unique_ptr<VariableDecl> variableDecl();
    std::unique_ptr<Assignment> assignment();
    std::unique_ptr<PrintStmt> printStatement();
    std::unique_ptr<DelayStmt> delayStatement();
    std::unique_ptr<WriteBoxStmt> writeBoxStatement();
    std::unique_ptr<WriteStmt> writeStatement();
    std::unique_ptr<ClearStmt> clearStatement();
    std::unique_ptr<IfStmt> ifStmt();
    std::unique_ptr<ForStmt> forStmt();
    std::unique_ptr<WhileStmt> whileStmt();
    std::unique_ptr<ReturnStmt> returnStmt();
    std::unique_ptr<FunctionDecl> functionDecl();
    std::unique_ptr<FormalParam> formalParam();

    Token consumeType();
    Token consumeIdentifier();

    void initWindow();
    void moveWindow();
    Token nextToken();

    Token peek();
    Token peek(int lookahead);
    Token advance();
    Token previous();

    bool isAtEnd();
    bool check(Token::Type type);
    bool peekMatch(
        std::initializer_list<Token::Type> const& types
    );
    bool match(
        std::initializer_list<Token::Type> const& types
    );

    template <typename... T>
    void consume(
        Token::Type type, fmt::format_string<T...> fmt,
        T&&... args
    ) {
        if (check(type)) {
            advance();
        } else {
            error(fmt, args...);
        }
    }

    template <typename... T>
    void error(fmt::format_string<T...> fmt, T&&... args) {
        mHasError = true;

        Token violatingToken = peek();

        fmt::println(
            stderr, "parsing error at {}:{}:: {}",
            violatingToken.getLine(),
            violatingToken.getColumn(),
            fmt::format(fmt, args...)
        );

        throw SyncParser{};
    }

    void synchronize();

    Lexer& mLexer;

    // error info
    bool mHasError{false};

    std::unique_ptr<Program> mAst{};

    Token mPreviousToken;

    std::array<Token, LOOKAHEAD> mTokenBuffer;
};

}  // namespace PArL
