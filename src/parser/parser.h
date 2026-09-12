// Don't want to change it!
#ifndef ICE_PARSER_H
#define ICE_PARSER_H

#include "../include/parser.h"

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
			ImportFile.push_back(srcf);
		}

		bool main() {
			for (; i < size; ) {
				i = dcl(i, root);
			}
			return error;
		}
	private:
	unsigned long long int import(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		std::string path = "";
		bool first = true;
		bool main = true;
		std::string local = srcf.c_str();
		local.erase(local.find_last_of('/') + local.begin(), local.end());
		for (; j < size && !strequ(";", tokens[j]->token); j++) {
			if (strequ(tokens[j]->token, "..") && first) {
				path.append(local + "/../");
				main = false;
			}
			else if (strequ(".", tokens[j]->token)) {
				if (first) path.append(local + "/");
				else path.append("/");
				main = false;
			}
			else if (tokens[j]->type == lexer::Identifier || tokens[j]->type == lexer::Keyword) {
				if (first) path.append(env + "/lib/" + tokens[j]->token + "/src");
				else path.append(tokens[j]->token);
			}
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid delimiter %s in import statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			first = false;
		}
		path += (std::string)((main) ? "/main" : "") + ".ice";
		Node* node = new Node();
		node->type = Path; node->value = path;
		parent->child.push_back(node);
		// std::string moudle = path;
		// moudle.erase(moudle.begin(), moudle.begin() + moudle.find_last_of('/') + 1);
		// moudle.erase(moudle.begin() + moudle.find_last_of('.'), moudle.end());
		// {
		// 	FILE* handle = fopen(path.c_str(), "rb");
		// 	if (handle == nullptr) {
		// 		error = true;
		// 		printf("In (%s:%llu:%llu): \n\terror: moudle %s (%s) not found.\n", srcf, tokens[j]->line, tokens[j]->column, moudle.c_str(), path.c_str());
		// 		return ++j;
		// 	}
		// 	else fclose(handle);
		// }
		// if (std::find(ImportFile.begin(), ImportFile.end(), path) == ImportFile.end()) {
		// 	ImportFile.push_back(path);
		// 	Node* lib_ast = nullptr;
		// 	lib_ast = AstCompile(path.c_str());
		// 	parent->child.push_back(lib_ast);
		// }
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
			j = type(++j, node);
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
		unsigned long long int last_size = body->child.size();
		for (; j < size && !strequ("}", tokens[j]->token); j++) {
			if (strequ(",", tokens[j]->token)) j++;
			j = expr(j, body);
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
					if (strequ(":", tokens[++k]->token)) k = type(++k, param);
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
		if (strequ("->", tokens[j]->token)) j = parser::type(++j, node);
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
		unsigned long long int j = i;
		Node* node = new Node();
		parent->child.push_back(node);
		node->type = Type;
		if (strequ("val", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Type; cnode->value = "val";
			j++;
		}
		if (strequ(tokens[j]->token, "(")) {
			node->type = Tuple; node->value = "tuple";
			j++;
			for (; j < size && !strequ(tokens[j]->token, ")");) {
				if (strequ(",", tokens[j]->token)) j++;
				j = type(j, node);
			}
			j++;
		}
		else node->value = tokens[j++]->token;
		while (j < size && strequ(tokens[j]->token, "[")) {
			Node* cnode = new Node();
			cnode->type = Type; cnode->value = "[ ]";
			j++;
			if (!strequ(tokens[j]->token, "]"))  j = expr(j, cnode);
			j++;
			node->child.push_back(cnode);
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
		else if (strequ("{", tokens[j]->token)) {
			Node* cnode = new Node();
			static unsigned long long int body_cnt = 0;
			std::string body_name = "B";
			body_name += std::to_string(body_cnt);
			cnode->type = Body; cnode->value = body_name;
			j = stmt_body(++j, cnode);
			if (!strequ("}", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: miss '}'.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			j++;
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
				printf("In (%s:%llu:%llu): \n\terror: invalid while statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			node->child.push_back(cnode);
		}
		else if (strequ("for", tokens[j]->token)) {
			Node* cnode = new Node();
			cnode->type = Statement; cnode->value = "for-in";
			j = expr(j + 1, cnode);
			if (!strequ("in", tokens[j]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid for-in statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
			}
			j = expr(++j, cnode);
			if (strequ("{", tokens[j]->token)) j = stmt_body(j, cnode);
			else if (strequ(",", tokens[j]->token)) j = stmt(j + 1, cnode);
			else {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid for-in statement.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column);
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
			if (!strequ(";", tokens[j]->token) && (!strequ("}", tokens[j]->token) && (cnode->value == "match"))) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: a statement must end with a semicolon.\n", srcf.c_str(), tokens[j]->line, tokens[j]->column, tokens[j]->token);
			}
			j++;
		}
		return j;
	}
	unsigned long long int expr(unsigned long long int i, Node*& parent) {
		this->i = i;
		Node* expr_ = pratt(0);
		parent->child.push_back(expr_);
		if (expr_ != nullptr) {
			return this->i;
		}
		printf("In (%s:%llu:%llu): \n\terror: failed to parse expression.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
		return i;			
	}
	// pratt
	std::string stop_del;
	Node* pratt(unsigned int rbp) {
		Node* left = nud();
		if (left->value == "match") {
			delete left;
			left = match();

			return left;
		}
		while (i < size && GetBindingPower(tokens[i]->token) > rbp && left != nullptr) {
			if (IsRightBinding(tokens[i]->token) && !(GetBindingPower(tokens[i]->token) >= rbp)) break;
			else if (!IsRightBinding(tokens[i]->token) && !(GetBindingPower(tokens[i]->token) > rbp)) break;
			if (strequ(stop_del.c_str(), tokens[i]->token)) {
				stop_del.clear();
				break;
			}
			left = led(left);
		}
		return left;
	}

	// Utils
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

	// Special Expression
	Node* match() {
		Node* parent = new Node();
		parent->type = Expression; parent->value = "match";
		Node* cnode = pratt(0);
		parent->child.push_back(cnode);
		if (!strequ("{", tokens[i]->token)) {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: invalid character %s in match expression.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
		}
		i++;
		// match cnode { expr => body; }
		for (; i < size && !strequ("}", tokens[i]->token);) {
			Node* node = new Node();
			node->type = Case; node->value = "case";
			Node* expr = pratt(0);
			node->child.push_back(expr);
			if (!strequ("=>", tokens[i]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid character %s in match statement.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
			}
			i = stmt(++i, node);
			parent->child.push_back(node);
		}
		i++;
		return parent;
	}

	Node* nud() {
		if (tokens[i]->type != lexer::Delimiter) {
			Node* node = new Node();
			switch (tokens[i]->type) {
				case lexer::Character:
				case lexer::Number:
				case lexer::String:
				case lexer::Keyword:
					if (IsType(tokens[i]->token)) {
						node->type = Type;
						break;
					}
					node->type = Literal;
					break;
				case lexer::Identifier:	
					node->type = Identifier;
					break;
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
					Node* arg = pratt(0);
					node->child.push_back(arg);
				}
				i++;
				return node;
			}
			else if (strequ(tokens[i]->token, "(")) {
				std::string cpy = tokens[i]->token;
				Node* node = new Node();
				node->type = Tuple;
				i++;
				for (; i < size && !strequ(tokens[i]->token, ")");) {
					if (strequ(",", tokens[i]->token)) i++;
					Node* arg = pratt(0);
					node->child.push_back(arg);
				}
				i++;
				return node;
			}
				for (auto& op : optable) {
					if (strequ(op.left.c_str(), tokens[i]->token) && op.right.empty()) {
						Node* node = new Node();
						node->type = PrefixOp; node->value = tokens[i]->token;
						i++;
						Node* right = pratt(op.binding_power + ((op.right_binding) ? 0 : 1));
						node->child.push_back(right);
						return node;
					}
					else if (strequ(op.left.c_str(), tokens[i]->token) && !op.right.empty() && !op.ternary) {
						i++;
						Node* node = pratt(0);
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
					Node* right = pratt(op.binding_power + ((op.right_binding) ? 0 : 1));
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
					Node* mid = pratt(0);
					node->child.push_back(mid);
					
					i++;
					Node* right = pratt(0);
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
								Node* arg = pratt(0);
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
						Node* right = pratt(0);
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
#endif