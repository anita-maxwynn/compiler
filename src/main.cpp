#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../include/tokenizer.h"
#include "../include/parser.h"
#include "../include/ast_generator.h"
#include "../include/interpreter.h"

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
        cerr << "Usage: " << argv[0] << " <source_file> [options]\n";
        cerr << "Options:\n";
        cerr << "  --interpret    Run with interpreter (default)\n";
        cerr << "  --compile      Generate code (future feature)\n";
        cerr << "  --check-types  Type checking only (future feature)\n";
        return 1;
    }
    string filename = argv[1];
    
    // Parse command line options
    bool useInterpreter = true;
    bool compileOnly = false;
    bool typeCheckOnly = false;
    
    for (int i = 2; i < argc; i++) {
        if (string(argv[i]) == "--compile") {
            useInterpreter = false;
            compileOnly = true;
        } else if (string(argv[i]) == "--check-types") {
            typeCheckOnly = true;
        } else if (string(argv[i]) == "--interpret") {
            useInterpreter = true;
        }
    }
    
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
        
        // Generate AST
        auto ast = generateAST(tokens, source);
        if (ast) {
            cout << "AST generation successful!" << endl;
            
            if (typeCheckOnly) {
                cout << "Type checking only - not implemented yet" << endl;
                // TODO: Add type checker here
            } else if (compileOnly) {
                cout << "Code generation - not implemented yet" << endl;
                // TODO: Add LLVM code generator here
            } else if (useInterpreter) {
                // Execute with interpreter (current working system)
                Interpreter interpreter;
                interpreter.execute(ast);
            }
        } else {
            cerr << "AST generation failed!" << endl;
            return 1;
        }
    } else {
        cerr << "Parsing failed!" << endl;
        return 1;
    }

    return 0;
}
