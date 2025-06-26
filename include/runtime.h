#ifndef RUNTIME_H
#define RUNTIME_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

// Define what types our language can have at runtime
enum class RuntimeType {
    STRING,
    INTEGER, 
    FLOAT,
    BOOLEAN,
    ARRAY,
    UNDEFINED  // For uninitialized variables
};

// This is the core - every value in our language will be wrapped in this
struct RuntimeValue {
    RuntimeType type;
    
    // Use union to save memory - only one of these will be used at a time
    union {
        std::string* stringValue;
        int intValue;
        double floatValue;
        bool boolValue;
        std::vector<RuntimeValue>* arrayValue;
    };
    
    // Constructors for each type
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
    
    // Copy constructor
    RuntimeValue(const RuntimeValue& other);
    
    // Assignment operator
    RuntimeValue& operator=(const RuntimeValue& other);
    
    // Destructor to clean up dynamic memory
    ~RuntimeValue();
    
    // Helper methods
    std::string toString() const;
    void print() const;
};

// ===== TYPE COERCION SYSTEM =====
// These functions implement your dynamic typing rules

// Convert string to number if possible, otherwise return 0
RuntimeValue stringToNumber(const RuntimeValue& value);

// Convert any value to boolean (for if statements, etc.)
RuntimeValue toBoolean(const RuntimeValue& value);

// The heart of your dynamic typing: binary operations with auto-coercion
RuntimeValue performBinaryOperation(const RuntimeValue& left, const RuntimeValue& right, const std::string& op);

// Handle unary operations (-, !)
RuntimeValue performUnaryOperation(const RuntimeValue& operand, const std::string& op);

// Type coercion helpers
bool isNumeric(const RuntimeValue& value);
double getNumericValue(const RuntimeValue& value);

#endif // RUNTIME_H
