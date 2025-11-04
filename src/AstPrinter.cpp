#include "AstPrinter.h"
#include <sstream>

namespace chtholly {

std::string AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        out << std::any_cast<std::string>(stmt->accept(*this)) << std::endl;
    }
    return out.str();
}

std::any AstPrinter::visitBinaryExpr(BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

std::any AstPrinter::visitUnaryExpr(UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()});
}

std::any AstPrinter::visitLiteralExpr(LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    } else if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return std::string("nil");
}

std::any AstPrinter::visitVariableExpr(VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any AstPrinter::visitCallExpr(CallExpr& expr) {
    std::stringstream out;
    out << "(" << std::any_cast<std::string>(expr.callee->accept(*this));
    for (const auto& arg : expr.arguments) {
        out << " " << std::any_cast<std::string>(arg->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitExpressionStmt(ExpressionStmt& stmt) {
    return std::any_cast<std::string>(stmt.expression->accept(*this));
}

std::any AstPrinter::visitBlockStmt(BlockStmt& stmt) {
    std::stringstream out;
    out << "(block";
    for (const auto& statement : stmt.statements) {
        out << " " << std::any_cast<std::string>(statement->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitVarStmt(VarStmt& stmt) {
    std::stringstream out;
    out << "(" << stmt.keyword.lexeme << " " << stmt.name.lexeme;
    if (stmt.type) {
        out << " : " << std::any_cast<std::string>(stmt.type->accept(*this));
    }
    if (stmt.initializer) {
        out << " = " << std::any_cast<std::string>(stmt.initializer->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitBaseTypeExpr(BaseTypeExpr& expr) {
    return expr.name.lexeme;
}

std::any AstPrinter::visitFunctionStmt(FunctionStmt& stmt) {
    std::stringstream out;
    out << "(func " << stmt.name.lexeme << " (";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out << stmt.params[i].lexeme;
        if (i < stmt.params.size() - 1) out << " ";
    }
    out << ")";
    if (stmt.returnType) {
        out << " -> " << std::any_cast<std::string>(stmt.returnType->accept(*this));
    }
    out << " " << std::any_cast<std::string>(stmt.body->accept(*this)) << ")";
    return out.str();
}

std::any AstPrinter::visitReturnStmt(ReturnStmt& stmt) {
    std::stringstream out;
    out << "(return";
    if (stmt.value) {
        out << " " << std::any_cast<std::string>(stmt.value->accept(*this));
    }
    out << ")";
    return out.str();
}

std::string AstPrinter::parenthesize(const std::string& name, const std::vector<Expr*>& exprs) {
    std::stringstream out;
    out << "(" << name;
    for (Expr* expr : exprs) {
        out << " ";
        out << std::any_cast<std::string>(expr->accept(*this));
    }
    out << ")";
    return out.str();
}

} // namespace chtholly
