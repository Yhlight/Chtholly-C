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
    }
}

void CodeGen::visit(ExpressionStatement* node) {
    visit(node->expression.get());
    m_out << ";\n";
}

void CodeGen::visit(LetStatement* node) {
    m_out << "auto " << node->name->value << " = ";
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

} // namespace Chtholly
