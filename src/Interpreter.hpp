#pragma once

#include <memory>
#include <vector>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"

#include "Environment.hpp"

namespace cloxx {

class Lox;
class LoxObject;
class LoxFunction;

class Interpreter : StmtVisitor, ExprVisitor {
public:
    explicit Interpreter(Lox* lox, std::shared_ptr<Environment> const& globals);

    void interpret(std::vector<std::shared_ptr<Stmt>> const& stmts);
    void resolve(Expr const& expr, size_t depth);

private:
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

    void execute(Stmt const& stmt);
    void executeBlock(std::vector<std::shared_ptr<Stmt>> const& stmts, std::shared_ptr<Environment> const& environment);

    std::shared_ptr<LoxObject> evaluate(Expr const& expr);

    std::shared_ptr<LoxObject> lookUpVariable(Token const& name, Expr const& expr) const;

    template <typename OperandType, typename Callback>
    bool matchOperand(std::shared_ptr<LoxObject> const& right, Callback&& callback);
    template <typename OperandType, typename Callback>
    void ensureOperand(Token const& op, std::shared_ptr<LoxObject> const& right, Callback&& callback);

    template <typename OperandType, typename Callback>
    bool matchOperands(std::shared_ptr<LoxObject> const& left, std::shared_ptr<LoxObject> const& right,
                       Callback&& callback);
    template <typename OperandType, typename Callback>
    void ensureOperands(Token const& op, std::shared_ptr<LoxObject> const& left,
                        std::shared_ptr<LoxObject> const& right, Callback&& callback);

    std::shared_ptr<LoxFunction> makeFunction(bool isInitializer, Token const& name, std::vector<Token> const params,
                                              std::vector<std::shared_ptr<Stmt>> const& body);

    struct ReturnValue {
        std::shared_ptr<LoxObject> object;
    };

    Lox* const _lox;
    std::shared_ptr<Environment> _globals;
    std::shared_ptr<Environment> _environment;

    std::map<Expr const*, /*depth*/ size_t> _locals;

    std::vector<std::shared_ptr<LoxObject>> _evalResults;
};

} // namespace cloxx