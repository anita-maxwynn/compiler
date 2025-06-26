#ifndef RUNTIME_H
#define RUNTIME_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

enum class RuntimeType {
    STRING,
    INTEGER, 
    FLOAT,
    BOOLEAN,
    ARRAY,
    UNDEFINED
};

struct RuntimeValue {
    RuntimeType type;
    
    union {
        std::string* stringValue;
        int intValue;
        double floatValue;
        bool boolValue;
        std::vector<RuntimeValue>* arrayValue;
    };
    
    RuntimeValue() : type(RuntimeType::UNDEFINED) {}
    
    explicit RuntimeValue(const std::string& str) : type(RuntimeType::STRING) {
        stringValue = new std::string(str);
    }
    
    explicit RuntimeValue(const char* str) : type(RuntimeType::STRING) {
        stringValue = new std::string(str);
    }
    
    explicit RuntimeValue(int val) : type(RuntimeType::INTEGER), intValue(val) {}
    
    explicit RuntimeValue(double val) : type(RuntimeType::FLOAT), floatValue(val) {}
    
    explicit RuntimeValue(bool val) : type(RuntimeType::BOOLEAN), boolValue(val) {}
    
    explicit RuntimeValue(const std::vector<RuntimeValue>& arr) : type(RuntimeType::ARRAY) {
        arrayValue = new std::vector<RuntimeValue>(arr);
    }
    
    RuntimeValue(const RuntimeValue& other);
    
    RuntimeValue& operator=(const RuntimeValue& other);
    
    ~RuntimeValue();
    
    std::string toString() const;
    void print() const;
};

RuntimeValue stringToNumber(const RuntimeValue& value);

RuntimeValue toBoolean(const RuntimeValue& value);

RuntimeValue performBinaryOperation(const RuntimeValue& left, const RuntimeValue& right, const std::string& op);

RuntimeValue performUnaryOperation(const RuntimeValue& operand, const std::string& op);

bool isNumeric(const RuntimeValue& value);
double getNumericValue(const RuntimeValue& value);

#endif // RUNTIME_H
