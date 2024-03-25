// fmt
#include <fmt/core.h>

// std
#include <initializer_list>
#include <memory>
#include <utility>

// parl
#include <common/AST.hpp>
#include <common/Abort.hpp>
#include <common/Token.hpp>
#include <common/Value.hpp>
#include <lexer/Lexer.hpp>
#include <parser/Parser.hpp>

namespace PArL {

Parser::Parser(Lexer &lexer) : mLexer(lexer) {
    initWindow();
}

void Parser::parse(std::string const &source) {
    mLexer.addSource(source);

    reset();

    mAst = program();
}

bool Parser::hasError() const {
    return mHasError;
}

void Parser::reset() {
    mHasError = false;
    mAst.reset();
    initWindow();
}

std::unique_ptr<Program> Parser::getAst() {
    abortIf(
        mHasError,
        "parser could not generate tree due to parsing "
        "error(s)"
    );

    return std::move(mAst);
}

std::unique_ptr<Program> Parser::program() {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!isAtEnd()) {
        try {
            stmts.emplace_back(statement());
        } catch (SyncParser const &) {
            synchronize();
        }
    }

    return std::make_unique<Program>(std::move(stmts));
}

std::unique_ptr<Stmt> Parser::statement() {
    Token peekToken = peek();

    switch (peekToken.getType()) {
        case Token::Type::BUILTIN: {
            auto const builtinType =
                peekToken.as<Builtin>();

            switch (builtinType) {
                case Builtin::PRINT: {
                    std::unique_ptr<PrintStmt> stmt =
                        printStatement();
                    consume(
                        Token::Type::SEMICOLON,
                        "expected ';' after __print "
                        "statement"
                    );
                    return stmt;
                }
                case Builtin::DELAY: {
                    std::unique_ptr<DelayStmt> stmt =
                        delayStatement();
                    consume(
                        Token::Type::SEMICOLON,
                        "expected ';' after __delay "
                        "statement"
                    );
                    return stmt;
                }
                case Builtin::WRITE: {
                    std::unique_ptr<WriteStmt> stmt =
                        writeStatement();
                    consume(
                        Token::Type::SEMICOLON,
                        "expected ';' after __write "
                        "statement"
                    );
                    return stmt;
                }
                case Builtin::CLEAR: {
                    std::unique_ptr<ClearStmt> stmt =
                        clearStatement();
                    consume(
                        Token::Type::SEMICOLON,
                        "expected ';' after __clear "
                        "statement"
                    );
                    return stmt;
                }
                case Builtin::WRITE_BOX: {
                    std::unique_ptr<WriteBoxStmt> stmt =
                        writeBoxStatement();
                    consume(
                        Token::Type::SEMICOLON,
                        "expected ';' after "
                        "__write_box statement"
                    );
                    return stmt;
                }
                default:
                    error(
                        "unexpected token {} for "
                        "builtin "
                        "statement",
                        peekToken.toString()
                    );
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
            consume(
                Token::Type::SEMICOLON,
                "expected ';' after "
                "return statement"
            );
            return stmt;
        }
        case Token::Type::LET: {
            std::unique_ptr<VariableDecl> stmt =
                variableDecl();
            consume(
                Token::Type::SEMICOLON,
                "expected ';' after "
                "variable declaration"
            );
            return stmt;
        }
        case Token::Type::IDENTIFIER: {
            std::unique_ptr<Assignment> stmt = assignment();
            consume(
                Token::Type::SEMICOLON,
                "expected ';' after "
                "assignment"
            );
            return stmt;
        }
        default:
            error(
                "unexpected token {} for "
                "statement start",
                peekToken.toString()
            );
    }

    abort("unreachable");
}

std::unique_ptr<Block> Parser::block() {
    consume(
        Token::Type::LEFT_BRACE,
        "expected '{{' at start of block"
    );

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!isAtEnd() &&
           !peekMatch({Token::Type::RIGHT_BRACE})) {
        try {
            stmts.emplace_back(statement());
        } catch (SyncParser const &) {
            synchronize();
        }
    }

    consume(
        Token::Type::RIGHT_BRACE,
        "expected '}}' at end of block"
    );

    return std::make_unique<Block>(std::move(stmts));
}

std::unique_ptr<VariableDecl> Parser::variableDecl() {
    consume(
        Token::Type::LET,
        "expected 'let' at the start of variable "
        "declaration"
    );

    Token identifier = consumeIdentifier();

    consume(
        Token::Type::COLON, "expected ':' after identifier"
    );

    Token type = consumeType();

    consume(Token::Type::EQUAL, "expected '=' after type");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<VariableDecl>(
        std::move(identifier), std::move(type),
        std::move(expression)
    );
}

std::unique_ptr<Assignment> Parser::assignment() {
    Token identifier = consumeIdentifier();

    consume(
        Token::Type::EQUAL, "expected '=' after identifier"
    );

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<Assignment>(
        std::move(identifier), std::move(expression)
    );
}

std::unique_ptr<PrintStmt> Parser::printStatement() {
    consume(Token::Type::BUILTIN, "expected __print");

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<PrintStmt>(std::move(expression)
    );
}

std::unique_ptr<DelayStmt> Parser::delayStatement() {
    consume(Token::Type::BUILTIN, "expected __delay");

    Token token = previous();

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<DelayStmt>(
        std::move(token), std::move(expression)
    );
}

std::unique_ptr<WriteStmt> Parser::writeStatement() {
    consume(Token::Type::BUILTIN, "expected __write");

    Token token = previous();

    std::unique_ptr<Expr> x = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> y = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> color = expr();

    return std::make_unique<WriteStmt>(
        std::move(token), std::move(x), std::move(y),
        std::move(color)
    );
}

std::unique_ptr<ClearStmt> Parser::clearStatement() {
    consume(Token::Type::BUILTIN, "expected __clear");

    Token token = previous();

    std::unique_ptr<Expr> color = expr();

    return std::make_unique<ClearStmt>(
        std::move(token), std::move(color)
    );
}

std::unique_ptr<WriteBoxStmt> Parser::writeBoxStatement() {
    consume(Token::Type::BUILTIN, "expected __write_box");

    Token token = previous();

    std::unique_ptr<Expr> x = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> y = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> xOffset = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> yOffset = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> color = expr();

    return std::make_unique<WriteBoxStmt>(
        std::move(token), std::move(x), std::move(y),
        std::move(xOffset), std::move(yOffset),
        std::move(color)
    );
}

std::unique_ptr<IfStmt> Parser::ifStmt() {
    consume(
        Token::Type::IF,
        "expected 'if' at start of if statement"
    );

    Token token = previous();

    consume(
        Token::Type::LEFT_PAREN, "expected '(' after 'if'"
    );

    std::unique_ptr<Expr> expression = expr();

    consume(
        Token::Type::RIGHT_PAREN,
        "expected ')' after expression"
    );

    std::unique_ptr<Block> ifThen = block();

    std::unique_ptr<Block> ifElse{};

    if (match({Token::Type::ELSE})) {
        ifElse = block();
    }

    return std::make_unique<IfStmt>(
        std::move(token), std::move(expression),
        std::move(ifThen), std::move(ifElse)
    );
}

std::unique_ptr<ForStmt> Parser::forStmt() {
    consume(
        Token::Type::FOR,
        "expected 'for' at start of for statement"
    );

    Token token = previous();

    consume(
        Token::Type::LEFT_PAREN, "expected '(' after 'for'"
    );

    std::unique_ptr<VariableDecl> varDecl{};

    if (!peekMatch({Token::Type::SEMICOLON})) {
        varDecl = variableDecl();
    }

    consume(
        Token::Type::SEMICOLON,
        "expected ';' after '(' or variable "
        "declaration"
    );

    std::unique_ptr<Expr> expression = expr();

    consume(
        Token::Type::SEMICOLON,
        "expected ';' after expression"
    );

    std::unique_ptr<Assignment> assign{};

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        assign = assignment();
    }

    consume(
        Token::Type::RIGHT_PAREN,
        "expected ')' after ';' or assignment"
    );

    std::unique_ptr<Block> blk = block();

    return std::make_unique<ForStmt>(
        std::move(token), std::move(varDecl),
        std::move(expression), std::move(assign),
        std::move(blk)
    );
}

std::unique_ptr<WhileStmt> Parser::whileStmt() {
    consume(
        Token::Type::WHILE,
        "expected 'while' at start of while statement"
    );

    Token token = previous();

    consume(
        Token::Type::LEFT_PAREN,
        "expected '(' after 'while'"
    );

    std::unique_ptr<Expr> expression = expr();

    consume(
        Token::Type::RIGHT_PAREN,
        "expected ')' after expression"
    );

    std::unique_ptr<Block> blk = block();

    return std::make_unique<WhileStmt>(
        std::move(token), std::move(expression),
        std::move(blk)
    );
}

std::unique_ptr<ReturnStmt> Parser::returnStmt() {
    consume(
        Token::Type::RETURN,
        "expected 'return' at start of return "
        "statement"
    );

    Token token = previous();

    std::unique_ptr<Expr> expression = expr();

    return std::make_unique<ReturnStmt>(
        std::move(token), std::move(expression)
    );
}

std::unique_ptr<FormalParam> Parser::formalParam() {
    Token identifier = consumeIdentifier();

    consume(
        Token::Type::COLON, "expected ':' after identifier"
    );

    Token type = consumeType();

    return std::make_unique<FormalParam>(
        std::move(identifier), std::move(type)
    );
}

std::unique_ptr<FunctionDecl> Parser::functionDecl() {
    consume(
        Token::Type::FUN,
        "expected 'fun' at start of function "
        "declaration"
    );

    Token identifier = consumeIdentifier();

    consume(
        Token::Type::LEFT_PAREN,
        "expected '(' after identifier"
    );

    std::vector<std::unique_ptr<FormalParam>> formalParams{
    };

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        do {
            formalParams.emplace_back(formalParam());
        } while (match({Token::Type::COMMA}));
    }

    consume(
        Token::Type::RIGHT_PAREN,
        "expected ')' after formal parameters"
    );

    consume(Token::Type::ARROW, "Expected '->' after ')'");

    Token type = consumeType();

    std::unique_ptr<Block> blk = block();

    return std::make_unique<FunctionDecl>(
        std::move(identifier), std::move(formalParams),
        std::move(type), std::move(blk)
    );
}

std::unique_ptr<Expr> Parser::expr() {
    std::unique_ptr<Expr> expr = logicOr();

    if (match({Token::Type::AS}))
        expr->type = consumeType();

    return expr;
}

std::unique_ptr<Expr> Parser::logicOr() {
    std::unique_ptr<Expr> expr = logicAnd();

    while (match({Token::Type::OR})) {
        Token oper = previous();

        std::unique_ptr<Expr> right = logicAnd();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::logicAnd() {
    std::unique_ptr<Expr> expr = equality();

    while (match({Token::Type::AND})) {
        Token oper = previous();

        std::unique_ptr<Expr> right = equality();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right)
        );
    }

    if (match({Token::Type::AS}))
        expr->type = consumeType();

    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();

    while (match(
        {Token::Type::EQUAL_EQUAL, Token::Type::BANG_EQUAL}
    )) {
        Token oper = previous();

        std::unique_ptr<Expr> right = equality();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();

    while (match(
        {Token::Type::LESS, Token::Type::LESS_EQUAL,
         Token::Type::GREATER, Token::Type::GREATER_EQUAL}
    )) {
        Token oper = previous();

        std::unique_ptr<Expr> right = term();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();

    while (match({Token::Type::PLUS, Token::Type::MINUS})) {
        Token oper = previous();

        std::unique_ptr<Expr> right = factor();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();

    while (match({Token::Type::STAR, Token::Type::SLASH})) {
        Token oper = previous();

        std::unique_ptr<Expr> right = unary();

        expr = std::make_unique<Binary>(
            std::move(expr), oper, std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({Token::Type::MINUS, Token::Type::NOT})) {
        Token oper = previous();

        std::unique_ptr<Expr> expression = unary();

        return std::make_unique<Unary>(
            std::move(oper), std::move(expression)
        );
    }

    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    Token peekToken = peek();

    switch (peekToken.getType()) {
        case Token::Type::BUILTIN: {
            auto builtinType = peekToken.as<Builtin>();

            switch (builtinType) {
                case Builtin::RANDOM_INT:
                    return padRandI();
                case Builtin::WIDTH:
                    return padWidth();
                case Builtin::HEIGHT:
                    return padHeight();
                case Builtin::READ:
                    return padRead();
                default:
                    error(
                        "unexpected token {} for "
                        "builtin "
                        "statement",
                        peekToken.toString()
                    );
            }
        } break;
        case Token::Type::LEFT_PAREN:
            return subExpr();
        case Token::Type::IDENTIFIER: {
            Token next = peek(1);

            if (next.getType() == Token::Type::LEFT_PAREN) {
                return functionCall();
            } else {
                return std::make_unique<Variable>(
                    std::move(advance())
                );
            }
        }
        default: {
            if (match(
                    {Token::Type::BOOL,
                     Token::Type::INTEGER,
                     Token::Type::FLOAT, Token::Type::COLOR}
                )) {
                return std::make_unique<Literal>(previous()
                );
            } else {
                error(
                    "unexpected token {} for factor",
                    peekToken.toString()
                );
            }
        }
    }
}

std::unique_ptr<Expr> Parser::subExpr() {
    consume(
        Token::Type::LEFT_PAREN,
        "expected '(' at start of sub expression"
    );

    std::unique_ptr<Expr> expression = expr();

    consume(
        Token::Type::RIGHT_PAREN,
        "expected ')' at end of sub expression"
    );

    return std::make_unique<SubExpr>(std::move(expression));
}

std::unique_ptr<Expr> Parser::functionCall() {
    Token identifier = consumeIdentifier();

    consume(
        Token::Type::LEFT_PAREN,
        "expected '(' after identifier"
    );

    std::vector<std::unique_ptr<Expr>> params{};

    if (!peekMatch({Token::Type::RIGHT_PAREN})) {
        do {
            params.emplace_back(expr());
        } while (match({Token::Type::COMMA}));
    }

    consume(
        Token::Type::RIGHT_PAREN,
        "expected ')' after parameters"
    );

    return std::make_unique<FunctionCall>(
        std::move(identifier), std::move(params)
    );
}

std::unique_ptr<BuiltinWidth> Parser::padWidth() {
    consume(Token::Type::BUILTIN, "expected __width");

    return std::make_unique<BuiltinWidth>(previous());
}

std::unique_ptr<BuiltinHeight> Parser::padHeight() {
    consume(Token::Type::BUILTIN, "expected __height");

    return std::make_unique<BuiltinHeight>(previous());
}

std::unique_ptr<BuiltinRead> Parser::padRead() {
    consume(Token::Type::BUILTIN, "expected __read");

    Token token = previous();

    std::unique_ptr<Expr> x = expr();

    consume(
        Token::Type::COMMA, "expected ',' after expression"
    );

    std::unique_ptr<Expr> y = expr();

    return std::make_unique<BuiltinRead>(
        std::move(token), std::move(x), std::move(y)
    );
}

std::unique_ptr<BuiltinRandomInt> Parser::padRandI() {
    consume(Token::Type::BUILTIN, "expected __random_int");

    Token token = previous();

    std::unique_ptr<Expr> max = expr();

    return std::make_unique<BuiltinRandomInt>(
        std::move(token), std::move(max)
    );
}

Token Parser::consumeType() {
    if (match(
            {Token::Type::BOOL_TYPE,
             Token::Type::INTEGER_TYPE,
             Token::Type::FLOAT_TYPE,
             Token::Type::COLOR_TYPE}
        )) {
        return previous();
    } else {
        error(
            "expected type token instead received {}",
            peek().toString()
        );
    }
}

Token Parser::consumeIdentifier() {
    if (match({Token::Type::IDENTIFIER}))
        return previous();
    else
        error(
            "expected identifier token "
            "instead received {}",
            peek().toString()
        );
}

void Parser::initWindow() {
    for (int i = 0; i < LOOKAHEAD; i++) {
        mTokenBuffer[i] = nextToken();
    }

    mPreviousToken = mTokenBuffer[0];
}

void Parser::moveWindow() {
    mPreviousToken = mTokenBuffer[0];

    for (int i = 1; i < LOOKAHEAD; i++) {
        mTokenBuffer[i - 1] = mTokenBuffer[i];
    }

    mTokenBuffer[LOOKAHEAD - 1] = nextToken();
}

Token Parser::nextToken() {
    std::optional<Token> token{};

    do {
        token = mLexer.nextToken();

    } while (!token.has_value() ||
             token->getType() == Token::Type::WHITESPACE ||
             token->getType() == Token::Type::COMMENT);

    return *token;
}

Token Parser::peek() {
    return peek(0);
}

Token Parser::peek(int lookahead) {
    abortIf(
        !(0 <= lookahead && lookahead < LOOKAHEAD),
        "exceeded lookahead of {}", LOOKAHEAD
    );

    return mTokenBuffer[lookahead];
}

Token Parser::advance() {
    moveWindow();

    return mPreviousToken;
}

Token Parser::previous() {
    return mPreviousToken;
}

bool Parser::isAtEnd() {
    return peek().getType() == Token::Type::END_OF_FILE;
}

bool Parser::check(Token::Type type) {
    if (isAtEnd()) {
        return false;
    }

    return peek().getType() == type;
}

bool Parser::peekMatch(
    std::initializer_list<Token::Type> const &types
) {
    return std::any_of(
        types.begin(), types.end(),
        [&](auto type) {
            return check(type);
        }
    );
}

bool Parser::match(
    std::initializer_list<Token::Type> const &types
) {
    if (peekMatch(types)) {
        advance();

        return true;
    }

    return false;
}

// NOTE: synchronization is all best effort

void Parser::synchronize() {
    while (!isAtEnd()) {
        Token peekToken = peek();

        switch (peekToken.getType()) {
            case Token::Type::SEMICOLON:
                advance();

                return;
            case Token::Type::FOR:
                /* fall through */
            case Token::Type::FUN:
                /* fall through */
            case Token::Type::IF:
                /* fall through */
            case Token::Type::LET:
                /* fall through */
            case Token::Type::RETURN:
                /* fall through */
            case Token::Type::WHILE:
                /* fall through */
                return;

            case Token::Type::BUILTIN: {
                auto builtinType = peekToken.as<Builtin>();

                switch (builtinType) {
                    case Builtin::PRINT:
                        /* fall through */
                    case Builtin::DELAY:
                        /* fall through */
                    case Builtin::WRITE:
                        /* fall through */
                    case Builtin::CLEAR:
                        /* fall through */
                    case Builtin::WRITE_BOX:
                        return;
                    default:;  // Do nothing
                }
            }
            default:;  // Do nothing
        }

        advance();
    }
}

}  // namespace PArL
