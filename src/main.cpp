#include "lang.h"

std::map<std::string, std::string> ImportFile;
std::string MainMoudle;
int main(int argc, const char** argv) {
	Node* ast = AstCompile(argv[1]);
	if (ast == nullptr) {
		printf("compilation terminated.\n");
		return 1;
	}
	cgen::PrintAST(ast, 0);
	return 0;
}