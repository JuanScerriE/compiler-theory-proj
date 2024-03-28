// parl
#include <parl/AST.hpp>

namespace PArL::core {

Type::Type(
    const Position &position,
    const Base &primitive,
    bool isArray,
    std::unique_ptr<IntegerLiteral> size
)
    : position(position),
      primitive(primitive),
      isArray(isArray),
      size(std::move(size)) {
}

void Type::accept(Visitor *visitor) {
    visitor->visit(this);
}

void Expr::accept(Visitor *visitor) {
    visitor->visit(this);
}

PadWidth::PadWidth(const Position &position)
    : position(position) {
}

void PadWidth::accept(Visitor *visitor) {
    visitor->visit(this);
}

PadHeight::PadHeight(const Position &position)
    : position(position) {
}

void PadHeight::accept(Visitor *visitor) {
    visitor->visit(this);
}

PadRead::PadRead(
    const Position &position,
    std::unique_ptr<Expr> x,
    std::unique_ptr<Expr> y
)
    : position(position), x(std::move(x)), y(std::move(y)) {
}

void PadRead::accept(Visitor *visitor) {
    visitor->visit(this);
}

PadRandomInt::PadRandomInt(
    const Position &position,
    std::unique_ptr<Expr> max
)
    : position(position), max(std::move(max)) {
}

void PadRandomInt::accept(Visitor *visitor) {
    visitor->visit(this);
}

BooleanLiteral::BooleanLiteral(
    const Position &position,
    bool value
)
    : position(position), value(value) {
}

void BooleanLiteral::accept(Visitor *visitor) {
    visitor->visit(this);
}

IntegerLiteral::IntegerLiteral(
    const Position &position,
    int value
)
    : position(position), value(value) {
}

void IntegerLiteral::accept(Visitor *visitor) {
    visitor->visit(this);
}

FloatLiteral::FloatLiteral(
    const Position &position,
    float value
)
    : position(position), value(value) {
}

void FloatLiteral::accept(Visitor *visitor) {
    visitor->visit(this);
}

ColorLiteral::ColorLiteral(
    const Position &position,
    const Color &value
)
    : position(position), value(value) {
}

void ColorLiteral::accept(Visitor *visitor) {
    visitor->visit(this);
}

ArrayLiteral::ArrayLiteral(
    const Position &position,
    std::vector<std::unique_ptr<Expr>> exprs
)
    : position(position), exprs(std::move(exprs)) {
}

void ArrayLiteral::accept(Visitor *visitor) {
    visitor->visit(this);
}

Variable::Variable(
    const Position &position,
    std::string identifier
)
    : position(position),
      identifier(std::move(identifier)) {
}

void Variable::accept(Visitor *visitor) {
    visitor->visit(this);
}

ArrayAccess::ArrayAccess(
    const Position &position,
    std::string identifier,
    std::unique_ptr<Expr> index
)
    : position(position),
      identifier(std::move(identifier)),
      index(std::move(index)) {
}

void ArrayAccess::accept(Visitor *visitor) {
    visitor->visit(this);
}

FunctionCall::FunctionCall(
    const Position &position,
    std::string identifier,
    std::vector<std::unique_ptr<Expr>> params
)
    : position(position),
      identifier(std::move(identifier)),
      params(std::move(params)) {
}

void FunctionCall::accept(Visitor *visitor) {
    visitor->visit(this);
}

SubExpr::SubExpr(std::unique_ptr<Expr> subExpr)
    : subExpr(std::move(subExpr)) {
}

void SubExpr::accept(Visitor *visitor) {
    visitor->visit(this);
}

Binary::Binary(
    const Position &position,
    std::unique_ptr<Expr> left,
    Operation op,
    std::unique_ptr<Expr> right
)
    : position(position),
      left(std::move(left)),
      op(op),
      right(std::move(right)) {
}

void Binary::accept(Visitor *visitor) {
    visitor->visit(this);
}

Unary::Unary(
    const Position &position,
    Operation op,
    std::unique_ptr<Expr> expr
)
    : position(position), op(op), expr(std::move(expr)) {
}

void Unary::accept(Visitor *visitor) {
    visitor->visit(this);
}

Assignment::Assignment(
    const Position &position,
    std::string identifier,
    std::unique_ptr<Expr> index,
    std::unique_ptr<Expr> expr
)
    : position(position),
      identifier(std::move(identifier)),
      index(std::move(index)),
      expr(std::move(expr)) {
}

void Assignment::accept(Visitor *visitor) {
    visitor->visit(this);
}

VariableDecl::VariableDecl(
    const Position &position,
    std::string identifier,
    std::unique_ptr<Type> type,
    std::unique_ptr<Expr> expr
)
    : position(position),
      identifier(std::move(identifier)),
      type(std::move(type)),
      expr(std::move(expr)) {
}

void VariableDecl::accept(Visitor *visitor) {
    visitor->visit(this);
}

PrintStmt::PrintStmt(std::unique_ptr<Expr> expr)
    : expr(std::move(expr)) {
}

void PrintStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

DelayStmt::DelayStmt(
    const Position &position,
    std::unique_ptr<Expr> expr
)
    : position(position), expr(std::move(expr)) {
}

void DelayStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

WriteBoxStmt::WriteBoxStmt(
    const Position &position,
    std::unique_ptr<Expr> x,
    std::unique_ptr<Expr> y,
    std::unique_ptr<Expr> w,
    std::unique_ptr<Expr> h,
    std::unique_ptr<Expr> color
)
    : position(position),
      x(std::move(x)),
      y(std::move(y)),
      w(std::move(w)),
      h(std::move(h)),
      color(std::move(color)) {
}

void WriteBoxStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

WriteStmt::WriteStmt(
    const Position &position,
    std::unique_ptr<Expr> x,
    std::unique_ptr<Expr> y,
    std::unique_ptr<Expr> color
)
    : position(position),
      x(std::move(x)),
      y(std::move(y)),
      color(std::move(color)) {
}

void WriteStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

ClearStmt::ClearStmt(
    const Position &position,
    std::unique_ptr<Expr> color
)
    : position(position), color(std::move(color)) {
}

void ClearStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

Block::Block(std::vector<std::unique_ptr<Stmt>> stmts)
    : stmts(std::move(stmts)) {
}

void Block::accept(Visitor *visitor) {
    visitor->visit(this);
}

FormalParam::FormalParam(
    const Position &position,
    std::string identifier,
    std::unique_ptr<Type> type
)
    : position(position),
      identifier(std::move(identifier)),
      type(std::move(type)) {
}

void FormalParam::accept(Visitor *visitor) {
    visitor->visit(this);
}

FunctionDecl::FunctionDecl(
    const Position &position,
    std::string identifier,
    std::vector<std::unique_ptr<FormalParam>> params,
    std::unique_ptr<Type> type,
    std::unique_ptr<Block> block
)
    : position(position),
      identifier(std::move(identifier)),
      params(std::move(params)),
      type(std::move(type)),
      block(std::move(block)) {
}

void FunctionDecl::accept(Visitor *visitor) {
    visitor->visit(this);
}

IfStmt::IfStmt(
    const Position &position,
    std::unique_ptr<Expr> cond,
    std::unique_ptr<Block> thenBlock,
    std::unique_ptr<Block> elseBlock
)
    : position(position),
      cond(std::move(cond)),
      thenBlock(std::move(thenBlock)),
      elseBlock(std::move(elseBlock)) {
}

void IfStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

ForStmt::ForStmt(
    const Position &position,
    std::unique_ptr<VariableDecl> decl,
    std::unique_ptr<Expr> cond,
    std::unique_ptr<Assignment> assignment,
    std::unique_ptr<Block> block
)
    : position(position),
      decl(std::move(decl)),
      cond(std::move(cond)),
      assignment(std::move(assignment)),
      block(std::move(block)) {
}

void ForStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

WhileStmt::WhileStmt(
    const Position &position,
    std::unique_ptr<Expr> cond,
    std::unique_ptr<Block> block
)
    : position(position),
      cond(std::move(cond)),
      block(std::move(block)) {
}

void WhileStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

ReturnStmt::ReturnStmt(
    const Position &position,
    std::unique_ptr<Expr> expr
)
    : position(position), expr(std::move(expr)) {
}

void ReturnStmt::accept(Visitor *visitor) {
    visitor->visit(this);
}

Program::Program(std::vector<std::unique_ptr<Stmt>> stmts)
    : stmts(std::move(stmts)) {
}

void Program::accept(Visitor *visitor) {
    visitor->visit(this);
}

}  // namespace PArL::core
