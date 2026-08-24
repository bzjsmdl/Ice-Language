#include "lang.h"
using namespace parser;
namespace cgen{
	void main(parser::Node* node, FILE* out) {
		if (!node) return;

		if (node->type == Program) {
			fprintf(out, "#include <bits/stdc++.h>\n");
			for (auto& c : node->child) main(c, out);
		}
		else if (node->type == Function) {
			std::string name, ret_type;

			for (auto& c : node->child) {
				if (c->type == Name) name = c->value;
				else if (c->type == Type) {
					ret_type = c->value;
					ret_type.append("_t");
				}
				else if (c->type == Body) {
					fprintf(out, "%s %s()\n", ret_type.c_str(), name.c_str());
					main(c, out);
				}
			}
		}
		else if (node->type == Body) {
			fprintf(out, "{\n");
			for (auto& c : node->child) main(c, out);
			fprintf(out, "}\n");
		}
		else if (node->type == Statement) {
			if (strequ(node->value.c_str(), "return")) {
				fprintf(out, "return ");
			}
			for (auto& c : node->child) main(c, out);
			fprintf(out, ";\n");
		}
		else if (node->type == Number) {
			fprintf(out, "%s", node->value.c_str());
		}
	}
	void PrintAST(parser::Node* node, unsigned int dep) {
		if (!node) return;
		for (unsigned int i = 0; i < dep; i++) printf("\t");
		switch (node->type) {
			case parser::Program:
				printf("Program ");
				break;
			case parser::Function:
				printf("Function ");
				break;
			case parser::Argument:
				printf("Argument ");
				break;
			case parser::Type:
				printf("Type ");
				break;
			case parser::Statement:
				printf("Statement ");
				break;
			case parser::Number:
				printf("Number ");
				break;
			case parser::Body:
				printf("Body ");
				break;
			case parser::Name:
				printf("Name ");
				break;
			default:
				printf("Unkown ");
		}
		printf("%s\n", node->value.c_str());
		for (auto& cnode : node->child) {
			PrintAST(cnode, dep + 1);
		}
	}
}