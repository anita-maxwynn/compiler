#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "tokenizer.h"

bool parseProgramInternal(const std::vector<GenericToken>& tokens, const std::string& source);

#endif // PARSER_H