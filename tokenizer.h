#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

enum TokenType {
    INT,
    CHAR,
    DOUBLE,
    BOOL,
    STRING,
};

enum TokenTypeKeyword {
    KEYWORD_DEF,
    KEYWORD_TRUE,
    KEYWORD_FALSE,
    KEYWORD_INPUT,
    KEYWORD_OUTPUT,
    KEYWORD_EXIT,
    KEYWORD_UNKNOWN
};

enum TokenTypeSeparator {
    SEMI,
    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_BRACE,
    CLOSE_BRACE,
    ASSIGN,
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
    std::string stringValue;  // for STRING
};

struct TokenKeyword {
    TokenTypeKeyword type;
    std::string name;
};

struct TokenSeparator {
    TokenTypeSeparator type;
    char symbol;
};
void printTokenLiteral(const TokenLiteral* token);
void printTokenKeyword(const TokenKeyword* token);
void printTokenSeparator(const TokenSeparator* token);

TokenKeyword matchKeyword(const std::string& word);
TokenSeparator matchSeparator(char ch);
void tokenizeFile(FILE* file);


#endif
