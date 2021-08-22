#include "Interpreter.hpp"

#include <chrono>   // for clock() native function
#include <iostream> // for print statement

#include "Assert.hpp"
#include "Lox.hpp"
#include "LoxClass.hpp"
#include "LoxFunction.hpp"
#include "LoxInstance.hpp"
#include "RuntimeError.hpp"

namespace cloxx {

namespace {

template <typename T, size_t N>
struct OperandErrorMessage {};

template <>
struct OperandErrorMessage<LoxNumber, 1> {
    static auto get()
    {
        return "Operand must be a number.";
    }
};

template <size_t N>
struct OperandErrorMessage<LoxNumber, N> {
    static auto get()
    {
        return "Operands must be numbers.";
    }
};

template <size_t N>
struct OperandErrorMessage<LoxString, N> {
    static auto get()
    {
        return "Operands must be strings.";
    }
};

} // namespace

Interpreter::Interpreter(Lox* lox, std::shared_ptr<Environment> const& globals)
    : _lox{lox}, _globals{globals}, _environment{globals}
{
    _globals->define("clock", std::make_shared<LoxNativeFunction>("clock", 0, [](auto& /*args*/) {
                         auto duration = std::chrono::steady_clock::now().time_since_epoch();
                         auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
                         return toLoxNumber(millis / 1000.0);
                     }));
}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> const& stmts)
{
    try {
        for (auto const& stmt : stmts) {
            LOX_ASSERT(stmt);
            execute(*stmt);
        }
    }
    catch (RuntimeError& error) {
        _lox->runtimeError(error);
    }
}

void Interpreter::resolve(Expr const& expr, size_t depth)
{
    LOX_ASSERT(_locals.find(&expr) == _locals.end());

    _locals.emplace(&expr, depth);
}

void Interpreter::visit(BlockStmt const& stmt)
{
    auto blockEnv = _lox->create<Environment>(_environment);
    executeBlock(stmt.stmts, blockEnv);
}

void Interpreter::visit(ExprStmt const& stmt)
{
    evaluate(*stmt.expr);
}

void Interpreter::visit(IfStmt const& stmt)
{
    if (evaluate(*stmt.cond)->isTruthy()) {
        execute(*stmt.thenBranch);
    }
    else if (stmt.elseBranch) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::visit(WhileStmt const& stmt)
{
    while (evaluate(*stmt.cond)->isTruthy()) {
        execute(*stmt.body);
    }
}

void Interpreter::visit(ReturnStmt const& stmt)
{
    ReturnValue returnValue;
    if (stmt.value) {
        returnValue.object = evaluate(*stmt.value);
    }
    else {
        returnValue.object = makeLoxNil();
    }

    throw returnValue;
}

void Interpreter::visit(PrintStmt const& stmt)
{
    auto value = evaluate(*stmt.expr);
    LOX_ASSERT(value);
    std::cout << value->toString() << '\n';
}

void Interpreter::visit(VarStmt const& stmt)
{
    std::shared_ptr<LoxObject> value;
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    else {
        value = makeLoxNil();
    }
    _environment->define(stmt.name.lexeme, value);
}

void Interpreter::visit(FunStmt const& stmt)
{
    auto function = makeFunction(false, stmt.name, stmt.params, stmt.body);
    _environment->define(stmt.name.lexeme, function);
}

void Interpreter::visit(ClassStmt const& stmt)
{
    std::shared_ptr<LoxClass> superclass;
    if (stmt.superclass) {
        auto object = evaluate(*stmt.superclass);
        superclass = std::dynamic_pointer_cast<LoxClass>(object);
        if (!superclass) {
            throw RuntimeError(stmt.superclass->name, "Superclass must be a class.");
        }
    }

    _environment->define(stmt.name.lexeme, makeLoxNil());

    auto enclosingEnvironment = _environment;
    if (superclass) {
        _environment = _lox->create<Environment>(_environment);
        _environment->define("super", superclass);
    }

    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    for (auto const& method : stmt.methods) {
        bool isInitializer = method->name.lexeme == "init";
        auto function = makeFunction(isInitializer, method->name, method->params, method->body);
        methods.emplace(method->name.lexeme, function);
    }

    auto klass = std::make_shared<LoxClass>(stmt.name.lexeme, superclass, methods);

    if (superclass) {
        _environment = enclosingEnvironment;
    }

    _environment->assign(stmt.name, klass);
}

void Interpreter::visit(AssignExpr const& expr)
{
    auto value = evaluate(*expr.value);
    LOX_ASSERT(value);

    if (auto it = _locals.find(&expr); it != _locals.end()) {
        auto distance = it->second;
        _environment->assignAt(distance, expr.name.lexeme, value);
    }
    else {
        _globals->assign(expr.name, value);
    }
    _evalResults.push_back(value);
}

void Interpreter::visit(BinaryExpr const& expr)
{
    auto left = evaluate(*expr.left);
    auto right = evaluate(*expr.right);

    LOX_ASSERT(left);
    LOX_ASSERT(right);

    switch (expr.op.type) {
    case Token::GREATER:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxBoolean(left > right));
        });
        break;
    case Token::GREATER_EQUAL:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxBoolean(left >= right));
        });
        break;
    case Token::LESS:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxBoolean(left < right));
        });
        break;
    case Token::LESS_EQUAL:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxBoolean(left <= right));
        });
        break;
    case Token::MINUS:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxNumber(left - right));
        });
        break;
    case Token::PLUS:
        if (matchOperands<LoxNumber>(left, right, [this](double left, double right) {
                _evalResults.push_back(toLoxNumber(left + right));
            })) {
            break; // handled number + number
        }
        if (matchOperands<LoxString>(left, right, [this](auto& left, auto& right) {
                _evalResults.push_back(toLoxString(left + right));
            })) {
            break; // handled string + string
        }
        throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
    case Token::SLASH:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxNumber(left / right));
        });
        break;
    case Token::STAR:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](double left, double right) {
            _evalResults.push_back(toLoxNumber(left * right));
        });
        break;
    case Token::BANG_EQUAL:
        _evalResults.push_back(toLoxBoolean(!left->equals(*right)));
        break;
    case Token::EQUAL_EQUAL:
        _evalResults.push_back(toLoxBoolean(left->equals(*right)));
        break;
    default:
        // Unreachable.
        break;
    }
}

void Interpreter::visit(CallExpr const& expr)
{
    auto callee = evaluate(*expr.callee);

    std::vector<std::shared_ptr<LoxObject>> args;
    for (auto const& arg : expr.args) {
        args.push_back(evaluate(*arg));
    }

    auto callable = dynamic_cast<LoxCallable*>(callee.get());
    if (!callable) {
        throw RuntimeError(expr.paren, "Can only call functions and classes.");
    }

    if (args.size() != callable->arity()) {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(callable->arity()) + " arguments but got " +
                                           std::to_string(args.size()) + ".");
    }

    _evalResults.push_back(callable->call(args));
}

void Interpreter::visit(GetExpr const& expr)
{
    auto object = evaluate(*expr.object);

    if (auto instance = dynamic_cast<LoxInstance*>(object.get())) {
        _evalResults.push_back(instance->get(expr.name));
        return;
    }

    throw RuntimeError(expr.name, "Only instances have properties.");
}

void Interpreter::visit(GroupingExpr const& expr)
{
    _evalResults.push_back(evaluate(*expr.expr));
}

void Interpreter::visit(LiteralExpr const& expr)
{
    _evalResults.push_back(expr.value);
}

void Interpreter::visit(LogicalExpr const& expr)
{
    auto left = evaluate(*expr.left);

    // Check left and short-circuit if possible.
    if (expr.op.type == Token::OR) {
        if (left->isTruthy()) {
            _evalResults.push_back(left);
            return;
        }
    }
    else {
        LOX_ASSERT(expr.op.type == Token::AND);
        if (!left->isTruthy()) {
            _evalResults.push_back(left);
            return;
        }
    }

    _evalResults.push_back(evaluate(*expr.right));
}

void Interpreter::visit(SetExpr const& expr)
{
    auto object = evaluate(*expr.object);

    if (auto instance = dynamic_cast<LoxInstance*>(object.get())) {
        auto value = evaluate(*expr.value);
        instance->set(expr.name, value);

        _evalResults.push_back(value);
        return;
    }

    throw RuntimeError(expr.name, "Only instances have fields.");
}

void Interpreter::visit(ThisExpr const& expr)
{
    _evalResults.push_back(lookUpVariable(expr.keyword, expr));
}

void Interpreter::visit(SuperExpr const& expr)
{
    LOX_ASSERT(expr.keyword.lexeme == "super");

    if (auto it = _locals.find(&expr); it != _locals.end()) {
        auto distance = it->second;
        auto superclass = std::dynamic_pointer_cast<LoxClass>(_environment->getAt(distance, "super"));
        auto instance = std::dynamic_pointer_cast<LoxInstance>(_environment->getAt(distance - 1, "this"));
        if (superclass && instance) {
            auto method = superclass->findMethod(expr.method.lexeme);
            if (!method) {
                throw RuntimeError(expr.method, "Undefined property '" + expr.method.lexeme + "'.");
            }
            _evalResults.push_back(method->bind(instance));
            return;
        }
    }

    LOX_ASSERT(false); // If we have reached here, we have a scope resolve bug.
}

void Interpreter::visit(UnaryExpr const& expr)
{
    auto right = evaluate(*expr.right);
    LOX_ASSERT(right);

    if (expr.op.type == Token::BANG) {
        _evalResults.push_back(toLoxBoolean(!right->isTruthy()));
    }
    else {
        LOX_ASSERT(expr.op.type == Token::MINUS);
        ensureOperand<LoxNumber>(expr.op, right, [this](double value) {
            _evalResults.push_back(toLoxNumber(-value));
        });
    }
}

void Interpreter::visit(VariableExpr const& expr)
{
    _evalResults.push_back(lookUpVariable(expr.name, expr));
}

void Interpreter::execute(Stmt const& stmt)
{
    stmt.accept(*this);
}

void Interpreter::executeBlock(std::vector<std::shared_ptr<Stmt>> const& stmts,
                               std::shared_ptr<Environment> const& environment)
{
    auto previous = _environment;
    try {
        _environment = environment;
        for (auto const& stmt : stmts) {
            execute(*stmt);
        }
        _environment = previous;
    }
    catch (...) {
        _environment = previous;
        throw;
    }
}

std::shared_ptr<LoxObject> Interpreter::evaluate(Expr const& expr)
{
#ifdef LOX_DEBUG
    auto prevResultsCount = _evalResults.size();
#endif

    expr.accept(*this);

#ifdef LOX_DEBUG
    LOX_ASSERT(_evalResults.size() == prevResultsCount + 1);
#endif

    auto value = _evalResults.back();
    _evalResults.pop_back();
    return value;
}

std::shared_ptr<LoxObject> Interpreter::lookUpVariable(Token const& name, Expr const& expr) const
{
    if (auto it = _locals.find(&expr); it != _locals.end()) {
        auto distance = it->second;
        return _environment->getAt(distance, name.lexeme);
    }
    return _globals->get(name);
}

template <typename OperandType, typename Callback>
bool Interpreter::matchOperand(std::shared_ptr<LoxObject> const& right, Callback&& callback)
{
    if (auto r = dynamic_cast<OperandType*>(right.get())) {
        callback(r->value);
        return true;
    }
    return false;
}

template <typename OperandType, typename Callback>
void Interpreter::ensureOperand(Token const& op, std::shared_ptr<LoxObject> const& right, Callback&& callback)
{
    if (matchOperand<OperandType>(right, std::move(callback)))
        return;

    throw RuntimeError{op, OperandErrorMessage<OperandType, 1>::get()};
}

template <typename OperandType, typename Callback>
bool Interpreter::matchOperands(std::shared_ptr<LoxObject> const& left, std::shared_ptr<LoxObject> const& right,
                                Callback&& callback)
{
    if (auto l = dynamic_cast<OperandType*>(left.get())) {
        if (auto r = dynamic_cast<OperandType*>(right.get())) {
            callback(l->value, r->value);
            return true;
        }
    }
    return false;
}

template <typename OperandType, typename Callback>
void Interpreter::ensureOperands(Token const& op, std::shared_ptr<LoxObject> const& left,
                                 std::shared_ptr<LoxObject> const& right, Callback&& callback)
{
    if (matchOperands<OperandType>(left, right, std::move(callback)))
        return;

    throw RuntimeError{op, OperandErrorMessage<OperandType, 2>::get()};
}

std::shared_ptr<LoxFunction> Interpreter::makeFunction(bool isInitializer, Token const& name,
                                                       std::vector<Token> const params,
                                                       std::vector<std::shared_ptr<Stmt>> const& body)
{
    auto executor = [this](std::shared_ptr<Environment> const& env,
                           std::vector<std::shared_ptr<Stmt>> const& stmts) -> std::shared_ptr<LoxObject> {
        try {
            executeBlock(stmts, env);
        }
        catch (ReturnValue& retVal) {
            return retVal.object;
        }
        return makeLoxNil();
    };

    return _lox->create<LoxFunction>(_lox, _environment, isInitializer, name, params, body, executor);
}

} // namespace cloxx