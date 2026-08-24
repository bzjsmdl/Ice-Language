#include "lang.h"
namespace rt {
	unsigned long long int flen(FILE* file) {
		fseek(file, 0, SEEK_END);
		unsigned long long int len = ftell(file);
		fseek(file, 0, SEEK_SET);
		return len;
	}
}
