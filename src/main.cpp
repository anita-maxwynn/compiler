#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../include/tokenizer.h"
#include "../include/parser.h"

using namespace std;

// Read source file content into a string
string readSourceFile(const string& path) {
    ifstream in(path);
    if (!in) {
        cerr << "Failed to open file: " << path << endl;
        exit(1);
    }
    stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }
    string filename = argv[1];
    string source = readSourceFile(filename);

    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        cerr << "Failed to open file!" << endl;
        return 1;
    }

    vector<GenericToken> tokens = tokenizeFile(file);
    fclose(file);

    for (const auto& token : tokens) {
        cout << "Token at " << token.position << ": ";
        switch (token.type) {
            case TOKEN_IDENTIFIER:
                cout << "IDENTIFIER - " << token.identifier.name << "\n"; break;
            case TOKEN_OPERATOR:
                cout << "OPERATOR - " << token.op.symbol << "\n"; break;
            case TOKEN_SEPARATOR:
                cout << "SEPARATOR - '" << token.separator.symbol << "'\n"; break;
            case TOKEN_LITERAL:
                cout << "LITERAL\n"; break;
            case TOKEN_KEYWORD:
                cout << "KEYWORD - " << token.keyword.name << "\n"; break;
            case TOKEN_ERROR:
                cerr << "ERROR - " << token.error.message << "\n"; break;
        }
    }

    if (parseProgramInternal(tokens, source)) {
        cout << "Parsing successful!" << endl;
    } else {
        cerr << "Parsing failed!" << endl;
        return 1;
    }

    return 0;
}
