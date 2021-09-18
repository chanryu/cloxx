#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "ast/Expr.hpp"
#include "ast/Stmt.hpp"

#include "GC.hpp"

namespace cloxx {

class Environment;
class ErrorReporter;

class LoxObject;
class LoxFunction;

class Interpreter;
using GlobalObjectsProc = std::function<std::map<std::string, std::shared_ptr<LoxObject>>(Interpreter*)>;

class Interpreter : StmtVisitor, ExprVisitor {
public:
    Interpreter(ErrorReporter* errorReporter, GlobalObjectsProc globalObjectsProc);

    template <typename T, typename... Args>
    std::shared_ptr<T> create(Args&&... args)
    {
        return _gc.create<T>(std::forward<Args>(args)...);
    }

    std::shared_ptr<LoxObject> toLoxBool(bool value);

    void interpret(Stmt const& stmt);

private:
    // StmtVisitor
    void visit(BlockStmt const& stmt) override;
    void visit(BreakStmt const& stmt) override;
    void visit(ClassStmt const& stmt) override;
    void visit(ContinueStmt const& stmt) override;
    void visit(ExprStmt const& stmt) override;
    void visit(ForStmt const& stmt) override;
    void visit(FunStmt const& stmt) override;
    void visit(IfStmt const& stmt) override;
    void visit(ReturnStmt const& stmt) override;
    void visit(VarStmt const& stmt) override;

    // ExprVisitor
    void visit(AssignExpr const& expr) override;
    void visit(BinaryExpr const& expr) override;
    void visit(CallExpr const& expr) override;
    void visit(GetExpr const& expr) override;
    void visit(GroupingExpr const& expr) override;
    void visit(LiteralExpr const& expr) override;
    void visit(LogicalExpr const& expr) override;
    void visit(SetExpr const& expr) override;
    void visit(SuperExpr const& expr) override;
    void visit(ThisExpr const& expr) override;
    void visit(UnaryExpr const& expr) override;
    void visit(VariableExpr const& expr) override;

    void execute(Stmt const& stmt);
    void executeBlock(std::vector<Stmt> const& stmts, std::shared_ptr<Environment> const& environment);

    std::shared_ptr<LoxObject> evaluate(Expr const& expr);

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
                                              std::vector<Stmt> const& body);

    struct ReturnValue {
        std::shared_ptr<LoxObject> object;
    };

    struct LoopBreak {};
    struct LoopContinue {};

    class ScopeSwitcher;

    GarbageCollector _gc;

    ErrorReporter* const _errorReporter;

    std::shared_ptr<Environment> _globals;
    std::shared_ptr<Environment> _environment;

    std::vector<std::shared_ptr<LoxObject>> _evalResults;
};

} // namespace cloxx
