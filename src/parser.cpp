#include "lang.h"
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
		root->type = Program; root->value = "root";
		for (unsigned long long int i = 0; i < size; i++) {
			if (strequ("fn", tokens[i]->token)) {
				i = parser::fndcl(i, root);
			}
		}
		return error;
	}
	unsigned long long int fndcl(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		node->type = Function; node->value = "fn";

		Node* cnode = new Node();
		cnode->type = Name; cnode->value = tokens[j]->token;
		node->child.push_back(cnode);
		j++;
		
		if (strequ("(", tokens[j]->token)) {
			unsigned long long int k = i + 1;
			for (; k < size && !strequ(")", tokens[k]->token);) {
				k++;
			}
			j = k + 1;
		}
		if (strequ("->", tokens[j]->token)) j = parser::type(j, node);
		if (strequ("{", tokens[j]->token)) j = parser::stmt_body(j, node);
		else {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: Invalid syntax.\n", srcf, tokens[j]->line, tokens[j]->column);
		}
		parent->child.push_back(node);
		return ++j;
	}
	unsigned long long int type(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i + 1;
		Node* node = new Node();
		parent->child.push_back(node);
		node->type = Type;
		if (tokens[j]->type == lexer::Keyword || tokens[j]->type == lexer::Identifier) node->value = tokens[j]->token;
		else {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: Invalid type %s.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
		}
		return ++j;
	}
	unsigned long long int stmt_body(unsigned long long int i, Node*& parent) {
		unsigned long long int j = i;
		Node* node = new Node();
		node->type = Body; node->value = "body";
		for (; j < size && !strequ("}", tokens[j]->token); ) {
			if (strequ("return", tokens[j]->token)) {
				Node* cnode = new Node();
				cnode->type = Statement; cnode->value = tokens[j]->token;
				Node* ccnode = new Node();
				ccnode->type = Number;
				j++;
				if (tokens[j]->type == lexer::Number) ccnode->value = tokens[j]->token;
				else {
					error = true;
					printf("In (%s:%llu:%llu): \n\terror: Invalid return value %s.\n", srcf, tokens[j]->line, tokens[j]->column, tokens[j]->token);
				}
				cnode->child.push_back(ccnode);
				node->child.push_back(cnode);
			}
			else {
				j++;
			}
		}
		parent->child.push_back(node);
		return j;
	}
}