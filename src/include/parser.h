#pragma once

#include "lang.h"
#include <io.h>

extern std::vector<Operator> optable;
// std::vector<Symbol*> SymbolTable;
extern std::vector<std::string> ImportFile;

typedef enum {
	Moudle,
	Path,
	Function,
	Argument,
	Type,
	Statement,
	Identifier,

	Literal,

	PrefixOp,
	PostfixOp,
	InfixOp,
	TernaryOp,
	Expression,
	Label,

	Visibility,
	Attribute,
	Class, 

	Body,
	Variable,
	Enum,
	Array,
	Tuple,
	
	Case
} NodeType;

typedef struct ASTNode {
	NodeType type;
	std::string value;
	std::vector<struct ASTNode*> child;
} Node;

Node* AstCompile(const char* _src);

namespace cgen{
	// void main(Node* node, FILE* out);
	void PrintAST(Node* node, unsigned int dep);
	void FreeAST(Node* node);
}
