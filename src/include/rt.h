#pragma once

#include "parser.h"

using json = nlohmann::json;
extern std::map<std::string, std::string> ImportFile;
extern std::vector<Operator> optable;

namespace core {
    void parser_init();
    Node* AstCompile(const char* _src);
    bool ReportError(std::string e, std::string src_file, unsigned long long int line, unsigned long long int column, std::string _help);
}

namespace ast {
    void PrintAST(Node* node, unsigned int dep);
    void FreeAST(Node* node);
}

namespace utils {
	unsigned long long int flen(FILE* file);
	std::string unix_path(std::string path);
	bool string_replace(std::string& self, std::string _old, std::string _new);
	std::string GetLineText(std::string src_file, unsigned long long int line);
	size_t GetColorStringLength(std::string s);
}


#ifdef __WIN32__
	#include <windows.h>
	inline void EnableAnsiOnWindows() {
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		if (out == INVALID_HANDLE_VALUE) return;
		DWORD mode = 0;
		if (!GetConsoleMode(out, &mode)) return;
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(out, mode);
	}
#endif

namespace fmt {
	void print(std::string __msg);
}