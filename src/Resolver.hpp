#pragma once

#include <map>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"

namespace cloxx {

class Lox;
class Interpreter;

class Resolver : StmtVisitor, ExprVisitor {
public:
    explicit Resolver(Lox* lox);

    void resolve(std::vector<Stmt> const& stmts);

private:
    enum class FunctionType {
        NONE,
        FUNCTION,
        METHOD,
        INITIALIZER,
    };

    enum class ClassType {
        NONE,
        CLASS,
        SUBCLASS,
    };

    void resolve(Stmt const& stmt);
    void resolve(Expr const& expr);

    using Scope = std::map<std::string, /*isDefined*/ bool>;
    Scope& beginScope();
    void endScope();

    void declare(Token const& name);
    void define(Token const& name);

    int resolveLocal(Token const& name);
    void resolveFunction(FunStmt const& stmt, FunctionType functionType);

    // StmtVisitor
    void visit(BlockStmt const& stmt) override;
    void visit(ExprStmt const& stmt) override;
    void visit(IfStmt const& stmt) override;
    void visit(WhileStmt const& stmt) override;
    void visit(ReturnStmt const& stmt) override;
    void visit(PrintStmt const& stmt) override;
    void visit(VarStmt const& stmt) override;
    void visit(FunStmt const& stmt) override;
    void visit(ClassStmt const& stmt) override;

    // ExprVisitor
    void visit(AssignExpr const& expr) override;
    void visit(BinaryExpr const& expr) override;
    void visit(CallExpr const& expr) override;
    void visit(GetExpr const& expr) override;
    void visit(GroupingExpr const& expr) override;
    void visit(LiteralExpr const& expr) override;
    void visit(LogicalExpr const& expr) override;
    void visit(SetExpr const& expr) override;
    void visit(ThisExpr const& expr) override;
    void visit(SuperExpr const& expr) override;
    void visit(UnaryExpr const& expr) override;
    void visit(VariableExpr const& expr) override;

private:
    Lox* const _lox;

    std::vector<Scope> _scopes;
    FunctionType _currentFunction = FunctionType::NONE;
    ClassType _currentClass = ClassType::NONE;
};

} // namespace cloxx
