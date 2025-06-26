// ast_generator.h
#ifndef AST_GENERATOR_H
#define AST_GENERATOR_H

#include "ast.h"
#include "tokenizer.h"
#include <memory>
#include <vector>
#include <string>

std::shared_ptr<ASTProgram> generateAST(const std::vector<GenericToken>& tokens, const std::string& sourceCode);

#endif