#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "runtime.h"
#include <unordered_map>
#include <string>
#include <memory>

class Interpreter {
private:
    std::unordered_map<std::string, RuntimeValue> variables;
    
    std::unordered_map<std::string, std::shared_ptr<ASTFunction>> functions;
    
    std::vector<std::unordered_map<std::string, RuntimeValue>> callStack;
    
    bool hasReturnValue;
    RuntimeValue returnValue;

public:
    Interpreter();
    
    void execute(std::shared_ptr<ASTProgram> program);
    
    RuntimeValue evaluateExpression(ASTNodePtr expr);
    
    void executeStatement(ASTNodePtr stmt);
    
    void setVariable(const std::string& name, const RuntimeValue& value);
    RuntimeValue getVariable(const std::string& name);
    
    RuntimeValue callFunction(const std::string& name, const std::vector<RuntimeValue>& args);
    
    RuntimeValue handleBuiltinFunction(const std::string& name, const std::vector<RuntimeValue>& args);
    
    RuntimeValue handleInput();
    void handleOutput(const RuntimeValue& value);
};

#endif // INTERPRETER_H
