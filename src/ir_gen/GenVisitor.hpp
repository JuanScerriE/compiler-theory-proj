#pragma once

// parl
#include <ir_gen/InstructionCountVisitor.hpp>
#include <ir_gen/RefStack.hpp>
#include <ir_gen/TypeVisitor.hpp>
#include <ir_gen/VarDeclCountVisitor.hpp>
#include <parl/Visitor.hpp>
#include <preprocess/IsFunctionVisitor.hpp>

// std
#include <cstddef>
#include <string>

namespace PArL {

class GenVisitor : public core::Visitor {
   public:
    explicit GenVisitor(Environment *global);

    void visit(core::Type *) override;
    void visit(core::Expr *) override;
    void visit(core::PadWidth *) override;
    void visit(core::PadHeight *) override;
    void visit(core::PadRead *) override;
    void visit(core::PadRandomInt *) override;
    void visit(core::BooleanLiteral *) override;
    void visit(core::IntegerLiteral *) override;
    void visit(core::FloatLiteral *) override;
    void visit(core::ColorLiteral *) override;
    void visit(core::ArrayLiteral *) override;
    void visit(core::Variable *) override;
    void visit(core::ArrayAccess *) override;
    void visit(core::FunctionCall *) override;
    void visit(core::SubExpr *) override;
    void visit(core::Binary *) override;
    void visit(core::Unary *) override;
    void visit(core::Assignment *) override;
    void visit(core::VariableDecl *) override;
    void visit(core::PrintStmt *) override;
    void visit(core::DelayStmt *) override;
    void visit(core::WriteBoxStmt *) override;
    void visit(core::WriteStmt *) override;
    void visit(core::ClearStmt *) override;
    void visit(core::Block *) override;
    void visit(core::FormalParam *) override;
    void visit(core::FunctionDecl *) override;
    void visit(core::IfStmt *) override;
    void visit(core::ForStmt *) override;
    void visit(core::WhileStmt *) override;
    void visit(core::ReturnStmt *) override;
    void visit(core::Program *) override;

    template <typename... T>
    void
    emit_line(fmt::format_string<T...> fmt, T &&...args) {
        mCode.push_back(fmt::format(fmt, args...));

        mPC++;
    }

    void print();

    size_t computeLevel(Environment *stoppingEnv);

    void reset() override;

   private:
    IsFunctionVisitor isFunction{};

    InstructionCountVisitor mIRCounter{};
    VarDeclCountVisitor mDeclCounter{};
    TypeVisitor mType{};

    RefStack mRefStack;

    std::vector<std::string> mCode{};
    size_t mPC{0};
    size_t mFrameDepth{0};
};

}  // namespace PArL
