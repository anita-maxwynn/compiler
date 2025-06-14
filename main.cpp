#include <iostream>
#include "tokenizer.h"

using namespace std;
int main() {
    FILE* file = fopen("test.pc", "r");
    if (!file) {
        cerr << "Failed to open file!" << endl;
        return 1;
    }

    tokenizeFile(file);
    fclose(file);
    return 0;
}
