// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// std
#include <initializer_list>
#include <memory>
#include <utility>
#include <iostream>

// lox
#include <common/AST.hpp>
#include <common/Token.hpp>
#include <common/Value.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

namespace Vought {

char const *ParserException::what() const noexcept {
    return mMessage.c_str();
}

Parser::Parser(Lexer &lexer) : mLexer(lexer) {
    initWindow();
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
        stmts.emplace_back(statement());
    }

    return std::make_unique<Program>(std::move(stmts));
}

std::unique_ptr<Stmt> Parser::statement() {
    Token peekToken = peek(0);
    switch (peekToken.getType()) {
        case Token::Type::BUILTIN: {
            Builtin builtinType = std::get<Builtin>(
                peekToken.getValue().value().data);
            switch (builtinType) {
                case __PRINT:
                    consume(Token::Type::BUILTIN, "");
                    return printStatement();
                case __DELAY:
                    consume(Token::Type::BUILTIN, "");
                    return delayStatement();
                case __WRITE:
                    consume(Token::Type::BUILTIN, "");
                    return writeStatement();
                case __WRITE_BOX:
                    consume(Token::Type::BUILTIN, "");
                    return writeBoxStatement();
                default:
                    throw ParserException(
                        peekToken,
                        fmt::format(
                            "unexpected token: {}",
                            peekToken.toString(true)));
            }
        } break;
        case Token::Type::LEFT_BRACE:
            consume(Token::Type::LEFT_BRACE, "");
            return block();
        case Token::Type::IF:
            consume(Token::Type::IF, "");
            return ifStmt();
        case Token::Type::FOR:
            consume(Token::Type::FOR, "");
            return forStmt();
        case Token::Type::WHILE:
            consume(Token::Type::WHILE, "");
            return whileStmt();
        case Token::Type::RETURN:
            consume(Token::Type::RETURN, "");
            return returnStmt();
        case Token::Type::FUN:
            consume(Token::Type::FUN, "");
            return functionDecl();
        case Token::Type::LET:
            consume(Token::Type::LET, "");
            return variableDecl();
        case Token::Type::IDENTIFIER:
            return assignment();
        default:
            throw ParserException(
                peekToken,
                fmt::format("unexpected token: {}",
                            peekToken.toString(true)));
    }
}

std::unique_ptr<Block> Parser::block() {
    consume(Token::Type::LEFT_BRACE,
            "Expected '{' after expression");

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!peekMatch({Token::Type::RIGHT_BRACE})) {
        stmts.emplace_back(statement());
    }

    consume(Token::Type::RIGHT_BRACE,
            "Expected '}' after expression");

    return std::make_unique<Block>(std::move(stmts));
}

std::unique_ptr<VariableDecl> Parser::variableDecl() {
    Token identifier;

    if (match({Token::Type::IDENTIFIER})) {
        identifier = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    consume(Token::Type::COLON,
            "Expected ':' after expression");

    Token type;

    if (match({Token::Type::BOOL_TYPE,
               Token::Type::INTEGER_TYPE,
               Token::Type::FLOAT_TYPE,
               Token::Type::COLOR_TYPE})) {
        type = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    consume(Token::Type::EQUAL,
            "Expected '=' after expression");

    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<VariableDecl>(
        std::move(identifier), std::move(type),
        std::move(expression));
}

std::unique_ptr<Assignment> Parser::assignment() {
    Token identifier;

    if (match({Token::Type::IDENTIFIER})) {
        identifier = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    consume(Token::Type::EQUAL,
            "Expected '=' after expression");

    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<Assignment>(
        std::move(identifier), std::move(expression));
}

std::unique_ptr<PrintStmt> Parser::printStatement() {
    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<PrintStmt>(
        std::move(expression));
}

std::unique_ptr<DelayStmt> Parser::delayStatement() {
    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<DelayStmt>(
        std::move(expression));
}

std::unique_ptr<WriteStmt> Parser::writeStatement() {
    std::unique_ptr<Expr> x = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> y = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> color = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<WriteStmt>(
        std::move(x), std::move(y), std::move(color));
}

std::unique_ptr<WriteBoxStmt> Parser::writeBoxStatement() {
    std::unique_ptr<Expr> x = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> y = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> xOffset = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> yOffset = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> color = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<WriteBoxStmt>(
        std::move(x), std::move(y), std::move(xOffset),
        std::move(yOffset), std::move(color));
}

std::unique_ptr<IfStmt> Parser::ifStmt() {
    consume(Token::Type::LEFT_PAREN,
            "Expected '(' after expression");

    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::RIGHT_PAREN,
            "Expected ')' after expression");

    std::unique_ptr<Block> ifThen = block();

    std::unique_ptr<Block> ifElse{};

    if (match({Token::Type::ELSE})) {
        ifElse = block();
    }

    return std::make_unique<IfStmt>(std::move(expression),
                                    std::move(ifThen),
                                    std::move(ifElse));
}

std::unique_ptr<ForStmt> Parser::forStmt() {
    consume(Token::Type::LEFT_PAREN,
            "Expected '(' after expression");

    std::unique_ptr<VariableDecl> varDecl{};

    if (!match({Token::Type::SEMICOLON})) {
        varDecl = variableDecl();

        consume(Token::Type::SEMICOLON,
                "Expected ';' after expression");
    }

    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    std::unique_ptr<Assignment> assign{};

    if (!match({Token::Type::RIGHT_PAREN})) {
        assign = assignment();

        consume(Token::Type::RIGHT_PAREN,
                "Expected ')' after expression");
    }

    std::unique_ptr<Block> blk = block();

    return std::make_unique<ForStmt>(
        std::move(varDecl), std::move(expression),
        std::move(assign), std::move(blk));
}

std::unique_ptr<WhileStmt> Parser::whileStmt() {
    consume(Token::Type::LEFT_PAREN,
            "Expected '(' after expression");

    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::RIGHT_PAREN,
            "Expected ')' after expression");

    std::unique_ptr<Block> blk = block();

    return std::make_unique<WhileStmt>(
        std::move(expression), std::move(blk));
}

std::unique_ptr<ReturnStmt> Parser::returnStmt() {
    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::SEMICOLON,
            "Expected ';' after expression");

    return std::make_unique<ReturnStmt>(
        std::move(expression));
}

std::unique_ptr<FormalParam> Parser::formalParam() {
    Token identifier;

    if (match({Token::Type::IDENTIFIER})) {
        identifier = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    consume(Token::Type::COLON,
            "Expected ':' after expression");

    Token type;

    if (match({Token::Type::BOOL_TYPE,
               Token::Type::INTEGER_TYPE,
               Token::Type::FLOAT_TYPE,
               Token::Type::COLOR_TYPE})) {
        type = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    return std::make_unique<FormalParam>(
        std::move(identifier), std::move(type));
}

std::unique_ptr<FunctionDecl> Parser::functionDecl() {
    Token identifier;

    if (match({Token::Type::IDENTIFIER})) {
        identifier = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    consume(Token::Type::LEFT_PAREN,
            "Expected '(' after expression");

    std::unique_ptr<
        std::vector<std::unique_ptr<FormalParam>>>
        formalParams = std::make_unique<
            std::vector<std::unique_ptr<FormalParam>>>();

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        do {
            formalParams->emplace_back(formalParam());
        } while (match({Token::Type::COMMA}));
    }

    consume(Token::Type::RIGHT_PAREN,
            "Expected ')' after expression");

    consume(Token::Type::ARROW,
            "Expected '->' after expression");

    Token type;

    if (match({Token::Type::BOOL_TYPE,
               Token::Type::INTEGER_TYPE,
               Token::Type::FLOAT_TYPE,
               Token::Type::COLOR_TYPE})) {
        type = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    std::unique_ptr<Block> blk = block();

    return std::make_unique<FunctionDecl>(
        std::move(identifier), std::move(formalParams),
        std::move(type), std::move(blk));
}

std::unique_ptr<Expr> Parser::expr() {
    std::unique_ptr<Expr> expr = simpleExpr();

    while (match({Token::Type::LESS, Token::Type::GREATER,
                  Token::Type::BANG_EQUAL,
                  Token::Type::EQUAL_EQUAL,
                  Token::Type::LESS_EQUAL,
                  Token::Type::GREATER_EQUAL})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = simpleExpr();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right));
    }

    Token type{};

    if (match({Token::Type::AS})) {
        if (match({Token::Type::BOOL_TYPE,
                   Token::Type::INTEGER_TYPE,
                   Token::Type::FLOAT_TYPE,
                   Token::Type::COLOR_TYPE})) {
            type = previous();
        } else {
            throw ParserException(
                peek(0),
                fmt::format("unexpected token {}",
                            peek(0).toString(true)));
        }
    }

    expr->type = type;

    return expr;
}

std::unique_ptr<Expr> Parser::simpleExpr() {
    std::unique_ptr<Expr> expr = term();

    while (match({Token::Type::PLUS, Token::Type::MINUS,
                  Token::Type::OR})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = term();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({Token::Type::STAR, Token::Type::SLASH,
                  Token::Type::AND})) {
        Token oper = previous();
        std::unique_ptr<Expr> right = factor();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right));
    }

    return expr;
}

std::unique_ptr<BuiltinWidth> Parser::padWidth() {
    return std::make_unique<BuiltinWidth>();
}

std::unique_ptr<BuiltinHeight> Parser::padHeight() {
    return std::make_unique<BuiltinHeight>();
}

std::unique_ptr<BuiltinRead> Parser::padRead() {
    std::unique_ptr<Expr> x = expr();

    consume(Token::Type::COMMA,
            "Expected ',' after expression");

    std::unique_ptr<Expr> y = expr();

    return std::make_unique<BuiltinRead>(std::move(x),
                                         std::move(y));
}

std::unique_ptr<BuiltinRandomInt> Parser::padRandI() {
    std::unique_ptr<Expr> max = expr();

    return std::make_unique<BuiltinRandomInt>(
        std::move(max));
}

std::unique_ptr<Expr> Parser::factor() {
    Token peekToken = peek(0);

    switch (peekToken.getType()) {
        case Token::Type::BUILTIN: {
            Builtin builtinType = std::get<Builtin>(
                peekToken.getValue().value().data);
            switch (builtinType) {
                case __RANDOM_INT:
                    consume(Token::Type::BUILTIN, "");
                    return padRandI();
                case __WIDTH:
                    consume(Token::Type::BUILTIN, "");
                    return padWidth();
                case __HEIGHT:
                    consume(Token::Type::BUILTIN, "");
                    return padHeight();
                case __READ:
                    consume(Token::Type::BUILTIN, "");
                    return padRead();
                default:
                    throw ParserException(
                        peekToken,
                        fmt::format(
                            "unexpected token: {}",
                            peekToken.toString(true)));
            }
        } break;
        case Token::Type::MINUS:
        case Token::Type::NOT:
            return unary();
        case Token::Type::LEFT_PAREN:
            return subExpr();
        case Token::Type::IDENTIFIER: {
            Token next = peek(1);

            if (next.getType() == Token::Type::LEFT_PAREN) {
                return functionCall();
            } else {
                consume(Token::Type::IDENTIFIER, "");
                return std::make_unique<Variable>(
                    std::move(peekToken));
            }
        }
        default: {
            if (match({Token::Type::BOOL,
                       Token::Type::INTEGER,
                       Token::Type::FLOAT,
                       Token::Type::COLOR})) {
                return std::make_unique<Literal>(
                    previous());
            } else {
                throw ParserException(
                    peekToken,
                    fmt::format("unexpected token: {}",

                                peekToken.toString(true)));
            }
        }
    }
}

std::unique_ptr<FunctionCall> Parser::functionCall() {
    Token identifier;

    if (match({Token::Type::IDENTIFIER})) {
        identifier = previous();
    } else {
        throw ParserException(
            peek(0), fmt::format("unexpected token {}",
                                 peek(0).toString(true)));
    }

    consume(Token::Type::LEFT_PAREN,
            "Expected '(' after expression");

    std::unique_ptr<std::vector<std::unique_ptr<Expr>>>
        params = std::make_unique<
            std::vector<std::unique_ptr<Expr>>>();

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        do {
            params->emplace_back(expr());
        } while (match({Token::Type::COMMA}));
    }

    consume(Token::Type::RIGHT_PAREN,
            "Expected ')' after expression");

    return std::make_unique<FunctionCall>(
        std::move(identifier), std::move(params));
}

std::unique_ptr<Unary> Parser::unary() {
    Token oper = advance();

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<Unary>(std::move(oper),
                                   std::move(expression));
}

std::unique_ptr<SubExpr> Parser::subExpr() {
    consume(Token::Type::LEFT_PAREN,
            "Expected '(' before subexpression");

    std::unique_ptr<Expr> expression = expr();

    consume(Token::Type::RIGHT_PAREN,
            "Expected ')' after subexpression");

    return std::make_unique<SubExpr>(std::move(expression));
}

void Parser::initWindow() {
    for (int i = 0; i < LOOKAHEAD; i++) {
        std::optional<Token> token = mLexer.nextToken();

        if (!token.has_value()) {
            throw LexerException(
                "error occured in lexical analysis");
        }

        while (
            token->getType() == Token::Type::WHITESPACE ||
            token->getType() == Token::Type::WHITESPACE) {
            token = mLexer.nextToken();

            if (!token.has_value()) {
                throw LexerException(
                    "error occured in lexical analysis");
            }
        }

        mTokenBuffer[i] = token.value();
    }
}

Token Parser::moveWindow() {
    Token previous = mTokenBuffer[0];

    for (int i = 1; i < LOOKAHEAD; i++) {
        mTokenBuffer[i - 1] = mTokenBuffer[i];
    }

    std::optional<Token> token = mLexer.nextToken();

    if (!token.has_value()) {
        throw LexerException(
            "error occured in lexical analysis");
    }

    while (token->getType() == Token::Type::WHITESPACE ||
           token->getType() == Token::Type::COMMENT) {
        token = mLexer.nextToken();

        if (!token.has_value()) {
            throw LexerException(
                "error occured in lexical analysis");
        }
    }

    mTokenBuffer[LOOKAHEAD - 1] = token.value();

    fmt::print("Buffer: ");
    for (auto& token : mTokenBuffer) {
        fmt::print("{}, ", token.toString(true));
    }
    fmt::print("\n");

    return previous;
}

Token &Parser::peek(int lookahead) {
    if (!(0 <= lookahead && lookahead < LOOKAHEAD))
        throw std::exception();

    return mTokenBuffer[lookahead];
}

bool Parser::isAtEnd() {
    return peek(0).getType() == Token::Type::END_OF_FILE;
}

inline Token Parser::advance() {
    mPreviousToken = moveWindow();

    return mPreviousToken;
}

Token Parser::previous() {
    return mPreviousToken;
}

bool Parser::check(Token::Type type) {
    if (isAtEnd()) {
        return false;
    }

    return peek(0).getType() == type;
}

Token Parser::consume(Token::Type type,
                      std::string message) {
    if (check(type)) {
        return advance();
    }

    throw ParserException(peek(0), std::move(message));
}

bool Parser::match(
    std::initializer_list<Token::Type> const &types) {
    if (peekMatch(types)) {
        advance();

        return true;
    }

    return false;
}

bool Parser::peekMatch(
    std::initializer_list<Token::Type> const &types) {
    for (Token::Type type : types) {
        if (check(type)) {
            return true;
        }
    }

    return false;
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().getType() == Token::Type::SEMICOLON)
            return;

        switch (peek(0).getType()) {
            case Token::Type::FUN:
            case Token::Type::FOR:
            case Token::Type::IF:
            case Token::Type::WHILE:
            case Token::Type::RETURN:
                return;
            default:;  // Do nothing
        }

        advance();
    }
}

}  // namespace Vought
