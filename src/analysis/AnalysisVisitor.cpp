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
                error(
                    expr->oper,
                    "operator {} expects boolean operands",
                    expr->oper.getLexeme()
                );
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
                error(
                    expr->oper,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme()
                );
            }

            mReturn = FundamentalType::BOOL;
            break;
        case Token::Type::PLUS:
        case Token::Type::MINUS:
        case Token::Type::STAR:
            if (leftSig != rightSig) {
                error(
                    expr->oper,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme()
                );
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
                error(
                    expr->oper,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme()
                );
            }

            mReturn = leftSig;
            break;
        default:
            abort("unreachable");
    }

    optionalCast(expr);
}

void AnalysisVisitor::visitLiteral(Literal *expr) {
    mReturn =
        Signature::createLiteralSignature(expr->value);

    optionalCast(expr);
}

void AnalysisVisitor::visitVariable(Variable *expr) {
    SymbolTable *scope = mSymbolStack.currentScope();

    SymbolTable *terminatinScope = scope;

    while (terminatinScope->getType() !=
               SymbolTable::Type::GLOBAL &&
           terminatinScope->getType() !=
               SymbolTable::Type::FUNCTION) {
        terminatinScope = terminatinScope->getEnclosing();
    }

    std::optional<Signature> signature{};

    for (;;) {
        signature =
            scope->findIdentifier(expr->name.getLexeme());

        if (signature.has_value() ||
            scope == terminatinScope)
            break;

        scope = scope->getEnclosing();
    }

    if (!signature.has_value()) {
        error(
            expr->name, "{} is an undefined",
            expr->name.getLexeme()
        );
    }

    if (!signature->is<LiteralSignature>()) {
        error(
            expr->name,
            "function {} is being used as a variable",
            expr->name.getLexeme()
        );
    }

    mReturn = signature.value();

    optionalCast(expr);
}

void AnalysisVisitor::visitUnary(Unary *expr) {
    expr->expr->accept(this);

    switch (expr->oper.getType()) {
        case Token::Type::NOT:
            if (mReturn != FundamentalType::BOOL) {
                error(
                    expr->oper,
                    "operator {} expects boolean operand",
                    expr->oper.getLexeme()
                );
            }
            break;
        case Token::Type::MINUS:
            if (mReturn == FundamentalType::BOOL) {
                error(
                    expr->oper,
                    "operator {} does not expect "
                    "boolean operand",
                    expr->oper.getLexeme()
                );
            }
            break;
        default:
            abort("unreachable");
    }

    optionalCast(expr);
}

void AnalysisVisitor::visitFunctionCall(FunctionCall *expr
) {
    SymbolTable *scope = mSymbolStack.currentScope();

    std::optional<Signature> signature{};

    for (;;) {
        signature = scope->findIdentifier(
            expr->identifier.getLexeme()
        );

        scope = scope->getEnclosing();

        if (signature.has_value() || scope == nullptr)
            break;
    }

    if (!signature.has_value()) {
        error(
            expr->identifier,
            "{}(...) is an "
            "undefined function",
            expr->identifier.getLexeme()
        );
    }

    if (!signature->is<FunctionSignature>()) {
        error(
            expr->identifier,
            "variable {} is being used as a function",
            expr->identifier.getLexeme()
        );
    }

    auto funcSig = signature->as<FunctionSignature>();

    std::vector<Signature> actualParamTypes{};

    for (auto &param : expr->params) {
        param->accept(this);

        actualParamTypes.push_back(mReturn);
    }

    if (funcSig.paramTypes.size() !=
        actualParamTypes.size()) {
        error(
            expr->identifier,
            "function {}(...) received {} parameters, "
            "expected {}",
            expr->identifier.getLexeme(),
            expr->params.size(), funcSig.paramTypes.size()
        );
    }

    for (int i = 0; i < actualParamTypes.size(); i++) {
        if (actualParamTypes[i] != funcSig.paramTypes[i]) {
            error(
                expr->identifier,
                "function {}(...) received parameter "
                "of unexpected type",
                expr->identifier.getLexeme()
            );
        }
    }

    mReturn = funcSig.returnType;

    optionalCast(expr);
}

void AnalysisVisitor::visitBuiltinWidth(BuiltinWidth *expr
) {
    mReturn = FundamentalType::INTEGER;
}

void AnalysisVisitor::visitBuiltinHeight(BuiltinHeight *expr
) {
    mReturn = FundamentalType::INTEGER;
}

void AnalysisVisitor::visitBuiltinRead(BuiltinRead *expr) {
    expr->x->accept(this);
    Signature xSig = mReturn;

    expr->y->accept(this);
    Signature ySig = mReturn;

    if (xSig != FundamentalType::INTEGER) {
        error(
            expr->token, "__read expects x to be an integer"
        );
    }

    if (ySig != FundamentalType::INTEGER) {
        error(
            expr->token, "__read expects y to be an integer"
        );
    }

    mReturn = FundamentalType::COLOR;

    optionalCast(expr);
}

void AnalysisVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr
) {
    expr->max->accept(this);
    Signature maxSig = mReturn;

    if (maxSig != FundamentalType::INTEGER) {
        error(
            expr->token,
            "__random_int expects max to be an integer"
        );
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
        error(
            stmt->token,
            "__delay expects delay to be an integer"
        );
    }
}

void AnalysisVisitor::visitWriteBoxStmt(WriteBoxStmt *stmt
) {
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
        error(
            stmt->token,
            "__write_box expects x to be an integer"
        );
    }

    if (yCSig != FundamentalType::INTEGER) {
        error(
            stmt->token,
            "__write_box expects y to be an integer"
        );
    }

    if (xOSig != FundamentalType::INTEGER) {
        error(
            stmt->token,
            "__write_box expects xOffset to be an integer"
        );
    }

    if (yOSig != FundamentalType::INTEGER) {
        error(
            stmt->token,
            "__write_box expects yOffset to be an integer"
        );
    }

    if (colorSig != FundamentalType::COLOR) {
        error(stmt->token, "__write_box expects a color");
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
        error(
            stmt->token,
            "__write expects x to be an integer"
        );
    }

    if (yCSig != FundamentalType::INTEGER) {
        error(
            stmt->token,
            "__write expects y to be an integer"
        );
    }

    if (colorSig != FundamentalType::COLOR) {
        error(
            stmt->token,
            "__write expects color to be a color"
        );
    }
}

void AnalysisVisitor::visitClearStmt(ClearStmt *stmt) {
    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (colorSig != FundamentalType::COLOR) {
        error(
            stmt->token,
            "__clear expects color to be a color"
        );
    }
}

void AnalysisVisitor::visitAssignment(Assignment *stmt) {
    SymbolTable *scope = mSymbolStack.currentScope();

    SymbolTable *terminatinScope = scope;

    while (terminatinScope->getType() !=
               SymbolTable::Type::GLOBAL &&
           terminatinScope->getType() !=
               SymbolTable::Type::FUNCTION) {
        terminatinScope = terminatinScope->getEnclosing();
    }

    std::optional<Signature> leftSignature{};

    for (;;) {
        leftSignature = scope->findIdentifier(
            stmt->identifier.getLexeme()
        );

        if (leftSignature.has_value() ||
            scope == terminatinScope)
            break;

        scope = scope->getEnclosing();
    }

    if (!leftSignature.has_value()) {
        error(
            stmt->identifier, "{} is an undefined variable",
            stmt->identifier.getLexeme()
        );
    }

    if (!leftSignature->is<LiteralSignature>()) {
        error(
            stmt->identifier,
            "function {} is being used as a "
            "variable in assignment",
            stmt->identifier.getLexeme()
        );
    }

    stmt->expr->accept(this);

    Signature rightSignature = mReturn;

    if (leftSignature.value() != rightSignature) {
        error(
            stmt->identifier,
            "right-hand side of {} assignment is not of "
            "correct type",
            stmt->identifier.getLexeme()
        );
    }
}

void AnalysisVisitor::visitVariableDecl(VariableDecl *stmt
) {
    Signature signature =
        Signature::createLiteralSignature(stmt->type);

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(stmt->identifier.getLexeme())
            .has_value()) {
        error(
            stmt->identifier, "redeclarantion of {}",
            stmt->identifier.getLexeme()
        );
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(
                stmt->identifier.getLexeme()
            );

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSignature>()) {
            error(
                stmt->identifier,
                "redeclaration of {}(...) as a variable",
                stmt->identifier.getLexeme()
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        stmt->identifier.getLexeme(), signature
    );

    stmt->expr->accept(this);
    Signature rightSig = mReturn;

    if (signature != rightSig) {
        error(
            stmt->identifier,
            "right-hand side of {} those not have the "
            "expectedl type",
            stmt->identifier.getLexeme()
        );
    }
}

void AnalysisVisitor::visitBlock(Block *stmt) {
    mSymbolStack.pushScope().setType(
        SymbolTable::Type::BLOCK
    );

    unscopedBlock(stmt);

    mSymbolStack.popScope();
}

void AnalysisVisitor::visitIfStmt(IfStmt *stmt) {
    try {
        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            error(
                stmt->token, "if expects boolean condition"
            );
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
            SymbolTable::Type::ELSE
        );

        unscopedBlock(stmt->ifElse.get());

        mSymbolStack.popScope();

        elseBranch = mBranchReturns;
    }

    mBranchReturns = ifBranch && elseBranch;
}

void AnalysisVisitor::visitForStmt(ForStmt *stmt) {
    mSymbolStack.pushScope().setType(SymbolTable::Type::FOR
    );

    try {
        if (stmt->varDecl) {
            stmt->varDecl->accept(this);
        }

        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            error(
                stmt->token, "for expects boolean condition"
            );
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

void AnalysisVisitor::visitWhileStmt(WhileStmt *stmt) {
    try {
        stmt->expr->accept(this);
        Signature condSig = mReturn;

        if (condSig != FundamentalType::BOOL) {
            error(
                stmt->token,
                "while expects boolean condition"
            );
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    mSymbolStack.pushScope().setType(
        SymbolTable::Type::WHILE
    );

    unscopedBlock(stmt->block.get());

    mSymbolStack.popScope();

    mBranchReturns = false;
}

void AnalysisVisitor::visitReturnStmt(ReturnStmt *stmt) {
    mBranchReturns = true;

    stmt->expr->accept(this);
    Signature exprSignature = mReturn;

    SymbolTable *scope = mSymbolStack.currentScope();

    for (;;) {
        if (scope == nullptr) {
            error(
                stmt->token,
                "return statement must be within a "
                "function block"
            );
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
        error(
            stmt->token,
            "incorrect return type in function {}",
            enclosingFunction
        );
    }
}

void AnalysisVisitor::visitFormalParam(FormalParam *param) {
    Signature signature =
        Signature::createLiteralSignature(param->type);

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(param->identifier.getLexeme())
            .has_value()) {
        error(
            param->identifier, "redeclarantion of {}",
            param->identifier.getLexeme()
        );
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(
                param->identifier.getLexeme()
            );

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSignature>()) {
            error(
                param->identifier,
                "redeclaration of {}(...) as a parameter",
                param->identifier.getLexeme()
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        param->identifier.getLexeme(), signature
    );
}

void AnalysisVisitor::visitFunctionDecl(FunctionDecl *stmt
) {
    bool isGlobalScope =
        mSymbolStack.isCurrentScopeGlobal();

    if (!isGlobalScope) {
        error(
            stmt->identifier,
            "function declaration {}(...) is not allowed "
            "here",
            stmt->identifier.getLexeme()
        );
    }

    std::vector<Token> paramTypes{stmt->params.size()};

    for (size_t i = 0; i < paramTypes.size(); i++) {
        paramTypes[i] = stmt->params[i]->type;
    }

    Signature signature =
        Signature::createFunctionSignature(
            paramTypes, stmt->type
        );

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(stmt->identifier.getLexeme())
            .has_value()) {
        error(
            stmt->identifier, "redeclarantion of {}",
            stmt->identifier.getLexeme()
        );
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(
                stmt->identifier.getLexeme()
            );

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSignature>()) {
            error(
                stmt->identifier,
                "redeclaration of {}(...)",
                stmt->identifier.getLexeme()
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        stmt->identifier.getLexeme(), signature
    );

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
        error(
            stmt->identifier,
            "{}(...) does not return a value in all "
            "control paths",
            stmt->identifier.getLexeme()
        );
    }

    mBranchReturns = false;
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

void AnalysisVisitor::unscopedBlock(Block *block) {
    for (auto &stmt : block->stmts) {
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
            expr->type.value()
        );
    }
}

bool AnalysisVisitor::hasError() const {
    return mHasError;
}

void AnalysisVisitor::reset() {
    mHasError = false;
    mBranchReturns = false;
    mReturn = {};
    mSymbolStack = {};
}

}  // namespace PArL
