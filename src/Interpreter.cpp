#include "Interpreter.hpp"

#include "Assert.hpp"
#include "Environment.hpp"
#include "ErrorReporter.hpp"
#include "Resolver.hpp"
#include "RuntimeError.hpp"

#include "LoxBool.hpp"
#include "LoxClass.hpp"
#include "LoxList.hpp"
#include "LoxNil.hpp"
#include "LoxNumber.hpp"
#include "LoxString.hpp"
#include "LoxUserFunction.hpp"

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

class Interpreter::ScopeSwitcher {
public:
    ScopeSwitcher(Interpreter* interpreter, std::shared_ptr<Environment> const& newEnvironment)
        : _interpreter{interpreter}
    {
        _oldEnvironment = _interpreter->_environment;
        _interpreter->_environment = newEnvironment;
    }

    ~ScopeSwitcher()
    {
        _interpreter->_environment = _oldEnvironment;
    }

private:
    Interpreter* _interpreter;
    std::shared_ptr<Environment> _oldEnvironment;
};

Interpreter::Interpreter(ErrorReporter* errorReporter, GlobalObjectsProc globalObjectsProc)
    : _errorReporter{errorReporter}

{
    _globals = _runtime.root();
    _environment = _globals;

    // System defined objects.
    for (auto& [name, value] : globalObjectsProc(&_runtime)) {
        _globals->define(name, value);
    }
}

void Interpreter::interpret(Stmt const& stmt)
{
    try {
        execute(stmt);
    }
    catch (RuntimeError& error) {
        _errorReporter->runtimeError(error.token, error.what());
    }

    _runtime.collectGarbage();
}

void Interpreter::visit(BlockStmt const& stmt)
{
    auto blockEnv = _runtime.createEnvironment(_environment);
    executeBlock(stmt.stmts, blockEnv);
}

void Interpreter::visit(ExprStmt const& stmt)
{
    evaluate(stmt.expr);
}

void Interpreter::visit(ForStmt const& stmt)
{
    ScopeSwitcher _{this, _runtime.createEnvironment(_environment)};

    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }

    while (true) {
        if (stmt.condition && !evaluate(*stmt.condition)->isTruthy()) {
            break;
        }

        try {
            execute(stmt.body);
        }
        catch (LoopBreak&) {
            break;
        }
        catch (LoopContinue&) {
            // fall through to increment
        }

        if (stmt.increment) {
            stmt.increment->accept(*this);
        }
    }
}

void Interpreter::visit(IfStmt const& stmt)
{
    if (evaluate(stmt.cond)->isTruthy()) {
        execute(stmt.thenBranch);
    }
    else if (stmt.elseBranch) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::visit(ImportStmt const& stmt)
{
    // TODO: load file from stmt.path
    auto filePath = parseString(stmt.filePath);
    _errorReporter->runtimeError(stmt.keyword, "Cannot load module from \"" + stmt.filePath.lexeme + "\"");
}

void Interpreter::visit(BreakStmt const&)
{
    throw LoopBreak{};
}

void Interpreter::visit(ContinueStmt const&)
{
    throw LoopContinue{};
}

void Interpreter::visit(ReturnStmt const& stmt)
{
    ReturnValue returnValue;
    if (stmt.value) {
        returnValue.object = evaluate(*stmt.value);
    }
    else {
        returnValue.object = _runtime.getNil();
    }

    throw returnValue;
}

void Interpreter::visit(VarStmt const& stmt)
{
    std::shared_ptr<LoxObject> value;
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    else {
        value = _runtime.getNil();
    }
    _environment->define(stmt.name.lexeme, value);
}

void Interpreter::visit(FunStmt const& stmt)
{
    auto function = makeFunction(/*isInitializer*/ false, stmt.name, stmt.params, stmt.body);
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

    _environment->define(stmt.name.lexeme, _runtime.getNil());

    auto enclosingEnvironment = _environment;
    if (superclass) {
        _environment = _runtime.createEnvironment(_environment);
        _environment->define("super", superclass);
    }

    std::map<std::string, std::shared_ptr<LoxObject>> fields;
    for (auto const& field : stmt.fields) {
        std::shared_ptr<LoxObject> value;
        if (field.initializer) {
            value = evaluate(*field.initializer);
        }
        else {
            value = _runtime.getNil();
        }
        fields.emplace(field.name.lexeme, value);
    }

    std::map<std::string, std::shared_ptr<LoxFunction>> methods;
    for (auto const& method : stmt.methods) {
        bool isInitializer = method.name.lexeme == "init";
        auto function = makeFunction(isInitializer, method.name, method.params, method.body);
        methods.emplace(method.name.lexeme, function);
    }

    auto klass = _runtime.create<LoxClass>(stmt.name.lexeme, superclass, std::move(fields), std::move(methods),
                                           /*objectFactory*/ nullptr);

    if (superclass) {
        _environment = enclosingEnvironment;
    }

    _environment->assign(stmt.name, klass);
}

void Interpreter::visit(AssignExpr const& expr)
{
    auto value = evaluate(expr.value);
    LOX_ASSERT(value);

    if (expr.depth() >= 0) {
        _environment->assignAt(expr.depth(), expr.name.lexeme, value);
    }
    else {
        _globals->assign(expr.name, value);
    }
    _evalResults.push_back(value);
}

void Interpreter::visit(BinaryExpr const& expr)
{
    auto left = evaluate(expr.left);
    auto right = evaluate(expr.right);

    LOX_ASSERT(left);
    LOX_ASSERT(right);

    switch (expr.op.type) {
    case Token::GREATER:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxBool(left > right));
        });
        break;
    case Token::GREATER_EQUAL:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxBool(left >= right));
        });
        break;
    case Token::LESS:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxBool(left < right));
        });
        break;
    case Token::LESS_EQUAL:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxBool(left <= right));
        });
        break;
    case Token::MINUS:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxNumber(left - right));
        });
        break;
    case Token::PLUS:
        if (matchOperands<LoxNumber>(left, right, [this](auto left, auto right) {
                _evalResults.push_back(_runtime.toLoxNumber(left + right));
            })) {
            break; // handled number + number
        }
        if (matchOperands<LoxString>(left, right, [this](auto& left, auto& right) {
                _evalResults.push_back(_runtime.toLoxString(left + right));
            })) {
            break; // handled string + string
        }
        throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
    case Token::SLASH:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxNumber(left / right));
        });
        break;
    case Token::STAR:
        ensureOperands<LoxNumber>(expr.op, left, right, [this](auto left, auto right) {
            _evalResults.push_back(_runtime.toLoxNumber(left * right));
        });
        break;
    case Token::BANG_EQUAL:
        _evalResults.push_back(_runtime.toLoxBool(!left->equals(right)));
        break;
    case Token::EQUAL_EQUAL:
        _evalResults.push_back(_runtime.toLoxBool(left->equals(right)));
        break;
    default:
        // Unreachable.
        break;
    }
}

void Interpreter::visit(CallExpr const& expr)
{
    auto callee = evaluate(expr.callee);

    std::vector<std::shared_ptr<LoxObject>> args;
    for (auto const& arg : expr.args) {
        args.push_back(evaluate(arg));
    }

    auto callable = dynamic_cast<Callable*>(callee.get());
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
    auto object = evaluate(expr.object);
    _evalResults.push_back(object->get(expr.name));
}

void Interpreter::visit(GroupingExpr const& expr)
{
    _evalResults.push_back(evaluate(expr.expr));
}

void Interpreter::visit(LiteralExpr const& expr)
{
    std::shared_ptr<LoxObject> value;
    if (expr.literal.type == Token::FALSE || expr.literal.type == Token::TRUE) {
        value = _runtime.toLoxBool(expr.literal.type == Token::TRUE);
    }
    else if (expr.literal.type == Token::NUMBER) {
        value = _runtime.toLoxNumber(parseNumber(expr.literal));
    }
    else if (expr.literal.type == Token::STRING) {
        value = _runtime.toLoxString(parseString(expr.literal));
    }
    else {
        LOX_ASSERT(expr.literal.type == Token::NIL);
        value = _runtime.getNil();
    }
    _evalResults.push_back(value);
}

void Interpreter::visit(LogicalExpr const& expr)
{
    auto left = evaluate(expr.left);

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

    _evalResults.push_back(evaluate(expr.right));
}

void Interpreter::visit(SetExpr const& expr)
{
    auto object = evaluate(expr.object);
    auto value = evaluate(expr.value);

    object->set(expr.name, value);

    _evalResults.push_back(value);
}

void Interpreter::visit(ThisExpr const& expr)
{
    std::shared_ptr<LoxObject> object;
    if (expr.depth() >= 0) {
        object = _environment->getAt(expr.depth(), expr.keyword.lexeme);
    }
    else {
        object = _globals->get(expr.keyword);
    }

    _evalResults.push_back(object);
}

void Interpreter::visit(SuperExpr const& expr)
{
    LOX_ASSERT(expr.keyword.lexeme == "super");

    auto distance = expr.depth();
    LOX_ASSERT(distance >= 0);

    auto superclass = std::static_pointer_cast<LoxClass>(_environment->getAt(distance, "super"));
    auto instance = _environment->getAt(distance - 1, "this");
    LOX_ASSERT(superclass);
    LOX_ASSERT(instance);

    auto method = superclass->findMethod(expr.method.lexeme);
    if (!method) {
        throw RuntimeError(expr.method, "Undefined property '" + expr.method.lexeme + "'.");
    }
    _evalResults.push_back(method->bind(instance));
}

void Interpreter::visit(UnaryExpr const& expr)
{
    auto right = evaluate(expr.right);
    LOX_ASSERT(right);

    if (expr.op.type == Token::BANG) {
        _evalResults.push_back(_runtime.toLoxBool(!right->isTruthy()));
    }
    else {
        LOX_ASSERT(expr.op.type == Token::MINUS);
        ensureOperand<LoxNumber>(expr.op, right, [this](double value) {
            _evalResults.push_back(_runtime.toLoxNumber(-value));
        });
    }
}

void Interpreter::visit(VariableExpr const& expr)
{
    std::shared_ptr<LoxObject> object;
    if (expr.depth() >= 0) {
        object = _environment->getAt(expr.depth(), expr.name.lexeme);
    }
    else {
        object = _globals->get(expr.name);
    }
    _evalResults.push_back(object);
}

void Interpreter::execute(Stmt const& stmt)
{
    stmt.accept(*this);
}

void Interpreter::executeBlock(std::vector<Stmt> const& stmts, std::shared_ptr<Environment> const& environment)
{
    ScopeSwitcher _{this, environment};

    for (auto const& stmt : stmts) {
        execute(stmt);
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
                                                       std::vector<Token> const params, std::vector<Stmt> const& body)
{
    auto executor = [this](std::shared_ptr<Environment> const& env,
                           std::vector<Stmt> const& stmts) -> std::shared_ptr<LoxObject> {
        try {
            executeBlock(stmts, env);
        }
        catch (ReturnValue& retVal) {
            return retVal.object;
        }
        return _runtime.getNil();
    };

    return _runtime.create<LoxUserFunction>(_environment, isInitializer, name, params, body, executor);
}

double Interpreter::parseNumber(Token const& token)
{
    LOX_ASSERT(token.type == Token::NUMBER);

    return std::stod(token.lexeme);
}

std::string Interpreter::parseString(Token const& token)
{
    LOX_ASSERT(token.type == Token::STRING);
    LOX_ASSERT(token.lexeme.length() >= 2);

    auto const& lexeme = token.lexeme;
    return lexeme.substr(1, lexeme.size() - 2);
}

} // namespace cloxx
