#include "AstPrinter.h"

namespace chtholly {

std::string AstPrinter::print(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::string AstPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any AstPrinter::visit(const BinaryExpr& expr) {
    return std::string(""); // To be implemented
}

std::any AstPrinter::visit(const GroupingExpr& expr) {
    return std::string(""); // To be implemented
}

std::any AstPrinter::visit(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    return std::string("nil");
}

std::any AstPrinter::visit(const UnaryExpr& expr) {
    return std::string(""); // To be implemented
}

std::any AstPrinter::visit(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any AstPrinter::visit(const ExpressionStmt& stmt) {
    return print(*stmt.expression);
}

std::any AstPrinter::visit(const VarDeclStmt& stmt) {
    std::string s = "(var " + stmt.name.lexeme;
    if (stmt.initializer) {
        s += " = " + print(*stmt.initializer);
    }
    s += ")";
    return s;
}

std::any AstPrinter::visit(const BlockStmt& stmt) {
    std::string s = "(block";
    for(const auto& statement : stmt.statements) {
        s += " " + print(*statement);
    }
    s += ")";
    return s;
}

} // namespace chtholly
