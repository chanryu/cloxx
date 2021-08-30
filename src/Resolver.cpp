#include "Resolver.hpp"

#include "Assert.hpp"
#include "Interpreter.hpp"
#include "Lox.hpp"

namespace cloxx {

Resolver::Resolver(Lox* lox, Interpreter* interpreter) : _lox{lox}, _interpreter{interpreter}
{}

bool Resolver::resolve(Stmt const& stmt)
{
    auto prevErrorCount = _errorCount;

    stmt.accept(*this);

    return _errorCount == prevErrorCount;
}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>> const& stmts)
{
    for (auto const& stmt : stmts) {
        LOX_ASSERT(stmt);
        resolve(*stmt);
    }
}

void Resolver::resolve(Expr const& expr)
{
    expr.accept(*this);
}

Resolver::Scope& Resolver::beginScope()
{
    _scopes.push_back({});
    return _scopes.back();
}

void Resolver::endScope()
{
    _scopes.pop_back();
}

void Resolver::declare(Token const& name)
{
    if (_scopes.empty()) {
        return;
    }

    auto& scope = _scopes.back();

    if (scope.find(name.lexeme) != scope.end()) {
        error(name, "Already a variable with this name in this scope.");
    }

    scope.emplace(name.lexeme, false);
}

void Resolver::define(Token const& name)
{
    if (!_scopes.empty()) {
        _scopes.back()[name.lexeme] = true;
    }
}

void Resolver::resolveLocal(Expr const& expr, Token const& name)
{
    size_t depth = 0;
    for (auto i = _scopes.rbegin(); i != _scopes.rend(); ++i, ++depth) {
        auto& scope = *i;
        if (scope.find(name.lexeme) != scope.end()) {
            _interpreter->resolve(expr, depth);
            return;
        }
    }

    _interpreter->resolve(expr, static_cast<size_t>(-1));
}

void Resolver::resolveFunction(FunStmt const& stmt, FunctionType type)
{
    auto enclosingFunction = _currentFunction;
    _currentFunction = type;

    beginScope();
    for (auto const& param : stmt.params) {
        declare(param);
        define(param);
    }
    resolve(stmt.body);
    endScope();

    _currentFunction = enclosingFunction;
}

void Resolver::visit(BlockStmt const& stmt)
{
    beginScope();
    resolve(stmt.stmts);
    endScope();
}

void Resolver::visit(ExprStmt const& stmt)
{
    LOX_ASSERT(stmt.expr);

    resolve(*stmt.expr);
}

void Resolver::visit(IfStmt const& stmt)
{
    LOX_ASSERT(stmt.cond);
    LOX_ASSERT(stmt.thenBranch);

    resolve(*stmt.cond);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) {
        resolve(*stmt.elseBranch);
    }
}

void Resolver::visit(WhileStmt const& stmt)
{
    resolve(*stmt.cond);
    resolve(*stmt.body);
}

void Resolver::visit(ReturnStmt const& stmt)
{
    if (_currentFunction == FunctionType::NONE) {
        error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value) {
        if (_currentFunction == FunctionType::INITIALIZER) {
            error(stmt.keyword, "Can't return a value from an initializer.");
        }

        resolve(*stmt.value);
    }
}

void Resolver::visit(PrintStmt const& stmt)
{
    resolve(*stmt.expr);
}

void Resolver::visit(VarStmt const& stmt)
{
    declare(stmt.name);
    if (stmt.initializer) {
        resolve(*stmt.initializer);
    }
    define(stmt.name);
}

void Resolver::visit(FunStmt const& stmt)
{
    declare(stmt.name);
    define(stmt.name);

    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visit(ClassStmt const& stmt)
{
    auto enclosingClass = _currentClass;
    _currentClass = stmt.superclass ? ClassType::SUBCLASS : ClassType::CLASS;

    declare(stmt.name);
    define(stmt.name);

    if (stmt.superclass) {
        if (stmt.name.lexeme == stmt.superclass->name.lexeme) {
            error(stmt.superclass->name, "A class can't inherit from itself.");
        }

        resolve(*stmt.superclass);

        auto& superScope = beginScope();
        superScope.emplace("super", true);
    }

    auto& thisScope = beginScope();
    thisScope.emplace("this", true);

    for (auto const& method : stmt.methods) {
        auto type = FunctionType::METHOD;
        if (method->name.lexeme == "init") {
            type = FunctionType::INITIALIZER;
        }
        resolveFunction(*method, type);
    }

    endScope(); // end thisScope

    if (stmt.superclass) {
        endScope(); // end superScope
    }

    _currentClass = enclosingClass;
}

void Resolver::visit(AssignExpr const& expr)
{
    resolve(*expr.value);
    resolveLocal(expr, expr.name);
}

void Resolver::visit(BinaryExpr const& expr)
{
    resolve(*expr.left);
    resolve(*expr.right);
}

void Resolver::visit(CallExpr const& expr)
{
    resolve(*expr.callee);
    for (auto const& arg : expr.args) {
        resolve(*arg);
    }
}

void Resolver::visit(GetExpr const& expr)
{
    resolve(*expr.object);
}

void Resolver::visit(GroupingExpr const& expr)
{
    resolve(*expr.expr);
}

void Resolver::visit(LiteralExpr const& /*expr*/)
{
    // NOP
}

void Resolver::visit(LogicalExpr const& expr)
{
    resolve(*expr.left);
    resolve(*expr.right);
}

void Resolver::visit(SetExpr const& expr)
{
    resolve(*expr.object);
    resolve(*expr.value);
}

void Resolver::visit(ThisExpr const& expr)
{
    if (_currentClass == ClassType::NONE) {
        error(expr.keyword, "Can't use 'this' outside of a class.");
    }

    resolveLocal(expr, expr.keyword);
}

void Resolver::visit(SuperExpr const& expr)
{
    if (_currentClass == ClassType::NONE) {
        error(expr.keyword, "Can't use 'super' outside of a class.");
    }
    else if (_currentClass != ClassType::SUBCLASS) {
        error(expr.keyword, "Can't use 'super' in a class with no superclass.");
    }

    resolveLocal(expr, expr.keyword);
}

void Resolver::visit(UnaryExpr const& expr)
{
    resolve(*expr.right);
}

void Resolver::visit(VariableExpr const& expr)
{
    if (!_scopes.empty()) {
        auto& scope = _scopes.back();
        if (auto it = scope.find(expr.name.lexeme); it != scope.end()) {
            auto isDefined = it->second;
            if (!isDefined) {
                error(expr.name, "Can't read local variable in its own initializer.");
            }
        }
    }

    resolveLocal(expr, expr.name);
}

void Resolver::error(Token const& token, std::string_view message)
{
    _errorCount += 1;
    _lox->resolveError(token, message);
}
} // namespace cloxx
