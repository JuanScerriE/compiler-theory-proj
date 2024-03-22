// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// vought
#include <analysis/AnalysisVisitor.hpp>
#include <analysis/Signature.hpp>
#include <analysis/SymbolTable.hpp>
#include <common/AST.hpp>
#include <common/Abort.hpp>

// std
#include <memory>

#include "common/Token.hpp"

namespace PArL {

void AnalysisVisitor::visitSubExpr(SubExpr *expr) {
    expr->expr->accept(this);

    if (expr->type.has_value()) {
        mTempSig =
            Signature(LiteralSignature::fromTokenType(
                expr->type.value()));

        return;
    }
}

void AnalysisVisitor::visitBinary(Binary *expr) {
    expr->left->accept(this);
    Signature leftSig = mTempSig;
    expr->right->accept(this);
    Signature rightSig = mTempSig;

    if (expr->type.has_value()) {
        mTempSig =
            Signature(LiteralSignature::fromTokenType(
                expr->type.value()));

        return;
    }

    switch (expr->oper.getType()) {
        case Token::Type::AND:
        case Token::Type::OR:
            if (!(leftSig == FundamentalType::BOOL &&
                  rightSig == FundamentalType::BOOL)) {
                fmt::println(
                    stderr,
                    "operator {} expects boolean operands",
                    expr->oper.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }
            break;
        case Token::Type::LESS:
        case Token::Type::GREATER:
        case Token::Type::EQUAL_EQUAL:
        case Token::Type::BANG_EQUAL:
        case Token::Type::LESS_EQUAL:
        case Token::Type::GREATER_EQUAL:
            if (leftSig != rightSig) {
                fmt::println(
                    stderr,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }

            mTempSig = Signature({FundamentalType::BOOL});

            break;
        case Token::Type::PLUS:
        case Token::Type::MINUS:
        case Token::Type::STAR:
            if (leftSig != rightSig) {
                fmt::println(
                    stderr,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }

            mTempSig = leftSig;
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
                fmt::println(
                    stderr,
                    "operator {} was provided two distinct "
                    "types "
                    "upon which it could not operate",
                    expr->oper.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }

            mTempSig = leftSig;
            break;
        default:
            ABORTIF(true, "unreachable");
    }
}

void AnalysisVisitor::visitLiteral(Literal *expr) {
    if (expr->type.has_value()) {
        mTempSig =
            Signature(LiteralSignature::fromTokenType(
                expr->type.value()));

        return;
    }

    mTempSig = Signature(
        LiteralSignature::fromLiteral(expr->value));
}

void AnalysisVisitor::visitVariable(Variable *expr) {
    std::optional<Signature> sig =
        mSymbolStack.findIdentifier(expr->name.getLexeme());

    if (!sig.has_value()) {
        fmt::println(
            stderr,
            "scoping error {} is an undefined variable",
            expr->name.getLexeme());
    } else if (!sig->isLiteralSig()) {
        fmt::println(
            stderr,
            "function {} is being used as a variable",
            expr->name.getLexeme());
    }

    if (expr->type.has_value()) {
        mTempSig =
            Signature(LiteralSignature::fromTokenType(
                expr->type.value()));

        return;
    }

    mTempSig = sig.value();
}

void AnalysisVisitor::visitUnary(Unary *expr) {
    expr->expr->accept(this);

    if (expr->type.has_value()) {
        mTempSig =
            Signature(LiteralSignature::fromTokenType(
                expr->type.value()));

        return;
    }

    Signature innerSig = mTempSig;

    switch (expr->oper.getType()) {
        case Token::Type::NOT:
            if (innerSig != FundamentalType::BOOL) {
                fmt::println(
                    stderr,
                    "operator {} expects boolean operands",
                    expr->oper.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }
            break;
        case Token::Type::MINUS:
            if (innerSig == FundamentalType::BOOL) {
                fmt::println(stderr,
                             "operator {} does not expect "
                             "a boolean operands",
                             expr->oper.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }
            break;
        default:
            ABORTIF(true, "unreachable");
    }
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

        std::vector<Signature> actualParamTypes{};

        for (auto &param : expr->params) {
            param->accept(this);
            actualParamTypes.push_back(mTempSig);
        }

        if (funcSig.paramTypes.size() !=
            actualParamTypes.size()) {
            fmt::println(
                stderr,
                "function {}(...) received {} parameters, "
                "expected {}",
                expr->identifier.getLexeme(),
                expr->params.size(),
                funcSig.paramTypes.size());
            ABORTIF(true, "we are aborting for now");
        }

        for (int i = 0; i < actualParamTypes.size(); i++) {
            if (actualParamTypes[i] !=
                funcSig.paramTypes[i]) {
                fmt::println(
                    stderr,
                    "function {}(...) received parameter "
                    "of unexpected type",
                    expr->identifier.getLexeme());
                ABORTIF(true, "we are aborting for now");
            }
        }

        if (expr->type.has_value()) {
            mTempSig =
                Signature(LiteralSignature::fromTokenType(
                    expr->type.value()));

            return;
        }

        mTempSig = Signature({funcSig.returnType});
    }
}

void AnalysisVisitor::visitBuiltinWidth(
    BuiltinWidth *expr) {
    mTempSig = Signature({FundamentalType::INTEGER});
}

void AnalysisVisitor::visitBuiltinHeight(
    BuiltinHeight *expr) {
    mTempSig = Signature({FundamentalType::INTEGER});
}

void AnalysisVisitor::visitBuiltinRead(BuiltinRead *expr) {
    expr->x->accept(this);
    Signature xSig = mTempSig;
    expr->y->accept(this);
    Signature ySig = mTempSig;
    if (xSig != FundamentalType::INTEGER) {
        fmt::println(stderr,
                     "read expects x to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (ySig != FundamentalType::INTEGER) {
        fmt::println(stderr,
                     "read expects y to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    mTempSig = Signature({FundamentalType::COLOR});
}

void AnalysisVisitor::visitBuiltinRandomInt(
    BuiltinRandomInt *expr) {
    expr->max->accept(this);
    Signature maxSig = mTempSig;
    if (maxSig != FundamentalType::INTEGER) {
        fmt::println(
            stderr,
            "randomint expects max to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    mTempSig = Signature({FundamentalType::INTEGER});
}

void AnalysisVisitor::visitPrintStmt(PrintStmt *stmt) {
    stmt->expr->accept(this);
}

void AnalysisVisitor::visitDelayStmt(DelayStmt *stmt) {
    stmt->expr->accept(this);

    Signature delaySig = mTempSig;

    if (delaySig != FundamentalType::INTEGER) {
        fmt::println(
            stderr,
            "__delay expects delay to be an integer");
        ABORTIF(true, "we are aborting for now");
    }
}

void AnalysisVisitor::visitWriteBoxStmt(
    WriteBoxStmt *stmt) {
    stmt->xCoor->accept(this);
    Signature xCSig = mTempSig;
    stmt->yCoor->accept(this);
    Signature yCSig = mTempSig;
    stmt->xOffset->accept(this);
    Signature xOSig = mTempSig;
    stmt->yOffset->accept(this);
    Signature yOSig = mTempSig;
    stmt->color->accept(this);
    Signature colorSig = mTempSig;

    if (xCSig != FundamentalType::INTEGER) {
        fmt::println(stderr,
                     "writebox expects x to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (yCSig != FundamentalType::INTEGER) {
        fmt::println(stderr,
                     "writebox expects y to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (xOSig != FundamentalType::INTEGER) {
        fmt::println(
            stderr,
            "writebox expects xOffset to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (yOSig != FundamentalType::INTEGER) {
        fmt::println(
            stderr,
            "writebox expects yOffset to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (colorSig != FundamentalType::COLOR) {
        fmt::println(
            stderr, "writebox expects color to be a color");
        ABORTIF(true, "we are aborting for now");
    }
}

void AnalysisVisitor::visitWriteStmt(WriteStmt *stmt) {
    stmt->xCoor->accept(this);
    Signature xCSig = mTempSig;
    stmt->yCoor->accept(this);
    Signature yCSig = mTempSig;
    stmt->color->accept(this);
    Signature colorSig = mTempSig;

    if (xCSig != FundamentalType::INTEGER) {
        fmt::println(stderr,
                     "write expects x to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (yCSig != FundamentalType::INTEGER) {
        fmt::println(stderr,
                     "write expects y to be an integer");
        ABORTIF(true, "we are aborting for now");
    }

    if (colorSig != FundamentalType::COLOR) {
        fmt::println(stderr,
                     "write expects color to be a color");
        ABORTIF(true, "we are aborting for now");
    }
}

void AnalysisVisitor::visitClearStmt(ClearStmt *stmt) {
    stmt->color->accept(this);

    Signature colorSig = mTempSig;

    if (colorSig != FundamentalType::COLOR) {
        fmt::println(stderr,
                     "__clear expects color to be a color");
        ABORTIF(true, "we are aborting for now");
    }
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
    } else if (!sig->isLiteralSig()) {
        fmt::println(stderr,
                     "function {} is being used as a "
                     "variable in assignment",
                     stmt->identifier.getLexeme());
    }

    stmt->expr->accept(this);

    Signature rightSig = mTempSig;

    if (sig.value() != rightSig) {
        fmt::println(
            stderr,
            "right-hand side of {} is not of correct type",
            stmt->identifier.getLexeme());
        ABORTIF(true, "we are aborting for now");
    }
}

void AnalysisVisitor::visitVariableDecl(
    VariableDecl *stmt) {
    Signature signature(
        LiteralSignature::fromTokenType(stmt->type));

    try {
        mSymbolStack.addIdentifier(
            stmt->identifier.getLexeme(), signature);
    } catch (RepeatSymbolException &) {
        fmt::println(
            stderr, "cannot repeat sumbol {} in same scope",
            stmt->identifier.getLexeme());
    }

    stmt->expr->accept(this);

    Signature rightSig = mTempSig;

    if (signature != rightSig) {
        fmt::println(
            stderr,
            "right-hand side of {} is not of correct type",
            stmt->identifier.getLexeme());
        ABORTIF(true, "we are aborting for now");
    }
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

    Signature condSig = mTempSig;

    if (condSig != FundamentalType::BOOL) {
        fmt::println(stderr,
                     "if expects boolean condition");
        ABORTIF(true, "we are aborting for now");
    }

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

    Signature condSig = mTempSig;

    if (condSig != FundamentalType::BOOL) {
        fmt::println(stderr,
                     "for expects boolean condition");
        ABORTIF(true, "we are aborting for now");
    }

    if (stmt->assignment) {
        stmt->assignment->accept(this);
    }
    stmt->block->accept(this);
    mSymbolStack.popScope();
}

void AnalysisVisitor::visitWhileStmt(WhileStmt *stmt) {
    stmt->expr->accept(this);

    Signature condSig = mTempSig;

    if (condSig != FundamentalType::BOOL) {
        fmt::println(stderr,
                     "while expects boolean condition");
        ABORTIF(true, "we are aborting for now");
    }

    stmt->block->accept(this);
}

void AnalysisVisitor::visitReturnStmt(ReturnStmt *stmt) {
    stmt->expr->accept(this);

    Signature exprSig = mTempSig;

    auto enclosingFunc =
        mSymbolStack.getEnclosingFunction();

    if (!enclosingFunc.has_value()) {
        fmt::println(stderr,
                     "return statement must be within a "
                     "function block");
        ABORTIF(true, "we are aborting for now");
    }

    auto &[identifier, funcSig] = enclosingFunc.value();

    if (exprSig != funcSig.asFunctionSig().returnType) {
        fmt::println(
            stderr,
            "returning the incorrect type in function {}",
            identifier);
        ABORTIF(true, "we are aborting for now");
    }
}

void AnalysisVisitor::visitFormalParam(FormalParam *param) {
    Signature signature(
        LiteralSignature::fromTokenType(param->type));

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

    mSymbolStack.currentScope().addInsertRule(
        {{"redeclaration of calling function is prohibited",
          [=](auto ident, Signature sig) {
              return !(ident ==
                           stmt->identifier.getLexeme() &&
                       sig.isFunctionSig());
          }}});
    mSymbolStack.currentScope().addSearchRule(
        {{"access to identifiers outside "
          "functions is prohibited",
          [](auto _, Signature sig) {
              return sig.isFunctionSig();
          }}});

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

}  // namespace PArL
