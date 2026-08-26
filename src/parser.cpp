#include "lang.h"
extern std::vector<Operator> optable;
// std::vector<Symbol*> SymbolTable;

namespace parser {
	std::vector<lexer::Token*> tokens;
	unsigned long long int size;
	const char* srcf;
	bool error = false;
	bool main(std::vector<lexer::Token*>& toks, unsigned long long int _size, const char* _srcf) {
		root = new Node();
		tokens = toks;
		size = _size;
		srcf = _srcf;
		rt::parser_init();
		root->type = Program; root->value = "root";
		for (unsigned long long int i = 0; i < size; ) {
			i = dcl_body(i, root);
		}
		return error;
	}
	unsigned long long int dcl_body(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i;
		if (strequ("fn", tokens[j]->token)) {
			j = parser::fndcl(j, parent);
		}
		else if (strequ("let", tokens[j]->token)) {
			j = vardcl(j, parent);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		else j++;
		return j;
	}
	unsigned long long int vardcl(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		node->type = Variable; node->value = "var";
		
		Node* cnode = new Node();
		cnode->type = Identifier; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);
		j++;

		if (!strequ(":", tokens[j]->token)) {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: colon in variable declaration was ignored.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
		}
		j = type(j, node);
		if (strequ(tokens[j]->token, "=")) {
			j++;
			j = expr(j, node);
		}
		parent->child.push_back(node);
		return j;
	}
	unsigned long long int fndcl(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		node->type = Function; node->value = "fn";

		Node* cnode = new Node();
		cnode->type = Identifier; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);
		j++;
		
		if (strequ("(", tokens[j]->token)) {
			unsigned long long int k = j + 1;
			for (; k < size && !strequ(")", tokens[k]->token);) {
				if (strequ(",", tokens[k]->token)) k++;
				Node* param = new Node();
				param->type = Argument; param->value = "param";
				Node* ccnode = new Node();
				ccnode->type = Identifier; ccnode->value = tokens[k]->token;
				if (strequ(":", tokens[++k]->token)) k = type(k, param);
				else {
					error = true;
					printf("In (%s:%llu:%llu): \n\terror: invalid parameter list.\n", srcf, tokens[j]->line, tokens[j]->column);
					k++;
				}
				param->child.push_back(ccnode);
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
		if (strequ("{", tokens[j]->token)) j = parser::stmt_body(j, node);		
		else {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: invalid syntax.\n", srcf, tokens[j]->line, tokens[j]->column);
		}
		parent->child.push_back(node);
		return j;
	}
	unsigned long long int type(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		parent->child.push_back(node);
		node->type = Type;
		if (strequ("const", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Type; cnode->value = "const";
			node->child.push_back(cnode);
			j++;
		}
		if ((tokens[j]->type == lexer::Keyword && IsType(tokens[j]->token))|| tokens[j]->type == lexer::Identifier) node->value = tokens[j]->token;
		else {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: invalid type %s.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
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
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		else if (strequ("let", tokens[j]->token)) {
			j = vardcl(j, node);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
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
				printf("In (%s:%llu:%llu): \n\terror: invalid if statement.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
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
				printf("In (%s:%llu:%llu): \n\terror: invalid if statement.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			node->child.push_back(cnode);
		}
		else if (strequ("goto", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = "goto";
			j = expr(j + 1, cnode);
			if (!strequ(";", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			node->child.push_back(cnode);
			j++;
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
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		return j;
	}
	unsigned long long int expr(unsigned long long int i, Node*& parent) {
		parser::pratt::i = i;
		Node* expr_ = pratt::main(0);
		parent->child.push_back(expr_);
		if (expr_ != nullptr) {
			return parser::pratt::i;
		}
		error = true;
		printf("In (%s:%llu:%llu): \n\terror: failed to parse expression.\n", srcf, tokens[i]->line, tokens[i]->column, tokens[i]->token);
		return i;			
	}
}
namespace parser::pratt {
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
						printf("In (%s:%llu:%llu): \n\terror: a expression must consisit of literals.\n", srcf, tokens[i]->line, tokens[i]->column, tokens[i]->token);
					}
			}
			node->value = tokens[i]->token;
			i++;
			return node;
		}
		else {
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
						printf("In (%s:%llu:%llu): \n\terror: the enclosing operator was missing its clossing delimiter.\n", srcf, tokens[i]->line, tokens[i]->column, tokens[i]->token);
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
							printf("In (%s:%llu:%llu): \n\terror: syntax for array indexing was missing a closing square bracket.\n", srcf, tokens[i]->line, tokens[i]->column, tokens[i]->token);
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
}