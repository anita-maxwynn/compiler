#include "../include/tokenizer.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <cctype>

TokenKeyword matchKeyword(const std::string& word, size_t position) {
    if (word == "def") return { KEYWORD_DEF, word, position };
    // if (word == "true") return { KEYWORD_TRUE, word, position };
    // if (word == "false") return { KEYWORD_FALSE, word, position };
    if (word == "input") return { KEYWORD_INPUT, word, position };
    if (word == "output") return { KEYWORD_OUTPUT, word, position };
    if (word == "exit") return { KEYWORD_EXIT, word, position };
    if (word == "if") return { KEYWORD_IF, word, position };
    if (word == "else") return { KEYWORD_ELSE, word, position };
    if (word == "elif") return { KEYWORD_ELIF, word, position };
    if (word == "for") return { KEYWORD_FOR, word, position };
    if (word == "return") return { KEYWORD_RETURN, word, position };
    return { KEYWORD_UNKNOWN, word, position };
}

TokenSeparator matchSeparator(char ch, size_t position) {
    switch (ch) {
        case ';': return { SEMI, ch, position };
        case '(': return { OPEN_PAREN, ch, position };
        case ')': return { CLOSE_PAREN, ch, position };
        case '{': return { OPEN_BRACE, ch, position };
        case '}': return { CLOSE_BRACE, ch, position };
        case '[': return { OPEN_BRACKET, ch, position };
        case ']': return { CLOSE_BRACKET, ch, position };
        case ',': return { COMMA, ch, position };
        // case '=': return { ASSIGN, ch, position };
        default: return { SEPARATOR_UNKNOWN, ch, position };
    }
}

TokenOperator matchOperator(char first, char second, size_t position) {
    std::string op;
    op += first;
    op += second;

    if (op == "==") return { OPERATOR_EQUAL, op, position };
    if (op == "!=") return { OPERATOR_NOT_EQUAL, op, position };
    if (op == "<=") return { OPERATOR_LESS_EQUAL, op, position };
    if (op == ">=") return { OPERATOR_GREATER_EQUAL, op, position };
    if (op == "&&") return { OPERATOR_AND, op, position };
    if (op == "||") return { OPERATOR_OR, op, position };
    if (op == "<<") return { OPERATOR_SHIFT_LEFT, op, position };
    if (op == ">>") return { OPERATOR_SHIFT_RIGHT, op, position };

    return { OPERATOR_UNKNOWN, std::string(1, first), position };
}

TokenOperator matchSingleOperator(char ch, size_t position) {
    switch (ch) {
        case '+': return { OPERATOR_PLUS, "+", position };
        case '-': return { OPERATOR_MINUS, "-", position };
        case '*': return { OPERATOR_MULTIPLY, "*", position };
        case '/': return { OPERATOR_DIVIDE, "/", position };
        case '%': return { OPERATOR_MODULO, "%", position };
        case '<': return { OPERATOR_LESS_THAN, "<", position };
        case '>': return { OPERATOR_GREATER_THAN, ">", position };
        case '!': return { OPERATOR_NOT, "!", position };
        case '&': return { OPERATOR_BIT_AND, "&", position };
        case '|': return { OPERATOR_BIT_OR, "|", position };
        case '^': return { OPERATOR_BIT_XOR, "^", position };
        case '=': return { OPERATOR_ASSIGN, "=", position };
        default:  return { OPERATOR_UNKNOWN, std::string(1, ch), position };
    }
}

int peekNextChar(FILE* file) {
    int c = fgetc(file);
    if (c != EOF) ungetc(c, file);
    return c;
}

void printTokenLiteral(const TokenLiteral* token) {
    std::cout << "TokenLiteral(";
    switch (token->type) {
        case INT: std::cout << "INT): " << token->value.intValue; break;
        case CHAR: std::cout << "CHAR): " << token->value.charValue; break;
        case DOUBLE: std::cout << "DOUBLE): " << token->value.doubleValue; break;
        case BOOL: std::cout << "BOOL): " << (token->value.boolValue ? "true" : "false"); break;
        case STRING: std::cout << "STRING): " << token->stringValue; break;
    }
    std::cout << " at position " << token->position << "\n";
}

void printTokenKeyword(const TokenKeyword* token) {
    std::cout << "TokenKeyword: " << token->name << " at position " << token->position << "\n";
}

void printTokenSeparator(const TokenSeparator* token) {
    std::cout << "TokenSeparator: '" << token->symbol << "' at position " << token->position << "\n";
}

void printTokenIdentifier(const TokenIdentifier* token) {
    std::cout << "TokenIdentifier: " << token->name << " at position " << token->position << "\n";
}

void printTokenError(const TokenError* token) {
    std::cerr << "Error at position " << token->position << ": " << token->message << "\n";
}
void printTokenOperator(const TokenOperator* token) {
    std::cout << "TokenOperator: " << token->symbol << " at position " << token->position << "\n";
}

std::vector<GenericToken> tokenizeFile(FILE* file) {
    std::vector<GenericToken> tokens;
    char ch;
    size_t index = 0;

    while ((ch = fgetc(file)) != EOF) {
        if (isspace(ch)) {
            index++;
            continue;
        }
        // Handle comments
        if (ch == '/') {
            char next = fgetc(file);
            if (next == '/') {
                index += 2;
                while ((ch = fgetc(file)) != '\n' && ch != EOF) {
                    index++;
                }
                continue;
            } else if (next == '*') {
                // Multi-line comment
                index += 2;
                bool closed = false;
                while ((ch = fgetc(file)) != EOF) {
                    index++;
                    if (ch == '*') {
                        char maybeSlash = fgetc(file);
                        index++;
                        if (maybeSlash == '/') {
                            closed = true;
                            break;
                        } else {
                            ungetc(maybeSlash, file);
                            index--;
                        }
                    }
                }
                if (!closed) {
                    std::cerr << "Error: Unclosed multi-line comment starting at position " << index << "\n";
                }
                continue;
            } else {
                ungetc(next, file);
            }
        }

        if (isdigit(ch) || ch == '.' || (ch == '-' && isdigit(peekNextChar(file)))) {
            bool isNegative = false;
            if (ch == '-') {
                isNegative = true;
                ch = fgetc(file); // Consume the digit after '-'
                index++;
            }

            std::string number;
            size_t start = index;
            bool hasDecimal = false;

            if (isNegative) number += '-';

            // Handle leading dot
            if (ch == '.') {
                hasDecimal = true;
                number += ch;
                ch = fgetc(file);
                index++;
                if (!isdigit(ch)) {
                    GenericToken token;
                    token.type = TOKEN_ERROR;
                    token.position = start;
                    token.error = { "Invalid float literal starting with '.'", start };
                    tokens.push_back(token);
                    continue;
                }
            }

            while (isdigit(ch) || ch == '.') {
                if (ch == '.') {
                    if (hasDecimal) {
                        GenericToken token;
                        token.type = TOKEN_ERROR;
                        token.position = index;
                        token.error = { "Multiple dots in number literal", index };
                        tokens.push_back(token);
                        break;
                    }
                    hasDecimal = true;
                }

                number += ch;
                ch = fgetc(file);
                index++;
            }
            
            if (!tokens.empty() && tokens.back().type == TOKEN_ERROR) {
                while (ch != EOF && !isspace(ch) && ch != ';') {
                    ch = fgetc(file);
                    index++;
                }
                continue;
            }

            if (!isspace(ch) && ch != EOF) ungetc(ch, file);

            TokenLiteral literal;
            literal.position = start;

            try {
                if (hasDecimal) {
                    literal.type = DOUBLE;
                    literal.value.doubleValue = std::stod(number);
                } else {
                    literal.type = INT;
                    literal.value.intValue = std::stoi(number);
                }
            } catch (...) {
                GenericToken token;
                token.type = TOKEN_ERROR;
                token.position = start;
                token.error = { "Invalid number format: " + number, start };
                tokens.push_back(token);
                continue;
            }

            GenericToken token;
            token.type = TOKEN_LITERAL;
            token.position = start;
            token.literal = literal;
            tokens.push_back(token);
            continue;
        }


        else if (std::string("=!<>|&+-*/%^").find(ch) != std::string::npos) {
            char next = peekNextChar(file);
            TokenOperator op = matchOperator(ch, next, index);
            if (op.type != OPERATOR_UNKNOWN) {
                fgetc(file); // consume next char
                index += 2;
            } else {
                op = matchSingleOperator(ch, index);
                index++;
            }

            GenericToken token;
            token.type = TOKEN_OPERATOR;
            token.position = op.position;
            token.op = op;
            tokens.push_back(token);
            continue;
        }

                // Handle identifiers and keywords
                // else if (isalpha(ch)) {
                //     std::string word;
                //     size_t start = index;
                //     word += ch;
                //     ch = fgetc(file);
                //     index++;
                //     while (isalnum(ch)|| ch == '_') {
                //         word += ch;
                //         ch = fgetc(file);
                //         index--;
                //     }
                //     ungetc(ch, file);

                //     if (word == "true" || word == "false") {
                //         TokenLiteral literal;
                //         literal.type = BOOL;
                //         literal.value.boolValue = (word == "true");
                //         literal.position = start;

                //         GenericToken token;
                //         token.type = TOKEN_LITERAL;
                //         token.position = start;
                //         token.literal = literal;
                //         tokens.push_back(token);
                //     } else {
                //         TokenKeyword keyword = matchKeyword(word, start);
                //         GenericToken token;
                //         token.type = TOKEN_KEYWORD;
                //         token.position = start;
                //         token.keyword = keyword;
                //         tokens.push_back(token);
                //     }
                // }

        // Handle string literals
        else if (ch == '"') {
            std::string str;
            size_t start = index;
            ch = fgetc(file);
            index++;
            while (ch != '"' && ch != EOF) {
                str += ch;
                ch = fgetc(file);
                index++;
            }

            TokenLiteral literal;
            literal.type = STRING;
            literal.stringValue = str;
            literal.position = start;

            GenericToken token;
            token.type = TOKEN_LITERAL;
            token.position = start;
            token.literal = literal;
            tokens.push_back(token);
        }
        else if (isalpha(ch) || ch == '_') {
    std::string word;
    size_t start = index;
    while (isalnum(ch) || ch == '_') {
        word += ch;
        ch = fgetc(file);
        index++;
    }
    ungetc(ch, file);

    if (word == "true" || word == "false") {
        TokenLiteral literal;
        literal.type = BOOL;
        literal.value.boolValue = (word == "true");
        literal.position = start;

        GenericToken token;
        token.type = TOKEN_LITERAL;
        token.position = start;
        token.literal = literal;
        tokens.push_back(token);
    } else {
        TokenKeyword keyword = matchKeyword(word, start);
        if (keyword.type != KEYWORD_UNKNOWN) {
            GenericToken token;
            token.type = TOKEN_KEYWORD;
            token.position = start;
            token.keyword = keyword;
            tokens.push_back(token);
        } else {
            TokenIdentifier identifier;
            identifier.name = word;
            identifier.position = start;

            GenericToken token;
            token.type = TOKEN_IDENTIFIER;
            token.position = start;
            token.identifier = identifier;
            tokens.push_back(token);
        }
    }
}

        else if (ch == '\'') {
            char charValue;
            size_t start = index;
            ch = fgetc(file);
            index++;
            if (ch == '\\') { // Handle escape sequences
                ch = fgetc(file);
                index++;
                switch (ch) {
                    case 'n': charValue = '\n'; break;
                    case 't': charValue = '\t'; break;
                    case '\\': charValue = '\\'; break;
                    case '\'': charValue = '\''; break;
                    case '\"': charValue = '\"'; break;
                    default: 
                        GenericToken token;
                        token.type = TOKEN_ERROR;
                        token.position = start;
                        token.error = { "Invalid escape sequence: \\" + std::string(1, ch), start };
                        tokens.push_back(token);
                        continue;
                }
            } else {
                charValue = ch; // Regular character
            }
            ch = fgetc(file);
            index++;

            if (ch != '\'') {
                GenericToken token;
                token.type = TOKEN_ERROR;
                token.position = start;
                token.error = { "Unclosed character literal", start };
                tokens.push_back(token);
                continue;
            }

            TokenLiteral literal;
            literal.type = CHAR;
            literal.value.charValue = charValue;
            literal.position = start;

            GenericToken token;
            token.type = TOKEN_LITERAL;
            token.position = start;
            token.literal = literal;
            tokens.push_back(token);
        }

        // Handle separators and unknown characters
        else {
            TokenSeparator sep = matchSeparator(ch, index);
            if (sep.type != SEPARATOR_UNKNOWN) {
                GenericToken token;
                token.type = TOKEN_SEPARATOR;
                token.position = index;
                token.separator = sep;
                tokens.push_back(token);
            } else {
                GenericToken token;
                token.type = TOKEN_ERROR;
                token.position = index;
                token.error = { std::string("Unknown character: ") + ch, index };
                tokens.push_back(token);
            }
            index++;
        }
    }

    // Print all tokens
    return tokens;
}

