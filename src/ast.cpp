#include "../include/ast.h"

void ASTLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Literal: ";
    std::visit([](const auto& val) { std::cout << val; }, value);
    std::cout << "\n";
}

void ASTIdentifier::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Identifier: " << name << "\n";
}

void ASTBinaryExpression::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BinaryExpr (" << op << ")\n";
    left->print(indent + 2);
    right->print(indent + 2);
}

void ASTUnaryExpression::print(int indent) const {
    std::cout << std::string(indent, ' ') << "UnaryExpr (" << op << ")\n";
    operand->print(indent + 2);
}

void ASTArrayLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ArrayLiteral:\n";
    for (const auto& e : elements) e->print(indent + 2);
}

void ASTGroupedExpression::print(int indent) const {
    std::cout << std::string(indent, ' ') << "GroupedExpr:\n";
    expression->print(indent + 2);
}

void ASTFunctionCall::print(int indent) const {
    std::cout << std::string(indent, ' ') << "FunctionCall:\n";
    callee->print(indent + 2);
    for (const auto& arg : arguments)
        arg->print(indent + 2);
}

void ASTArrayAccess::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ArrayAccess:\n";
    array->print(indent + 2);
    index->print(indent + 2);
}

void ASTAssignment::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Assignment: " << variable << "\n";
    expression->print(indent + 2);
}

void ASTOutput::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Output:\n";
    expression->print(indent + 2);
}

void ASTInput::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Input: " << variable << "\n";
}

void ASTReturn::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Return:\n";
    if (expression) expression->print(indent + 2);
    else std::cout << std::string(indent + 2, ' ') << "None\n";
}

void ASTIf::print(int indent) const {
    std::cout << std::string(indent, ' ') << "If:\n";
    condition->print(indent + 2);
    thenBlock->print(indent + 2);
    if (elseBlock) {
        std::cout << std::string(indent, ' ') << "Else:\n";
        elseBlock->print(indent + 2);
    }
}

void ASTFor::print(int indent) const {
    std::cout << std::string(indent, ' ') << "For:\n";
    init->print(indent + 2);
    condition->print(indent + 2);
    increment->print(indent + 2);
    body->print(indent + 2);
}

void ASTBlock::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Block:\n";
    for (const auto& stmt : statements)
        stmt->print(indent + 2);
}

void ASTFunction::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Function: " << name << "\n";
    for (const auto& p : parameters)
        std::cout << std::string(indent + 2, ' ') << "Param: " << p << "\n";
    body->print(indent + 2);
}

void ASTProgram::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Program:\n";
    for (const auto& fn : functions)
        fn->print(indent + 2);
}
