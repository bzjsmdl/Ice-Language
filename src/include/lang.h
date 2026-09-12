#ifndef ICE_LANG_H
#define ICE_LANG_H

// Library
#include <bits/stdc++.h>

// Macro
#define strequ(s, S) (strcmp(s, S) == 0)
#define IsKeyword(s) (strequ(s, "if") || strequ(s, "else") || strequ(s, "while") || strequ(s, "goto") ||\
					  strequ(s, "return") || strequ(s, "continue") || strequ(s, "break") || strequ(s, "as") ||\
					  strequ(s, "sizeof") || strequ(s, "fn") || strequ(s, "class") || strequ(s, "var") ||\
					  strequ(s, "extern") || IsFactory(s) || strequ(s, "self") || strequ(s, "enum") ||\
					  strequ(s, "public") || strequ(s, "private") || strequ(s, "val"))
#define IsFactory(s) (IsType(s) || strequ(s, "false") || strequ(s, "true") || strequ(s, "self"))
#define IsType(s) (strequ(s, "int8") || strequ(s, "int16") || strequ(s, "int32") || strequ(s, "int64") ||\
					strequ(s, "uint8") || strequ(s, "uint16") || strequ(s, "uint32") || strequ(s, "uint64") ||\
					strequ(s, "bool") || strequ(s, "string") || strequ(s, "ptr") || strequ(s, "usize") ||\
					strequ(s, "float32") || strequ(s, "float64") || strequ(s, "ssize") || strequ(s, "null"))

#define IsAttribute(s) (strequ(s, "@init") || strequ(s, "@deinit"))

// Runtime
namespace rt {
	unsigned long long int flen(FILE* file);
	void parser_init();
	std::string unix_path(std::string path);
}

// typedef struct {
// 	enum SymbolType{
// 		New_Operator,
// 		Function,
// 		Variable,
// 		Overload_Operator,
// 	};
// 	std::string name;
// 	enum SymbolType type;
// 	unsigned long long int optableidx;
// 	parser::Node* body;
// } Symbol;

typedef struct {
	std::string left;
	std::string right;
	bool mid;
	bool inner;
	bool ternary;
	bool overload;
	bool right_binding;
	unsigned int binding_power;
}Operator;

// Lexer
class lexer {
	public:
	typedef enum {
		Nothing,
		Keyword,
		Delimiter,
		Identifier,
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

	std::vector<Token*> tokens;

	bool main(char*& text, unsigned long long int& length, const char* srcf);
};

// Parser
#include "../parser/parser.h"

#endif

/*
std::string moudle = path;
		moudle.erase(moudle.begin(), moudle.begin() + moudle.find_last_of('/') + 1);
		moudle.erase(moudle.begin() + moudle.find_last_of('.'), moudle.end());
		{
			FILE* handle = fopen(path.c_str(), "rb");
			if (handle == nullptr) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: moudle %s (%s) not found.\n", srcf, tokens[j]->line, tokens[j]->column, moudle.c_str(), path.c_str());
				return ++j;
			}
			else fclose(handle);
		}
		if (ImportFile.find(path) == ImportFile.end()) {
			ImportFile[path] = moudle;
			Node* lib_ast = nullptr;
			lib_ast = AstCompile(path.c_str());
			parent->child.push_back(lib_ast);
		}
		return
*/