// fmt
#include <fmt/core.h>
#include <fmt/format.h>

// parl
#include <common/AST.hpp>
#include <common/Abort.hpp>
#include <common/Token.hpp>
#include <initializer_list>
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
    emitLine(fmt::format("push {}", expr->value.getLexeme())
    );
}

void GenVisitor::visitVariable(Variable *expr) {
    size_t level = 0;
    Frame *frame = mStack.currentFrame();

    std::optional<SymbolInfo> info{};

    for (;;) {
        info = frame->findSymbol(expr->name.getLexeme());

        if (info.has_value())
            break;

        level++;
        frame = frame->getEnclosing();

        abortIf(frame == nullptr, "undefined variable");
    }

    emitLine(fmt::format(
        "push [{}:{}]", info->as<VariableInfo>()->idx, level
    ));
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
    BuiltinRandomInt *expr
) {
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

    emitLine("write");
}

void GenVisitor::visitClearStmt(ClearStmt *stmt) {
    stmt->color->accept(this);

    emitLine("clear");
}

void GenVisitor::visitAssignment(Assignment *stmt) {
    stmt->expr->accept(this);

    size_t level = 0;
    Frame *frame = mStack.currentFrame();

    std::optional<SymbolInfo> info{};

    for (;;) {
        info =
            frame->findSymbol(stmt->identifier.getLexeme());

        if (info.has_value())
            break;

        level++;
        frame = frame->getEnclosing();

        abortIf(frame == nullptr, "undefined variable");
    }

    emitLine(fmt::format(
        "push {}", info->as<VariableInfo>()->idx
    ));
    emitLine(fmt::format("push {}", level));
    emitLine("st");
}

void GenVisitor::visitVariableDecl(VariableDecl *stmt) {
    stmt->expr->accept(this);

    Frame *frame = mStack.currentFrame();

    size_t idx = frame->getIdx();

    frame->addSymbol(
        stmt->identifier.getLexeme(), VariableInfo{idx}
    );

    emitLine(fmt::format("push {}", idx));
    emitLine("push 0");
    emitLine("st");
}

void GenVisitor::visitBlock(Block *stmt) {
    size_t count = 0;

    for (auto &stmt : stmt->stmts) {
        count += mCounter.count(stmt.get());
    }

    emitLine(fmt::format("push {}", count));
    emitLine("oframe");

    mStack.pushFrame(count);

    for (auto &stmt : stmt->stmts) {
        stmt->accept(this);
    }

    mStack.popFrame();

    emitLine("cframe");
}

void GenVisitor::visitIfStmt(IfStmt *stmt) {
    stmt->expr->accept(this);

    emitLine("not");

    size_t ifSize = mIRCounter.count(stmt->ifThen.get());

    // 1 for the offset, 1 for the cjmp, 1 for else if exits
    emitLine(fmt::format(
        "push #PC+{}",
        1 + 1 + (stmt->ifElse ? 2 : 0) + ifSize
    ));
    emitLine("cjmp");

    stmt->ifThen->accept(this);

    if (stmt->ifElse) {
        size_t elseSize =
            mIRCounter.count(stmt->ifElse.get());

        emitLine(
            fmt::format("push #PC+{}", 1 + 1 + elseSize)
        );
        emitLine("jmp");
        stmt->ifElse->accept(this);
    }
}

void GenVisitor::visitForStmt(ForStmt *stmt) {
    size_t count = stmt->varDecl ? 1 : 0;

    for (auto &stmt : stmt->block->stmts) {
        count += mCounter.count(stmt.get());
    }

    emitLine(fmt::format("push {}", count));
    emitLine("oframe");

    mStack.pushFrame(count);

    if (stmt->varDecl) {
        stmt->varDecl->accept(this);
    }

    size_t exprSize = mIRCounter.count(stmt->expr.get());

    stmt->expr->accept(this);

    size_t forSize = 0;

    for (auto &stmt : stmt->block->stmts) {
        forSize += mIRCounter.count(stmt.get());
    }

    size_t assignSize =
        mIRCounter.count(stmt->assignment.get());

    emitLine("not");
    emitLine(fmt::format(
        "push #PC+{}", 1 + forSize + assignSize + 2 + 1
    ));
    emitLine("cjmp");

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    stmt->assignment->accept(this);

    emitLine(fmt::format(
        "push #PC-{}", assignSize + forSize + 3 + exprSize
    ));
    emitLine("jmp");

    mStack.popFrame();

    emitLine("cframe");
}

void GenVisitor::visitWhileStmt(WhileStmt *stmt) {
    size_t exprSize = mIRCounter.count(stmt->expr.get());

    stmt->expr->accept(this);

    size_t whileSize = mIRCounter.count(stmt->block.get());

    emitLine("not");
    emitLine(
        fmt::format("push #PC+{}", 1 + whileSize + 2 + 1)
    );
    emitLine("cjmp");

    stmt->block->accept(this);

    emitLine(
        fmt::format("push #PC-{}", whileSize + 3 + exprSize)
    );
    emitLine("jmp");
}

void GenVisitor::visitReturnStmt(ReturnStmt *stmt) {
    // mBranchReturns = true;
    //
    // stmt->expr->accept(this);
    // Signature exprSignature = mReturn;
    //
    // SymbolTable *scope = mSymbolStack.currentScope();
    //
    // for (;;) {
    //     if (scope == nullptr) {
    //         error(stmt->token,
    //               "return statement must be within a "
    //               "function block");
    //     }
    //
    //     if (scope->getType() ==
    //         SymbolTable::Type::FUNCTION) {
    //         break;
    //     }
    //
    //     scope = scope->getEnclosing();
    // }
    //
    // std::string enclosingFunction =
    //     scope->getName().value();
    //
    // auto functionSignature =
    //     scope->getEnclosing()
    //         ->findIdentifier(enclosingFunction)
    //         ->as<FunctionSignature>();
    //
    // if (exprSignature != functionSignature.returnType) {
    //     error(stmt->token,
    //           "incorrect return type in function {}",
    //           enclosingFunction);
    // }
}

void GenVisitor::visitFormalParam(FormalParam *param) {
    Frame *frame = mStack.currentFrame();

    size_t idx = frame->getIdx();

    frame->addSymbol(
        param->identifier.getLexeme(), VariableInfo{idx}
    );
}

// the below is an example of program which does not
// properly close its scopes .test push 1 oframe push 44
// push 0
// push 0
// st
// push 1
// oframe
// push 20
// push 0
// push 0
// st
// push 10
// ret
// cframe
// push 20
// cframe
// ret
// .main
// push 0
// push .test
// call
// print
// push [0:0]
// print
// halt

void GenVisitor::visitFunctionDecl(FunctionDecl *stmt) {
    size_t arity = stmt->params.size();

    size_t count = 0;

    for (auto &stmt : stmt->block->stmts) {
        count += mCounter.count(stmt.get());
    }

    emitLine(
        fmt::format(".{}", stmt->identifier.getLexeme())
    );

    mStack.pushFrame(arity + count);

    for (auto &param : stmt->params) {
        param->accept(this);
    }

    emitLine(fmt::format("push {}", count));
    emitLine("alloc");

    for (auto &stmt : stmt->block->stmts) {
        stmt->accept(this);
    }

    mStack.popFrame();
}

void GenVisitor::visitProgram(Program *prog) {
    auto itr = prog->stmts.begin();

    for (; itr != prog->stmts.end(); itr++) {
        if (isFunction.check(itr->get())) {
            (*itr)->accept(this);
        } else {
            break;
        }
    }

    size_t count = 0;

    auto itr_ = itr;

    for (; itr_ != prog->stmts.end(); itr_++) {
        count += mCounter.count(itr_->get());
    }

    emitLine(".main");
    emitLine(fmt::format("push {}", count));
    emitLine("oframe");

    mStack.pushFrame(count);

    for (; itr != prog->stmts.end(); itr++) {
        abortIf(
            isFunction.check(itr->get()),
            "no function declaration allowed in .main"
        );

        (*itr)->accept(this);
    }

    mStack.popFrame();

    emitLine("cframe");
    emitLine("halt");
}

// void GenVisitor::unscopedBlock(Block *block) {
//     for (auto &stmt : block->stmts) {
//         try {
//             stmt->accept(this);
//         } catch (SyncAnalysis &) {
//             // noop
//         }
//     }
// }

void GenVisitor::emitLines(
    std::initializer_list<std::string> lines
) {
    for (auto &line : lines) {
        emitLine(line);
    }
}

void GenVisitor::emitLine(std::string const &line) {
    mCode.push_back(line);

    mPC++;
}

void GenVisitor::print() {
    for (auto &line : mCode) {
        fmt::println(line);
    }
}

void GenVisitor::reset() {
    isFunction.reset();
    mCounter.reset();
    mStack = {};
    mCode.clear();
    mPC = 0;
}

}  // namespace PArL
