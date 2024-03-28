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

void AnalysisVisitor::isViableCast(
    core::Primitive &from,
    core::Primitive &to

) {
    const core::Primitive *lPtr = &from;
    const core::Primitive *rPtr = &to;

    while (lPtr != nullptr && rPtr != nullptr) {
        if (lPtr->is<core::Base>() &&
            rPtr->is<core::Array>()) {
            error(
                mPosition,
                "primitive cannot be cast to an array"
            );
        }

        if (lPtr->is<core::Array>() &&
            rPtr->is<core::Base>()) {
            error(
                mPosition,
                "array cannot be cast to a primitive"
            );
        }

        if (lPtr->is<core::Array>() &&
            rPtr->is<core::Array>()) {
            size_t lSize = lPtr->as<core::Array>().size;
            size_t rSize = rPtr->as<core::Array>().size;

            lPtr = lPtr->as<core::Array>().type.get();
            rPtr = rPtr->as<core::Array>().type.get();

            if (lSize != rSize) {
                error(
                    mPosition,
                    "array of size {} cannot be cast to an "
                    "array of size {}",
                    lSize,
                    rSize
                );
            }

            continue;
        }

        return;
    }

    core::abort("unreachable");
}

void AnalysisVisitor::visit(core::Type *type) {
    if (!type->isArray) {
        mReturn = type->base;

        return;
    }

    size_t size;

    if (!type->size) {
        core::abort("emtpy array index cannot be present");
    }

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

    mReturn = core::Array{
        size,
        core::box{core::Primitive{type->base}}
    };
}

void AnalysisVisitor::visit(core::Expr *expr) {
    if (expr->type.has_value()) {
        core::Type *type = (*expr->type).get();

        type->accept(this);

        mPosition = type->position;
    }
}

void AnalysisVisitor::visit(core::PadWidth *expr) {
    mReturn = core::Base::INT;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PadHeight *expr) {
    mReturn = core::Base::INT;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PadRead *expr) {
    expr->x->accept(this);
    auto xSig = mReturn;

    expr->y->accept(this);
    auto ySig = mReturn;

    if (xSig != core::Primitive{core::Base::INT}) {
        error(
            expr->position,
            "__read expects x to be an integer"
        );
    }

    if (ySig != core::Primitive{core::Base::INT}) {
        error(
            expr->position,
            "__read expects y to be an integer"
        );
    }

    mReturn = core::Base::COLOR;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PadRandomInt *expr) {
    expr->max->accept(this);
    auto maxSig = mReturn;

    if (maxSig != core::Primitive{core::Base::INT}) {
        error(
            expr->position,
            "__random_int expects max to be an integer"
        );
    }

    mReturn = core::Base::INT;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::BooleanLiteral *expr) {
    mReturn = core::Base::BOOL;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::IntegerLiteral *expr) {
    mReturn = core::Base::INT;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::FloatLiteral *expr) {
    mReturn = core::Base::FLOAT;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::ColorLiteral *expr) {
    mReturn = core::Base::COLOR;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::ArrayLiteral *expr) {
    if (expr->exprs.empty()) {
        error(expr->position, "array literal is empty");
    }

    expr->exprs[0]->accept(this);
    auto initialSig = mReturn;

    if (initialSig.is<core::Array>()) {
        error(
            expr->position,
            "nested arrays are not supported"
        );
    }

    for (size_t i = 1; i < expr->exprs.size(); i++) {
        expr->exprs[i]->accept(this);

        if (mReturn.is<core::Array>()) {
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

    mReturn = core::Array{
        expr->exprs.size(),
        core::box{initialSig},
    };
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

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

    mReturn = signature->as<PrimitiveSig>()->type;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

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

    if (signature->is<FunctionSig>()) {
        error(
            expr->position,
            "{}(...) being used as an array",
            expr->identifier
        );
    }

    auto primitive = signature->as<PrimitiveSig>()->type;

    if (!primitive.is<core::Array>()) {
        error(
            expr->position,
            "{} being used as an array",
            expr->identifier
        );
    }

    expr->index->accept(this);
    auto indexSig = mReturn;

    if (indexSig != core::Primitive{core::Base::INT}) {
        error(
            expr->position,
            "array {} indexed with non-integer",
            expr->identifier
        );
    }

    mReturn = primitive.as<core::Array>().type;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

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

    std::vector<core::Primitive> paramTypes{};

    for (auto &param : expr->params) {
        param->accept(this);

        paramTypes.push_back(mReturn);
    }

    if (funcSig.paramTypes.size() != paramTypes.size()) {
        error(
            expr->position,
            "function {}(...) received {} parameters, "
            "expected {}",
            expr->identifier,
            expr->params.size(),
            funcSig.paramTypes.size()
        );
    }

    for (int i = 0; i < paramTypes.size(); i++) {
        if (paramTypes[i] != funcSig.paramTypes[i]) {
            error(
                expr->position,
                "function {}(...) received parameter "
                "of unexpected type",
                expr->identifier
            );
        }
    }

    mReturn = funcSig.returnType;
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::SubExpr *expr) {
    expr->subExpr->accept(this);
    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::Binary *expr) {
    expr->left->accept(this);
    auto leftSig{mReturn};

    expr->right->accept(this);
    auto rightSig{mReturn};

    switch (expr->op) {
        case core::Operation::AND:
        case core::Operation::OR:
            if (!(leftSig ==
                      core::Primitive{core::Base::BOOL} &&
                  rightSig ==
                      core::Primitive{core::Base::BOOL})) {
                error(
                    expr->position,
                    "operator {} expects boolean operands",
                    core::operationToString(expr->op)
                );
            }

            mReturn = core::Base::BOOL;
            break;
        case core::Operation::EQ:
        case core::Operation::GE:
        case core::Operation::GT:
        case core::Operation::LE:
        case core::Operation::LT:
        case core::Operation::NEQ:
            if (leftSig.is<core::Array>()) {
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

            mReturn = core::Base::BOOL;
            break;
        case core::Operation::ADD:
        case core::Operation::SUB:
            if (leftSig.is<core::Array>()) {
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
            if (leftSig.is<core::Array>() ||
                rightSig.is<core::Array>()) {
                error(
                    expr->position,
                    "operator {} is not defined on array "
                    "types",
                    core::operationToString(expr->op)
                );
            }

            if (leftSig ==
                    core::Primitive{core::Base::COLOR} ||
                rightSig ==
                    core::Primitive{core::Base::COLOR}) {
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

    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::Unary *expr) {
    expr->expr->accept(this);

    switch (expr->op) {
        case core::Operation::NOT:
            if (mReturn !=
                core::Primitive{core::Base::BOOL}) {
                error(
                    expr->position,
                    "operator {} expects boolean operand",
                    core::operationToString(expr->op)
                );
            }
            break;
        case core::Operation::SUB:
            if (mReturn.is<core::Array>()) {
                error(
                    expr->position,
                    "operator {} is not defined on array "
                    "types",
                    core::operationToString(expr->op)
                );
            }
            if (mReturn ==
                core::Primitive{core::Base::BOOL}) {
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

    auto from = mReturn;

    expr->core::Expr::accept(this);
    auto to = mReturn;

    isViableCast(from, to);
}

void AnalysisVisitor::visit(core::PrintStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visit(core::DelayStmt *stmt) {
    stmt->expr->accept(this);
    auto delaySig = mReturn;

    if (delaySig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__delay expects delay to be an integer"
        );
    }
}

void AnalysisVisitor::visit(core::WriteBoxStmt *stmt) {
    stmt->x->accept(this);
    auto xSig = mReturn;

    stmt->y->accept(this);
    auto ySig = mReturn;

    stmt->w->accept(this);
    auto wSig = mReturn;

    stmt->h->accept(this);
    auto hSig = mReturn;

    stmt->color->accept(this);
    auto colorSig = mReturn;

    if (xSig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects x to be an integer"
        );
    }

    if (ySig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects y to be an integer"
        );
    }

    if (wSig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects xOffset to be an integer"
        );
    }

    if (hSig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__write_box expects yOffset to be an integer"
        );
    }

    if (colorSig != core::Primitive{core::Base::COLOR}) {
        error(
            stmt->position,
            "__write_box expects a color"
        );
    }
}

void AnalysisVisitor::visit(core::WriteStmt *stmt) {
    stmt->x->accept(this);
    auto xSig = mReturn;

    stmt->y->accept(this);
    auto ySig = mReturn;

    stmt->color->accept(this);
    auto colorSig = mReturn;

    if (xSig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__write expects x to be an integer"
        );
    }

    if (ySig != core::Primitive{core::Base::INT}) {
        error(
            stmt->position,
            "__write expects y to be an integer"
        );
    }

    if (colorSig != core::Primitive{core::Base::COLOR}) {
        error(
            stmt->position,
            "__write expects color to be a color"
        );
    }
}

void AnalysisVisitor::visit(core::ClearStmt *stmt) {
    stmt->color->accept(this);
    auto colorSig = mReturn;

    if (colorSig != core::Primitive{core::Base::COLOR}) {
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
        leftSignature =
            scope->findIdentifier(stmt->identifier);

        if (leftSignature.has_value() ||
            scope == terminatinScope)
            break;

        scope = scope->getEnclosing();
    }

    if (!leftSignature.has_value()) {
        error(
            stmt->position,
            "{} is undefined",
            stmt->identifier
        );
    }

    if (leftSignature->is<FunctionSig>()) {
        error(
            stmt->position,
            "{}(...) is being assigned to",
            stmt->identifier
        );
    }

    bool isArrayAccess = false;

    if (stmt->index) {
        stmt->index->accept(this);
        auto indexPrimitive = mReturn;

        if (!indexPrimitive.is<core::Base>() &&
            indexPrimitive !=
                core::Primitive{core::Base::INT}) {
            error(
                stmt->position,
                "{} indexed with non-integer",
                stmt->identifier
            );
        }

        isArrayAccess = true;
    }

    auto leftPrimitive =
        leftSignature->as<PrimitiveSig>()->type;

    if (isArrayAccess && !leftPrimitive.is<core::Array>()) {
        error(
            stmt->position,
            "{} is not an array",
            stmt->identifier
        );
    }

    if (isArrayAccess) {
        leftPrimitive =
            leftPrimitive.as<core::Array>().type;
    }

    stmt->expr->accept(this);
    auto rightPrimitive = mReturn;

    if (leftPrimitive != rightPrimitive) {
        error(
            stmt->position,
            "left-hand side of {} is of type {} whilst "
            "right-hand side is of type {}",
            stmt->identifier,
            core::primitiveToString(&leftPrimitive),
            core::primitiveToString(&rightPrimitive)
        );
    }
}

void AnalysisVisitor::visit(core::VariableDecl *stmt) {
    stmt->type->accept(this);
    auto leftPrimitive = mReturn;

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(stmt->identifier)
            .has_value()) {
        error(
            stmt->position,
            "redeclarantion of {}",
            stmt->identifier
        );
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(stmt->identifier);

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSig>()) {
            error(
                stmt->position,
                "redeclaration of {}(...) as a "
                "variable",
                stmt->identifier
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        stmt->identifier,
        PrimitiveSig{leftPrimitive}
    );

    stmt->expr->accept(this);
    auto rightPrimitive = mReturn;

    if (leftPrimitive != rightPrimitive) {
        error(
            stmt->position,
            "left-hand side of {} is of type {} whilst "
            "right-hand side is of type {}",
            stmt->identifier,
            core::primitiveToString(&leftPrimitive),
            core::primitiveToString(&rightPrimitive)
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

        if (condSig != core::Primitive{core::Base::BOOL}) {
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

        if (condSig != core::Primitive{core::Base::BOOL}) {
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

        if (condSig != core::Primitive{core::Base::BOOL}) {
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
    auto exprSignature = mReturn;

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

    if (exprSignature != functionSig.returnType) {
        error(
            stmt->position,
            "incorrect return type in function {}",
            enclosingFunction
        );
    }
}

void AnalysisVisitor::visit(core::FormalParam *param) {
    param->type->accept(this);
    auto primitive = mReturn;

    SymbolTable *scope = mSymbolStack.currentScope();

    if (scope->findIdentifier(param->identifier)
            .has_value()) {
        error(
            param->position,
            "redeclarantion of {}",
            param->identifier
        );
    }

    SymbolTable *enclosing = scope->getEnclosing();

    for (;;) {
        if (enclosing == nullptr)
            break;

        std::optional<Signature> identifierSignature =
            enclosing->findIdentifier(param->identifier);

        if (identifierSignature.has_value() &&
            identifierSignature->is<FunctionSig>()) {
            error(
                param->position,
                "redeclaration of {}(...) as a "
                "parameter",
                param->identifier
            );
        }

        enclosing = enclosing->getEnclosing();
    }

    scope->addIdentifier(
        param->identifier,
        PrimitiveSig{primitive}
    );
}

void AnalysisVisitor::visit(core::FunctionDecl *stmt) {
    bool isGlobalScope =
        mSymbolStack.isCurrentScopeGlobal();

    if (!isGlobalScope) {
        error(
            stmt->position,
            "function declaration {}(...) is not "
            "allowed "
            "here",
            stmt->identifier
        );
    }

    std::vector<core::Primitive> paramTypes{
        stmt->params.size()
    };

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
