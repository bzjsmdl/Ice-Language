#include "include/lang.h"
std::vector<Operator> optable;
namespace rt {
	unsigned long long int flen(FILE* file) {
		fseek(file, 0, SEEK_END);
		unsigned long long int len = ftell(file);
		fseek(file, 0, SEEK_SET);
		return len;
	}
	std::string unix_path(std::string path) {
		std::string result = path;
		for (unsigned long long int k = 0; k < result.length(); k++) {
			if (result[k] == '\\') result[k] = '/';
		}
		return result;
	}
	void parser_init() {
		Operator op;

		// bp 100
		op.left = ""; op.right = "."; op.mid = true; op.inner = true; op.overload = false; op.right_binding = false; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "with"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<-"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "as"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "("; op.right = ")"; op.mid = false; op.inner = true; op.overload = false; op.right_binding = false; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "["; op.right = "]"; op.mid = false; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "+"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "-"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		// bp 90
		op.left = ""; op.right = ":"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = false; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "sizeof"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "++"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "--"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "&"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "*"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "!"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "~"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "++"; op.mid = false; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "--"; op.mid = false; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		// bp 80
		op.left = ""; op.right = "&"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "|"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "^"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<<"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">>"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		// bp 70
		op.left = ""; op.right = "*"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 70; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "/"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 70; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "%"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 70; op.ternary = false;
		optable.push_back(op);

		// bp 60
		op.left = ""; op.right = "+"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "-"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "=="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "!="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		// bp 50
		op.left = ""; op.right = "&&"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 50; op.ternary = false;
		optable.push_back(op);

		// bp 40
		op.left = ""; op.right = "||"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 40; op.ternary = false;
		optable.push_back(op);

		// bp 30
		op.left = "?"; op.right = ":"; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 30; op.ternary = true;
		optable.push_back(op);

		// bp 20
		op.left = ""; op.right = "="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "+="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "-="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "*="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "/="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "%="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "&="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "|="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "^="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<<="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">>="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);
	}
}

Node* AstCompile(const char* _src) {
	// std::cout << ImportFile.empty() << std::endl;
	FILE* src = fopen(_src, "rb");
	if (src == nullptr) {
		printf("error: input file not found.\n");
		return nullptr;
	}
	unsigned long long int nfo = ftell(src);
	unsigned long long int length = rt::flen(src);
	char* text = new char[length + 1]{0};
	fread(text, 1, length, src);
	fseek(src, nfo, SEEK_SET);
	fclose(src);
	lexer main_lexer = lexer();
	if (main_lexer.main(text, length, _src)) {
		for (auto& tok : main_lexer.tokens) {
			delete tok->token;
			delete tok;
		}
		delete text;
		return nullptr;
	}
	delete text;
	// for (auto& tok : main_lexer.tokens) printf("Token %s of %u at (%s:%llu:%llu)\n", tok->token, tok->type, _src, tok->line, tok->column);
	parser main_parser = parser(main_lexer.tokens, main_lexer.tokens.size(), _src);
	if (main_parser.main()) {
		cgen::FreeAST(main_parser.root);
		for (auto& tok : main_lexer.tokens) {
			delete tok->token;
			delete tok;
		}
		return nullptr;
	}
	for (auto& tok : main_lexer.tokens) {
		delete tok->token;
		delete tok;
	}
	return main_parser.root;
}
