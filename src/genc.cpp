#include "include/lang.h"

namespace cgen {
    void PrintAST(Node* node, unsigned int dep) {
        if (!node) return;

        for (unsigned int i = 0; i < dep; i++) printf("\t");

        switch (node->type) {
            case Moudle:         printf("Moudle "); break;
            case Path:           printf("Path "); break;
            case Function:       printf("Function "); break;
            case Argument:       printf("Argument "); break;
            case Type:           printf("Type "); break;
            case Statement:      printf("Statement "); break;
            case Identifier:     printf("Identifier "); break;
            case Literal:         printf("Literal "); break;
            case PrefixOp:       printf("PrefixOp "); break;
            case PostfixOp:      printf("PostfixOp "); break;
            case InfixOp:        printf("InfixOp "); break;
            case TernaryOp:      printf("TernaryOp "); break;
            case Expression:     printf("Expression "); break;
            case Body:           printf("Body "); break;
            case Variable:       printf("Variable "); break;
            case Label:          printf("Label "); break;
            case Visibility:     printf("Visibility "); break;
            case Attribute:      printf("Attribute "); break;
            case Class:          printf("Class "); break;
            case Enum:           printf("Enum "); break;
            case Array:          printf("Array "); break;    
            case Tuple:          printf("Tuple "); break;    
            default:                     printf("Unknown ");
        }

        printf("%s\n", node->value.c_str());

        for (auto& cnode : node->child) {
            PrintAST(cnode, dep + 1);
        }
    }
	void FreeAST(Node* node) {
		if (!node) return;

		for (auto& child : node->child) {
			FreeAST(child);
		}

		delete node;
	}
}