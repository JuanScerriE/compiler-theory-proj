// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <analysis/AnalysisVisitor.hpp>
#include <analysis/Signature.hpp>
#include <common/AST.hpp>

// std
#include <memory>

#include "analysis/SymbolTable.hpp"

namespace Vought {

void AnalysisVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);
}

void AnalysisVisitor::visitBinary(Binary *expr) {
    expr->left->accept(this);
    expr->right->accept(this);
}

void AnalysisVisitor::visitLiteral(Literal *expr) {
    // nop
}

void AnalysisVisitor::visitVariable(Variable *expr) {
    std::optional<Signature> sig =
        mSymbolStack.findIdentifier(expr->name.getLexeme());

    if (!sig.has_value()) {
        fmt::println(
            stderr,
            "scoping error {} is an undefined variable",
            expr->name.getLexeme());
    } else if (!sig->isVariableSig()) {
        fmt::println(
            stderr,
            "function {} is being used as a variable",
            expr->name.getLexeme());
    }
}

void AnalysisVisitor::visitUnary(Unary *expr) {
    expr->expr->accept(this);
}

void AnalysisVisitor::visitFunctionCall(
    FunctionCall *expr) {
    std::optional<Signature> sig =
        mSymbolStack.findIdentifier(
            expr->identifier.getLexeme());

    if (!sig.has_value()) {
        fmt::println(stderr,
                     "scoping error {}(...) is an "
                     "undefined function",
                     expr->identifier.getLexeme());
    } else if (!sig->isFunctionSig()) {
        fmt::println(
            stderr,
            "variable {} is being used as a function",
            expr->identifier.getLexeme());
    } else {
        FunctionSignature funcSig =
            std::get<FunctionSignature>(sig.value().data);

        if (funcSig.paramTypes.size() !=
            expr->params.size()) {
            fmt::println(
                stderr,
                "function {}(...) received {} parameters, "
                "expected {}",
                expr->identifier.getLexeme(),
                expr->params.size(),
                funcSig.paramTypes.size());
        }
    }

    for (auto &param : expr->params) {
        param->accept(this);
    }
}

void AnalysisVisitor::visitBuiltinWidth(
    BuiltinWidth *expr) {
    // nop
}

void AnalysisVisitor::visitBuiltinHeight(
    BuiltinHeight *expr) {
    // nop
}

void AnalysisVisitor::visitBuiltinRead(BuiltinRead *expr) {
    expr->x->accept(this);
    expr->y->accept(this);
}

void AnalysisVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr) {
    expr->max->accept(this);
}

void AnalysisVisitor::visitPrintStmt(PrintStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visitDelayStmt(DelayStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visitWriteBoxStmt(
    WriteBoxStmt *stmt) {
    stmt->xCoor->accept(this);
    stmt->yCoor->accept(this);
    stmt->xOffset->accept(this);
    stmt->yOffset->accept(this);
    stmt->color->accept(this);
}

void AnalysisVisitor::visitWriteStmt(WriteStmt *stmt) {
    stmt->xCoor->accept(this);
    stmt->yCoor->accept(this);
    stmt->color->accept(this);
}

void AnalysisVisitor::visitClearStmt(ClearStmt *stmt) {
    stmt->color->accept(this);
}

void AnalysisVisitor::visitAssignment(Assignment *stmt) {
    std::optional<Signature> sig =
        mSymbolStack.findIdentifier(
            stmt->identifier.getLexeme());

    if (!sig.has_value()) {
        fmt::println(
            stderr,
            "scoping error {} is an undefined variable",
            stmt->identifier.getLexeme());
    } else if (!sig->isVariableSig()) {
        fmt::println(stderr,
                     "function {} is being used as a "
                     "variable in assignment",
                     stmt->identifier.getLexeme());
    }

    stmt->expr->accept(this);
}

void AnalysisVisitor::visitVariableDecl(
    VariableDecl *stmt) {
    Signature signature(
        VariableSignature::fromTokenType(stmt->type));

    try {
        mSymbolStack.addIdentifier(
            stmt->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        fmt::println(
            stderr, "cannot repeat sumbol {} in same scope",
            stmt->identifier.getLexeme());
    }

    stmt->expr->accept(this);
}

void AnalysisVisitor::visitBlock(Block *stmt) {
    mSymbolStack.pushScope();

    for (auto &stmt : stmt->stmts) {
        stmt->accept(this);
    }

    mSymbolStack.popScope();
}

void AnalysisVisitor::visitIfStmt(IfStmt *stmt) {
    stmt->expr->accept(this);
    stmt->ifThen->accept(this);

    if (stmt->ifElse) {
        stmt->ifElse->accept(this);
    }
}

void AnalysisVisitor::visitForStmt(ForStmt *stmt) {
    mSymbolStack.pushScope();
    if (stmt->varDecl) {
        stmt->varDecl->accept(this);
    }
    stmt->expr->accept(this);
    if (stmt->assignment) {
        stmt->assignment->accept(this);
    }
    stmt->block->accept(this);
    mSymbolStack.popScope();
}

void AnalysisVisitor::visitWhileStmt(WhileStmt *stmt) {
    stmt->expr->accept(this);
    stmt->block->accept(this);
}

void AnalysisVisitor::visitReturnStmt(ReturnStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visitFormalParam(FormalParam *param) {
    Signature signature(
        VariableSignature::fromTokenType(param->type));

    try {
        mSymbolStack.addIdentifier(
            param->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        fmt::println(
            stderr, "cannot repeat sumbol {} in same scope",
            param->identifier.getLexeme());
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

    Signature signature(
        FunctionSignature::fromParamsAndReturnTypes(
            paramTypes, stmt->type));

    try {
        mSymbolStack.addIdentifier(
            stmt->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        fmt::println(
            stderr, "cannot repeat sumbol {} in same scope",
            stmt->identifier.getLexeme());
    }

    mSymbolStack.pushScope();
    for (auto &param : stmt->params) {
        param->accept(this);
    }

    stmt->block->accept(this);
    mSymbolStack.popScope();
}

void AnalysisVisitor::visitProgram(Program *prog) {
    for (auto &stmt : prog->stmts) {
        stmt->accept(this);
    }
}

void AnalysisVisitor::reset() {
}

}  // namespace Vought
