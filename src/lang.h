#ifndef ICE_LANG_H
#define ICE_LANG_H

// Library
#include <bits/stdc++.h>

// Macro
#define strequ(s, S) (strcmp(s, S) == 0)
#define IsKeyword(s) (strequ(s, "if") || strequ(s, "else") || strequ(s, "while") || strequ(s, "goto") ||\
					  strequ(s, "return") || strequ(s, "continue") || strequ(s, "break") || strequ(s, "as") ||\
					  strequ(s, "sizeof") || strequ(s, "true") || strequ(s, "false") || strequ(s, "fn") ||\
					  strequ(s, "null") || strequ(s, "class") || strequ(s, "let") || strequ(s, "extern") ||\
					  strequ(s, "int8") || strequ(s, "int16") || strequ(s, "int32") || strequ(s, "int64") ||\
					  strequ(s, "uint8") || strequ(s, "uint16") || strequ(s, "uint32") || strequ(s, "uint64") ||\
					  strequ(s, "bool") || strequ(s, "string") || strequ(s, "ptr") || strequ(s, "usize") ||\
					  strequ(s, "ssize"))

// Runtime
namespace rt {
	unsigned long long int flen(FILE* file);
}

// Lexer
namespace lexer {
	typedef enum {
		Nothing,
		Keyword,
		Identifier,
		Delimiter,
		Number,
		String,
		Character
	} TokenType;

	typedef struct {
		char* token;
		TokenType type;
		unsigned long long int line;
		unsigned long long int column;
	} Token;

	inline std::vector<Token*> tokens;

	bool lexer(char*& text, unsigned long long int& length, const char* srcf);
}


// Parser
namespace parser {
	typedef enum {
		Program,
		Function,
		Argument,
		Type,
		Name,
		Statement,
		Number,
		Body,
	} NodeType;

	typedef struct ASTNode {
		NodeType type;
		std::string value;
		std::vector<struct ASTNode*> child;
	} Node;

	inline Node* root = nullptr;

	bool main(std::vector<lexer::Token*>& toks, unsigned long long int _size, const char* _srcf);
	unsigned long long int fndcl(unsigned long long int i, Node*& parent);
	unsigned long long int stmt_body(unsigned long long int i, Node*& parent);
	unsigned long long int body(unsigned long long int i, Node*& parent);
	unsigned long long int type(unsigned long long int i, Node*& parent);
}
namespace cgen{
	void main(parser::Node* node, FILE* out);
	void PrintAST(parser::Node* node, unsigned int dep);
}

#endif
