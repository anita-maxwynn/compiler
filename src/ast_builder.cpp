#include <iostream>
#include "../include/ast_generator.h"
#include "../include/ast.h"
#include "../include/tokenizer.h"

static size_t current = 0;
static const std::vector<GenericToken>* tokens = nullptr;

static const GenericToken& peek() {
    if (current < tokens->size()) return (*tokens)[current];
    static GenericToken endToken = {TOKEN_ERROR, 0, {}, {}, {}, {}, {}, {"Unexpected end of input", 0}};
    return endToken;
}

static const GenericToken& next() {
    if (current < tokens->size()) return (*tokens)[current++];
    static GenericToken endToken = {TOKEN_ERROR, 0, {}, {}, {}, {}, {}, {"Unexpected end of input", 0}};
    return endToken;
}

static bool matchSeparator(char symbol) {
    if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == symbol) {
        next();
        return true;
    }
    return false;
}

static bool matchOperator(const std::string& symbol) {
    if (peek().type == TOKEN_OPERATOR && peek().op.symbol == symbol) {
        next();
        return true;
    }
    return false;
}

static ASTNodePtr parseExpression();
static ASTNodePtr parseStatement();
static ASTNodePtr parseBlock();
static ASTNodePtr parseLogicalOr();
static ASTNodePtr parseLogicalAnd();
static ASTNodePtr parseEquality();
static ASTNodePtr parseComparison();
static ASTNodePtr parseAddition();
static ASTNodePtr parseMultiplication();
static ASTNodePtr parseUnary();
static ASTNodePtr parsePrimary();
static ASTNodePtr parseSimpleAssignment();

// Helper function to convert TokenLiteral to LiteralValue
static LiteralValue convertTokenLiteral(const TokenLiteral& tokenLit) {
    switch (tokenLit.type) {
        case INT:
            return tokenLit.value.intValue;
        case DOUBLE:
            return tokenLit.value.doubleValue;
        case CHAR:
            return tokenLit.value.charValue;
        case BOOL:
            return tokenLit.value.boolValue;
        case STRING:
            return tokenLit.stringValue;
        default:
            return 0; // Default to int 0
    }
}

std::shared_ptr<ASTProgram> generateAST(const std::vector<GenericToken>& inputTokens, const std::string&) {
    tokens = &inputTokens;
    current = 0;
    std::vector<ASTNodePtr> functions;
    while (current < tokens->size()) {
        if (peek().type == TOKEN_KEYWORD && peek().keyword.type == KEYWORD_DEF) {
            next();
            if (peek().type != TOKEN_IDENTIFIER) return nullptr;
            std::string name = peek().identifier.name;
            next();
            if (!matchSeparator('(')) return nullptr;
            std::vector<std::string> params;
            while (peek().type == TOKEN_IDENTIFIER) {
                params.push_back(peek().identifier.name);
                next();
                if (!matchSeparator(',')) break;
            }
            if (!matchSeparator(')')) return nullptr;
            ASTNodePtr body = parseBlock();
            functions.push_back(std::make_shared<ASTFunction>(name, params, body));
        } else {
            std::cerr << "Expected function definition\n";
            return nullptr;
        }
    }
    return std::make_shared<ASTProgram>(functions);
}

static ASTNodePtr parseBlock() {
    if (!matchSeparator('{')) return nullptr;
    std::vector<ASTNodePtr> statements;
    while (!(peek().type == TOKEN_SEPARATOR && peek().separator.symbol == '}')) {
        ASTNodePtr stmt = parseStatement();
        if (!stmt) return nullptr;
        statements.push_back(stmt);
    }
    next();
    return std::make_shared<ASTBlock>(statements);
}

static ASTNodePtr parseStatement() {
    const GenericToken& token = peek();
    if (token.type == TOKEN_KEYWORD) {
        if (token.keyword.type == KEYWORD_INPUT) {
            next();
            if (peek().type != TOKEN_IDENTIFIER) return nullptr;
            std::string name = peek().identifier.name;
            next();
            if (!matchSeparator(';')) return nullptr;
            return std::make_shared<ASTInput>(name);
        } else if (token.keyword.type == KEYWORD_OUTPUT) {
            next();
            ASTNodePtr expr = parseExpression();
            if (!expr) return nullptr;
            if (!matchSeparator(';')) return nullptr;
            return std::make_shared<ASTOutput>(expr);
        } else if (token.keyword.type == KEYWORD_RETURN) {
            next();
            if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == ';') {
                next();
                return std::make_shared<ASTReturn>(nullptr);
            }
            ASTNodePtr expr = parseExpression();
            if (!matchSeparator(';')) return nullptr;
            return std::make_shared<ASTReturn>(expr);
        } else if (token.keyword.type == KEYWORD_IF) {
            next();
            if (!matchSeparator('(')) return nullptr;
            ASTNodePtr condition = parseExpression();
            if (!condition || !matchSeparator(')')) return nullptr;
            ASTNodePtr thenBlock = parseBlock();
            if (!thenBlock) return nullptr;
            
            ASTNodePtr elseBlock = nullptr;
            if (peek().type == TOKEN_KEYWORD && peek().keyword.type == KEYWORD_ELSE) {
                next();
                elseBlock = parseBlock();
                if (!elseBlock) return nullptr;
            }
            return std::make_shared<ASTIf>(condition, thenBlock, elseBlock);
        } else if (token.keyword.type == KEYWORD_FOR) {
            next();
            if (!matchSeparator('(')) return nullptr;
            ASTNodePtr init = parseSimpleAssignment();
            if (!init || !matchSeparator(';')) return nullptr;
            ASTNodePtr condition = parseExpression();
            if (!condition || !matchSeparator(';')) return nullptr;
            ASTNodePtr increment = parseSimpleAssignment();
            if (!increment || !matchSeparator(')')) return nullptr;
            ASTNodePtr body = parseBlock();
            if (!body) return nullptr;
            return std::make_shared<ASTFor>(init, condition, increment, body);
        }
    } else if (token.type == TOKEN_IDENTIFIER) {
        std::string name = token.identifier.name;
        next();
        if (matchOperator("=")) {
            ASTNodePtr expr = parseExpression();
            if (!matchSeparator(';')) return nullptr;
            return std::make_shared<ASTAssignment>(name, expr);
        }
    }
    return nullptr;
}

// Helper function for simple assignments (used in for loops)
static ASTNodePtr parseSimpleAssignment() {
    if (peek().type != TOKEN_IDENTIFIER) return nullptr;
    std::string name = peek().identifier.name;
    next();
    if (!matchOperator("=")) return nullptr;
    ASTNodePtr expr = parseExpression();
    if (!expr) return nullptr;
    return std::make_shared<ASTAssignment>(name, expr);
}

static ASTNodePtr parseExpression() {
    return parseLogicalOr();
}

static ASTNodePtr parseLogicalOr() {
    ASTNodePtr left = parseLogicalAnd();
    if (!left) return nullptr;
    
    while (peek().type == TOKEN_OPERATOR && peek().op.symbol == "||") {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr right = parseLogicalAnd();
        if (!right) return nullptr;
        left = std::make_shared<ASTBinaryExpression>(left, right, op);
    }
    return left;
}

static ASTNodePtr parseLogicalAnd() {
    ASTNodePtr left = parseEquality();
    if (!left) return nullptr;
    
    while (peek().type == TOKEN_OPERATOR && peek().op.symbol == "&&") {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr right = parseEquality();
        if (!right) return nullptr;
        left = std::make_shared<ASTBinaryExpression>(left, right, op);
    }
    return left;
}

static ASTNodePtr parseEquality() {
    ASTNodePtr left = parseComparison();
    if (!left) return nullptr;
    
    while (peek().type == TOKEN_OPERATOR && 
           (peek().op.symbol == "==" || peek().op.symbol == "!=")) {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr right = parseComparison();
        if (!right) return nullptr;
        left = std::make_shared<ASTBinaryExpression>(left, right, op);
    }
    return left;
}

static ASTNodePtr parseComparison() {
    ASTNodePtr left = parseAddition();
    if (!left) return nullptr;
    
    while (peek().type == TOKEN_OPERATOR && 
           (peek().op.symbol == "<" || peek().op.symbol == "<=" || 
            peek().op.symbol == ">" || peek().op.symbol == ">=")) {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr right = parseAddition();
        if (!right) return nullptr;
        left = std::make_shared<ASTBinaryExpression>(left, right, op);
    }
    return left;
}

static ASTNodePtr parseAddition() {
    ASTNodePtr left = parseMultiplication();
    if (!left) return nullptr;
    
    while (peek().type == TOKEN_OPERATOR && 
           (peek().op.symbol == "+" || peek().op.symbol == "-")) {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr right = parseMultiplication();
        if (!right) return nullptr;
        left = std::make_shared<ASTBinaryExpression>(left, right, op);
    }
    return left;
}

static ASTNodePtr parseMultiplication() {
    ASTNodePtr left = parseUnary();
    if (!left) return nullptr;
    
    while (peek().type == TOKEN_OPERATOR && 
           (peek().op.symbol == "*" || peek().op.symbol == "/" || peek().op.symbol == "%")) {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr right = parseUnary();
        if (!right) return nullptr;
        left = std::make_shared<ASTBinaryExpression>(left, right, op);
    }
    return left;
}

static ASTNodePtr parseUnary() {
    if (peek().type == TOKEN_OPERATOR && 
        (peek().op.symbol == "-" || peek().op.symbol == "!")) {
        std::string op = peek().op.symbol;
        next();
        ASTNodePtr operand = parseUnary();
        if (!operand) return nullptr;
        return std::make_shared<ASTUnaryExpression>(op, operand);
    }
    return parsePrimary();
}

static ASTNodePtr parsePrimary() {
    const GenericToken& token = peek();
    
    if (token.type == TOKEN_LITERAL) {
        next();
        return std::make_shared<ASTLiteral>(convertTokenLiteral(token.literal));
    } 
    
    if (token.type == TOKEN_IDENTIFIER) {
        std::string name = token.identifier.name;
        next();
        
        // Check for function call
        if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == '(') {
            next(); // consume '('
            std::vector<ASTNodePtr> args;
            
            if (!(peek().type == TOKEN_SEPARATOR && peek().separator.symbol == ')')) {
                do {
                    ASTNodePtr arg = parseExpression();
                    if (!arg) return nullptr;
                    args.push_back(arg);
                } while (matchSeparator(','));
            }
            
            if (!matchSeparator(')')) return nullptr;
            return std::make_shared<ASTFunctionCall>(std::make_shared<ASTIdentifier>(name), args);
        }
        
        // Check for array access
        if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == '[') {
            next(); // consume '['
            ASTNodePtr index = parseExpression();
            if (!index || !matchSeparator(']')) return nullptr;
            return std::make_shared<ASTArrayAccess>(std::make_shared<ASTIdentifier>(name), index);
        }
        
        return std::make_shared<ASTIdentifier>(name);
    }
    
    // Handle grouped expressions
    if (token.type == TOKEN_SEPARATOR && token.separator.symbol == '(') {
        next(); // consume '('
        ASTNodePtr expr = parseExpression();
        if (!expr || !matchSeparator(')')) return nullptr;
        return std::make_shared<ASTGroupedExpression>(expr);
    }
    
    // Handle array literals
    if (token.type == TOKEN_SEPARATOR && token.separator.symbol == '[') {
        next(); // consume '['
        std::vector<ASTNodePtr> elements;
        
        if (!(peek().type == TOKEN_SEPARATOR && peek().separator.symbol == ']')) {
            do {
                ASTNodePtr element = parseExpression();
                if (!element) return nullptr;
                elements.push_back(element);
            } while (matchSeparator(','));
        }
        
        if (!matchSeparator(']')) return nullptr;
        return std::make_shared<ASTArrayLiteral>(elements);
    }
    
    return nullptr;
}
