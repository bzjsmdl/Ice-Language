#include "lang.h"
namespace lexer {
	bool lexer(char*& text, unsigned long long int& length, const char* srcf) {
		bool error = false;
		// Clear Text
		{
			unsigned long long int line = 1, ls = 0;
			for (unsigned long long int i = 0; i < length; i++) {
				if (text[i] == '\n') {
					line++;
					ls = i;
				}
				if (text[i] == '\"' || text[i] == '\'') {
					char quote = text[i];
					char* chrp = strchr(text + i + 1, quote);
					if (chrp == nullptr) {
						if (quote == '\"') printf("In (%s:%llu:%llu): \n\terror: String not closed.\n", srcf, line, i - ls + 1);
						else printf("In (%s:%llu:%llu): \n\terror: Character not closed.\n", srcf, line, i - ls + 1);
						error = true;
						break;
					}
					else {
						i = chrp - text;
						continue;
					}
				}
				else if (memcmp("//", text + i, 2) == 0) {
					for (; i < length && text[i] != '\n'; i++) text[i] = 0;
					if (text[i] == '\n') {
						line++;
						ls = i;
					}
					continue;
				}
				else if (memcmp("/*", text + i, 2) == 0) {
					while (i < length && memcmp("*/", text + i, 2) != 0) {
						if (text[i] != '\n') text[i] = 0;
						else {
							line++;
							ls = i;
						}
						i++;
					}
					if (i >= length) {
						printf("In (%s:%llu:%llu): \n\terror: Block comment not closed.\n", srcf, line, ((i - ls) + 1));
						error = true;
						break;
					}
					text[i] = 0; text[i + 1] = 0;
				}
			}
		}
		if (!error) { 		// Lexer
			char quote = 0;
			unsigned long long int line = 1, ls = 0, start = 0;
			bool str = false, nl = false;
			char buf[2] = {0};
			Token* tok = nullptr;
			for (unsigned long long int i = 0; i < length; i++) {
				buf[0] = text[i]; buf[1] = text[i + 1];
				if (buf[0] == '\n') {
					line++;
					ls = i + 1;
				}
				if (str) {
					if (buf[0] == '\\' && buf[1] == '\n') nl = true;
					else if (buf[0] == quote) goto s;
					else if (!nl && (buf[0] == '\n' || i + 1 >= length)) {
						printf("In (%s:%llu:%llu):\n\terror: The string misses closing quote.\n", srcf, line, start - ls + 1);
						error = true;
					}
					else if (nl && buf[0] == '\n') nl = false;
					else continue;
				}
				else {
					if (buf[0] == '\'' || buf[0] == '\"') goto s;
					else if (buf[0] < '!') goto clearNC;
					else if (isalpha(buf[0]) || buf[0] == '_') {
						if (isalnum(buf[1]) || buf[1] == '_') continue;
						else goto GetToken;
					}
					else if (isdigit(buf[0]) || buf[0] == '.') {
						char chr = buf[1] & 0b11011111;
						if (isdigit(buf[1]) || buf[1] == '.' || (chr <= 'F' && chr >= 'A') || chr == 'X') continue;
						else goto GetToken;
					}
					else if (ispunct(buf[0])) {
						if (ispunct(buf[1]) &&\
							!(buf[0] == '(' || buf[0] == ')' || buf[0] == '[' || buf[0] == ']' || buf[0] == ':' ||\
							buf[0] == '[' || buf[0] == ']') &&\
							!(buf[1] == '\"' || buf[1] == '\'' || buf[1] == ';' || buf[1] == '_' ||\
							buf[1] == '(' || buf[1] == ')' || buf[1] == '[' || buf[1] == ']' || buf[1] == ':' ||\
							buf[1] == '[' || buf[1] == ']')) continue;
						else goto GetToken;
					}
					else continue;
				}
				GetToken:
					tok = new Token();
					tok->token = new char[i - start + 2]{0};
					strncpy(tok->token, text + start, i - start + 1);
					tok->line = line;
					tok->column = start - ls + 1;
					{		// Check
						if (isdigit(tok->token[0])) {
							tok->type = Number;
							if (memcmp("0b", tok->token, 2) == 0) {
								std::string cpy = tok->token;
								for (unsigned long long int j = 2; j < strlen(tok->token); j++) {
									if (tok->token[j] == '0' || tok->token[j] == '1') continue;
									else {
										error = true;
										cpy.erase(j);
									}
								}
								if (strcmp(cpy.c_str(), tok->token) != 0) {
									if (cpy.empty()) cpy = "0b0";
									printf("In (%s:%llu:%llu):\n\t error: Invalid binary. Do you mean %s?\n", srcf, tok->line, tok->column, cpy.c_str());
								}
							}
							else if (memcmp("0x", tok->token, 2) == 0) {
								std::string cpy = tok->token;
								for (unsigned long long int j = 2; j < strlen(tok->token); j++) {
									if (isxdigit(buf[0])) continue;
									else {
										error = true;
										cpy.erase(j);
									}
								}
								if (strcmp(cpy.c_str(), tok->token) != 0) {
									if (cpy.empty()) cpy = "0x0";
									printf("In (%s:%llu:%llu):\n\t error: Invalid hexadecimal. Do you mean %s?\n", srcf, tok->line, tok->column, cpy.c_str());
								}
							}
							else {
								bool dot = false;
								std::string cpy = tok->token;
								for (unsigned long long int j = 0; j < strlen(tok->token); j++) {
									if (isdigit(tok->token[j]) || (!dot && tok->token[j] == '.')) continue;
									else {
										error = true;
										cpy.erase(j);
									}
								}
								if (strcmp(cpy.c_str(), tok->token) != 0) {
									if (cpy.empty())
										cpy = (dot) ? "0.0" : "0";
									printf("In (%s:%llu:%llu):\n\t error: Invalid number. Do you mean %s?\n", srcf, tok->line, tok->column, cpy.c_str());
								}
							}
						}
						else if (tok->token[0] == '\'' || tok->token[0] == '\"') {
							tok->type = (tok->token[0] == '\'') ? Character : String;
							std::string cpy = tok->token;
							unsigned int chr_max_len = 3;
							if (cpy.find('\\') != cpy.npos) chr_max_len++;
							if (tok->token[0] == '\'' && strlen(tok->token) != chr_max_len) {
								cpy.erase(cpy.begin() + chr_max_len, cpy.begin() + cpy.length());
								cpy[chr_max_len - 1] = '\'';
								error = true;
								printf("In (%s:%llu:%llu):\n\t error: Invalid character %s? Do you mean %s?\n", srcf, tok->line, tok->column, tok->token, cpy.c_str());
							}
						}
						else if (ispunct(tok->token[0])) tok->type = Delimiter;
						else if (IsKeyword(tok->token)) tok->type = Keyword;
						else if (isalpha(tok->token[0]) || tok->token[0] == '_') tok->type = Identifier;
						else {
							tok->type = Nothing;
							printf("In (%s:%llu:%llu): error: Unkown token %s.\n", srcf, tok->line, tok->column, tok->token);
						}
					}
					tokens.push_back(tok);
				clearNC:
					start = i + 1;
					continue;
				s:
					str = !str;
					if (!str) goto GetToken;
					else quote = buf[0];
			}
		}
		return error;
	}
}