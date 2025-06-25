#include <iostream>
#include "../include/parser.h"
#include "../include/tokenizer.h"
#include <vector>

using namespace std;

static size_t current = 0;
static const std::vector<GenericToken>* tokens = nullptr;
static const std::string* sourceCode = nullptr;

// Print detailed error context
void printErrorWithContext(const std::string& message, size_t position) {
    std::cerr << message << " at position " << position << "\n";

    if (!sourceCode || position >= sourceCode->size()) return;

    size_t lineStart = position, lineEnd = position;
    while (lineStart > 0 && (*sourceCode)[lineStart - 1] != '\n') lineStart--;
    while (lineEnd < sourceCode->size() && (*sourceCode)[lineEnd] != '\n') lineEnd++;

    std::string line = sourceCode->substr(lineStart, lineEnd - lineStart);
    std::cerr << "  " << line << "\n  ";

    for (size_t i = lineStart; i < position; ++i) {
        std::cerr << ((*sourceCode)[i] == '\t' ? '\t' : ' ');
    }
    std::cerr << "^\n";
}
static bool parseProgram();  // ← Add this line before using it

bool parseProgramInternal(const std::vector<GenericToken>& inputTokens, const std::string& source) {
    tokens = &inputTokens;
    sourceCode = &source;
    current = 0;
    return parseProgram();  // defined below
}


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

static bool matchKeyword(GenericTokenType type) {
    if (peek().type == type) { next(); return true; }
    return false;
}

static bool matchSeparator(char symbol) {
    if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == symbol) {
        next(); return true;
    }
    return false;
}

static bool matchIdentifier() {
    if (peek().type == TOKEN_IDENTIFIER) { next(); return true; }
    return false;
}

static bool matchLiteral() {
    if (peek().type == TOKEN_LITERAL) { next(); return true; }
    return false;
}

static bool matchOperator(const std::string& symbol) {
    if (peek().type == TOKEN_OPERATOR && peek().op.symbol == symbol) {
        next(); return true;
    }
    return false;
}

// Parser forward declarations
static bool parseProgram();
static bool parseBlock();
static bool parseStatementlist();
static bool parseStatement();
static bool parseAssignment();
static bool parseInput();
static bool parseOutput();
static bool parseIf();
static bool parseFor();
static bool parseExpression();
static bool parseTerm();
static bool parseFactor();
static bool parseReturn();

static bool parseProgram() {
    while (peek().type == TOKEN_KEYWORD && peek().keyword.type == KEYWORD_DEF) {
        next();
        if (!matchIdentifier()) {
            printErrorWithContext("Expected identifier after 'def'", peek().position);
            return false;
        }
        if (!matchSeparator('(')) {
            printErrorWithContext("Expected '(' after function name", peek().position);
            return false;
        }
        if (peek().type == TOKEN_IDENTIFIER) {
            do {
                if (!matchIdentifier()) {
                    printErrorWithContext("Expected parameter name", peek().position);
                    return false;
                }
            } while (matchSeparator(','));
        }
        if (!matchSeparator(')')) {
            printErrorWithContext("Expected ')' after parameter list", peek().position);
            return false;
        }
        if (!parseBlock()) {
            printErrorWithContext("Expected block after function definition", peek().position);
            return false;
        }
    }
    return true;
}


static bool parseBlock() {
    if (!matchSeparator('{')) {
        printErrorWithContext("Expected '{'", peek().position);
        return false;
    }
    if (!parseStatementlist()) {
        printErrorWithContext("Error parsing statements inside block", peek().position);
        return false;
    }
    if (!matchSeparator('}')) {
        printErrorWithContext("Expected '}'", peek().position);
        return false;
    }
    return true;
}

static bool parseStatementlist() {
    while (peek().type != TOKEN_SEPARATOR || peek().separator.symbol != '}') {
        if (!parseStatement()) {
            printErrorWithContext("Failed to parse statement", peek().position);
            return false;
        }
    }
    return true;
}

static bool parseStatement() {
    const GenericToken& token = peek();
    switch (token.type) {
        case TOKEN_KEYWORD:
            switch (token.keyword.type) {
                case KEYWORD_INPUT: return parseInput();
                case KEYWORD_OUTPUT: return parseOutput();
                case KEYWORD_IF: return parseIf();
                case KEYWORD_FOR: return parseFor();
                case KEYWORD_RETURN: return parseReturn();
                default:
                    printErrorWithContext("Unexpected keyword", token.position);
                    return false;
            }
        case TOKEN_IDENTIFIER:
            return parseAssignment();
        default:
            printErrorWithContext("Unexpected token", token.position);
            return false;
    }
}

static bool parseAssignment() {
    if (!matchIdentifier()) {
        printErrorWithContext("Expected identifier", peek().position);
        return false;
    }
    if (!matchOperator("=")) {
        printErrorWithContext("Expected '='", peek().position);
        return false;
    }
    if (!parseExpression()) {
        printErrorWithContext("Expected expression after '='", peek().position);
        return false;
    }
    if (!matchSeparator(';')) {
        printErrorWithContext("Expected ';' after assignment", peek().position);
        return false;
    }
    return true;
}

static bool parseInput() {
    next();
    if (!matchIdentifier()) {
        printErrorWithContext("Expected identifier after 'input'", peek().position);
        return false;
    }
    if (!matchSeparator(';')) {
        printErrorWithContext("Expected ';' after input", peek().position);
        return false;
    }
    return true;
}

static bool parseOutput() {
    next();
    if (!parseExpression()) {
        printErrorWithContext("Expected expression after 'output'", peek().position);
        return false;
    }
    if (!matchSeparator(';')) {
        printErrorWithContext("Expected ';' after output", peek().position);
        return false;
    }
    return true;
}

static bool parseReturn() {
    next();
    if (matchSeparator(';')) return true;
    if (!parseExpression()) {
        printErrorWithContext("Invalid expression after return", peek().position);
        return false;
    }
    if (!matchSeparator(';')) {
        printErrorWithContext("Expected ';' after return", peek().position);
        return false;
    }
    return true;
}

static bool parseIf() {
    next();
    if (!matchSeparator('(') || !parseExpression() || !matchSeparator(')')) {
        printErrorWithContext("Malformed 'if' condition", peek().position);
        return false;
    }
    if (!parseBlock()) return false;

    while (peek().type == TOKEN_KEYWORD && peek().keyword.type == KEYWORD_ELIF) {
        next();
        if (!matchSeparator('(') || !parseExpression() || !matchSeparator(')')) {
            printErrorWithContext("Malformed 'elif' condition", peek().position);
            return false;
        }
        if (!parseBlock()) return false;
    }

    if (peek().type == TOKEN_KEYWORD && peek().keyword.type == KEYWORD_ELSE) {
        next();
        if (!parseBlock()) return false;
    }
    return true;
}
static bool parseSimpleAssignment() {
    if (!matchIdentifier()) return false;
    if (!matchOperator("=")) return false;
    if (!parseExpression()) return false;
    return true;
}
static bool parseFor() {
    next(); // consume 'for'
    if (!matchSeparator('(')) {
        printErrorWithContext("Expected '(' in 'for' loop", peek().position);
        return false;
    }
    if (!parseSimpleAssignment()) {
        printErrorWithContext("Invalid init in 'for'", peek().position);
        return false;
    }
    if (!matchSeparator(';')) {
        printErrorWithContext("Expected ';' after init in 'for'", peek().position);
        return false;
    }
    if (!parseExpression()) {
        printErrorWithContext("Invalid condition in 'for'", peek().position);
        return false;
    }
    if (!matchSeparator(';')) {
        printErrorWithContext("Expected ';' after condition in 'for'", peek().position);
        return false;
    }
    if (!parseSimpleAssignment()) {  // ← fix here
        printErrorWithContext("Invalid increment in 'for'", peek().position);
        return false;
    }
    if (!matchSeparator(')')) {
        printErrorWithContext("Expected ')' after increment in 'for'", peek().position);
        return false;
    }
    return parseBlock();
}


static bool parseExpression() {
    // Parse the left-hand side: a term
    if (!parseTerm()) {
        printErrorWithContext("Expected term in expression", peek().position);
        return false;
    }

    // Handle chained '+' and '-' operators: x + y - z
    while (peek().type == TOKEN_OPERATOR &&
          (peek().op.symbol == "+" || peek().op.symbol == "-")) {
        next();  // consume '+' or '-'
        if (!parseTerm()) {
            printErrorWithContext("Expected term after '+' or '-'", peek().position);
            return false;
        }
    }

    // Optionally handle one comparison operator
    if (peek().type == TOKEN_OPERATOR &&
        (peek().op.symbol == "==" || peek().op.symbol == "!=" ||
         peek().op.symbol == "<" || peek().op.symbol == "<=" ||
         peek().op.symbol == ">" || peek().op.symbol == ">=")) {
        next(); // consume comparison
        if (!parseExpression()) {
            printErrorWithContext("Expected expression after comparison operator", peek().position);
            return false;
        }
    }
    // Optionally handle logical operators
    while (peek().type == TOKEN_OPERATOR &&
           (peek().op.symbol == "&&" || peek().op.symbol == "||")) {
        next(); // consume '&&' or '||'
        if (!parseExpression()) {
            printErrorWithContext("Expected expression after logical operator", peek().position);
            return false;
        }
    }

    return true;
}


static bool parseTerm() {
    if (!parseFactor()) return false;
    while (peek().type == TOKEN_OPERATOR && (peek().op.symbol == "*" || peek().op.symbol == "/" || peek().op.symbol == "%")) {
        next();
        if (!parseFactor()) return false;
    }
    return true;
}

static bool parseFactor() {
    const GenericToken& token = peek();
    if (token.type == TOKEN_LITERAL) {
        next();
        return true;
    }
    if (token.type == TOKEN_IDENTIFIER) {
        next();
        if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == '(') {
            // Function call
            next();  // consume '('
            if (peek().type != TOKEN_SEPARATOR || peek().separator.symbol != ')') {
                if (!parseExpression()) return false;
                while (matchSeparator(',')) {
                    if (!parseExpression()) return false;
                }
            }
            if (!matchSeparator(')')) {
                printErrorWithContext("Expected ')' after function call arguments", peek().position);
                return false;
            }
        } else if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == '[') {
            next();
            if (!parseExpression() || !matchSeparator(']')) {
                printErrorWithContext("Invalid array indexing", peek().position);
                return false;
            }
        }
        return true;
    }

    if (token.type == TOKEN_SEPARATOR && token.separator.symbol == '(') {
        next();
        if (!parseExpression() || !matchSeparator(')')) {
            printErrorWithContext("Invalid parentheses expression", peek().position);
            return false;
        }
        return true;
    }
    if (token.type == TOKEN_OPERATOR && (token.op.symbol == "-" || token.op.symbol == "!")) {
        next();
        return parseFactor();
    }
    if (peek().type == TOKEN_SEPARATOR && peek().separator.symbol == '[') {
        next(); // consume '['

        if (!parseExpression()) {
            printErrorWithContext("Expected expression in list literal", peek().position);
            return false;
        }

        while (matchSeparator(',')) {
            if (!parseExpression()) {
                printErrorWithContext("Expected expression after ',' in list literal", peek().position);
                return false;
            }
        }

        if (!matchSeparator(']')) {
            printErrorWithContext("Expected ']' to close list literal", peek().position);
            return false;
        }

        return true;  // successfully parsed a list literal
    }
    if (token.type == TOKEN_KEYWORD &&
        (token.keyword.type == KEYWORD_TRUE || token.keyword.type == KEYWORD_FALSE)) {
        next();
        return true;
    }


    printErrorWithContext("Invalid factor", token.position);
    return false;
}