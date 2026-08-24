#include "lang.h"

int main(int argc, const char** argv) {
	bool error = false;
	if (argc < 3) {
		printf("error: Input file and output file not found.\n");
		return 1;
	}
	FILE* src = fopen(argv[1], "rb");
	if (src == nullptr) {
		printf("error: Input file not found.\n");
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
		return 1;
	}
	delete text;
	if (parser::main(lexer::tokens, lexer::tokens.size(), argv[1])) {
		for (auto& tok : lexer::tokens) {
			delete tok->token;
			delete tok;
		}
		return 1;
	}
	PrintAST(parser::root, 0);
	FILE* out = fopen(argv[2], "wb");
	cgen::main(parser::root, out);
	fclose(out);
	return 0;
}