// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// std
#include <initializer_list>
#include <memory>
#include <utility>

// vought
#include <common/AST.hpp>
#include <common/Assert.hpp>
#include <common/Token.hpp>
#include <common/Value.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>
#include <runner/Runner.hpp>

#ifdef INTERNAL_DEBUG

#define CONSUME(token_type, msg)                           \
    (consume(token_type,                                   \
             fmt::format(__FILE__ ":" LINE_STRING ":: {}", \
                         msg)))

#define ERROR(msg)                                       \
    (error(fmt::format(__FILE__ ":" LINE_STRING ":: {}", \
                       msg)))

#else

#define CONSUME(token_type, msg) \
    (consume(token_type, fmt::format("{}", msg)))

#define ERROR(msg) (error(fmt::format("{}", msg)))

#endif

namespace Vought {

Parser::Parser(Lexer &lexer) : mLexer(lexer) {
    initWindow();
}

void Parser::parse() {
    mAST = program();
}

bool Parser::hasError() const {
    return mHasError;
}

void Parser::reset() {
    mHasError = false;
    mAST.reset();
    mPreviousToken = {};
    mTokenBuffer = {};
}

std::unique_ptr<Program> Parser::getAST() {
    ABORTIF(mHasError,
            "parser could not generate tree due to parsing "
            "error(s)");

    return std::move(mAST);
}

std::unique_ptr<Program> Parser::program() {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!isAtEnd()) {
        try {
            stmts.emplace_back(statement());
        } catch (SyncObject const &) {
            synchronize();
        }
    }

    return std::make_unique<Program>(std::move(stmts));
}

std::unique_ptr<Stmt> Parser::statement() {
    Token peekToken = peek();

    switch (peekToken.getType()) {
        case Token::Type::BUILTIN: {
            Builtin builtinType = std::get<Builtin>(
                peekToken.getValue().value().data);
            switch (builtinType) {
                case __PRINT: {
                    std::unique_ptr<PrintStmt> stmt =
                        printStatement();
                    CONSUME(Token::Type::SEMICOLON,
                            "expecetd ';' after __print "
                            "statement");
                    return stmt;
                }
                case __DELAY: {
                    std::unique_ptr<DelayStmt> stmt =
                        delayStatement();
                    CONSUME(Token::Type::SEMICOLON,
                            "expecetd ';' after __delay "
                            "statement");
                    return stmt;
                }
                case __WRITE: {
                    std::unique_ptr<WriteStmt> stmt =
                        writeStatement();
                    CONSUME(Token::Type::SEMICOLON,
                            "expecetd ';' after __write "
                            "statement");
                    return stmt;
                }
                case __WRITE_BOX: {
                    std::unique_ptr<WriteBoxStmt> stmt =
                        writeBoxStatement();
                    CONSUME(Token::Type::SEMICOLON,
                            "expecetd ';' after "
                            "__write_box statement");
                    return stmt;
                }
                default:
                    throw ERROR(fmt::format(
                        "unexpected token {} for builtin "
                        "statment",
                        peekToken.toString(false)));
            }
        } break;
        case Token::Type::LEFT_BRACE:
            return block();
        case Token::Type::IF:
            return ifStmt();
        case Token::Type::FOR:
            return forStmt();
        case Token::Type::WHILE:
            return whileStmt();
        case Token::Type::FUN:
            return functionDecl();
        case Token::Type::RETURN: {
            std::unique_ptr<ReturnStmt> stmt = returnStmt();
            CONSUME(Token::Type::SEMICOLON,
                    "expecetd ';' after "
                    "return statement");
            return stmt;
        }
        case Token::Type::LET: {
            std::unique_ptr<VariableDecl> stmt =
                variableDecl();
            CONSUME(Token::Type::SEMICOLON,
                    "expecetd ';' after "
                    "variable declaration");
            return stmt;
        }
        case Token::Type::IDENTIFIER: {
            std::unique_ptr<Assignment> stmt = assignment();
            CONSUME(Token::Type::SEMICOLON,
                    "expecetd ';' after "
                    "assignment");
            return stmt;
        }
        default:
            throw ERROR(fmt::format(
                "unexpected token {} for statement start",
                peekToken.toString(false)));
    }
}

std::unique_ptr<Block> Parser::block() {
    CONSUME(Token::Type::LEFT_BRACE,
            "expected '{' at start of block");

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!isAtEnd() &&
           !peekMatch({Token::Type::RIGHT_BRACE})) {
        try {
            stmts.emplace_back(statement());
        } catch (SyncObject const &) {
            synchronize();
        }
    }

    CONSUME(Token::Type::RIGHT_BRACE,
            "expected '}' at end of block");

    return std::make_unique<Block>(std::move(stmts));
}

std::unique_ptr<VariableDecl> Parser::variableDecl() {
    CONSUME(Token::Type::LET,
            "expected 'let' at the start of variable "
            "declaration");

    Token identifier = consumeIdentifier();

    CONSUME(Token::Type::COLON,
            "expected ':' after identifier");

    Token type = consumeType();

    CONSUME(Token::Type::EQUAL, "expected '=' after type");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<VariableDecl>(
        std::move(identifier), std::move(type),
        std::move(expression));
}

std::unique_ptr<Assignment> Parser::assignment() {
    Token identifier = consumeIdentifier();

    CONSUME(Token::Type::EQUAL,
            "expected '=' after identifier");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<Assignment>(
        std::move(identifier), std::move(expression));
}

std::unique_ptr<PrintStmt> Parser::printStatement() {
    CONSUME(Token::Type::BUILTIN, "expected __print");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<PrintStmt>(
        std::move(expression));
}

std::unique_ptr<DelayStmt> Parser::delayStatement() {
    CONSUME(Token::Type::BUILTIN, "expected __delay");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<DelayStmt>(
        std::move(expression));
}

std::unique_ptr<WriteStmt> Parser::writeStatement() {
    CONSUME(Token::Type::BUILTIN, "expected __write");

    std::unique_ptr<Expr> x = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> y = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> color = expr();

    return std::make_unique<WriteStmt>(
        std::move(x), std::move(y), std::move(color));
}

std::unique_ptr<WriteBoxStmt> Parser::writeBoxStatement() {
    CONSUME(Token::Type::BUILTIN, "expected __write_box");

    std::unique_ptr<Expr> x = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> y = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> xOffset = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> yOffset = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> color = expr();

    return std::make_unique<WriteBoxStmt>(
        std::move(x), std::move(y), std::move(xOffset),
        std::move(yOffset), std::move(color));
}

std::unique_ptr<IfStmt> Parser::ifStmt() {
    CONSUME(Token::Type::IF,
            "expected 'if' at start of if statement");

    CONSUME(Token::Type::LEFT_PAREN,
            "expected '(' after 'if'");

    std::unique_ptr<Expr> expression = expr();

    CONSUME(Token::Type::RIGHT_PAREN,
            "expected ')' after expression");

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
    CONSUME(Token::Type::FOR,
            "expected 'for' at start of for statement");

    CONSUME(Token::Type::LEFT_PAREN,
            "expected '(' after 'for'");

    std::unique_ptr<VariableDecl> varDecl{};

    if (!peekMatch({Token::Type::SEMICOLON})) {
        varDecl = variableDecl();
    }

    CONSUME(
        Token::Type::SEMICOLON,
        "expected ';' after '(' or variable declaration");

    std::unique_ptr<Expr> expression = expr();

    CONSUME(Token::Type::SEMICOLON,
            "expected ';' after expression");

    std::unique_ptr<Assignment> assign{};

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        assign = assignment();
    }

    CONSUME(Token::Type::RIGHT_PAREN,
            "expected ')' after ';' or assignment");

    std::unique_ptr<Block> blk = block();

    return std::make_unique<ForStmt>(
        std::move(varDecl), std::move(expression),
        std::move(assign), std::move(blk));
}

std::unique_ptr<WhileStmt> Parser::whileStmt() {
    CONSUME(Token::Type::WHILE,
            "expected 'while' at start of while statement");

    CONSUME(Token::Type::LEFT_PAREN,
            "expected '(' after 'while'");

    std::unique_ptr<Expr> expression = expr();

    CONSUME(Token::Type::RIGHT_PAREN,
            "expected ')' after expression");

    std::unique_ptr<Block> blk = block();

    return std::make_unique<WhileStmt>(
        std::move(expression), std::move(blk));
}

std::unique_ptr<ReturnStmt> Parser::returnStmt() {
    CONSUME(
        Token::Type::RETURN,
        "expected 'return' at start of return statement");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<ReturnStmt>(
        std::move(expression));
}

std::unique_ptr<FormalParam> Parser::formalParam() {
    Token identifier = consumeIdentifier();

    CONSUME(Token::Type::COLON,
            "expected ':' after identifier");

    Token type = consumeType();

    return std::make_unique<FormalParam>(
        std::move(identifier), std::move(type));
}

std::unique_ptr<FunctionDecl> Parser::functionDecl() {
    CONSUME(
        Token::Type::FUN,
        "expected 'fun' at start of function declaration");

    Token identifier = consumeIdentifier();

    CONSUME(Token::Type::LEFT_PAREN,
            "expected '(' after identifier");

    std::vector<std::unique_ptr<FormalParam>>
        formalParams{};

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        do {
            formalParams.emplace_back(formalParam());
        } while (match({Token::Type::COMMA}));
    }

    CONSUME(Token::Type::RIGHT_PAREN,
            "expected ')' after formal parameters");

    CONSUME(Token::Type::ARROW, "Expected '->' after ')'");

    Token type = consumeType();

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

    if (match({Token::Type::AS}))
        expr->type = consumeType();

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
    CONSUME(Token::Type::BUILTIN, "expected __width");

    return std::make_unique<BuiltinWidth>();
}

std::unique_ptr<BuiltinHeight> Parser::padHeight() {
    CONSUME(Token::Type::BUILTIN, "expected __height");

    return std::make_unique<BuiltinHeight>();
}

std::unique_ptr<BuiltinRead> Parser::padRead() {
    CONSUME(Token::Type::BUILTIN, "expected __read");

    std::unique_ptr<Expr> x = expr();

    CONSUME(Token::Type::COMMA,
            "expected ',' after expression");

    std::unique_ptr<Expr> y = expr();

    return std::make_unique<BuiltinRead>(std::move(x),
                                         std::move(y));
}

std::unique_ptr<BuiltinRandomInt> Parser::padRandI() {
    CONSUME(Token::Type::BUILTIN, "expected __random_int");

    std::unique_ptr<Expr> max = expr();

    return std::make_unique<BuiltinRandomInt>(
        std::move(max));
}

std::unique_ptr<Expr> Parser::factor() {
    Token peekToken = peek();

    switch (peekToken.getType()) {
        case Token::Type::BUILTIN: {
            Builtin builtinType = std::get<Builtin>(
                peekToken.getValue().value().data);
            switch (builtinType) {
                case __RANDOM_INT:
                    return padRandI();
                case __WIDTH:
                    return padWidth();
                case __HEIGHT:
                    return padHeight();
                case __READ:
                    return padRead();
                default:
                    throw ERROR(fmt::format(
                        "unexpected token {}",
                        peekToken.toString(false)));
            }
        } break;
        case Token::Type::MINUS:
            return unary();
        case Token::Type::NOT:
            return unary();
        case Token::Type::LEFT_PAREN:
            return subExpr();
        case Token::Type::IDENTIFIER: {
            Token next = peek(1);

            if (next.getType() == Token::Type::LEFT_PAREN) {
                return functionCall();
            } else {
                return std::make_unique<Variable>(
                    std::move(advance()));
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
                throw ERROR(
                    fmt::format("unexpected token {}",
                                peekToken.toString(false)));
            }
        }
    }
}

std::unique_ptr<FunctionCall> Parser::functionCall() {
    Token identifier = consumeIdentifier();

    CONSUME(Token::Type::LEFT_PAREN,
            "expected '(' after identifier");

    std::vector<std::unique_ptr<Expr>> params{};

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        do {
            params.emplace_back(expr());
        } while (match({Token::Type::COMMA}));
    }

    CONSUME(Token::Type::RIGHT_PAREN,
            "expected ')' after parameters");

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
    CONSUME(Token::Type::LEFT_PAREN,
            "expected '(' at start of sub expression");

    std::unique_ptr<Expr> expression = expr();

    CONSUME(Token::Type::RIGHT_PAREN,
            "expected ')' at end of sub expression");

    return std::make_unique<SubExpr>(std::move(expression));
}

Token Parser::consumeIdentifier() {
    if (match({Token::Type::IDENTIFIER}))
        return previous();
    else
        throw ERROR(fmt::format(
            "expected identifier token instead received {}",
            peek().toString(false)));
}

Token Parser::consumeType() {
    if (match({Token::Type::BOOL_TYPE,
               Token::Type::INTEGER_TYPE,
               Token::Type::FLOAT_TYPE,
               Token::Type::COLOR_TYPE})) {
        return previous();
    } else {
        throw ERROR(fmt::format(
            "expected type token instead received {}",
            peek().toString(false)));
    }
}

void Parser::initWindow() {
    for (int i = 0; i < LOOKAHEAD; i++) {
        std::optional<Token> token = mLexer.nextToken();

        if (!token.has_value()) {
            ABORTIF(true, "");
            // throw LexerException(
            //     "error occured in lexical analysis");
        }

        while (token->getType() ==
                   Token::Type::WHITESPACE ||
               token->getType() == Token::Type::COMMENT) {
            token = mLexer.nextToken();

            if (!token.has_value()) {
                ABORTIF(true, "");
                // throw LexerException(
                //     "error occured in lexical analysis");
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
        ABORTIF(true, "");
        // throw LexerException(
        //     "error occured in lexical analysis");
    }

    while (token->getType() == Token::Type::WHITESPACE ||
           token->getType() == Token::Type::COMMENT) {
        token = mLexer.nextToken();

        if (!token.has_value()) {
            ABORTIF(true, "");
            // throw LexerException(
            //     "error occured in lexical analysis");
        }
    }

    mTokenBuffer[LOOKAHEAD - 1] = token.value();

    return previous;
}

Token &Parser::peek() {
    return peek(0);
}

Token &Parser::peek(int lookahead) {
    ABORTIF(
        !(0 <= lookahead && lookahead < LOOKAHEAD),
        fmt::format("exceeded lookahead {}", LOOKAHEAD));

    return mTokenBuffer[lookahead];
}

bool Parser::isAtEnd() {
    return peek().getType() == Token::Type::END_OF_FILE;
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

    return peek().getType() == type;
}

SyncObject Parser::error(std::string message) {
    mHasError = true;

    Token violatingToken = peek();

    fmt::println(stderr, "parsing error at {}:{}:: {}",
                 violatingToken.getLine(),
                 violatingToken.getColumn(), message);

    return SyncObject();
}

Token Parser::consume(Token::Type type,
                      std::string message) {
    if (check(type)) {
        return advance();
    }

    throw error(message);
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

// NOTE: synchronization is all best effort

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().getType() ==
            Token::Type::SEMICOLON) {
            return;
        }

        Token peekToken = peek();

        switch (peekToken.getType()) {
            case Token::Type::FOR:
                /* fallthrough */
            case Token::Type::FUN:
                /* fallthrough */
            case Token::Type::IF:
                /* fallthrough */
            case Token::Type::LET:
                /* fallthrough */
            case Token::Type::RETURN:
                /* fallthrough */
            case Token::Type::WHILE:
                return;

            case Token::Type::BUILTIN: {
                Builtin builtinType = std::get<Builtin>(
                    peekToken.getValue()->data);

                switch (builtinType) {
                    case __PRINT:
                        /* fallthrough */
                    case __DELAY:
                        /* fallthrough */
                    case __WRITE:
                        /* fallthrough */
                    case __WRITE_BOX:
                        return;
                    default:;  // Do nothing
                }
            }
            default:;  // Do nothing
        }

        advance();
    }
}

}  // namespace Vought
