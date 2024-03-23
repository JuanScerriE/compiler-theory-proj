// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <analysis/AnalysisVisitor.hpp>
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>
#include <common/AST.hpp>
#include <common/Abort.hpp>
#include <common/Token.hpp>

// std
#include <memory>

// definitions
#ifdef INTERNAL_DEBUG

#define ERROR(token, msg, ...)                            \
    do {                                                  \
        error(token, fmt::format(__FILE__ ":" LINE_STRING \
                                          ":: " msg,      \
                                 ##__VA_ARGS__));         \
    } while (0)

#else

#define ERROR(token, msg, ...)                         \
    do {                                               \
        error(token, fmt::format(msg, ##__VA_ARGS__)); \
    } while (0)

#endif

namespace PArL {

void AnalysisVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);

    optionalCast(expr);
}

void AnalysisVisitor::visitBinary(Binary *expr) {
    expr->left->accept(this);
    Signature leftSig = mReturn;

    expr->right->accept(this);
    Signature rightSig = mReturn;

    switch (expr->oper.getType()) {
        case Token::Type::AND:
        case Token::Type::OR:
            if (!(leftSig == FundamentalType::BOOL &&
                  rightSig == FundamentalType::BOOL)) {
                ERROR(
                    expr->oper,
                    "operator {} expects boolean operands",
                    expr->oper.getLexeme());
            }

            mReturn = FundamentalType::BOOL;
            break;
        case Token::Type::LESS:
        case Token::Type::GREATER:
        case Token::Type::EQUAL_EQUAL:
        case Token::Type::BANG_EQUAL:
        case Token::Type::LESS_EQUAL:
        case Token::Type::GREATER_EQUAL:
            if (leftSig != rightSig) {
                ERROR(
                    expr->oper,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme());
            }

            mReturn = FundamentalType::BOOL;
            break;
        case Token::Type::PLUS:
        case Token::Type::MINUS:
        case Token::Type::STAR:
            if (leftSig != rightSig) {
                ERROR(
                    expr->oper,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme());
            }

            mReturn = leftSig;
            break;

            // NOTE: Quotient division in the VM is
            // performed as follows:
            // push 7
            // push 15
            // push 7
            // push 15
            // mod
            // sub
            // div
            // print
            // in the above case is are doing 15 // 7
        case Token::Type::SLASH:
            if (leftSig != rightSig) {
                ERROR(
                    expr->oper,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme());
            }

            mReturn = leftSig;
            break;
        default:
            ABORT("unreachable");
    }

    optionalCast(expr);
}

void AnalysisVisitor::visitLiteral(Literal *expr) {
    mReturn =
        Signature::createLiteralSignature(expr->value);

    optionalCast(expr);
}

void AnalysisVisitor::visitVariable(Variable *expr) {
    std::optional<Signature> sig =
        mSymbolStack.findIdentifier(expr->name.getLexeme());

    if (!sig.has_value()) {
        ERROR(expr->name, "{} is an undefined within scope",
              expr->name.getLexeme());
    } else if (!sig->is<LiteralSignature>()) {
        ERROR(expr->name,
              "function {} is being used as a variable",
              expr->name.getLexeme());
    }

    mReturn = sig.value();

    optionalCast(expr);
}

void AnalysisVisitor::visitUnary(Unary *expr) {
    expr->expr->accept(this);

    switch (expr->oper.getType()) {
        case Token::Type::NOT:
            if (mReturn != FundamentalType::BOOL) {
                ERROR(expr->oper,
                      "operator {} expects boolean operand",
                      expr->oper.getLexeme());
            }
            break;
        case Token::Type::MINUS:
            if (mReturn == FundamentalType::BOOL) {
                ERROR(expr->oper,
                      "operator {} does not expect "
                      "boolean operand",
                      expr->oper.getLexeme());
            }
            break;
        default:
            ABORT("unreachable");
    }

    optionalCast(expr);
}

void AnalysisVisitor::visitFunctionCall(
    FunctionCall *expr) {
    std::optional<Signature> sig =
        mSymbolStack.findIdentifier(
            expr->identifier.getLexeme());

    if (!sig.has_value()) {
        ERROR(expr->identifier,
              "{}(...) is an "
              "undefined function",
              expr->identifier.getLexeme());
    } else if (!sig->is<FunctionSignature>()) {
        ERROR(expr->identifier,
              "variable {} is being used as a function",
              expr->identifier.getLexeme());
    } else {
        auto funcSig = sig->as<FunctionSignature>();

        std::vector<Signature> actualParamTypes{};

        for (auto &param : expr->params) {
            param->accept(this);

            actualParamTypes.push_back(mReturn);
        }

        if (funcSig.paramTypes.size() !=
            actualParamTypes.size()) {
            ERROR(
                expr->identifier,
                "function {}(...) received {} parameters, "
                "expected {}",
                expr->identifier.getLexeme(),
                expr->params.size(),
                funcSig.paramTypes.size());
        }

        for (int i = 0; i < actualParamTypes.size(); i++) {
            if (actualParamTypes[i] !=
                funcSig.paramTypes[i]) {
                ERROR(expr->identifier,
                      "function {}(...) received parameter "
                      "of unexpected type",
                      expr->identifier.getLexeme());
            }
        }

        mReturn = funcSig.returnType;

        optionalCast(expr);
    }
}

void AnalysisVisitor::visitBuiltinWidth(
    BuiltinWidth *expr) {
    mReturn = FundamentalType::INTEGER;
}

void AnalysisVisitor::visitBuiltinHeight(
    BuiltinHeight *expr) {
    mReturn = FundamentalType::INTEGER;
}

void AnalysisVisitor::visitBuiltinRead(BuiltinRead *expr) {
    expr->x->accept(this);
    Signature xSig = mReturn;

    expr->y->accept(this);
    Signature ySig = mReturn;

    if (xSig != FundamentalType::INTEGER) {
        ERROR(expr->token,
              "__read expects x to be an integer");
    }

    if (ySig != FundamentalType::INTEGER) {
        ERROR(expr->token,
              "__read expects y to be an integer");
    }

    mReturn = FundamentalType::COLOR;

    optionalCast(expr);
}

void AnalysisVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr) {
    expr->max->accept(this);
    Signature maxSig = mReturn;

    if (maxSig != FundamentalType::INTEGER) {
        ERROR(expr->token,
              "__random_int expects max to be an integer");
    }

    mReturn = FundamentalType::INTEGER;

    optionalCast(expr);
}

void AnalysisVisitor::visitPrintStmt(PrintStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visitDelayStmt(DelayStmt *stmt) {
    stmt->expr->accept(this);
    Signature delaySig = mReturn;

    if (delaySig != FundamentalType::INTEGER) {
        ERROR(stmt->token,
              "__delay expects delay to be an integer");
    }
}

void AnalysisVisitor::visitWriteBoxStmt(
    WriteBoxStmt *stmt) {
    stmt->xCoor->accept(this);
    Signature xCSig = mReturn;

    stmt->yCoor->accept(this);
    Signature yCSig = mReturn;

    stmt->xOffset->accept(this);
    Signature xOSig = mReturn;

    stmt->yOffset->accept(this);
    Signature yOSig = mReturn;

    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (xCSig != FundamentalType::INTEGER) {
        ERROR(stmt->token,
              "__write_box expects x to be an integer");
    }

    if (yCSig != FundamentalType::INTEGER) {
        ERROR(stmt->token,
              "__write_box expects y to be an integer");
    }

    if (xOSig != FundamentalType::INTEGER) {
        ERROR(
            stmt->token,
            "__write_box expects xOffset to be an integer");
    }

    if (yOSig != FundamentalType::INTEGER) {
        ERROR(
            stmt->token,
            "__write_box expects yOffset to be an integer");
    }

    if (colorSig != FundamentalType::COLOR) {
        ERROR(stmt->token,
              "__write_box expects color to be a color");
    }
}

void AnalysisVisitor::visitWriteStmt(WriteStmt *stmt) {
    stmt->xCoor->accept(this);
    Signature xCSig = mReturn;

    stmt->yCoor->accept(this);
    Signature yCSig = mReturn;

    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (xCSig != FundamentalType::INTEGER) {
        ERROR(stmt->token,
              "__write expects x to be an integer");
    }

    if (yCSig != FundamentalType::INTEGER) {
        ERROR(stmt->token,
              "__write expects y to be an integer");
    }

    if (colorSig != FundamentalType::COLOR) {
        ERROR(stmt->token,
              "__write expects color to be a color");
    }
}

void AnalysisVisitor::visitClearStmt(ClearStmt *stmt) {
    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (colorSig != FundamentalType::COLOR) {
        ERROR(stmt->token,
              "__clear expects color to be a color");
    }
}

void AnalysisVisitor::visitAssignment(Assignment *stmt) {
    std::optional<Signature> leftSig =
        mSymbolStack.findIdentifier(
            stmt->identifier.getLexeme());

    if (!leftSig.has_value()) {
        ERROR(stmt->identifier,
              "{} is an undefined variable",
              stmt->identifier.getLexeme());
    } else if (!leftSig->is<LiteralSignature>()) {
        ERROR(stmt->identifier,
              "function {} is being used as a "
              "variable in assignment",
              stmt->identifier.getLexeme());
    }

    stmt->expr->accept(this);

    Signature rightSig = mReturn;

    if (leftSig.value() != rightSig) {
        ERROR(stmt->identifier,
              "right-hand side of {} assignment is not of "
              "correct type",
              stmt->identifier.getLexeme());
    }
}

void AnalysisVisitor::visitVariableDecl(
    VariableDecl *stmt) {
    Signature signature =
        Signature::createLiteralSignature(stmt->type);

    try {
        mSymbolStack.addIdentifier(
            stmt->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        ERROR(stmt->identifier,
              "cannot repeat symbol {} in same scope",
              stmt->identifier.getLexeme());
    } catch (RuleViolation &violation) {
        ERROR(stmt->identifier, "{}", violation.what());
    }

    stmt->expr->accept(this);
    Signature rightSig = mReturn;

    if (signature != rightSig) {
        ERROR(stmt->identifier,
              "right-hand side of {} those not have the "
              "expectedl type",
              stmt->identifier.getLexeme());
    }
}

void AnalysisVisitor::visitBlock(Block *stmt) {
    mSymbolStack.pushScope();

    for (auto &stmt : stmt->stmts) {
        try {
            stmt->accept(this);
        } catch (SyncAnalysis &) {
            // noop
        }
    }

    mSymbolStack.popScope();
}

void AnalysisVisitor::visitIfStmt(IfStmt *stmt) {
    try {
        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            ERROR(stmt->token,
                  "if expects boolean condition");
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    stmt->ifThen->accept(this);

    if (stmt->ifElse) {
        stmt->ifElse->accept(this);
    }
}

void AnalysisVisitor::visitForStmt(ForStmt *stmt) {
    mSymbolStack.pushScope();

    try {
        if (stmt->varDecl) {
            stmt->varDecl->accept(this);
        }

        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            ERROR(stmt->token,
                  "for expects boolean condition");
        }

        if (stmt->assignment) {
            stmt->assignment->accept(this);
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    stmt->block->accept(this);

    mSymbolStack.popScope();
}

void AnalysisVisitor::visitWhileStmt(WhileStmt *stmt) {
    try {
        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            ERROR(stmt->token,
                  "while expects boolean condition");
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    stmt->block->accept(this);
}

void AnalysisVisitor::visitReturnStmt(ReturnStmt *stmt) {
    stmt->expr->accept(this);
    Signature exprSig = mReturn;

    auto enclosingFunc =
        mSymbolStack.getEnclosingFunction();

    if (!enclosingFunc.has_value()) {
        ERROR(stmt->token,
              "return statement must be within a "
              "function block");
    }

    auto &[identifier, funcSig] = enclosingFunc.value();

    if (exprSig !=
        funcSig.as<FunctionSignature>().returnType) {
        ERROR(stmt->token,
              "returning the incorrect type in function {}",
              identifier);
    }
}

void AnalysisVisitor::visitFormalParam(FormalParam *param) {
    Signature signature =
        Signature::createLiteralSignature(param->type);

    try {
        mSymbolStack.addIdentifier(
            param->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        ERROR(param->identifier,
              "cannot repeat sumbol {} in same scope",
              param->identifier.getLexeme());
    } catch (RuleViolation &violation) {
        ERROR(param->identifier, "{}", violation.what());
    }
}

void AnalysisVisitor::visitFunctionDecl(
    FunctionDecl *stmt) {
    // TODO: make sure that it actually only happens if you
    // are in global scope but for now allow it in all scope
    // levels.
    std::vector<Token> paramTypes;

    for (auto &param : stmt->params) {
        paramTypes.push_back(param->type);
    }

    Signature signature =
        Signature::createFunctionSignature(paramTypes,
                                           stmt->type);

    try {
        mSymbolStack.addIdentifier(
            stmt->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        ERROR(stmt->identifier,
              "cannot repeat sumbol {} in same scope",
              stmt->identifier.getLexeme());
    } catch (RuleViolation &violation) {
        ERROR(stmt->identifier, "{}", violation.what());
    }

    mSymbolStack.pushScope();

    mSymbolStack.currentScope().addInsertRule(
        {{"redeclaration of calling function is prohibited",
          [=](std::string const &ident,
              Signature const &sig) {
              return !(ident ==
                           stmt->identifier.getLexeme() &&
                       sig.is<FunctionSignature>());
          }}});
    mSymbolStack.currentScope().addSearchRule(
        {{"access to identifiers outside "
          "functions is prohibited",
          [](std::string const &ident,
             Signature const &sig) {
              return sig.is<FunctionSignature>();
          }}});

    for (auto &param : stmt->params) {
        try {
            param->accept(this);
        } catch (SyncAnalysis &) {
            // noop
        }
    }

    stmt->block->accept(this);

    mSymbolStack.popScope();
}

void AnalysisVisitor::visitProgram(Program *prog) {
    for (auto &stmt : prog->stmts) {
        try {
            stmt->accept(this);
        } catch (SyncAnalysis &) {
            // noop
        }
    }
}

void AnalysisVisitor::optionalCast(Expr *expr) {
    if (expr->type.has_value()) {
        mReturn = Signature::createLiteralSignature(
            expr->type.value());
    }
}

void AnalysisVisitor::error(Token const &token,
                            const std::string &msg) {
    mHasError = true;

    fmt::println(stderr, "semantic error at {}:{}:: {}",
                 token.getLine(), token.getColumn(), msg);

    throw SyncAnalysis{};
}

bool AnalysisVisitor::hasError() const {
    return mHasError;
}

void AnalysisVisitor::reset() {
    mHasError = false;
    mReturn = {};
    mSymbolStack = {};
}

}  // namespace PArL
