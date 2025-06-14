#include "tokenizer.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <cstdio>
#include <cstring>

using namespace std;

const size_t BUFFER_SIZE = 4096;

unordered_map<string, TokenTypeKeyword> keywordMap = {
    {"def", KEYWORD_DEF}, {"true", KEYWORD_TRUE}, {"false", KEYWORD_FALSE},
    {"input", KEYWORD_INPUT}, {"output", KEYWORD_OUTPUT}, {"exit", KEYWORD_EXIT}
};

TokenKeyword matchKeyword(const string& word) {
    auto it = keywordMap.find(word);
    if (it != keywordMap.end()) {
        return {it->second, word};
    }
    return {KEYWORD_UNKNOWN, word};
}

TokenSeparator matchSeparator(char ch) {
    switch (ch) {
        case ';': return {SEMI, ch};
        case '(': return {OPEN_PAREN, ch};
        case ')': return {CLOSE_PAREN, ch};
        case '{': return {OPEN_BRACE, ch};
        case '}': return {CLOSE_BRACE, ch};
        case '=': return {ASSIGN, ch};
        default: return {SEPARATOR_UNKNOWN, ch};
    }
}

void tokenizeFile(FILE* file) {
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    size_t index = 0;
    vector<string> identifiers;
    vector<TokenLiteral> literals;
    vector<TokenKeyword> keywords;
    vector<TokenSeparator> separators;

    string lexeme;

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        index = 0;
        while (index < bytesRead) {
            char curr = buffer[index++];

            if (isspace(curr)) continue;

            // Integer literal
            if (isdigit(curr)) {
                lexeme = "";
                while (isdigit(curr)) {
                    lexeme += curr;
                    if (index < bytesRead)
                        curr = buffer[index++];
                    else
                        break;
                }
                if (index <= bytesRead) index--;

                TokenLiteral tok;
                tok.type = INT;
                tok.value.intValue = stoi(lexeme);
                literals.push_back(tok);
            }

            else if (curr == '"') {
                lexeme = "";
                while (index < bytesRead && buffer[index] != '"') {
                    lexeme += buffer[index++];
                }
                index++;

                TokenLiteral tok;
                tok.type = STRING;
                tok.stringValue = lexeme;
                literals.push_back(tok);
            }


            else if (isalpha(curr)) {
                lexeme = "";
                while (isalnum(curr) || curr == '_') {
                    lexeme += curr;
                    if (index < bytesRead)
                        curr = buffer[index++];
                    else
                        break;
                }
                if (index <= bytesRead) index--; 

                TokenKeyword key = matchKeyword(lexeme);
                if (key.type != KEYWORD_UNKNOWN) {
                    keywords.push_back(key);

                    if (key.type == KEYWORD_TRUE || key.type == KEYWORD_FALSE) {
                        TokenLiteral tok;
                        tok.type = BOOL;
                        tok.value.boolValue = (key.type == KEYWORD_TRUE);
                        literals.push_back(tok);
                    }
                } else {
                    identifiers.push_back(lexeme);
                }
            }


            else {
                TokenSeparator sep = matchSeparator(curr);
                separators.push_back(sep);
            }
        }
    }

    // After tokenization, print results
    for (auto& k : keywords) {
        cout << "TokenKeyword: " << k.name << endl;
    }
    for (auto& lit : literals) {
        switch (lit.type) {
            case INT: cout << "TokenLiteral(INT): " << lit.value.intValue << endl; break;
            case STRING: cout << "TokenLiteral(STRING): " << lit.stringValue << endl; break;
            case BOOL: cout << "TokenLiteral(BOOL): " << (lit.value.boolValue ? "true" : "false") << endl; break;
            default: cout << "TokenLiteral(UNKNOWN TYPE)" << endl;
        }
    }
    for (auto& s : separators) {
        cout << "TokenSeparator: " << s.symbol << endl;
    }
    for (auto& id : identifiers) {
        cout << "Identifier: " << id << endl;
    }
}
