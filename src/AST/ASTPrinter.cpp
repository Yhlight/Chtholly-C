#include "ASTPrinter.h"
#include <sstream>

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        out << std::any_cast<std::string>(stmt->accept(*this)) << std::endl;
    }
    return out.str();
}

std::any ASTPrinter::visit(const NumericLiteral& expr) {
    return expr.value.lexeme;
}

std::any ASTPrinter::visit(const LetStmt& stmt) {
    std::stringstream out;
    out << "(let " << stmt.name.lexeme;
    if (stmt.initializer) {
        out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
    }
    out << ")";
    return out.str();
}
