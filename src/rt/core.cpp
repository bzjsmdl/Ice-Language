#include "../include/rt.h"
	Node* core::AstCompile(const char* _src) {
		// std::cout << ImportFile.empty() << std::endl;
		FILE* src = fopen(_src, "rb");
		if (src == nullptr) {
			printf("error: input file not found.\n");
			return nullptr;
		}
		unsigned long long int nfo = ftell(src);
		unsigned long long int length = utils::flen(src);
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
			ast::FreeAST(main_parser.root);
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
		// sema::main(main_parser.root);
		return main_parser.root;
	}

	bool core::ReportError(std::string e, std::string src_file, unsigned long long int line, unsigned long long int column, std::string _help) {
		bool _error = true;
		for (auto _if : ImportFile) {
			// Read File
			_if.second.erase(_if.second.begin() + _if.second.find_last_of('/'), _if.second.end());
			std::string error_file = _if.second + "/../data/errors.json";
			FILE* hErr = fopen(error_file.c_str(), "rb");
			if (hErr == nullptr) continue;
			unsigned long long int length = utils::flen(hErr); 
			char* text = new char[length + 1]{0};
			fread(text, 1, length, hErr);
			fclose(hErr);

			// Parse Json
			json ej = json::parse(text);
			delete text;
			if (ej.contains(e)) {
				json error = ej[e];
				if (error.contains("type") && error["type"] == "warning") _error = false;
				if (error.contains("format")) {
					std::string format = error["format"];
					utils::string_replace(format, "{e}", e);
					utils::string_replace(format, "{src_file}", src_file);
					std::string __line = std::to_string(line);
					utils::string_replace(format, "{line}", __line);
					std::string MaxSpaceLen;
					unsigned long long int line_length = __line.length() + 1;
					do {
						MaxSpaceLen.clear();
						for (unsigned long long int i = 0; i < line_length; i++) MaxSpaceLen.append(" ");
						utils::string_replace(format, "{MaxSpaceLen}", MaxSpaceLen);
					} while (utils::string_replace(format, "{MaxSpaceLen}", MaxSpaceLen));
					utils::string_replace(format, "{col}", std::to_string(column));

					std::string _line = utils::GetLineText(src_file, line);
					utils::string_replace(format, "{current_line}", _line);

					std::string tmp = (error.contains("->col")) ? error["->col"] : "$5$Sr^^^$r";
					unsigned long long int ntl = (column - utils::GetColorStringLength(tmp) - line_length);
					for (unsigned long long int i = 0; i < ntl; i++) {
						tmp.insert(tmp.begin(), ' ');
					}
					utils::string_replace(format, "{->col}", tmp);

					std::string help = _help;
					for (unsigned long long int i = 0; i < ntl; i++) {
						help.insert(help.begin(), ' ');
					}
					utils::string_replace(format, "{help:col}", help);

					fmt::print(format);
					return _error;
				}
			}		
		}
		printf("%s:%llu:%llu error: %llu %s\n", src_file.c_str(), line, column, e, _help.c_str());
		return _error;
	}