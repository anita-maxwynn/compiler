#include "../include/interpreter.h"
#include <iostream>
#include <sstream>

Interpreter::Interpreter() : hasReturnValue(false) {}

// Main execution - finds and runs the main function
void Interpreter::execute(std::shared_ptr<ASTProgram> program) {
    // First, register all functions
    for (const auto& funcNode : program->functions) {
        auto func = std::dynamic_pointer_cast<ASTFunction>(funcNode);
        if (func) {
            functions[func->name] = func;
        }
    }
    
    // Look for main function and execute it
    if (functions.find("main") != functions.end()) {
        std::cout << "=== Executing Program ===" << std::endl;
        callFunction("main", {});
    } else {
        std::cerr << "Error: No main function found!" << std::endl;
    }
}

// THE CORE: Evaluate expressions and return RuntimeValue
RuntimeValue Interpreter::evaluateExpression(ASTNodePtr expr) {
    // Literal values - convert to RuntimeValue
    if (auto literal = std::dynamic_pointer_cast<ASTLiteral>(expr)) {
        // Convert LiteralValue to RuntimeValue
        return std::visit([](const auto& val) -> RuntimeValue {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                return RuntimeValue(val);
            } else if constexpr (std::is_same_v<T, double>) {
                return RuntimeValue(val);
            } else if constexpr (std::is_same_v<T, char>) {
                return RuntimeValue(std::string(1, val));
            } else if constexpr (std::is_same_v<T, bool>) {
                return RuntimeValue(val);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return RuntimeValue(val);
            }
            return RuntimeValue();
        }, literal->value);
    }
    
    // Variable lookup
    if (auto identifier = std::dynamic_pointer_cast<ASTIdentifier>(expr)) {
        return getVariable(identifier->name);
    }
    
    // Binary expressions - use our type coercion system!
    if (auto binary = std::dynamic_pointer_cast<ASTBinaryExpression>(expr)) {
        RuntimeValue left = evaluateExpression(binary->left);
        RuntimeValue right = evaluateExpression(binary->right);
        return performBinaryOperation(left, right, binary->op);
    }
    
    // Unary expressions
    if (auto unary = std::dynamic_pointer_cast<ASTUnaryExpression>(expr)) {
        RuntimeValue operand = evaluateExpression(unary->operand);
        return performUnaryOperation(operand, unary->op);
    }
    
    // Function calls
    if (auto funcCall = std::dynamic_pointer_cast<ASTFunctionCall>(expr)) {
        auto callee = std::dynamic_pointer_cast<ASTIdentifier>(funcCall->callee);
        if (!callee) {
            std::cerr << "Error: Invalid function call" << std::endl;
            return RuntimeValue();
        }
        
        // Evaluate arguments
        std::vector<RuntimeValue> args;
        for (const auto& arg : funcCall->arguments) {
            args.push_back(evaluateExpression(arg));
        }
        
        return callFunction(callee->name, args);
    }
    
    // Array literals
    if (auto arrayLit = std::dynamic_pointer_cast<ASTArrayLiteral>(expr)) {
        std::vector<RuntimeValue> elements;
        for (const auto& elem : arrayLit->elements) {
            elements.push_back(evaluateExpression(elem));
        }
        return RuntimeValue(elements);
    }
    
    // Array access
    if (auto arrayAccess = std::dynamic_pointer_cast<ASTArrayAccess>(expr)) {
        RuntimeValue array = evaluateExpression(arrayAccess->array);
        RuntimeValue index = evaluateExpression(arrayAccess->index);
        
        if (array.type != RuntimeType::ARRAY) {
            std::cerr << "Error: Trying to index non-array value" << std::endl;
            return RuntimeValue();
        }
        
        int idx = static_cast<int>(getNumericValue(index));
        if (idx < 0 || idx >= static_cast<int>(array.arrayValue->size())) {
            std::cerr << "Error: Array index out of bounds" << std::endl;
            return RuntimeValue();
        }
        
        return (*array.arrayValue)[idx];
    }
    
    // Grouped expressions
    if (auto grouped = std::dynamic_pointer_cast<ASTGroupedExpression>(expr)) {
        return evaluateExpression(grouped->expression);
    }
    
    std::cerr << "Error: Unknown expression type" << std::endl;
    return RuntimeValue();
}

// Execute statements - perform actions
void Interpreter::executeStatement(ASTNodePtr stmt) {
    // Assignment statements
    if (auto assignment = std::dynamic_pointer_cast<ASTAssignment>(stmt)) {
        RuntimeValue value = evaluateExpression(assignment->expression);
        setVariable(assignment->variable, value);
        return;
    }
    
    // Input statements - your "input x;" requirement
    if (auto input = std::dynamic_pointer_cast<ASTInput>(stmt)) {
        RuntimeValue value = handleInput();  // Always returns string from user
        setVariable(input->variable, value);
        return;
    }
    
    // Output statements
    if (auto output = std::dynamic_pointer_cast<ASTOutput>(stmt)) {
        RuntimeValue value = evaluateExpression(output->expression);
        handleOutput(value);
        return;
    }
    
    // Return statements
    if (auto returnStmt = std::dynamic_pointer_cast<ASTReturn>(stmt)) {
        if (returnStmt->expression) {
            returnValue = evaluateExpression(returnStmt->expression);
        } else {
            returnValue = RuntimeValue();  // undefined
        }
        hasReturnValue = true;
        return;
    }
    
    // If statements
    if (auto ifStmt = std::dynamic_pointer_cast<ASTIf>(stmt)) {
        RuntimeValue condition = evaluateExpression(ifStmt->condition);
        RuntimeValue condBool = toBoolean(condition);
        
        if (condBool.boolValue) {
            executeStatement(ifStmt->thenBlock);
        } else if (ifStmt->elseBlock) {
            executeStatement(ifStmt->elseBlock);
        }
        return;
    }
    
    // For loops
    if (auto forStmt = std::dynamic_pointer_cast<ASTFor>(stmt)) {
        // Execute initialization
        executeStatement(forStmt->init);
        
        // Loop while condition is true
        while (true) {
            RuntimeValue condition = evaluateExpression(forStmt->condition);
            RuntimeValue condBool = toBoolean(condition);
            if (!condBool.boolValue) break;
            
            // Execute body
            executeStatement(forStmt->body);
            if (hasReturnValue) break;  // Exit loop on return
            
            // Execute increment
            executeStatement(forStmt->increment);
        }
        return;
    }
    
    // Block statements
    if (auto block = std::dynamic_pointer_cast<ASTBlock>(stmt)) {
        for (const auto& s : block->statements) {
            executeStatement(s);
            if (hasReturnValue) break;  // Exit block on return
        }
        return;
    }
    
    std::cerr << "Error: Unknown statement type" << std::endl;
}

// Variable management
void Interpreter::setVariable(const std::string& name, const RuntimeValue& value) {
    if (!callStack.empty()) {
        // Set in current function's local scope
        callStack.back()[name] = value;
    } else {
        // Set in global scope
        variables[name] = value;
    }
}

RuntimeValue Interpreter::getVariable(const std::string& name) {
    // Check local scope first (if in function)
    if (!callStack.empty()) {
        auto& locals = callStack.back();
        if (locals.find(name) != locals.end()) {
            return locals[name];
        }
    }
    
    // Check global scope
    if (variables.find(name) != variables.end()) {
        return variables[name];
    }
    
    std::cerr << "Error: Undefined variable '" << name << "'" << std::endl;
    return RuntimeValue();  // undefined
}

// Function call handling
RuntimeValue Interpreter::callFunction(const std::string& name, const std::vector<RuntimeValue>& args) {
    // Check for built-in functions first
    RuntimeValue builtinResult = handleBuiltinFunction(name, args);
    if (builtinResult.type != RuntimeType::UNDEFINED) {
        return builtinResult;
    }
    
    // Look up user-defined function
    if (functions.find(name) == functions.end()) {
        std::cerr << "Error: Undefined function '" << name << "'" << std::endl;
        return RuntimeValue();
    }
    
    auto func = functions[name];
    
    // Check parameter count
    if (args.size() != func->parameters.size()) {
        std::cerr << "Error: Function '" << name << "' expects " << func->parameters.size() 
                  << " arguments, got " << args.size() << std::endl;
        return RuntimeValue();
    }
    
    // Create new scope for function
    std::unordered_map<std::string, RuntimeValue> localScope;
    
    // Bind parameters
    for (size_t i = 0; i < func->parameters.size(); ++i) {
        localScope[func->parameters[i]] = args[i];
    }
    
    // Push scope
    callStack.push_back(localScope);
    
    // Execute function body
    hasReturnValue = false;
    executeStatement(func->body);
    
    // Pop scope
    callStack.pop_back();
    
    // Return value
    if (hasReturnValue) {
        RuntimeValue result = returnValue;
        hasReturnValue = false;
        return result;
    }
    
    return RuntimeValue();  // undefined return
}

// Built-in functions (none for now, but extensible)
RuntimeValue Interpreter::handleBuiltinFunction(const std::string& name, const std::vector<RuntimeValue>& args) {
    // Add built-in functions here if needed
    return RuntimeValue();  // undefined = not a built-in
}

// Input handling - always returns string (your requirement)
RuntimeValue Interpreter::handleInput() {
    std::string input;
    std::getline(std::cin, input);
    return RuntimeValue(input);  // Always string!
}

// Output handling - prints the value
void Interpreter::handleOutput(const RuntimeValue& value) {
    std::cout << value.toString() << std::endl;
}
