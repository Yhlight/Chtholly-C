#include "CodeGen.h"

namespace Chtholly {

std::string CodeGen::generate(Program* program) {
    m_program = program;
    for (const auto& stmt : program->statements) {
        visit(stmt.get());
    }
    return m_out.str();
}

void CodeGen::visit(Node* node) {
    if (auto n = dynamic_cast<ExpressionStatement*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<LetStatement*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<ReturnStatement*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<Identifier*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<IntegerLiteral*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<StringLiteral*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<Boolean*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<PrefixExpression*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<InfixExpression*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<BlockStatement*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<IfExpression*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<FunctionLiteral*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<CallExpression*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<TypeName*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<StructStatement*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<GenericInstantiation*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<StructLiteral*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<MemberAccessExpression*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<EnumStatement*>(node)) {
        visit(n);
    } else if (auto n = dynamic_cast<ImportStatement*>(node)) {
        visit(n);
    }
}

void CodeGen::visit(ExpressionStatement* node) {
    visit(node->expression.get());
    if (!dynamic_cast<IfExpression*>(node->expression.get()) && !dynamic_cast<FunctionLiteral*>(node->expression.get())) {
        m_out << ";\n";
    }
}

void CodeGen::visit(LetStatement* node) {
    if (node->isMutable) {
        if (node->type) {
            visit(node->type.get());
        } else {
            m_out << "auto";
        }
    } else {
        if (node->type) {
            m_out << "const ";
            visit(node->type.get());
        } else {
            m_out << "const auto";
        }
    }
    m_out << " " << node->name->value << " = ";
    visit(node->value.get());
    m_out << ";\n";
}

void CodeGen::visit(ReturnStatement* node) {
    m_out << "return ";
    visit(node->returnValue.get());
    m_out << ";\n";
}

void CodeGen::visit(Identifier* node) {
    m_out << node->value;
}

void CodeGen::visit(IntegerLiteral* node) {
    m_out << node->value;
}

void CodeGen::visit(StringLiteral* node) {
    m_out << "\"" << node->value << "\"";
}

void CodeGen::visit(Boolean* node) {
    m_out << (node->value ? "true" : "false");
}

void CodeGen::visit(PrefixExpression* node) {
    m_out << "(" << node->op;
    visit(node->right.get());
    m_out << ")";
}

void CodeGen::visit(InfixExpression* node) {
    if (node->op == "::") {
        visit(node->left.get());
        m_out << "::";
        visit(node->right.get());
    } else {
        m_out << "(";
        visit(node->left.get());
        m_out << " " << node->op << " ";
        visit(node->right.get());
        m_out << ")";
    }
}

void CodeGen::visit(BlockStatement* node) {
    m_out << "{\n";
    for (const auto& stmt : node->statements) {
        visit(stmt.get());
    }
    m_out << "}\n";
}

void CodeGen::visit(IfExpression* node) {
    m_out << "if (";
    visit(node->condition.get());
    m_out << ") ";
    visit(node->consequence.get());
    if (node->alternative) {
        m_out << "else ";
        visit(node->alternative.get());
    }
}

void CodeGen::visit(FunctionLiteral* node) {
    if (!node->templateParams.empty()) {
        m_out << "template<";
        for (size_t i = 0; i < node->templateParams.size(); ++i) {
            m_out << "typename " << node->templateParams[i]->value;
            if (i < node->templateParams.size() - 1) {
                m_out << ", ";
            }
        }
        m_out << ">\n";
    }
    m_out << "[&](";
    for (size_t i = 0; i < node->parameters.size(); ++i) {
        if (node->parameters[i]->type) {
            visit(node->parameters[i]->type.get());
        } else {
            m_out << "auto";
        }
        m_out << " " << node->parameters[i]->value;
        if (i < node->parameters.size() - 1) {
            m_out << ", ";
        }
    }
    m_out << ") ";
    if (node->returnType) {
        m_out << "-> ";
        visit(node->returnType.get());
        m_out << " ";
    }
    visit(node->body.get());
}

void CodeGen::visit(CallExpression* node) {
    visit(node->function.get());
    if (!node->templateArgs.empty()) {
        m_out << "<";
        for (size_t i = 0; i < node->templateArgs.size(); ++i) {
            visit(node->templateArgs[i].get());
            if (i < node->templateArgs.size() - 1) {
                m_out << ", ";
            }
        }
        m_out << ">";
    }
    m_out << "(";
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        visit(node->arguments[i].get());
        if (i < node->arguments.size() - 1) {
            m_out << ", ";
        }
    }
    m_out << ")";
}

void CodeGen::visit(TypeName* node) {
    m_out << node->name;
    if (!node->templateArgs.empty()) {
        m_out << "<";
        for (size_t i = 0; i < node->templateArgs.size(); ++i) {
            visit(node->templateArgs[i].get());
            if (i < node->templateArgs.size() - 1) {
                m_out << ", ";
            }
        }
        m_out << ">";
    }
}

void CodeGen::visit(StructStatement* node) {
    if (!node->templateParams.empty()) {
        m_out << "template<";
        for (size_t i = 0; i < node->templateParams.size(); ++i) {
            m_out << "typename " << node->templateParams[i]->value;
            if (i < node->templateParams.size() - 1) {
                m_out << ", ";
            }
        }
        m_out << ">\n";
    }
    m_out << "struct " << node->name->value << " {\n";
    for (const auto& member : node->members) {
        if (auto field = dynamic_cast<Field*>(member.get())) {
            visit(field);
        } else if (auto method = dynamic_cast<Method*>(member.get())) {
            visit(method);
        }
    }
    m_out << "};\n";
}

void CodeGen::visit(Field* node) {
    if (node->isPublic) {
        m_out << "public: ";
    } else {
        m_out << "private: ";
    }
    visit(node->type.get());
    m_out << " " << node->name->value << ";\n";
}

void CodeGen::visit(Method* node) {
    if (node->isPublic) {
        m_out << "public: ";
    } else {
        m_out << "private: ";
    }
    m_out << "auto " << node->name->value << " = ";
    visit(node->function.get());
    m_out << ";\n";
}

void CodeGen::visit(GenericInstantiation* node) {
    visit(node->base.get());
    m_out << "<";
    for (size_t i = 0; i < node->arguments.size(); ++i) {
        visit(node->arguments[i].get());
        if (i < node->arguments.size() - 1) {
            m_out << ", ";
        }
    }
    m_out << ">";
}

void CodeGen::visit(StructLiteral* node) {
    visit(node->structName.get());
    m_out << "{";

    StructStatement* structDef = nullptr;
    std::string structName;

    if (auto genInst = dynamic_cast<GenericInstantiation*>(node->structName.get())) {
        if (auto ident = dynamic_cast<Identifier*>(genInst->base.get())) {
            structName = ident->value;
        }
    } else if (auto ident = dynamic_cast<Identifier*>(node->structName.get())) {
        structName = ident->value;
    }

    for (const auto& stmt : m_program->statements) {
        if (auto s = dynamic_cast<StructStatement*>(stmt.get())) {
            if (s->name->value == structName) {
                structDef = s;
                break;
            }
        }
    }

    if (structDef) {
        std::vector<std::string> fieldOrder;
        for (const auto& member : structDef->members) {
            if (auto field = dynamic_cast<Field*>(member.get())) {
                fieldOrder.push_back(field->name->value);
            }
        }

        for (size_t i = 0; i < fieldOrder.size(); ++i) {
            for (const auto& fieldInit : node->fields) {
                if (fieldInit.first->value == fieldOrder[i]) {
                    visit(fieldInit.second.get());
                    if (i < fieldOrder.size() - 1) {
                        m_out << ", ";
                    }
                    break;
                }
            }
        }
    }

    m_out << "}";
}

void CodeGen::visit(MemberAccessExpression* node) {
    visit(node->object.get());
    m_out << ".";
    visit(node->member.get());
}

void CodeGen::visit(EnumStatement* node) {
    m_out << "enum class " << node->name->value << " {\n";
    for (size_t i = 0; i < node->members.size(); ++i) {
        m_out << "    " << node->members[i]->value;
        if (i < node->members.size() - 1) {
            m_out << ",\n";
        }
    }
    m_out << "\n};\n";
}

void CodeGen::visit(ImportStatement* node) {
    if (auto ident = dynamic_cast<Identifier*>(node->path.get())) {
        if (ident->value == "iostream") {
            std::string existing_code = m_out.str();
            m_out.str("");
            m_out << "#include <iostream>\n\n";
            m_out << "template<typename T>\n";
            m_out << "void print(T value) {\n";
            m_out << "    std::cout << value << std::endl;\n";
            m_out << "}\n\n";
            m_out << existing_code;
        } else if (ident->value == "filesystem") {
            std::string existing_code = m_out.str();
            m_out.str("");
            m_out << "#include <filesystem>\n";
            m_out << "#include <fstream>\n\n";
            m_out << "namespace filesystem {\n";
            m_out << "    bool exists(const std::string& path) { return std::filesystem::exists(path); }\n";
            m_out << "    bool is_file(const std::string& path) { return std::filesystem::is_regular_file(path); }\n";
            m_out << "    bool is_directory(const std::string& path) { return std::filesystem::is_directory(path); }\n";
            m_out << "    std::string read_file(const std::string& path) {\n";
            m_out << "        std::ifstream file(path);\n";
            m_out << "        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());\n";
            m_out << "        return content;\n";
            m_out << "    }\n";
            m_out << "    bool write_file(const std::string& path, const std::string& content) {\n";
            m_out << "        std::ofstream file(path);\n";
            m_out << "        file << content;\n";
            m_out << "        return file.good();\n";
            m_out << "    }\n";
            m_out << "}\n\n";
            m_out << existing_code;
        }
    }
}

} // namespace Chtholly
