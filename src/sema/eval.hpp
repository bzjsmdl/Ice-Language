#ifndef ICE_SEMA_EXPR_H
#define ICE_SEMA_EXPR_H
#include "../include/lang.h"
#define CPP_IS_INT_TYPE
namespace sema {
	std::string eval(Node* node) {
		switch (node->type) {
			case Literal:
			case InfixOp:
			case PostfixOp:
			case PrefixOp:
			default:
				error = false;
				break;
		}
	}
}
#endif