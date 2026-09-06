#ifndef ICE_PARSER_H
#define ICE_PARSER_H

#include "lang.h"
#include <io.h>

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

	Number,
	Character,
	String,
	IdentifierData,
	InnerData,

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
} NodeType;

typedef struct ASTNode {
	NodeType type;
	std::string value;
	std::vector<struct ASTNode*> child;
} Node;

Node* AstCompile(const char* _src);

class pratt {
	public:
	std::vector<lexer::Token*> tokens;
	unsigned long long int size;
	std::string srcf;
	bool error = false;

	unsigned long long int i = 0;
	bool inner = false;
	std::string stop_del;
	unsigned int GetBindingPower(std::string right) {
		for (auto& op : optable) {
			if (op.left.empty() && strequ(op.right.c_str(), right.c_str())) return op.binding_power;
			else if (!op.left.empty() && strequ(op.left.c_str(), right.c_str())) return op.binding_power;
		}
		return 0;
	}
	bool IsRightBinding(std::string right) {
		for (auto& op : optable) {
			if (strequ(op.right.c_str(), right.c_str()) && op.left.empty()) return op.right_binding;
		}
		return false;
	}
	Node* main(unsigned int rbp) {
		Node* left = nud();
		// printf("Can Run!\n");
		// printf("left = %p\n", left);
		while (i < size && GetBindingPower(tokens[i]->token) > rbp && left != nullptr) {
			if (IsRightBinding(tokens[i]->token) && !(GetBindingPower(tokens[i]->token) >= rbp)) break;
			else if (!IsRightBinding(tokens[i]->token) && !(GetBindingPower(tokens[i]->token) > rbp)) break;
			if (strequ(stop_del.c_str(), tokens[i]->token)) {
				stop_del.clear();
				break;
			}
			left = led(left);
			// i++;
		}
		return left;
	}
	Node* nud() {
		if (tokens[i]->type != lexer::Delimiter) {
			Node* node = new Node();
			switch (tokens[i]->type) {
				case lexer::Identifier:
					node->type = IdentifierData;
					break;
				case lexer::Number:
					node->type = Number;
					break;
				case lexer::String:
					node->type = String;
					break;
				case lexer::Character:
					node->type = Character;
					break;
				case lexer::Keyword:
					if (IsFactory(tokens[i]->token)) {
						node->type = InnerData;
					}
					else {
						error = true;
						printf("In (%s:%llu:%llu): \n\terror: a expression must consisit of literals.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
					}
			}
			node->value = tokens[i]->token;
			i++;
			return node;
		}
		else {
			if (strequ(tokens[i]->token, "[")) {
				Node* node = new Node();
				node->type = Array;
				i++;
				for (; i < size && !strequ(tokens[i]->token, "]");) {
					if (strequ(",", tokens[i]->token)) i++;
					Node* arg = main(0);
					node->child.push_back(arg);
				}
				i++;
				return node;
			}
			else if (strequ(tokens[i]->token, "(")) {
				unsigned long long int j = i;
				for (; j < size && !strequ(",", tokens[j]->token) && !strequ(")", tokens[j]->token); j++);
				if (strequ(",", tokens[j]->token)) {
					Node* node = new Node();
					node->type = Tuple;
					i++;
					for (; i < size && !strequ(tokens[i]->token, "]");) {
						if (strequ(",", tokens[i]->token)) i++;
						Node* arg = main(0);
						node->child.push_back(arg);
					}
					i++;
					return node;
				}
			}
				for (auto& op : optable) {
					if (strequ(op.left.c_str(), tokens[i]->token) && op.right.empty()) {
						Node* node = new Node();
						node->type = PrefixOp; node->value = tokens[i]->token;
						i++;
						Node* right = main(op.binding_power + ((op.right_binding) ? 0 : 1));
						node->child.push_back(right);
						return node;
					}
					else if (strequ(op.left.c_str(), tokens[i]->token) && !op.right.empty() && !op.ternary) {
						i++;
						Node* node = main(0);
						if (!strequ(tokens[i]->token, op.right.c_str())) {
							error = true;
							printf("In (%s:%llu:%llu): \n\terror: the enclosing operator was missing its clossing delimiter.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
						}
						i++;
						return node;
					}
				}
		}
		return nullptr;
	}
	Node* led(Node* left) {
		for (auto& op : optable) {
			if (strequ(tokens[i]->token, op.right.c_str()) && op.left.empty()) {
				Node* node = new Node();
				node->type = (op.mid) ? InfixOp : PostfixOp; node->value = tokens[i]->token;
				node->child.push_back(left);
				if (op.mid) {
					i++;
					Node* right = main(op.binding_power + ((op.right_binding) ? 0 : 1));
					node->child.push_back(right);
				}
				return node;
			}
			else if (strequ(tokens[i]->token, op.left.c_str()) && !op.right.empty()) {
				if (op.ternary) {
					Node* node = new Node();
					node->type = TernaryOp; node->value = tokens[i]->token;
					node->child.push_back(left);

					stop_del = op.right;
					i++;
					Node* mid = main(0);
					node->child.push_back(mid);
					
					i++;
					Node* right = main(0);
					node->child.push_back(right);
					return node;
				}
				else {
					if (strequ("(", tokens[i]->token)) {
						// Function Call
						Node* node = new Node();
						node->type = Expression; node->value = "call";
						node->child.push_back(left);
						i++;
						for (; i < size && !strequ(")", tokens[i]->token); ) {
							if (strequ(",", tokens[i]->token)) i++;
							else {
								Node* arg = main(0);
								node->child.push_back(arg);
							}
						}
						i++;
						return node;
					}
					else if (strequ("[", op.left.c_str())) {
						// Array Index
						Node* node = new Node();
						node->type = Expression; node->value.append(op.left).append(" ").append(op.right);
						node->child.push_back(left);
						i++;
						Node* right = main(0);
						if (!strequ(tokens[i]->token, op.right.c_str())) {
							error = true;
							printf("In (%s:%llu:%llu): \n\terror: syntax for array indexing was missing a closing square bracket.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
						}
						node->child.push_back(right);
						i++;
						return node;
					}
				}
			}
		}
		return nullptr;
	}
};

class parser {
	public:
		Node* root = nullptr;
		std::vector<lexer::Token*> tokens;
		unsigned long long int size;
		std::string srcf;
		std::string env;
		std::string cwd;

		parser(std::vector<lexer::Token*>& toks, unsigned long long int _size, const char* _srcf) {
			char* tmp_env = getenv("IGLOO");
			char* tmp_cwd = getcwd(NULL, 0);
			env = rt::unix_path(tmp_env);
			cwd = rt::unix_path(tmp_cwd);

			root = new Node();
			tokens = toks;
			size = _size;
			srcf = rt::unix_path(_srcf);
			
			if (srcf[0] == '.' && srcf.c_str()[1] == '/') {
				srcf.erase(srcf.begin());
				srcf.insert(0, cwd);
			}
			else if (srcf[0] == '.' && srcf.c_str()[1] == '.') {
				srcf.insert(0, cwd);
			}
			rt::parser_init();
			root->type = Moudle; root->value = srcf.c_str();
			root->value.erase(root->value.begin(), root->value.begin() + root->value.find_last_of('/') + 1);
			root->value.erase(root->value.begin() + root->value.find_last_of('.'), root->value.end());
			ImportFile[srcf] = root->value;
		}

		bool main() {
			for (unsigned long long int i = 0; i < size; ) {
				i = dcl(i, root);
			}
			return error;
		}
	private:
		bool error = false;

	unsigned long long int import(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		std::string path = "";
		bool first = true;
		std::string local = srcf.c_str();
		local.erase(local.find_last_of('/') + local.begin(), local.end());
		for (; j < size && !strequ(";", tokens[j]->token); j++) {
			if (strequ(tokens[j]->token, "..") && first) path.append(local + "../");
			else if (strequ(".", tokens[j]->token)) {
				if (first) path.append(local + "/");
				else path.append("/");
			}
			else if (tokens[j]->type == lexer::Identifier || tokens[j]->type == lexer::Keyword) {
				if (first) path.append(env + "/lib/" + tokens[j]->token + "/src/");
				else path.append(tokens[j]->token);
			}
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid delimiter %s in import statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			first = false;
		}
		path += ".ice";
		path = rt::unix_path(path);
		Node* node = new Node();
		node->type = Statement; node->value = "import";
		Node* cnode = new Node();
		cnode->type = Path; cnode->value = path;
		node->child.push_back(cnode);
		parent->child.push_back(node);
		return ++j;
	}
	unsigned long long int dcl(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i;
		Node* visibility = new Node();
		visibility->type = Visibility; visibility->value =  "public";
		unsigned long long int nni = parent->child.size();
		if (strequ("public", tokens[j]->token) || strequ("private", tokens[j]->token)) {
			visibility->value = tokens[j]->token;
			j++;
		}
		if (strequ("fn", tokens[j]->token)) {
			j = parser::fndcl(j, parent, true);
			parent->child[nni]->child.push_back(visibility);
		}
		else if (strequ("var", tokens[j]->token) || strequ("val", tokens[j]->token)) {
			j = vardcl(j, parent);
			parent->child[nni]->child.push_back(visibility);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		else if (strequ("class", tokens[j]->token)) {
			j = classdcl(j, parent);
			parent->child[nni]->child.push_back(visibility);
		}
		else if (strequ("enum", tokens[j]->token)) {
			j = enumdcl(j, parent);
			parent->child[nni]->child.push_back(visibility);
		}
		else if (strequ(tokens[j]->token, "extern")) {
			delete visibility;
			j++;
			Node* node = new Node();
			parent->child.push_back(node);
			node->type = Statement; node->value = "extern";
			if (strequ(tokens[j]->token, "fn")) j = fndcl(j, node, false);
			else if (strequ(tokens[j]->token, "let")) j = vardcl(j, node);
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid extern statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
				for (; j < size && !strequ(tokens[j]->token, ";"); j++);
			}
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		else if (strequ(tokens[j]->token, "import")) {
			delete visibility;
			j = import(j, parent);
		}
		else {
			delete visibility;
			j++;
		}
		// printf("Can Run!\n");
		return j;
	}
	unsigned long long int vardcl(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i;
		Node* node = new Node();
		node->type = Variable; node->value = tokens[j]->token;
		j++;
		
		Node* cnode = new Node();
		cnode->type = Identifier; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);
		j++;

		if (strequ(":", tokens[j]->token)) {
			if (strequ("val", tokens[j + 1]->token)) {
				error = true;
				if (node->value == "var") 
					printf("In (%s:%llu:%llu): \n\terror: %s was a vriable, but wasn't a constant.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
				else
					printf("In (%s:%llu:%llu): \n\twarnning: %s already was a constant.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
				j++;
			}
			j = type(j, node);
		}
		
		if (strequ(tokens[j]->token, "=")) {
			j++;
			j = expr(j, node);
		}
		parent->child.push_back(node);
		return j;
	}
	unsigned long long int enumdcl(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		node->type = Enum; node->value = "enum";

		if (tokens[j]->type != lexer::Identifier) {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: identifier is needed here, but why is \"%s\"?\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
		}
		Node* cnode = new Node();
		cnode->type = Identifier; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);
		j++;

		if (!strequ(tokens[j]->token, "{")) {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: invalid enumerate; it have to a body.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
		}
		Node* body = new Node();
		body->type = Body; body->value = "body";
		node->child.push_back(body);
		j++;
		for (; j < size && !strequ("}", tokens[j]->token); j++) {
			if (strequ(",", tokens[j]->token)) j++;
			if (tokens[j]->type != lexer::Identifier) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: identifier is needed here, but why is \"%s\"?\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			Node* ccnode = new Node();
			ccnode->type = Identifier; ccnode->value = tokens[j]->token;
			body->child.push_back(ccnode);
		}
		parent->child.push_back(node);
		return ++j;
	}
	unsigned long long int fndcl(unsigned long long int i, Node*& parent, bool body) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		node->type = Function; node->value = "fn";
		if (strequ("[", tokens[j]->token)) {
			unsigned long long int k = j + 1;
			for (; k < size && !strequ("]", tokens[k]->token);) {
				if (strequ(",", tokens[k]->token)) k++;
				if (!IsAttribute(tokens[k]->token)) {
					error = true;
					printf("In (%s:%llu:%llu): \n\terror: invalid attribute %s.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
				}
				Node* attr = new Node();
				attr->type = Attribute; attr->value = tokens[k]->token;
				node->child.push_back(attr);
				k++;
			}
			j = k + 1;
		}

		Node* cnode = new Node();
		cnode->type = Identifier; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);
		j++;
		
		if (strequ("(", tokens[j]->token)) {
			unsigned long long int k = j + 1;
			for (; k < size && !strequ(")", tokens[k]->token);) {
				if (strequ(",", tokens[k]->token)) k++;
				Node* param = new Node();
				if (strequ(tokens[k]->token, "...")) {
					param->type = Argument; param->value = "...";
					k++;
				}
				else {
					param->type = Argument; param->value = "param";
					Node* ccnode = new Node();
					ccnode->type = Identifier; ccnode->value = tokens[k]->token;
					if (strequ(":", tokens[++k]->token)) k = type(k, param);
					else {
						error = true;
						printf("In (%s:%llu:%llu): \n\terror: invalid parameter list.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
						k++;
					}
					param->child.push_back(ccnode);
				}
				node->child.push_back(param);
			}
			j = k + 1;
		}
		if (strequ("->", tokens[j]->token)) j = parser::type(j, node);
		else {
			Node* cnode = new Node();
			cnode->type = Type; cnode->value = "auto";
			node->child.push_back(cnode);
		}
		if (body) {
			if (strequ("{", tokens[j]->token)) j = parser::stmt_body(j, node);		
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid function declaration.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
		}
		parent->child.push_back(node);
		return j;
	}
	unsigned long long int type(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		parent->child.push_back(node);
		node->type = Type;
		if (strequ("val", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Type; cnode->value = "val";
			node->child.push_back(cnode);
			j++;
		}
		if ((tokens[j]->type == lexer::Keyword && IsType(tokens[j]->token))|| tokens[j]->type == lexer::Identifier) node->value = tokens[j]->token;
		else {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: invalid type %s.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			node->value = "auto";
		}
		j++;
		if (strequ(tokens[j]->token, "[")) {
			Node* cnode = new Node();
			cnode->type = Type; cnode->value = "[ ]";
			node->child.push_back(cnode);
			if (strequ(tokens[j + 1]->token, "]")) return j + 2;
			j = expr(j, cnode);
		}
		return j;
	}
	unsigned long long int classdcl(unsigned long long int i,  Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		node->type = Class; node->value = "class";

		Node* cnode = new Node();
		cnode->type = Identifier; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);

		if (!strequ("{", tokens[++j]->token)) {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: left brace in class declaration not found.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			j++;
		}
		while (j < size && !strequ("}", tokens[j]->token)) {
			j = dcl(j, node);
		}
		parent->child.push_back(node);
		return ++j;
	}
	unsigned long long int stmt_body(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		bool stmt_end = false;
		Node* node = new Node();
		node->type = Body; node->value = "body";
		for (; j < size && !strequ("}", tokens[j]->token); ) {
			j = stmt(j, node);
		}
		parent->child.push_back(node);
		return ++j;
	}
	unsigned long long int stmt(unsigned long long int i, Node*& node) {
		unsigned long long int j = i;
		if (strequ("return", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = tokens[j]->token;
			j++;
			j = expr(j, cnode);
			node->child.push_back(cnode);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			j++;
		}
		else if (strequ("continue", tokens[j]->token) || strequ("break", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = tokens[j]->token;
			j++;
			node->child.push_back(cnode);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			j++;
		}
		else if (strequ("var", tokens[j]->token) || strequ("val", tokens[j]->token)) {
			j = vardcl(j, node);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			j++;
		}
		else if (strequ("if", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = "if";
			j = expr(j + 1, cnode);
			if (strequ("{", tokens[j]->token)) j = stmt_body(j, cnode);
			else if (strequ(",", tokens[j]->token)) j = stmt(j + 1, cnode);
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: left brace in if statement not found.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			if (strequ("else", tokens[j]->token)) {
				Node* ccnode = new Node();
				ccnode->type = Statement; ccnode->value = "else";
				if (strequ("{", tokens[j]->token)) j = stmt_body(j, ccnode);
				else j = stmt(j + 1, ccnode);
				cnode->child.push_back(ccnode);
			}
			node->child.push_back(cnode);
		}
		else if (strequ("while", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = "while";
			j = expr(j + 1, cnode);
			if (strequ("{", tokens[j]->token)) j = stmt_body(j, cnode);
			else if (strequ(",", tokens[j]->token)) j = stmt(j + 1, cnode);
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid if statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			node->child.push_back(cnode);
		}
		else if (strequ("goto", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = "goto";
			j = expr(j + 1, cnode);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			node->child.push_back(cnode);
			j++;
		}
		else if (strequ(tokens[j]->token, "import")) {
			j = import(j, node);
		}
		else if (tokens[j]->type == lexer::Identifier && strequ(":", tokens[j + 1]->token)) {
			Node* cnode = new Node();
			cnode->type = Label; cnode->value = tokens[j]->token;
			node->child.push_back(cnode);
			j += 3;
		}
		else {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = "expr";
			j = expr(j, cnode);
			node->child.push_back(cnode);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		return j;
	}
	unsigned long long int expr(unsigned long long int i, Node*& parent) {
		pratt p = pratt();
		p.i = i; p.error = error; p.size = size;
		p.srcf = srcf.c_str(); p.tokens = tokens;
		Node* expr_ = p.main(0);
		parent->child.push_back(expr_);
		if (expr_ != nullptr) {
			return p.i;
		}
		error = true;
		printf("In (%s:%llu:%llu): \n\terror: failed to parse expression.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
		return i;			
	}
};
#endif