#pragma once
#include "../include/rt.h"
#include "../rt/core.hpp"
#include "../rt/fmt.hpp"
#include "../rt/op.hpp"

namespace utils {
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
	bool string_replace(std::string& self, std::string _old, std::string _new) {
		if (self.find(_old) != self.npos) {
			auto start = self.begin() + self.find(_old);
			self.erase(start, start + _old.length());
			self.insert(start - self.begin(), _new);
			return true;
		}
		return false;
	}
	std::string GetLineText(std::string src_file, unsigned long long int line) {
		std::ifstream _ifs = std::ifstream(src_file.c_str());
		std::string current_line;
		for (unsigned long long int i = 0; i < line; i++) std::getline(_ifs, current_line);
		return current_line;
	}
	size_t GetColorStringLength(std::string s) {
		unsigned long long int result = s.length();
		for (unsigned long long int i = 0; i < s.length(); i++) {
			if (s[i] == '$') result -= 2;
			else if (s[i] == '\\') result -= 2;
		}
		return result;
	}
}
