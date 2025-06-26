#include "../include/runtime.h"
#include <sstream>

// Copy constructor - handles deep copying of dynamic memory
RuntimeValue::RuntimeValue(const RuntimeValue& other) : type(other.type) {
    switch (type) {
        case RuntimeType::STRING:
            stringValue = new std::string(*other.stringValue);
            break;
        case RuntimeType::INTEGER:
            intValue = other.intValue;
            break;
        case RuntimeType::FLOAT:
            floatValue = other.floatValue;
            break;
        case RuntimeType::BOOLEAN:
            boolValue = other.boolValue;
            break;
        case RuntimeType::ARRAY:
            arrayValue = new std::vector<RuntimeValue>(*other.arrayValue);
            break;
        case RuntimeType::UNDEFINED:
            break;
    }
}

// Assignment operator - handles memory cleanup and copying
RuntimeValue& RuntimeValue::operator=(const RuntimeValue& other) {
    if (this == &other) return *this;
    
    // Clean up existing memory
    if (type == RuntimeType::STRING && stringValue) {
        delete stringValue;
        stringValue = nullptr;
    } else if (type == RuntimeType::ARRAY && arrayValue) {
        delete arrayValue;
        arrayValue = nullptr;
    }
    
    // Copy new value
    type = other.type;
    switch (type) {
        case RuntimeType::STRING:
            stringValue = new std::string(*other.stringValue);
            break;
        case RuntimeType::INTEGER:
            intValue = other.intValue;
            break;
        case RuntimeType::FLOAT:
            floatValue = other.floatValue;
            break;
        case RuntimeType::BOOLEAN:
            boolValue = other.boolValue;
            break;
        case RuntimeType::ARRAY:
            arrayValue = new std::vector<RuntimeValue>(*other.arrayValue);
            break;
        case RuntimeType::UNDEFINED:
            break;
    }
    return *this;
}

// Destructor - clean up dynamic memory
RuntimeValue::~RuntimeValue() {
    if (type == RuntimeType::STRING && stringValue) {
        delete stringValue;
    } else if (type == RuntimeType::ARRAY && arrayValue) {
        delete arrayValue;
    }
}

// Convert any value to string representation
std::string RuntimeValue::toString() const {
    switch (type) {
        case RuntimeType::STRING:
            return *stringValue;
        case RuntimeType::INTEGER:
            return std::to_string(intValue);
        case RuntimeType::FLOAT:
            return std::to_string(floatValue);
        case RuntimeType::BOOLEAN:
            return boolValue ? "true" : "false";
        case RuntimeType::ARRAY: {
            std::string result = "[";
            for (size_t i = 0; i < arrayValue->size(); ++i) {
                if (i > 0) result += ", ";
                result += (*arrayValue)[i].toString();
            }
            result += "]";
            return result;
        }
        case RuntimeType::UNDEFINED:
            return "undefined";
    }
    return "unknown";
}

// Print the value to console
void RuntimeValue::print() const {
    std::cout << toString();
}

// ===== TYPE COERCION IMPLEMENTATIONS =====

// Convert string to number - handles your "x+5" requirement
RuntimeValue stringToNumber(const RuntimeValue& value) {
    if (value.type != RuntimeType::STRING) return value;
    
    std::string str = *value.stringValue;
    
    // Try to convert to integer first
    try {
        // Check if it's a valid integer (no decimal point)
        if (str.find('.') == std::string::npos) {
            int intVal = std::stoi(str);
            return RuntimeValue(intVal);
        } else {
            // Has decimal point, convert to float
            double floatVal = std::stod(str);
            return RuntimeValue(floatVal);
        }
    } catch (...) {
        // If conversion fails, return 0
        return RuntimeValue(0);
    }
}

// Convert any value to boolean
RuntimeValue toBoolean(const RuntimeValue& value) {
    switch (value.type) {
        case RuntimeType::BOOLEAN:
            return value;
        case RuntimeType::INTEGER:
            return RuntimeValue(value.intValue != 0);
        case RuntimeType::FLOAT:
            return RuntimeValue(value.floatValue != 0.0);
        case RuntimeType::STRING:
            return RuntimeValue(!value.stringValue->empty());
        case RuntimeType::ARRAY:
            return RuntimeValue(!value.arrayValue->empty());
        case RuntimeType::UNDEFINED:
            return RuntimeValue(false);
    }
    return RuntimeValue(false);
}

// Check if a value can be treated as a number
bool isNumeric(const RuntimeValue& value) {
    if (value.type == RuntimeType::INTEGER || value.type == RuntimeType::FLOAT) {
        return true;
    }
    if (value.type == RuntimeType::STRING) {
        try {
            std::stod(*value.stringValue);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

// Get numeric value from any type
double getNumericValue(const RuntimeValue& value) {
    switch (value.type) {
        case RuntimeType::INTEGER:
            return static_cast<double>(value.intValue);
        case RuntimeType::FLOAT:
            return value.floatValue;
        case RuntimeType::STRING:
            try {
                return std::stod(*value.stringValue);
            } catch (...) {
                return 0.0;
            }
        case RuntimeType::BOOLEAN:
            return value.boolValue ? 1.0 : 0.0;
        default:
            return 0.0;
    }
}

// THE CORE: Binary operations with your exact requirements
RuntimeValue performBinaryOperation(const RuntimeValue& left, const RuntimeValue& right, const std::string& op) {
    // Handle addition specially - your key requirement
    if (op == "+") {
        // If BOTH operands are strings, do concatenation
        if (left.type == RuntimeType::STRING && right.type == RuntimeType::STRING) {
            std::string result = *left.stringValue + *right.stringValue;
            return RuntimeValue(result);
        }
        
        // If one is string and one is string literal (like "5"), still concatenate
        // But if one is string and one is number, convert string to number
        if (left.type == RuntimeType::STRING && (right.type == RuntimeType::INTEGER || right.type == RuntimeType::FLOAT)) {
            // x + 5: convert x to number, do math
            RuntimeValue leftNum = stringToNumber(left);
            double leftVal = getNumericValue(leftNum);
            double rightVal = getNumericValue(right);
            
            // If right is float, result is float
            if (right.type == RuntimeType::FLOAT) {
                return RuntimeValue(leftVal + rightVal);
            }
            // If converted left is integer and right is integer, result is integer
            if (leftNum.type == RuntimeType::INTEGER && right.type == RuntimeType::INTEGER) {
                return RuntimeValue(static_cast<int>(leftVal + rightVal));
            }
            return RuntimeValue(leftVal + rightVal);
        }
        
        if (right.type == RuntimeType::STRING && (left.type == RuntimeType::INTEGER || left.type == RuntimeType::FLOAT)) {
            // 5 + x: convert x to number, do math
            RuntimeValue rightNum = stringToNumber(right);
            double leftVal = getNumericValue(left);
            double rightVal = getNumericValue(rightNum);
            
            // If left is float, result is float
            if (left.type == RuntimeType::FLOAT) {
                return RuntimeValue(leftVal + rightVal);
            }
            // If converted right is integer and left is integer, result is integer
            if (rightNum.type == RuntimeType::INTEGER && left.type == RuntimeType::INTEGER) {
                return RuntimeValue(static_cast<int>(leftVal + rightVal));
            }
            return RuntimeValue(leftVal + rightVal);
        }
        
        // Both are numeric - do math addition
        if (isNumeric(left) && isNumeric(right)) {
            double leftVal = getNumericValue(left);
            double rightVal = getNumericValue(right);
            
            // If both were integers, return integer
            if (left.type == RuntimeType::INTEGER && right.type == RuntimeType::INTEGER) {
                return RuntimeValue(static_cast<int>(leftVal + rightVal));
            }
            // Otherwise return float
            return RuntimeValue(leftVal + rightVal);
        }
    }
    
    // For all other operations, convert to numbers
    if (op == "-" || op == "*" || op == "/" || op == "%") {
        double leftVal = getNumericValue(left);
        double rightVal = getNumericValue(right);
        
        if (op == "-") {
            if (left.type == RuntimeType::INTEGER && right.type == RuntimeType::INTEGER) {
                return RuntimeValue(static_cast<int>(leftVal - rightVal));
            }
            return RuntimeValue(leftVal - rightVal);
        } else if (op == "*") {
            if (left.type == RuntimeType::INTEGER && right.type == RuntimeType::INTEGER) {
                return RuntimeValue(static_cast<int>(leftVal * rightVal));
            }
            return RuntimeValue(leftVal * rightVal);
        } else if (op == "/") {
            if (rightVal == 0) {
                std::cerr << "Error: Division by zero!" << std::endl;
                return RuntimeValue(0.0);
            }
            return RuntimeValue(leftVal / rightVal);  // Division always returns float
        } else if (op == "%") {
            if (rightVal == 0) {
                std::cerr << "Error: Modulo by zero!" << std::endl;
                return RuntimeValue(0);
            }
            return RuntimeValue(static_cast<int>(leftVal) % static_cast<int>(rightVal));
        }
    }
    
    // Comparison operations
    if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        // String comparison if both are strings
        if (left.type == RuntimeType::STRING && right.type == RuntimeType::STRING) {
            if (op == "==") return RuntimeValue(*left.stringValue == *right.stringValue);
            if (op == "!=") return RuntimeValue(*left.stringValue != *right.stringValue);
            if (op == "<") return RuntimeValue(*left.stringValue < *right.stringValue);
            if (op == "<=") return RuntimeValue(*left.stringValue <= *right.stringValue);
            if (op == ">") return RuntimeValue(*left.stringValue > *right.stringValue);
            if (op == ">=") return RuntimeValue(*left.stringValue >= *right.stringValue);
        }
        
        // Numeric comparison
        double leftVal = getNumericValue(left);
        double rightVal = getNumericValue(right);
        
        if (op == "==") return RuntimeValue(leftVal == rightVal);
        if (op == "!=") return RuntimeValue(leftVal != rightVal);
        if (op == "<") return RuntimeValue(leftVal < rightVal);
        if (op == "<=") return RuntimeValue(leftVal <= rightVal);
        if (op == ">") return RuntimeValue(leftVal > rightVal);
        if (op == ">=") return RuntimeValue(leftVal >= rightVal);
    }
    
    // Logical operations
    if (op == "&&") {
        RuntimeValue leftBool = toBoolean(left);
        if (!leftBool.boolValue) return RuntimeValue(false);
        return toBoolean(right);
    }
    if (op == "||") {
        RuntimeValue leftBool = toBoolean(left);
        if (leftBool.boolValue) return RuntimeValue(true);
        return toBoolean(right);
    }
    
    // Unknown operation
    std::cerr << "Error: Unknown binary operation: " << op << std::endl;
    return RuntimeValue();
}

// Handle unary operations
RuntimeValue performUnaryOperation(const RuntimeValue& operand, const std::string& op) {
    if (op == "-") {
        double val = getNumericValue(operand);
        if (operand.type == RuntimeType::INTEGER) {
            return RuntimeValue(static_cast<int>(-val));
        }
        return RuntimeValue(-val);
    }
    
    if (op == "!") {
        RuntimeValue boolVal = toBoolean(operand);
        return RuntimeValue(!boolVal.boolValue);
    }
    
    std::cerr << "Error: Unknown unary operation: " << op << std::endl;
    return RuntimeValue();
}
