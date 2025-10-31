#include "CodeGen.h"

namespace Chtholly {

std::string CodeGen::generate(Program* program) {
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
    }
}

void CodeGen::visit(ExpressionStatement* node) {
    visit(node->expression.get());
    if (!dynamic_cast<IfExpression*>(node->expression.get()) && !dynamic_cast<FunctionLiteral*>(node->expression.get())) {
        m_out << ";\n";
    }
}

void CodeGen::visit(LetStatement* node) {
    if (node->type) {
        visit(node->type.get());
    } else {
        m_out << "auto";
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

void CodeGen::visit(Boolean* node) {
    m_out << (node->value ? "true" : "false");
}

void CodeGen::visit(PrefixExpression* node) {
    m_out << "(" << node->op;
    visit(node->right.get());
    m_out << ")";
}

void CodeGen::visit(InfixExpression* node) {
    m_out << "(";
    visit(node->left.get());
    m_out << " " << node->op << " ";
    visit(node->right.get());
    m_out << ")";
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
}

} // namespace Chtholly
