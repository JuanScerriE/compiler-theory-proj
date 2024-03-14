
class Binary;
class Grouping;
class Literal;
class Variable;
class Unary;

class ExprVisitor {
   public:
    virtual void visitGroupingExpr(Grouping const *expr) = 0;
    virtual void visitBinaryExpr(Binary const *expr) = 0;
    virtual void visitLiteralExpr(Literal const *expr) = 0;
    virtual void visitVariableExpr(Variable const *expr) = 0;
    virtual void visitUnaryExpr(Unary const *expr) = 0;
};

class PrintStmt;
class ExprStmt;
class VarDecl;

class StmtVisitor {
   public:
    virtual void visitPrintStmt(PrintStmt const *expr) = 0;
    virtual void visitExprStmt(ExprStmt const *expr) = 0;
    virtual void visitVarDecl(VarDecl const *expr) = 0;
};
