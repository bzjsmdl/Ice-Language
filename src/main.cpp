#include "lang.h"

int main(int argc, const char** argv) {
	bool error = false;
	if (argc < 3) {
		printf("error: input file and output file not found.\n");
		printf("compilation terminated.\n");
		return 1;
	}
	FILE* src = fopen(argv[1], "rb");
	if (src == nullptr) {
		printf("error: input file not found.\n");
		printf("compilation terminated.\n");
		return 1;
	}
	unsigned long long int nfo = ftell(src);
	unsigned long long int length = rt::flen(src);
	char* text = new char[length + 1]{0};
	fread(text, 1, length, src);
	fseek(src, nfo, SEEK_SET);
	fclose(src);
	if (lexer::lexer(text, length, argv[1])) {
		for (auto& tok : lexer::tokens) {
			delete tok->token;
			delete tok;
		}
		delete text;
		printf("compilation terminated.\n");
		return 1;
	}
	delete text;
	// for (auto& tok : lexer::tokens) printf("Token %s of %u at (%s:%llu:%llu)\n", tok->token, tok->type, argv[1], tok->line, tok->column);
	if (parser::main(lexer::tokens, lexer::tokens.size(), argv[1])) {
		printf("compilation terminated.\n");
		return 1;
	}
	for (auto& tok : lexer::tokens) {
		delete tok->token;
		delete tok;
	}
	cgen::PrintAST(parser::root, 0);
	FILE* out = fopen(argv[2], "wb");
	cgen::main(parser::root, out);
	fclose(out);
	return 0;
}