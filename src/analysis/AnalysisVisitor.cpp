// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// parl
#include <analysis/AnalysisVisitor.hpp>
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>
#include <parl/AST.hpp>
#include <parl/Core.hpp>
#include <parl/Token.hpp>

// std
#include <memory>

namespace PArL {

bool AnalysisVisitor::isViableCast(
    Signature &from,
    Signature &to
) {
    if (from.is<PrimitiveSig>() && to.is<ArraySig>()) {
        error(
            mPosition,
            "primitive cannot be cast to an array"
        );
    }

    if (from.is<ArraySig>() && to.is<PrimitiveSig>()) {
        error(
            mPosition,
            "array cannot be cast to a primitive"
        );
    }

    if (from.is<ArraySig>() && to.is<ArraySig>()) {
        if (to.as<ArraySig>()->size.has_value()) {
            error(
                mPosition,
                "array cast contains integer literal"
            );
        }
    }
}

void AnalysisVisitor::visit(core::Type *type) {
    if (!type->isArray) {
        mReturn = PrimitiveSig{type->primitive};

        return;
    }

    std::optional<size_t> size{};

    if (type->size) {
        int value = type->size->value;

        if (value < 1) {
            error(
                type->position,
                "array size must be positive"
            );
        }

        size = static_cast<size_t>(value);
    }

    mReturn = ArraySig{type->primitive, size};
}

void AnalysisVisitor::visit(core::Expr *expr) {
    if (expr->type.has_value()) {
        core::Type *type = (*expr->type).get();

        type->accept(this);

        mPosition = type->position;
    }
}

void AnalysisVisitor::visit(core::PadWidth *expr) {
    mReturn = PrimitiveSig{core::Base::INT};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PadHeight *expr) {
    mReturn = PrimitiveSig{core::Base::INT};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PadRead *expr) {
    expr->x->accept(this);
    Signature xSig = mReturn;

    expr->y->accept(this);
    Signature ySig = mReturn;

    if (xSig != PrimitiveSig{core::Base::INT}) {
        error(
            expr->position,
            "__read expects x to be an integer"
        );
    }

    if (ySig != PrimitiveSig{core::Base::INT}) {
        error(
            expr->position,
            "__read expects y to be an integer"
        );
    }

    mReturn = PrimitiveSig{core::Base::COLOR};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PadRandomInt *expr) {
    expr->max->accept(this);
    Signature maxSig = mReturn;

    if (maxSig != PrimitiveSig{core::Base::INT}) {
        error(
            expr->position,
            "__random_int expects max to be an integer"
        );
    }

    mReturn = PrimitiveSig{core::Base::INT};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::BooleanLiteral *expr) {
    mReturn = PrimitiveSig{core::Base::BOOL};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::IntegerLiteral *expr) {
    mReturn = PrimitiveSig{core::Base::INT};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::FloatLiteral *expr) {
    mReturn = PrimitiveSig{core::Base::FLOAT};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::ColorLiteral *expr) {
    mReturn = PrimitiveSig{core::Base::COLOR};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::ArrayLiteral *expr) {
    if (expr->exprs.empty()) {
        error(expr->position, "array literal is empty");
    }

    expr->exprs[0]->accept(this);
    Signature initialSig = mReturn;

    if (initialSig.is<ArraySig>()) {
        error(
            expr->position,
            "nested arrays are not supported"
        );
    }

    for (size_t i = 1; i < expr->exprs.size(); i++) {
        expr->exprs[i]->accept(this);

        if (mReturn.is<ArraySig>()) {
            error(
                expr->position,
                "nested arrays are not supported"
            );
        }

        if (initialSig != mReturn) {
            error(
                expr->position,
                "array contains multiple different types"
            );
        }
    }

    mReturn = ArraySig{
        initialSig.as<PrimitiveSig>()->type,
        expr->exprs.size()
    };
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::Variable *expr) {
    SymbolTable *scope = mSymbolStack.currentScope();

    SymbolTable *terminatingScope = scope;

    while (terminatingScope->getType() !=
               SymbolTable::Type::GLOBAL &&
           terminatingScope->getType() !=
               SymbolTable::Type::FUNCTION) {
        terminatingScope = terminatingScope->getEnclosing();
    }

    std::optional<Signature> signature{};

    for (;;) {
        signature = scope->findIdentifier(expr->identifier);

        if (signature.has_value() ||
            scope == terminatingScope)
            break;

        scope = scope->getEnclosing();
    }

    if (!signature.has_value()) {
        error(
            expr->position,
            "{} is undefined",
            expr->identifier
        );
    }

    if (signature->is<FunctionSig>()) {
        error(
            expr->position,
            "{}(...) being used as a variable",
            expr->identifier
        );
    }

    mReturn = signature.value();
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::ArrayAccess *expr) {
    SymbolTable *scope = mSymbolStack.currentScope();

    SymbolTable *terminatingScope = scope;

    while (terminatingScope->getType() !=
               SymbolTable::Type::GLOBAL &&
           terminatingScope->getType() !=
               SymbolTable::Type::FUNCTION) {
        terminatingScope = terminatingScope->getEnclosing();
    }

    std::optional<Signature> signature{};

    for (;;) {
        signature = scope->findIdentifier(expr->identifier);

        if (signature.has_value() ||
            scope == terminatingScope)
            break;

        scope = scope->getEnclosing();
    }

    if (!signature.has_value()) {
        error(
            expr->position,
            "{} is undefined",
            expr->identifier
        );
    }

    if (!signature->is<ArraySig>()) {
        error(
            expr->position,
            "{} being used as an array",
            expr->identifier
        );
    }

    auto arraySig = *signature->as<ArraySig>();

    expr->index->accept(this);
    Signature indexSig = mReturn;

    if (indexSig != PrimitiveSig{core::Base::INT}) {
        error(
            expr->position,
            "array {} indexed with non-integer",
            expr->identifier
        );
    }

    mReturn = PrimitiveSig{arraySig.type};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::FunctionCall *expr) {
    SymbolTable *scope = mSymbolStack.currentScope();

    std::optional<Signature> signature{};

    for (;;) {
        signature = scope->findIdentifier(expr->identifier);

        scope = scope->getEnclosing();

        if (signature.has_value() || scope == nullptr)
            break;
    }

    if (!signature.has_value()) {
        error(
            expr->position,
            "{}(...) is undefined",
            expr->identifier
        );
    }

    if (!signature->is<FunctionSig>()) {
        error(
            expr->position,
            "{} being used as a function",
            expr->identifier
        );
    }

    auto funcSig = *signature->as<FunctionSig>();

    std::vector<Signature> actualParamTypes{};

    for (auto &param : expr->params) {
        param->accept(this);

        actualParamTypes.push_back(mReturn);
    }

    if (funcSig.paramTypes.size() !=
        actualParamTypes.size()) {
        error(
            expr->position,
            "function {}(...) received {} parameters, "
            "expected {}",
            expr->identifier,
            expr->params.size(),
            funcSig.paramTypes.size()
        );
    }

    for (int i = 0; i < actualParamTypes.size(); i++) {
        if (actualParamTypes[i] !=
            PrimitiveSig{funcSig.paramTypes[i]}) {
            error(
                expr->position,
                "function {}(...) received parameter "
                "of unexpected type",
                expr->identifier
            );
        }
    }

    mReturn = PrimitiveSig{funcSig.returnType};
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::SubExpr *expr) {
    expr->subExpr->accept(this);
    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::Binary *expr) {
    expr->left->accept(this);
    Signature leftSig{mReturn};

    expr->right->accept(this);
    Signature rightSig{mReturn};

    switch (expr->op) {
        case core::Operation::AND:
        case core::Operation::OR:
            if (!(leftSig ==
                      PrimitiveSig{core::Base::BOOL} &&
                  rightSig == PrimitiveSig{core::Base::BOOL}
                )) {
                error(
                    expr->position,
                    "operator {} expects boolean operands",
                    core::operationToString(expr->op)
                );
            }

            mReturn = PrimitiveSig{core::Base::BOOL};
            break;
        case core::Operation::EQ:
        case core::Operation::GE:
        case core::Operation::GT:
        case core::Operation::LE:
        case core::Operation::LT:
        case core::Operation::NEQ:
            if (leftSig.is<ArraySig>()) {
                error(
                    expr->position,
                    "operator {} is not defined on array "
                    "types",
                    core::operationToString(expr->op)
                );
            }

            if (leftSig != rightSig) {
                error(
                    expr->position,
                    "operator {} expects both operands to "
                    "be of same type",
                    core::operationToString(expr->op)
                );
            }

            mReturn = PrimitiveSig{core::Base::BOOL};
            break;
        case core::Operation::ADD:
        case core::Operation::SUB:
            if (leftSig.is<ArraySig>()) {
                error(
                    expr->position,
                    "operator {} is not defined on array "
                    "types",
                    core::operationToString(expr->op)
                );
            }

            if (leftSig != rightSig) {
                error(
                    expr->position,
                    "operator {} expects both operands to "
                    "be of same type",
                    core::operationToString(expr->op)
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
        case core::Operation::DIV:
        case core::Operation::MUL:
            if (leftSig.is<ArraySig>() ||
                rightSig.is<ArraySig>()) {
                error(
                    expr->position,
                    "operator {} is not defined on array "
                    "types",
                    core::operationToString(expr->op)
                );
            }

            if (leftSig ==
                    PrimitiveSig{core::Base::COLOR} ||
                rightSig ==
                    PrimitiveSig{core::Base::COLOR}) {
                error(
                    expr->position,
                    "operator {} is not defined on color "
                    "type",
                    core::operationToString(expr->op)
                );
            }

            if (leftSig != rightSig) {
                error(
                    expr->position,
                    "operator {} expects both operands to "
                    "be of same type",
                    core::operationToString(expr->op)
                );
            }

            mReturn = leftSig;
            break;
        default:
            core::abort("unreachable");
    }

    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::Unary *expr) {
    expr->expr->accept(this);

    switch (expr->op) {
        case core::Operation::NOT:
            if (mReturn != PrimitiveSig{core::Base::BOOL}) {
                error(
                    expr->position,
                    "operator {} expects boolean operand",
                    core::operationToString(expr->op)
                );
            }
            break;
        case core::Operation::SUB:
            if (mReturn.is<ArraySig>()) {
                error(
                    expr->position,
                    "operator {} is not defined on array "
                    "types",
                    core::operationToString(expr->op)
                );
            }
            if (mReturn == PrimitiveSig{core::Base::BOOL}) {
                error(
                    expr->position,
                    "operator {} does not expect "
                    "boolean operand",
                    core::operationToString(expr->op)
                );
            }
            break;
        default:
            core::abort("unreachable");
    }

    Signature from = mReturn;

    expr->core::Expr::accept(this);
    Signature to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PrintStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visit(core::DelayStmt *stmt) {
    stmt->expr->accept(this);
    Signature delaySig = mReturn;

    if (delaySig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__delay expects delay to be an integer"
        );
    }
}

void AnalysisVisitor::visit(core::WriteBoxStmt *stmt) {
    stmt->x->accept(this);
    Signature xSig = mReturn;

    stmt->y->accept(this);
    Signature ySig = mReturn;

    stmt->w->accept(this);
    Signature wSig = mReturn;

    stmt->h->accept(this);
    Signature hSig = mReturn;

    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (xSig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects x to be an integer"
        );
    }

    if (ySig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects y to be an integer"
        );
    }

    if (wSig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects xOffset to be an integer"
        );
    }

    if (hSig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects yOffset to be an integer"
        );
    }

    if (colorSig != PrimitiveSig{core::Base::COLOR}) {
        error(
            stmt->position,
            "__write_box expects a color"
        );
    }
}

void AnalysisVisitor::visit(core::WriteStmt *stmt) {
    stmt->x->accept(this);
    Signature xSig = mReturn;

    stmt->y->accept(this);
    Signature ySig = mReturn;

    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (xSig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__write expects x to be an integer"
        );
    }

    if (ySig != PrimitiveSig{core::Base::INT}) {
        error(
            stmt->position,
            "__write expects y to be an integer"
        );
    }

    if (colorSig != PrimitiveSig{core::Base::COLOR}) {
        error(
            stmt->position,
            "__write expects color to be a color"
        );
    }
}

void AnalysisVisitor::visit(core::ClearStmt *stmt) {
    stmt->color->accept(this);
    Signature colorSig = mReturn;

    if (colorSig != PrimitiveSig{core::Base::COLOR}) {
        error(
            stmt->position,
            "__clear expects color to be a color"
        );
    }
}

void AnalysisVisitor::visit(core::Assignment *stmt) {
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
            stmt->identifier,
            "{} is an undefined variable",
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
            "right-hand side of {} assignment is not "
            "of "
            "correct type",
            stmt->identifier.getLexeme()
        );
    }
}

void AnalysisVisitor::visit(core::VariableDecl *stmt) {
    Signature signature =
        Signature::createLiteralSignature(stmt->type);

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(stmt->identifier.getLexeme())
            .has_value()) {
        error(
            stmt->identifier,
            "redeclarantion of {}",
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
                "redeclaration of {}(...) as a "
                "variable",
                stmt->identifier.getLexeme()
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        stmt->identifier.getLexeme(),
        signature
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

void AnalysisVisitor::visit(core::Block *stmt) {
    mSymbolStack.pushScope().setType(
        SymbolTable::Type::BLOCK
    );

    unscopedBlock(stmt);

    mSymbolStack.popScope();
}

void AnalysisVisitor::visit(core::IfStmt *stmt) {
    try {
        stmt->cond->accept(this);
        Signature condSig = mReturn;

        if (condSig != PrimitiveSig{core::Base::BOOL}) {
            error(
                stmt->position,
                "if expects boolean condition"
            );
        }
    } catch (SyncAnalysis &) {
        // noop
    }

    mSymbolStack.pushScope().setType(SymbolTable::Type::IF);

    unscopedBlock(stmt->thenBlock.get());

    mSymbolStack.popScope();

    bool ifBranch = mBranchReturns;

    bool elseBranch = false;

    if (stmt->elseBlock) {
        mSymbolStack.pushScope().setType(
            SymbolTable::Type::ELSE
        );

        unscopedBlock(stmt->elseBlock.get());

        mSymbolStack.popScope();

        elseBranch = mBranchReturns;
    }

    mBranchReturns = ifBranch && elseBranch;
}

void AnalysisVisitor::visit(core::ForStmt *stmt) {
    mSymbolStack.pushScope().setType(SymbolTable::Type::FOR
    );

    try {
        if (stmt->decl) {
            stmt->decl->accept(this);
        }

        stmt->cond->accept(this);
        Signature condSig = mReturn;

        if (condSig != PrimitiveSig{core::Base::BOOL}) {
            error(
                stmt->position,
                "for expects boolean condition"
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

void AnalysisVisitor::visit(core::WhileStmt *stmt) {
    try {
        stmt->cond->accept(this);
        Signature condSig = mReturn;

        if (condSig != PrimitiveSig{core::Base::BOOL}) {
            error(
                stmt->position,
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

void AnalysisVisitor::visit(core::ReturnStmt *stmt) {
    mBranchReturns = true;

    stmt->expr->accept(this);
    Signature exprSignature = mReturn;

    SymbolTable *scope = mSymbolStack.currentScope();

    for (;;) {
        if (scope == nullptr) {
            error(
                stmt->position,
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

    auto functionSig =
        *(scope->getEnclosing()
              ->findIdentifier(enclosingFunction)
              ->as<FunctionSig>());

    if (exprSignature !=
        PrimitiveSig{functionSig.returnType}) {
        error(
            stmt->position,
            "incorrect return type in function {}",
            enclosingFunction
        );
    }
}

void AnalysisVisitor::visit(core::FormalParam *param) {
    Signature signature =
        Signature::createLiteralSignature(param->type);

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(param->identifier.getLexeme())
            .has_value()) {
        error(
            param->identifier,
            "redeclarantion of {}",
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
                "redeclaration of {}(...) as a "
                "parameter",
                param->identifier.getLexeme()
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        param->identifier.getLexeme(),
        signature
    );
}

void AnalysisVisitor::visit(core::FunctionDecl *stmt) {
    bool isGlobalScope =
        mSymbolStack.isCurrentScopeGlobal();

    if (!isGlobalScope) {
        error(
            stmt->identifier,
            "function declaration {}(...) is not "
            "allowed "
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
            paramTypes,
            stmt->type
        );

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(stmt->identifier.getLexeme())
            .has_value()) {
        error(
            stmt->identifier,
            "redeclarantion of {}",
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
        stmt->identifier.getLexeme(),
        signature
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

void AnalysisVisitor::visit(core::Program *prog) {
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

bool AnalysisVisitor::hasError() const {
    return mHasError;
}

void AnalysisVisitor::reset() {
    mHasError = false;
    mBranchReturns = false;
    mPosition = {0, 0};
    mReturn = {};
    mSymbolStack = {};
}

}  // namespace PArL
