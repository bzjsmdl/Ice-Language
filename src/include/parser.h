#pragma once

#include "lang.h"

extern std::vector<Operator> optable;
// std::vector<Symbol*> SymbolTable;
extern std::map<std::string, std::string> ImportFile;

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

class parser {
	public:
		Node* root = nullptr;
		std::vector<lexer::Token*> tokens;
		unsigned long long int size;
		std::string srcf;
		std::string env;
		std::string cwd;
		bool error = false;
		unsigned long long int i = 0;

		parser(std::vector<lexer::Token*>& toks, unsigned long long int _size, const char* _srcf);

		bool main();

	private:
		unsigned long long int import(unsigned long long int i, Node*& parent);
		unsigned long long int dcl(unsigned long long int i, Node*& parent);
		unsigned long long int vardcl(unsigned long long int i, Node*& parent);
		unsigned long long int enumdcl(unsigned long long int i, Node*& parent);
		unsigned long long int fndcl(unsigned long long int i, Node*& parent, bool body);
		unsigned long long int type(unsigned long long int i, Node*& parent);
		unsigned long long int classdcl(unsigned long long int i, Node*& parent);
		unsigned long long int stmt_body(unsigned long long int i, Node*& parent);
		unsigned long long int stmt(unsigned long long int i, Node*& node);
		unsigned long long int expr(unsigned long long int i, Node*& parent);
		std::string stop_del;
		Node* pratt(unsigned int rbp);
		unsigned int GetBindingPower(std::string right);
		bool IsRightBinding(std::string right);
		Node* match();
		Node* nud();
		Node* led(Node* left);
};

#include "rt.h"