#include "lang.h"
std::vector<Operator> optable;
namespace rt {
	unsigned long long int flen(FILE* file) {
		fseek(file, 0, SEEK_END);
		unsigned long long int len = ftell(file);
		fseek(file, 0, SEEK_SET);
		return len;
	}
	void parser_init() {
		Operator op;

		// bp 100
		op.left = ""; op.right = "."; op.mid = true; op.inner = true; op.overload = false; op.right_binding = false; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "with"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<-"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "as"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "("; op.right = ")"; op.mid = false; op.inner = true; op.overload = false; op.right_binding = false; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "["; op.right = "]"; op.mid = false; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "+"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		op.left = "-"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 100; op.ternary = false;
		optable.push_back(op);

		// bp 90
		op.left = ""; op.right = ":"; op.mid = true; op.inner = true; op.overload = false; op.right_binding = false; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "sizeof"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "++"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "--"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "&"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "*"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "!"; op.right = ""; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = "~"; op.right = ""; op.mid = false; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "++"; op.mid = false; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "--"; op.mid = false; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 90; op.ternary = false;
		optable.push_back(op);

		// bp 80
		op.left = ""; op.right = "&"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "|"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "^"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<<"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">>"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 80; op.ternary = false;
		optable.push_back(op);

		// bp 70
		op.left = ""; op.right = "*"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 70; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "/"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 70; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "%"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 70; op.ternary = false;
		optable.push_back(op);

		// bp 60
		op.left = ""; op.right = "+"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "-"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "=="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "!="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 60; op.ternary = false;
		optable.push_back(op);

		// bp 50
		op.left = ""; op.right = "&&"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 50; op.ternary = false;
		optable.push_back(op);

		// bp 40
		op.left = ""; op.right = "||"; op.mid = true; op.inner = true; op.overload = true; op.right_binding = false; op.binding_power = 40; op.ternary = false;
		optable.push_back(op);

		// bp 30
		op.left = "?"; op.right = ":"; op.mid = false; op.inner = true; op.overload = false; op.right_binding = true; op.binding_power = 30; op.ternary = true;
		optable.push_back(op);

		// bp 20
		op.left = ""; op.right = "="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "+="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "-="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "*="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "/="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "%="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "&="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "|="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "^="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = "<<="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);

		op.left = ""; op.right = ">>="; op.mid = true; op.inner = true; op.overload = true; op.right_binding = true; op.binding_power = 20; op.ternary = false;
		optable.push_back(op);
	}
}
