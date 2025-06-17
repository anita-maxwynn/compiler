#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <cstdio>

enum TokenType {
    INT,
    CHAR,
    DOUBLE,
    BOOL,
    STRING,
};
enum TokenTypeOperator {
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_MODULO,
    OPERATOR_EQUAL,
    OPERATOR_NOT_EQUAL,
    OPERATOR_LESS_THAN,
    OPERATOR_GREATER_THAN,
    OPERATOR_LESS_EQUAL,
    OPERATOR_GREATER_EQUAL,
    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_NOT,
    OPERATOR_BIT_AND,
    OPERATOR_BIT_OR,
    OPERATOR_BIT_XOR,
    OPERATOR_SHIFT_LEFT,
    OPERATOR_SHIFT_RIGHT,
    OPERATOR_BIT_NOT,
    OPERATOR_ASSIGN,
    OPERATOR_UNKNOWN
};
enum TokenTypeKeyword {
    KEYWORD_DEF,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_INPUT,
    KEYWORD_OUTPUT,
    KEYWORD_EXIT,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_ELIF,
    KEYWORD_FOR,
    KEYWORD_RETURN,
    KEYWORD_UNKNOWN
};

enum TokenTypeSeparator {
    SEMI,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    COMMA,
    // ASSIGN,
    SEPARATOR_UNKNOWN
};

struct TokenLiteral {
    TokenType type;
    union {
        int intValue;
        char charValue;
        double doubleValue;
        bool boolValue;
    } value;
    std::string stringValue;
    size_t position;
};

struct TokenKeyword {
    TokenTypeKeyword type;
    std::string name;
    size_t position;
};
struct TokenOperator {
    TokenTypeOperator type;
    std::string symbol;
    size_t position;
};
struct TokenSeparator {
    TokenTypeSeparator type;
    char symbol;
    size_t position;
};

struct TokenIdentifier {
    std::string name;
    size_t position;
};

enum GenericTokenType {
    TOKEN_LITERAL,
    TOKEN_KEYWORD,
    TOKEN_SEPARATOR,
    TOKEN_IDENTIFIER,
    TOKEN_OPERATOR,
    TOKEN_ERROR
};
struct TokenError {
    std::string message;
    size_t position;
};

struct GenericToken {
    GenericTokenType type;
    size_t position;
    TokenLiteral literal;
    TokenKeyword keyword;
    TokenSeparator separator;
    TokenIdentifier identifier;
    TokenOperator op;
    TokenError error;
};


void printTokenLiteral(const TokenLiteral* token);
void printTokenKeyword(const TokenKeyword* token);
void printTokenSeparator(const TokenSeparator* token);
void printTokenIdentifier(const TokenIdentifier* token);

TokenKeyword matchKeyword(const std::string& word, size_t position);
TokenSeparator matchSeparator(char ch, size_t position);

void tokenizeFile(FILE* file);

#endif
