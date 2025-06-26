#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "runtime.h"
#include <unordered_map>
#include <string>
#include <memory>

// ===== INTERPRETER SYSTEM =====
// This executes your AST with dynamic typing

class Interpreter {
private:
    // Variable storage - maps variable names to their runtime values
    std::unordered_map<std::string, RuntimeValue> variables;
    
    // Function storage - maps function names to their AST nodes
    std::unordered_map<std::string, std::shared_ptr<ASTFunction>> functions;
    
    // Stack for function calls (for parameters and local variables)
    std::vector<std::unordered_map<std::string, RuntimeValue>> callStack;
    
    // Return value handling
    bool hasReturnValue;
    RuntimeValue returnValue;

public:
    Interpreter();
    
    // Main execution entry point
    void execute(std::shared_ptr<ASTProgram> program);
    
    // Expression evaluation - returns RuntimeValue
    RuntimeValue evaluateExpression(ASTNodePtr expr);
    
    // Statement execution - performs actions
    void executeStatement(ASTNodePtr stmt);
    
    // Helper functions
    void setVariable(const std::string& name, const RuntimeValue& value);
    RuntimeValue getVariable(const std::string& name);
    
    // Function call handling
    RuntimeValue callFunction(const std::string& name, const std::vector<RuntimeValue>& args);
    
    // Built-in functions
    RuntimeValue handleBuiltinFunction(const std::string& name, const std::vector<RuntimeValue>& args);
    
    // Input/Output handling
    RuntimeValue handleInput();
    void handleOutput(const RuntimeValue& value);
};

#endif // INTERPRETER_H
