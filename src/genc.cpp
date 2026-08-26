#include "lang.h"
using namespace parser;

namespace cgen {

    std::string MapType(const std::string& ice_type) {
        if (ice_type == "int8")   return "int8_t";
        if (ice_type == "int16")  return "int16_t";
        if (ice_type == "int32")  return "int32_t";
        if (ice_type == "int64")  return "int64_t";
        if (ice_type == "usize")  return "size_t";
        if (ice_type == "bool")   return "bool";
        if (ice_type == "float32") return "float";
        if (ice_type == "float64") return "double";
        if (ice_type == "char")   return "char";
        if (ice_type == "string") return "std::string";
        if (ice_type == "void")   return "void";
        return ice_type + "_t";
    }

    std::string GetTypeString(Node* type_node, bool is_const = false) {
        if (!type_node) return "void";

        std::string base = type_node->value;
        bool const_flag = is_const;

        // 检查 const 子节点
        for (auto& c : type_node->child) {
            if (c->type == Type && c->value == "const") {
                const_flag = true;
            }
        }

        // 检查数组子节点
        for (auto& c : type_node->child) {
            if (c->type == Type && c->value == "[ ]") {
                std::string arr_size = "0";
                if (!c->child.empty() && c->child[0]->type == Number) {
                    arr_size = c->child[0]->value;
                }
                std::string result = "std::array<" + MapType(base) + ", " + arr_size + ">";
                return (const_flag ? "const " + result : result);
            }
        }

        std::string result = MapType(base);
        return (const_flag ? "const " + result : result);
    }

    void main(parser::Node* node, FILE* out) {
        if (!node) return;

        switch (node->type) {

            case Program: {
                fprintf(out, "#include <bits/stdc++.h>\n");
                fprintf(out, "using namespace std;\n");
                for (auto& c : node->child) main(c, out);
                break;
            }

            case Function: {
                std::string name, ret_type;
                Node* body = nullptr;

                for (auto& c : node->child) {
                    if (c->type == Identifier) name = c->value;
                    else if (c->type == Type) ret_type = GetTypeString(c);
                    else if (c->type == Body) body = c;
                }

                fprintf(out, "%s %s(", ret_type.c_str(), name.c_str());

                bool first_param = true;
                for (auto& c : node->child) {
                    if (c->type == Argument) {
                        std::string pname, ptype;
                        for (auto& a : c->child) {
                            if (a->type == Identifier) pname = a->value;
                            else if (a->type == Type) ptype = GetTypeString(a);
                        }
                        if (!first_param) fprintf(out, ", ");
                        fprintf(out, "%s %s", ptype.c_str(), pname.c_str());
                        first_param = false;
                    }
                }

                fprintf(out, ")\n");
                main(body, out);
                break;
            }

            case Body: {
                fprintf(out, "{\n");
                for (auto& c : node->child) main(c, out);
                fprintf(out, "}\n");
                break;
            }

            case Variable: {
                std::string name, type;
                Node* init = nullptr;

                for (auto& c : node->child) {
                    if (c->type == Identifier) name = c->value;
                    else if (c->type == Type) type = GetTypeString(c);
                    else init = c;
                }

                fprintf(out, "%s %s", type.c_str(), name.c_str());
                if (init) {
                    fprintf(out, " = ");
                    main(init, out);
                }
                fprintf(out, ";\n");
                break;
            }

            case Statement: {
                if (strequ(node->value.c_str(), "return")) {
                    fprintf(out, "return");
                    if (!node->child.empty()) {
                        fprintf(out, " ");
                        main(node->child[0], out);
                    }
                    fprintf(out, ";\n");
                }
                else if (strequ(node->value.c_str(), "if")) {
                    if (node->child.size() >= 2) {
                        fprintf(out, "if (");
                        main(node->child[0], out);
                        fprintf(out, ")\n");

                        if (node->child[1]->type == Body) {
                            main(node->child[1], out);
                        } else {
                            fprintf(out, "{\n");
                            main(node->child[1], out);
                            fprintf(out, "}\n");
                        }

                        if (node->child.size() >= 3 && node->child[2]) {
                            fprintf(out, "else ");
                            if (node->child[2]->type == Statement &&
                                strequ(node->child[2]->value.c_str(), "if")) {
                                main(node->child[2], out);
                            }
                            else if (node->child[2]->type == Body) {
                                main(node->child[2], out);
                            }
                            else {
                                fprintf(out, "{\n");
                                main(node->child[2], out);
                                fprintf(out, "}\n");
                            }
                        }
                    }
                }
                else if (strequ(node->value.c_str(), "while")) {
                    if (node->child.size() >= 2) {
                        fprintf(out, "while (");
                        main(node->child[0], out);
                        fprintf(out, ")\n");

                        if (node->child[1]->type == Body) {
                            main(node->child[1], out);
                        } else {
                            fprintf(out, "{\n");
                            main(node->child[1], out);
                            fprintf(out, "}\n");
                        }
                    }
                }
                else if (strequ(node->value.c_str(), "expr")) {
                    if (!node->child.empty()) {
                        main(node->child[0], out);
                    }
                    fprintf(out, ";\n");
                }
                break;
            }

            case Number:
            case Character:
            case String:
            case IdentifierData:
            case InnerData: {
                fprintf(out, "%s", node->value.c_str());
                break;
            }

            case PrefixOp: {
                if (node->child.empty()) break;
                fprintf(out, "(%s", node->value.c_str());
                main(node->child[0], out);
                fprintf(out, ")");
                break;
            }

            case PostfixOp: {
                if (node->child.empty()) break;
                fprintf(out, "(");
                main(node->child[0], out);
                fprintf(out, ")%s", node->value.c_str());
                break;
            }

            case InfixOp: {
                if (node->child.size() < 2) break;
                fprintf(out, "(");
                main(node->child[0], out);
                fprintf(out, " %s ", node->value.c_str());
                main(node->child[1], out);
                fprintf(out, ")");
                break;
            }

            case TernaryOp: {
                if (node->child.size() < 3) break;
                fprintf(out, "(");
                main(node->child[0], out);
                fprintf(out, " ? ");
                main(node->child[1], out);
                fprintf(out, " : ");
                main(node->child[2], out);
                fprintf(out, ")");
                break;
            }

            case Expression: {
                if (node->value == "call" && !node->child.empty()) {
                    main(node->child[0], out);
                    fprintf(out, "(");
                    for (size_t i = 1; i < node->child.size(); i++) {
                        if (i > 1) fprintf(out, ", ");
                        main(node->child[i], out);
                    }
                    fprintf(out, ")");
                }
                else if (node->value == "[ ]" && node->child.size() >= 2) {
                    main(node->child[0], out);
                    fprintf(out, "[");
                    main(node->child[1], out);
                    fprintf(out, "]");
                }
                else if (node->value == "( )") {
                    fprintf(out, "(");
                    if (!node->child.empty()) main(node->child[0], out);
                    fprintf(out, ")");
                }
                break;
            }

            default:
                break;
        }
    }

    void PrintAST(parser::Node* node, unsigned int dep) {
        if (!node) return;

        for (unsigned int i = 0; i < dep; i++) printf("\t");

        switch (node->type) {
            case parser::Program:        printf("Program "); break;
            case parser::Function:       printf("Function "); break;
            case parser::Argument:       printf("Argument "); break;
            case parser::Type:           printf("Type "); break;
            case parser::Statement:      printf("Statement "); break;
            case parser::Number:         printf("Number "); break;
            case parser::Character:      printf("Character "); break;
            case parser::String:         printf("String "); break;
            case parser::Identifier:     printf("Identifier "); break;
            case parser::IdentifierData: printf("Identifier_Data "); break;
            case parser::InnerData:      printf("InnerData "); break;
            case parser::PrefixOp:       printf("PrefixOp "); break;
            case parser::PostfixOp:      printf("PostfixOp "); break;
            case parser::InfixOp:        printf("InfixOp "); break;
            case parser::TernaryOp:      printf("TernaryOp "); break;
            case parser::Expression:     printf("Expression "); break;
            case parser::Body:           printf("Body "); break;
            case parser::Variable:       printf("Variable "); break;
            case parser::Label: printf("Label "); break;
            default:                     printf("Unknown ");
        }

        printf("%s\n", node->value.c_str());

        for (auto& cnode : node->child) {
            PrintAST(cnode, dep + 1);
        }
    }

}