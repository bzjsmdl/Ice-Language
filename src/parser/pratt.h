#pragma once
#include "../include/parser.h"
#include "parser.h"

class pratt {
	public:
	unsigned long long int i = 0;
	bool error = false;
	pratt(unsigned long long int i, std::string srcf, std::vector<lexer::Token*> tokens, unsigned long long int size, bool error): i(i), srcf(srcf), tokens(tokens), size(size), error(error) {
		;
	}
	Node* main(unsigned int rbp) {
		Node* left = nud();
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
	

	private:
	std::vector<lexer::Token*> tokens;
	unsigned long long int size;
	std::string srcf;
	bool inner = false;
	std::string stop_del;

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
	Node* match(Node*& parent) {
		i++;
		Node* node = new Node();
		node->type = Expression; node->value = "match";
		Node* cnode = main(0);
		node->child.push_back(cnode);
		if (!strequ("{", tokens[i]->token)) {
			error = true;
			printf("In (%s:%llu:%llu): \n\terror: invalid character %s in match expression.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
		}
		i++;
		// match cnode { expr1 => expr2; }
		for (; i < size && !strequ("}", tokens[i]->token);) {
			Node* expr1 = main(0);
			if (!strequ("=>", tokens[i]->token)) {
				error = true;
				printf("In (%s:%llu:%llu): \n\terror: invalid character %s in match statement.\n", srcf.c_str(), tokens[i]->line, tokens[i]->column, tokens[i]->token);
			}
			i++;

		}
		return node;
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
					Node* arg = main(0);
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
					Node* arg = main(0);
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