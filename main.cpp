#include <iostream>
#include "tokenizer.h"
#include <vector>

using namespace std;
int main() {
    FILE* file = fopen("test.pc", "r");
    if (!file) {
        cerr << "Failed to open file!" << endl;
        return 1;
    }

    vector<GenericToken> tokens = tokenizeFile(file);
    fclose(file);
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "[" << i << "] ";
        const GenericToken& token = tokens[i];
        switch (token.type) {
            case TOKEN_LITERAL:
                printTokenLiteral(&token.literal);
                break;
            case TOKEN_KEYWORD:
                printTokenKeyword(&token.keyword);
                break;
            case TOKEN_SEPARATOR:
                printTokenSeparator(&token.separator);
                break;
            case TOKEN_IDENTIFIER:
                printTokenIdentifier(&token.identifier);
                break;
            case TOKEN_OPERATOR:
                printTokenOperator(&token.op);
                break;

            case TOKEN_ERROR:
                printTokenError(&token.error);
                break;
        }
    }

    return 0;
}
