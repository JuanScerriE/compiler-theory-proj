// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// parl
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>
#include <common/AST.hpp>
#include <common/Abort.hpp>
#include <common/Token.hpp>
#include <cwchar>
#include <ir_gen/GenVisitor.hpp>

// std
#include <memory>

namespace PArL {

void GenVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);
}

void GenVisitor::visitBinary(Binary *expr) {
    expr->right->accept(this);
    expr->left->accept(this);

    switch (expr->oper.getType()) {
        case Token::Type::AND:
            emitLine("and");
            break;
        case Token::Type::OR:
            emitLine("or");
            break;
        case Token::Type::LESS:
            emitLine("lt");
            break;
        case Token::Type::GREATER:
            emitLine("gt");
            break;
        case Token::Type::EQUAL_EQUAL:
            emitLine("eq");
            break;
        case Token::Type::BANG_EQUAL:
            emitLine("neq");
            break;
        case Token::Type::LESS_EQUAL:
            emitLine("le");
            break;
        case Token::Type::GREATER_EQUAL:
            emitLine("ge");
            break;
        case Token::Type::PLUS:
            emitLine("add");
            break;
        case Token::Type::MINUS:
            emitLine("sub");
            break;
        case Token::Type::STAR:
            emitLine("mul");
            break;
        case Token::Type::SLASH:
            emitLine("div");
            break;
        default:
            abort("unreachable");
    }
}

void GenVisitor::visitLiteral(Literal *expr) {
    emitLine(
        fmt::format("push {}", expr->value.getLexeme()));
}

void GenVisitor::visitVariable(Variable *) {
    // SymbolTable *scope = mSymbolStack.currentScope();
    //
    // SymbolTable *terminatinScope = scope;
    //
    // while (terminatinScope->getType() !=
    //            SymbolTable::Type::GLOBAL &&
    //        terminatinScope->getType() !=
    //            SymbolTable::Type::FUNCTION) {
    //     terminatinScope =
    //     terminatinScope->getEnclosing();
    // }
    //
    // std::optional<Signature> signature{};
    //
    // for (;;) {
    //     signature =
    //         scope->findIdentifier(expr->name.getLexeme());
    //
    //     if (signature.has_value() ||
    //         scope == terminatinScope)
    //         break;
    //
    //     scope = scope->getEnclosing();
    // }
    //
    // if (!signature.has_value()) {
    //     error(expr->name, "{} is an undefined",
    //           expr->name.getLexeme());
    // }
    //
    // if (!signature->is<LiteralSignature>()) {
    //     error(expr->name,
    //           "function {} is being used as a variable",
    //           expr->name.getLexeme());
    // }
    //
    // mReturn = signature.value();
    //
    // optionalCast(expr);
}

void GenVisitor::visitUnary(Unary *expr) {
    expr->expr->accept(this);

    switch (expr->oper.getType()) {
        case Token::Type::NOT:
            emitLine("not");
            break;
        case Token::Type::MINUS:
            emitLine("push -1");
            emitLine("mul");
            break;
        default:
            abort("unreachable");
    }
}

void GenVisitor::visitFunctionCall(FunctionCall *) {
    // SymbolTable *scope = mSymbolStack.currentScope();
    //
    // std::optional<Signature> signature{};
    //
    // for (;;) {
    //     signature = scope->findIdentifier(
    //         expr->identifier.getLexeme());
    //
    //     scope = scope->getEnclosing();
    //
    //     if (signature.has_value() || scope == nullptr)
    //         break;
    // }
    //
    // if (!signature.has_value()) {
    //     error(expr->identifier,
    //           "{}(...) is an "
    //           "undefined function",
    //           expr->identifier.getLexeme());
    // }
    //
    // if (!signature->is<FunctionSignature>()) {
    //     error(expr->identifier,
    //           "variable {} is being used as a function",
    //           expr->identifier.getLexeme());
    // }
    //
    // auto funcSig = signature->as<FunctionSignature>();
    //
    // std::vector<Signature> actualParamTypes{};
    //
    // for (auto &param : expr->params) {
    //     param->accept(this);
    //
    //     actualParamTypes.push_back(mReturn);
    // }
    //
    // if (funcSig.paramTypes.size() !=
    //     actualParamTypes.size()) {
    //     error(expr->identifier,
    //           "function {}(...) received {} parameters, "
    //           "expected {}",
    //           expr->identifier.getLexeme(),
    //           expr->params.size(),
    //           funcSig.paramTypes.size());
    // }
    //
    // for (int i = 0; i < actualParamTypes.size(); i++) {
    //     if (actualParamTypes[i] != funcSig.paramTypes[i])
    //     {
    //         error(expr->identifier,
    //               "function {}(...) received parameter "
    //               "of unexpected type",
    //               expr->identifier.getLexeme());
    //     }
    // }
    //
    // mReturn = funcSig.returnType;
    //
    // optionalCast(expr);
}

void GenVisitor::visitBuiltinWidth(BuiltinWidth *) {
    emitLine("width");
}

void GenVisitor::visitBuiltinHeight(BuiltinHeight *) {
    emitLine("height");
}

void GenVisitor::visitBuiltinRead(BuiltinRead *expr) {
    expr->y->accept(this);
    expr->x->accept(this);

    emitLine("read");
}

void GenVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr) {
    expr->max->accept(this);

    emitLine("irnd");
}

void GenVisitor::visitPrintStmt(PrintStmt *stmt) {
    stmt->expr->accept(this);

    emitLine("print");
}

void GenVisitor::visitDelayStmt(DelayStmt *stmt) {
    stmt->expr->accept(this);

    emitLine("delay");
}

void GenVisitor::visitWriteBoxStmt(WriteBoxStmt *stmt) {
    stmt->color->accept(this);
    stmt->yOffset->accept(this);
    stmt->xOffset->accept(this);
    stmt->yCoor->accept(this);
    stmt->xCoor->accept(this);

    emitLine("writebox");
}

void GenVisitor::visitWriteStmt(WriteStmt *stmt) {
    stmt->color->accept(this);
    stmt->yCoor->accept(this);
    stmt->xCoor->accept(this);

    emitLine("writebox");
}

void GenVisitor::visitClearStmt(ClearStmt *stmt) {
    stmt->color->accept(this);

    emitLine("clear");
}

void GenVisitor::visitAssignment(Assignment *) {
    // SymbolTable *scope = mSymbolStack.currentScope();
    //
    // SymbolTable *terminatinScope = scope;
    //
    // while (terminatinScope->getType() !=
    //            SymbolTable::Type::GLOBAL &&
    //        terminatinScope->getType() !=
    //            SymbolTable::Type::FUNCTION) {
    //     terminatinScope = terminatinScope->getEnclosing();
    // }
    //
    // std::optional<Signature> leftSignature{};
    //
    // for (;;) {
    //     leftSignature = scope->findIdentifier(
    //         stmt->identifier.getLexeme());
    //
    //     if (leftSignature.has_value() ||
    //         scope == terminatinScope)
    //         break;
    //
    //     scope = scope->getEnclosing();
    // }
    //
    // if (!leftSignature.has_value()) {
    //     error(stmt->identifier,
    //           "{} is an undefined variable",
    //           stmt->identifier.getLexeme());
    // }
    //
    // if (!leftSignature->is<LiteralSignature>()) {
    //     error(stmt->identifier,
    //           "function {} is being used as a "
    //           "variable in assignment",
    //           stmt->identifier.getLexeme());
    // }
    //
    // stmt->expr->accept(this);
    //
    // Signature rightSignature = mReturn;
    //
    // if (leftSignature.value() != rightSignature) {
    //     error(stmt->identifier,
    //           "right-hand side of {} assignment is not of "
    //           "correct type",
    //           stmt->identifier.getLexeme());
    // }
}

void GenVisitor::visitVariableDecl(VariableDecl *) {
    // Signature signature =
    //     Signature::createLiteralSignature(stmt->type);
    //
    // SymbolTable *scope = mSymbolStack.currentScope();
    //
    // if (scope->findIdentifier(stmt->identifier.getLexeme())
    //         .has_value()) {
    //     error(stmt->identifier, "redeclarantion of {}",
    //           stmt->identifier.getLexeme());
    // }
    //
    // SymbolTable *enclosing = scope->getEnclosing();
    //
    // for (;;) {
    //     if (enclosing == nullptr)
    //         break;
    //
    //     std::optional<Signature> identifierSignature =
    //         enclosing->findIdentifier(
    //             stmt->identifier.getLexeme());
    //
    //     if (identifierSignature.has_value() &&
    //         identifierSignature->is<FunctionSignature>()) {
    //         error(stmt->identifier,
    //               "redeclaration of {}(...) as a variable",
    //               stmt->identifier.getLexeme());
    //     }
    //
    //     enclosing = enclosing->getEnclosing();
    // }
    //
    // scope->addIdentifier(stmt->identifier.getLexeme(),
    //                      signature);
    //
    // stmt->expr->accept(this);
    // Signature rightSig = mReturn;
    //
    // if (signature != rightSig) {
    //     error(stmt->identifier,
    //           "right-hand side of {} those not have the "
    //           "expectedl type",
    //           stmt->identifier.getLexeme());
    // }
}

void GenVisitor::visitBlock(Block *stmt) {
    size_t count = mCounter.count(stmt);

    emitLine(fmt::format("push {}", count));

    emitLine("oframe");

    // mSymbolStack.pushScope().setType(
    //     SymbolTable::Type::BLOCK);
    //
    // unscopedBlock(stmt);
    //
    // mSymbolStack.popScope();

    emitLine("cframe");
}

void GenVisitor::visitIfStmt(IfStmt *stmt) {
    try {
        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            error(stmt->token,
                  "if expects boolean condition");
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    mSymbolStack.pushScope().setType(SymbolTable::Type::IF);

    unscopedBlock(stmt->ifThen.get());

    mSymbolStack.popScope();

    bool ifBranch = mBranchReturns;

    bool elseBranch = false;

    if (stmt->ifElse) {
        mSymbolStack.pushScope().setType(
            SymbolTable::Type::ELSE);

        unscopedBlock(stmt->ifElse.get());

        mSymbolStack.popScope();

        elseBranch = mBranchReturns;
    }

    mBranchReturns = ifBranch && elseBranch;
}

void GenVisitor::visitForStmt(ForStmt *stmt) {
    mSymbolStack.pushScope().setType(
        SymbolTable::Type::FOR);

    try {
        if (stmt->varDecl) {
            stmt->varDecl->accept(this);
        }

        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            error(stmt->token,
                  "for expects boolean condition");
        }

        if (stmt->assignment) {
            stmt->assignment->accept(this);
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    unscopedBlock(stmt->block.get());

    mSymbolStack.popScope();

    mBranchReturns = false;
}

void GenVisitor::visitWhileStmt(WhileStmt *stmt) {
    try {
        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            error(stmt->token,
                  "while expects boolean condition");
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    mSymbolStack.pushScope().setType(
        SymbolTable::Type::WHILE);

    unscopedBlock(stmt->block.get());

    mSymbolStack.popScope();

    mBranchReturns = false;
}

void GenVisitor::visitReturnStmt(ReturnStmt *stmt) {
    mBranchReturns = true;

    stmt->expr->accept(this);
    Signature exprSignature = mReturn;

    SymbolTable *scope = mSymbolStack.currentScope();

    for (;;) {
        if (scope == nullptr) {
            error(stmt->token,
                  "return statement must be within a "
                  "function block");
        }

        if (scope->getType() ==
            SymbolTable::Type::FUNCTION) {
            break;
        }

        scope = scope->getEnclosing();
    }

    std::string enclosingFunction =
        scope->getName().value();

    auto functionSignature =
        scope->getEnclosing()
            ->findIdentifier(enclosingFunction)
            ->as<FunctionSignature>();

    if (exprSignature != functionSignature.returnType) {
        error(stmt->token,
              "incorrect return type in function {}",
              enclosingFunction);
    }
}

void GenVisitor::visitFormalParam(FormalParam *param) {
    Signature signature =
        Signature::createLiteralSignature(param->type);

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(param->identifier.getLexeme())
            .has_value()) {
        error(param->identifier, "redeclarantion of {}",
              param->identifier.getLexeme());
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(
                param->identifier.getLexeme());

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSignature>()) {
            error(param->identifier,
                  "redeclaration of {}(...) as a parameter",
                  param->identifier.getLexeme());
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(param->identifier.getLexeme(),
                         signature);
}

void GenVisitor::visitFunctionDecl(FunctionDecl *stmt) {
    // TODO: make sure that it actually only happens if you
    // are in global scope but for now allow it in all scope
    // levels.
    std::vector<Token> paramTypes{stmt->params.size()};

    for (size_t i = 0; i < paramTypes.size(); i++) {
        paramTypes[i] = stmt->params[i]->type;
    }

    Signature signature =
        Signature::createFunctionSignature(paramTypes,
                                           stmt->type);

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(stmt->identifier.getLexeme())
            .has_value()) {
        error(stmt->identifier, "redeclarantion of {}",
              stmt->identifier.getLexeme());
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(
                stmt->identifier.getLexeme());

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSignature>()) {
            error(stmt->identifier,
                  "redeclaration of {}(...)",
                  stmt->identifier.getLexeme());
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(stmt->identifier.getLexeme(),
                         signature);

    mBranchReturns = false;

    mSymbolStack.pushScope()
        .setType(SymbolTable::Type::FUNCTION)
        .setName(stmt->identifier.getLexeme());

    for (auto &param : stmt->params) {
        try {
            param->accept(this);
        } catch (SyncAnalysis &) {
            // noop
        }
    }

    unscopedBlock(stmt->block.get());

    mSymbolStack.popScope();

    if (!mBranchReturns) {
        error(stmt->identifier,
              "{}(...) does not return a value in all "
              "control paths",
              stmt->identifier.getLexeme());
    }

    mBranchReturns = false;
}

void GenVisitor::visitProgram(Program *prog) {
    for (auto &stmt : prog->stmts) {
        try {
            stmt->accept(this);
        } catch (SyncAnalysis &) {
            // noop
        }
    }
}

void GenVisitor::unscopedBlock(Block *block) {
    for (auto &stmt : block->stmts) {
        try {
            stmt->accept(this);
        } catch (SyncAnalysis &) {
            // noop
        }
    }
}

void GenVisitor::optionalCast(Expr *expr) {
    if (expr->type.has_value()) {
        mReturn = Signature::createLiteralSignature(
            expr->type.value());
    }
}

void emit(std::string const &line) {
    mCode.push_back(line);

    mPC++;
}

bool GenVisitor::hasError() const {
    return mHasError;
}

void GenVisitor::reset() {
    mHasError = false;
    mBranchReturns = false;
    mReturn = {};
    mSymbolStack = {};
}

}  // namespace PArL
