#include "include/lang.h"
#include "include/parser.h"
#include "include/rt.h"

std::map<std::string, std::string> ImportFile;
std::vector<Operator> optable;
int main(int argc, const char** argv) {
	Node* _ast = core::AstCompile(argv[1]);
	if (_ast == nullptr) {
		printf("compilation terminated.\n");
		return 1;
	}
	ast::PrintAST(_ast, 0);
	return 0;
}