#include "include/lang.h"

std::map<std::string, std::string> ImportFile;
std::vector<Operator> optable;
int main(int argc, const char** argv) {
	Node* ast = AstCompile(argv[1]);
	if (ast == nullptr) {
		printf("compilation terminated.\n");
		return 1;
	}
	ast::PrintAST(ast, 0);
	return 0;
}