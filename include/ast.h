#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <iostream>

struct ASTNode {
    virtual void print(int indent = 0) const = 0;
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;
using LiteralValue = std::variant<int, double, char, bool, std::string>;

// ===== EXPRESSIONS =====
struct ASTLiteral : public ASTNode {
    LiteralValue value;
    explicit ASTLiteral(LiteralValue val) : value(std::move(val)) {}
    void print(int indent = 0) const override;
};

struct ASTIdentifier : public ASTNode {
    std::string name;
    explicit ASTIdentifier(std::string n) : name(std::move(n)) {}
    void print(int indent = 0) const override;
};

struct ASTBinaryExpression : public ASTNode {
    ASTNodePtr left, right;
    std::string op;
    ASTBinaryExpression(ASTNodePtr l, ASTNodePtr r, std::string o)
        : left(std::move(l)), right(std::move(r)), op(std::move(o)) {}
    void print(int indent = 0) const override;
};

struct ASTUnaryExpression : public ASTNode {
    std::string op;
    ASTNodePtr operand;
    ASTUnaryExpression(std::string o, ASTNodePtr expr) : op(std::move(o)), operand(std::move(expr)) {}
    void print(int indent = 0) const override;
};

struct ASTArrayLiteral : public ASTNode {
    std::vector<ASTNodePtr> elements;
    explicit ASTArrayLiteral(std::vector<ASTNodePtr> elems) : elements(std::move(elems)) {}
    void print(int indent = 0) const override;
};

struct ASTGroupedExpression : public ASTNode {
    ASTNodePtr expression;
    explicit ASTGroupedExpression(ASTNodePtr expr) : expression(std::move(expr)) {}
    void print(int indent = 0) const override;
};

struct ASTFunctionCall : public ASTNode {
    ASTNodePtr callee;
    std::vector<ASTNodePtr> arguments;
    ASTFunctionCall(ASTNodePtr callee, std::vector<ASTNodePtr> args)
        : callee(std::move(callee)), arguments(std::move(args)) {}
    void print(int indent = 0) const override;
};

struct ASTArrayAccess : public ASTNode {
    ASTNodePtr array;
    ASTNodePtr index;
    ASTArrayAccess(ASTNodePtr arr, ASTNodePtr idx)
        : array(std::move(arr)), index(std::move(idx)) {}
    void print(int indent = 0) const override;
};

// ===== STATEMENTS =====
struct ASTAssignment : public ASTNode {
    std::string variable;
    ASTNodePtr expression;
    ASTAssignment(std::string var, ASTNodePtr expr)
        : variable(std::move(var)), expression(std::move(expr)) {}
    void print(int indent = 0) const override;
};

struct ASTOutput : public ASTNode {
    ASTNodePtr expression;
    explicit ASTOutput(ASTNodePtr expr) : expression(std::move(expr)) {}
    void print(int indent = 0) const override;
};

struct ASTInput : public ASTNode {
    std::string variable;
    explicit ASTInput(std::string var) : variable(std::move(var)) {}
    void print(int indent = 0) const override;
};

struct ASTReturn : public ASTNode {
    ASTNodePtr expression;
    explicit ASTReturn(ASTNodePtr expr) : expression(std::move(expr)) {}
    void print(int indent = 0) const override;
};

struct ASTIf : public ASTNode {
    ASTNodePtr condition, thenBlock, elseBlock;
    ASTIf(ASTNodePtr cond, ASTNodePtr thenBlk, ASTNodePtr elseBlk = nullptr)
        : condition(std::move(cond)), thenBlock(std::move(thenBlk)), elseBlock(std::move(elseBlk)) {}
    void print(int indent = 0) const override;
};

struct ASTFor : public ASTNode {
    ASTNodePtr init, condition, increment, body;
    ASTFor(ASTNodePtr i, ASTNodePtr cond, ASTNodePtr inc, ASTNodePtr b)
        : init(std::move(i)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

// ===== STRUCTURES =====
struct ASTBlock : public ASTNode {
    std::vector<ASTNodePtr> statements;
    explicit ASTBlock(std::vector<ASTNodePtr> stmts) : statements(std::move(stmts)) {}
    void print(int indent = 0) const override;
};

struct ASTFunction : public ASTNode {
    std::string name;
    std::vector<std::string> parameters;
    ASTNodePtr body;
    ASTFunction(std::string n, std::vector<std::string> p, ASTNodePtr b)
        : name(std::move(n)), parameters(std::move(p)), body(std::move(b)) {}
    void print(int indent = 0) const override;
};

struct ASTProgram : public ASTNode {
    std::vector<ASTNodePtr> functions;
    explicit ASTProgram(std::vector<ASTNodePtr> funcs) : functions(std::move(funcs)) {}
    void print(int indent = 0) const override;
};

#endif // AST_H
