#!/usr/bin/env python3
#!/usr/bin/env python3

import os.path
import sys

def _makeParamType(type):
    if type == 'LoxObject':
        return 'std::shared_ptr<LoxObject> const&'
    if type[-1] == '?':
        return 'std::optional<' + type[:-1] + '> const&'
    if type.startswith('List<'):
        itemType = type[5:-1]
        return 'std::vector<' + itemType + '> const&'
    return type + ' const&'

def _makeMemVarType(type):
    if type == 'LoxObject':
        return 'std::shared_ptr<LoxObject>'
    if type[-1] == '?':
        return 'std::optional<' + type[:-1] + '>'
    if type.startswith('List<'):
        itemType = type[5:-1]
        return 'std::vector<' + itemType + '>'
    return type

def _declareFactoryFunction(file, node):
    file.write('    friend ' + node.name + ' make' + node.name + '(')
    for index, field in enumerate(node.fields):
        type = _makeParamType(field.type)
        file.write(type + ' ' + field.name)
        if index < len(node.fields) - 1:
           file.write(', ')
        else:
           file.write(');\n')

def _implementFactoryFunction(file, node):
    file.write('inline ' + node.name + ' make' + node.name + '(')
    for index, field in enumerate(node.fields):
        type = _makeParamType(field.type)
        file.write(type + ' ' + field.name)
        if index < len(node.fields) - 1:
           file.write(', ')
        else:
           file.write(')\n')
    file.write('{\n')
    file.write('    return ' + node.name + '{std::make_shared<' + node.name + '::Data>(')
    for index, field in enumerate(node.fields):
        file.write(field.name)
        if index < len(node.fields) - 1:
           file.write(', ')
        else:
           file.write(')};\n')
    file.write('}\n')


def _defineNode(file, baseName, node):
    # Node class
    file.write('class ' + node.name + ' final {\n')
    file.write('public:\n')
    file.write('    ' + node.name + '(' + node.name + ' const& other);\n')
    file.write('\n')
    for field in node.fields:
        file.write('    ' + _makeParamType(field.type) + ' ' + field.name + ';\n')
    if node.needsResolving:
        file.write('\n')
        file.write('    int depth() const;\n')
        file.write('    void resolve(int depth);\n')
    file.write('\n')
    file.write('private:\n')
    file.write('    friend class ' + baseName + ';\n')
    _declareFactoryFunction(file, node)
    file.write('\n')
    file.write('    struct Data;\n')
    file.write('    std::shared_ptr<Data> _data;\n')
    file.write('\n')
    file.write('    explicit ' + node.name + '(std::shared_ptr<Data> const& data);\n')
    file.write('};\n')
    file.write('\n')

    # Data class
    file.write('struct ' + node.name + '::Data : ' + baseName + '::Data, std::enable_shared_from_this<Data> {\n')
    file.write('    Data(')
    for index, field in enumerate(node.fields):
        type = _makeParamType(field.type)
        file.write(type + ' ' + field.name)
        if index < len(node.fields) - 1:
           file.write(', ')
        else:
           file.write(')\n')
    file.write('        : ')
    for index, field in enumerate(node.fields):
        file.write(field.name + '{' + field.name + '}')
        if index < len(node.fields) - 1:
            file.write(', ')
        else:
            file.write('\n')
    file.write('    {}\n')
    file.write('\n')
    file.write('    void accept(' + baseName + 'Visitor& visitor) override\n')
    file.write('    {\n')
    file.write('        visitor.visit(' + node.name + '{shared_from_this()});\n')
    file.write('    }\n')
    file.write('\n')
    for field in node.fields:
        type = _makeMemVarType(field.type);
        file.write('    ' + type + ' ' + field.name + ';\n')
    if node.needsResolving:
        file.write('    int depth;\n')
    file.write('};\n')
    file.write('\n')

    # Node constructors
    file.write('inline ' + node.name + '::' + node.name +  '(' + node.name + ' const& other)\n')
    file.write('    : ')
    for field in node.fields:
        file.write(field.name + '{other.' + field.name + '}, ')
    file.write('_data{other._data} {}\n')
    file.write('\n')
    file.write('inline ' + node.name + '::' + node.name +  '(std::shared_ptr<Data> const& data)\n')
    file.write('    : ')
    for field in node.fields:
        file.write(field.name + '{data->' + field.name + '}, ')
    file.write('_data{data} {}\n')

    if node.needsResolving:
        file.write('\n')
        file.write('inline int ' + node.name + '::depth() const\n')
        file.write('{\n')
        file.write('    return _data->depth;\n')
        file.write('}\n')
        file.write('\n')
        file.write('inline void ' + node.name + '::resolve(int depth)\n')
        file.write('{\n')
        file.write('    _data->depth = depth;\n')
        file.write('}\n')


def _defineAst(file, headers, baseName, nodes):
    file.write('// This is auto-generated by ' + os.path.basename(__file__) + '. Do not modify manually.\n')
    file.write('\n')
    file.write('#pragma once\n')
    file.write('\n')
    file.write('#include <optional>\n')
    file.write('#include <memory>\n')
    file.write('#include <vector>\n')
    file.write('\n')
    for header in headers:
        file.write('#include "' + header + '"\n')
    file.write('\n')
    file.write('namespace cloxx {\n')
    file.write('\n')
    
    # Forward declare types.
    for node in nodes:
        file.write('class ' + node.name + ';\n')
    file.write('\n')

    # Define Visitor
    file.write('class ' + baseName + 'Visitor {\n')
    file.write('public:\n')
    file.write('    virtual ~' + baseName + 'Visitor() = default;\n')
    file.write('\n')
    for node in nodes:
        file.write('    virtual void visit(' + node.name + ' const&) = 0;\n')
    file.write('};\n')
    file.write('\n')
    
    # Begin the base class.
    file.write('class ' + baseName + ' final {\n')
    file.write('public:\n')

    # Node to Base conversion
    for node in nodes:
        file.write('    ' + baseName + '(' + node.name + ' const& other);\n')
    file.write('\n')

    for node in nodes:
        file.write('    std::optional<' + node.name + '> to' + node.name + '() const;\n')
    file.write('\n')

    # Node to Base conversion
    file.write('    template <typename T>\n')
    file.write('    ' + baseName + '& operator=(T const& rhs)\n')
    file.write('    {\n')
    file.write('        _data = rhs._data;\n')
    file.write('        return *this;\n')
    file.write('    }\n')
    file.write('\n')
    
    # Declare accept() forwarding
    file.write('    void accept(' + baseName + 'Visitor& visitor) const\n')
    file.write('    {\n')
    file.write('        _data->accept(visitor);\n')
    file.write('    }\n')
    file.write('\n')

    file.write('private:\n')

    for node in nodes:
        file.write('    friend class ' + node.name + ';\n')
    file.write('\n')

    file.write('    struct Data;\n')
    file.write('    explicit ' + baseName + '(std::shared_ptr<Data> const& impl) : _data{impl} {}\n')
    file.write('\n')

    file.write('    struct Data {\n')
    file.write('        virtual ~Data() = default;\n')
    file.write('        virtual void accept(' + baseName + 'Visitor& visitor) = 0;\n')
    file.write('    };\n')
    file.write('    std::shared_ptr<Data> _data;\n')
    file.write('};\n')
    file.write('\n')

    for node in nodes:
        _defineNode(file, baseName, node)
        file.write('\n')
        _implementFactoryFunction(file, node)
        file.write('\n')
        file.write('inline std::optional<' + node.name + '> ' + baseName + '::to' + node.name + '() const\n')
        file.write('{\n')
        file.write('    if (auto data = std::dynamic_pointer_cast<' + node.name + '::Data>(_data)) {\n')
        file.write('        return ' + node.name + '{data};\n')
        file.write('    }\n')
        file.write('    return std::nullopt;\n')
        file.write('}\n')
        file.write('\n')
        file.write('inline ' + baseName + '::' + baseName + '(' + node.name + ' const& other)\n')
        file.write('{\n')
        file.write('    _data = other._data;\n')
        file.write('}\n')
        file.write('\n')
    file.write('\n')

    file.write('} // cloxx\n')


class Field:
    def __init__(self, spec):
        tokens = spec.split(' ')
        self.type = tokens[0].strip()
        self.name = tokens[1].strip()

    def __str__(self):
        return self.type + ' ' + self.name


class Node:
    def __init__(self, baseName, spec):
        tokens = spec.split(':')
        prefix = tokens[0].strip()
        if prefix[-1] == '^':
            prefix = prefix[:-1]
            self.needsResolving = True
        else:
            self.needsResolving = False
        self.name = prefix + baseName
        self.fields = []
        for fieldSpec in [x.strip() for x in tokens[1].split(',')]:
            self.fields.append(Field(fieldSpec))

    def __str__(self):
        return self.name + ': ' + ', '.join([str(field) for field in self.fields])

def _generateAst(outputDir, headers, baseName, nodeSpecs):
    nodes = []
    for nodeSpec in nodeSpecs:
        nodes.append(Node(baseName, nodeSpec))
    # for node in nodes:
    #     print(node)
    with open(os.path.join(outputDir, baseName + '.hpp'), 'w') as file:
       _defineAst(file, headers, baseName, nodes)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: ' + __file__ + ' dirpath')

    outputDir = sys.argv[1]

    _generateAst(outputDir, ['Token.hpp', 'LoxObject.hpp'], 'Expr', [
        "Assign^   : Token name, Expr value",
        "Binary    : Token op, Expr left, Expr right",
        "Call      : Expr callee, Token paren, List<Expr> args",
        "Get       : Expr object, Token name",
        "Grouping  : Expr expr",
        "Literal   : LoxObject value",
        "Logical   : Token op, Expr left, Expr right",
        "Set       : Expr object, Token name, Expr value",
        "Super^    : Token keyword, Token method",
        "This^     : Token keyword",
        "Unary     : Token op, Expr right",
        "Variable^ : Token name",
    ])

    _generateAst(outputDir, ['Token.hpp', 'Expr.hpp'], 'Stmt', [
        "Block    : List<Stmt> stmts",
        "Break    : Token keyword",
        "Class    : Token name, VariableExpr? superclass, List<FunStmt> methods",
        "Continue : Token keyword",
        "Expr     : Expr expr",
        "For      : Stmt? initializer, Expr? condition, Stmt? increment, Stmt body",
        "Fun      : Token name, List<Token> params, List<Stmt> body",
        "If       : Expr cond, Stmt thenBranch, Stmt? elseBranch",
        "Return   : Token keyword, Expr? value",
        "Var      : Token name, Expr? initializer",
    ])