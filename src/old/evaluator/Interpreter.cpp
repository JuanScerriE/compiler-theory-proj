// Lox
#include <common/Value.hpp>
#include <errors/RuntimeError.hpp>
#include <evaluator/Interpreter.hpp>

// std
#include <any>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace Lox {

void Interpreter::interpret(
    std::unique_ptr<Program> &program) {
    try {
        for (auto &stmt : program->stmts) {
            execute(stmt.get());
        }
    } catch (RuntimeError &error) {
        std::cout << error.what() << std::endl;
    }
}

void Interpreter::execute(Stmt *stmt) {
    stmt->accept(this);
}

void Interpreter::eval(Expr *expr) {
    expr->accept(this);
}

void Interpreter::visitVarDecl(const Lox::VarDecl *expr) {
}

void Interpreter::visitPrintStmt(const PrintStmt *stmt) {
    eval(stmt->expr.get());
    std::cout << mExprResult.toString() << std::endl;
}

void Interpreter::visitExprStmt(const ExprStmt *stmt) {
    eval(stmt->expr.get());
}

void Interpreter::visitLiteralExpr(Literal const *expr) {
    mExprResult = expr->value;
}

void Interpreter::visitGroupingExpr(Grouping const *expr) {
    eval(expr->expr.get());
}

void Interpreter::visitBinaryExpr(Binary const *expr) {
    eval(expr->left.get());
    Value left = std::move(mExprResult);
    eval(expr->right.get());
    Value right = std::move(mExprResult);

    switch (expr->oper.getType()) {
        case Token::Type::BANG_EQUAL:
            mExprResult =
                Value::createBool(!isEqual(left, right));
            return;
        case Token::Type::EQUAL_EQUAL:
            mExprResult =
                Value::createBool(isEqual(left, right));
            return;
        case Token::Type::GREATER:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createBool(
                std::get<double>(left.data) >
                std::get<double>(right.data));
            return;
        case Token::Type::GREATER_EQUAL:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createBool(
                std::get<double>(left.data) >=
                std::get<double>(right.data));
            return;
        case Token::Type::LESS:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createBool(
                std::get<double>(left.data) <
                std::get<double>(right.data));
            return;
        case Token::Type::LESS_EQUAL:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createBool(
                std::get<double>(left.data) <=
                std::get<double>(right.data));
            return;
        case Token::Type::MINUS:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createNumber(
                std::get<double>(left.data) -
                std::get<double>(right.data));
            return;
        case Token::Type::PLUS:
            if (left.type == Value::NUMBER &&
                right.type == Value::NUMBER) {
                mExprResult = Value::createNumber(
                    std::get<double>(left.data) +
                    std::get<double>(right.data));
                return;
            }

            if (left.type == Value::STRING &&
                right.type == Value::STRING) {
                mExprResult = Value::createString(
                    std::get<std::string>(left.data) +
                    std::get<std::string>(right.data));
                return;
            }

            throw RuntimeError(expr->oper,
                               "Operands must be two "
                               "numbers or two strings.");
        case Token::Type::SLASH:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createNumber(
                std::get<double>(left.data) /
                std::get<double>(right.data));
            return;
        case Token::Type::STAR:
            checkNumberOperands(expr->oper, left, right);
            mExprResult = Value::createNumber(
                std::get<double>(left.data) *
                std::get<double>(right.data));
            return;
        default:
            assert(nullptr != "Unreachable");
    }
}

void Interpreter::visitVariableExpr(
    const Lox::Variable *expr) {
}

void Interpreter::visitUnaryExpr(Unary const *expr) {
    eval(expr->expr.get());
    Value right = std::move(mExprResult);

    switch (expr->oper.getType()) {
        case Token::Type::MINUS:
            checkNumberOperand(expr->oper, right);
            mExprResult = Value::createNumber(
                -std::get<double>(right.data));
            return;
        case Token::Type::BANG:
            mExprResult =
                Value::createBool(!isTruthy(right));
            return;
        default:
            assert(nullptr != "Unreachable");
    }
}

bool Interpreter::isTruthy(Value &value) {
    if (value.type == Value::NIL) {
        return false;
    }

    if (value.type == Value::BOOL) {
        return std::get<bool>(value.data);
    }

    return true;
}

bool Interpreter::isEqual(Value &value1, Value &value2) {
    if (value1.type == Value::NIL &&
        value2.type == Value::NIL) {
        return true;
    }

    if (value1.type == Value::NIL) {
        return false;
    }

    if (value2.type == Value::NIL) {
        return false;
    }

    if (value1.type == Value::NUMBER &&
        value2.type == Value::NUMBER) {
        return std::get<bool>(value1.data) !=
               std::get<bool>(value2.data);
    }

    if (value1.type == Value::STRING &&
        value2.type == Value::STRING) {
        return std::get<std::string>(value1.data) !=
               std::get<std::string>(value2.data);
    }

    return false;
}

void Interpreter::checkNumberOperand(
    Token const &exprOperator, Value const &operand) {
    if (operand.type == Value::NUMBER) {
        return;
    }

    throw RuntimeError(exprOperator,
                       "Operand must be a number.");
}

void Interpreter::checkNumberOperands(
    Token const &exprOperator, Value const &left,
    Value const &right) {
    if (left.type == Value::NUMBER &&
        right.type == Value::NUMBER) {
        return;
    }

    throw RuntimeError(exprOperator,
                       "Operands must be numbers.");
}

}  // namespace Lox
