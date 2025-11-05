#include "AstPrinter.h"
#include <sstream>

namespace chtholly {

template<typename... Args>
std::string AstPrinter::parenthesize(const std::string& name, Args... args) {
    std::stringstream out;
    out << "(" << name;
    ((out << " " << std::any_cast<std::string>(args->accept(*this))), ...);
    out << ")";
    return out.str();
}

std::string AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    std::stringstream out;
    for (const auto& stmt : statements) {
        out << std::any_cast<std::string>(stmt->accept(*this));
    }
    return out.str();
}

std::any AstPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, expr.left.get(), expr.right.get());
}

std::any AstPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, expr.right.get());
}

std::any AstPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::nullptr_t>(expr.value)) {
        return std::string("nil");
    }
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    if (std::holds_alternative<long long>(expr.value)) {
        return std::to_string(std::get<long long>(expr.value));
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    if (std::holds_alternative<char>(expr.value)) {
        return std::string(1, std::get<char>(expr.value));
    }
    return std::string("unknown literal");
}

std::any AstPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return parenthesize("group", expr.expression.get());
}

std::any AstPrinter::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any AstPrinter::visitAssignExpr(const AssignExpr& expr) {
    return parenthesize("= " + expr.name.lexeme, expr.value.get());
}

std::any AstPrinter::visitCallExpr(const CallExpr& expr) {
    std::stringstream out;
    out << "(call " << std::any_cast<std::string>(expr.callee->accept(*this));
    for (const auto& arg : expr.arguments) {
        out << " " << std::any_cast<std::string>(arg->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitLambdaExpr(const LambdaExpr& expr) {
    return std::string("(lambda)");
}

std::any AstPrinter::visitGetExpr(const GetExpr& expr) {
    return parenthesize("." + expr.name.lexeme, expr.object.get());
}

std::any AstPrinter::visitSetExpr(const SetExpr& expr) {
    return parenthesize("= ." + expr.name.lexeme, expr.object.get(), expr.value.get());
}

std::any AstPrinter::visitSelfExpr(const SelfExpr& expr) {
    return std::string("self");
}

std::any AstPrinter::visitBorrowExpr(const BorrowExpr& expr) {
    return parenthesize(expr.isMutable ? "&mut" : "&", expr.expression.get());
}

std::any AstPrinter::visitDerefExpr(const DerefExpr& expr) {
    return parenthesize("*", expr.expression.get());
}

std::any AstPrinter::visitBlockStmt(const BlockStmt& stmt) {
    std::stringstream out;
    out << "(block";
    for (const auto& s : stmt.statements) {
        out << " " << std::any_cast<std::string>(s->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    return parenthesize(";", stmt.expression.get());
}

std::any AstPrinter::visitFunctionStmt(const FunctionStmt& stmt) {
    return parenthesize("func " + stmt.name.lexeme, stmt.body.get());
}

std::any AstPrinter::visitIfStmt(const IfStmt& stmt) {
    if (stmt.elseBranch) {
        return parenthesize("if-else", stmt.condition.get(), stmt.thenBranch.get(), stmt.elseBranch.get());
    }
    return parenthesize("if", stmt.condition.get(), stmt.thenBranch.get());
}

std::any AstPrinter::visitVarStmt(const VarStmt& stmt) {
    if (stmt.initializer) {
        return parenthesize((stmt.isMutable ? "mut " : "let ") + stmt.name.lexeme, stmt.initializer.get());
    }
    return "(" + std::string(stmt.isMutable ? "mut " : "let ") + stmt.name.lexeme + ")";
}

std::any AstPrinter::visitWhileStmt(const WhileStmt& stmt) {
    return parenthesize("while", stmt.condition.get(), stmt.body.get());
}

std::any AstPrinter::visitReturnStmt(const ReturnStmt& stmt) {
    if (stmt.value) {
        return parenthesize("return", stmt.value.get());
    }
    return std::string("(return)");
}

std::any AstPrinter::visitStructStmt(const StructStmt& stmt) {
    std::stringstream out;
    out << "(struct " << stmt.name.lexeme;
    for (const auto& field : stmt.fields) {
        out << " " << std::any_cast<std::string>(field->accept(*this));
    }
    for (const auto& method : stmt.methods) {
        out << " " << std::any_cast<std::string>(method->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitImportStmt(const ImportStmt& stmt) {
    if (std::holds_alternative<std::string>(stmt.path)) {
        LiteralExpr literal(std::get<std::string>(stmt.path));
        return parenthesize("import", &literal);
    }
    VariableExpr var(std::get<Token>(stmt.path));
    return parenthesize("import", &var);
}

std::any AstPrinter::visitSwitchStmt(const SwitchStmt& stmt) {
    std::stringstream out;
    out << "(switch";
    for (const auto& c : stmt.cases) {
        out << " " << std::any_cast<std::string>(c->accept(*this));
    }
    out << ")";
    return out.str();
}

std::any AstPrinter::visitCaseStmt(const CaseStmt& stmt) {
    return parenthesize("case", stmt.value.get(), stmt.body.get());
}

std::any AstPrinter::visitBreakStmt(const BreakStmt& stmt) {
    return std::string("(break)");
}

std::any AstPrinter::visitFallthroughStmt(const FallthroughStmt& stmt) {
    return std::string("(fallthrough)");
}

} // namespace chtholly
