#include "AstPrinter.h"
#include <sstream>

namespace chtholly {

std::string AstPrinter::print(const std::vector<std::shared_ptr<Stmt>>& statements) {
    std::stringstream ss;
    for (const auto& stmt : statements) {
        ss << std::any_cast<std::string>(stmt->accept(*this));
    }
    return ss.str();
}

std::string AstPrinter::print(std::shared_ptr<Expr> expr) {
    return std::any_cast<std::string>(expr->accept(*this));
}

std::string AstPrinter::print(std::shared_ptr<Stmt> stmt) {
    return std::any_cast<std::string>(stmt->accept(*this));
}

std::any AstPrinter::visitAssignExpr(std::shared_ptr<AssignExpr> expr) {
    return parenthesize("= " + expr->name.lexeme, {expr->value});
}

std::any AstPrinter::visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any AstPrinter::visitCallExpr(std::shared_ptr<CallExpr> expr) {
    return parenthesize("call " + std::any_cast<std::string>(expr->callee->accept(*this)), expr->arguments);
}

std::any AstPrinter::visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) {
    return parenthesize("group", {expr->expression});
}

std::any AstPrinter::visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) {
    if (expr->value.valueless_by_exception()) {
        return "nil";
    }
    if (auto pval = std::get_if<std::string>(&expr->value)) {
        return *pval;
    }
    if (auto pval = std::get_if<double>(&expr->value)) {
        return std::to_string(*pval);
    }
    if (auto pval = std::get_if<bool>(&expr->value)) {
        return *pval ? "true" : "false";
    }
    return "nil";
}

std::any AstPrinter::visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) {
    return parenthesize(expr->op.lexeme, {expr->left, expr->right});
}

std::any AstPrinter::visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) {
    return parenthesize(expr->op.lexeme, {expr->right});
}

std::any AstPrinter::visitVariableExpr(std::shared_ptr<VariableExpr> expr) {
    return expr->name.lexeme;
}

std::any AstPrinter::visitBlockStmt(std::shared_ptr<BlockStmt> stmt) {
    std::stringstream ss;
    ss << "(block ";
    for (const auto& statement : stmt->statements) {
        ss << std::any_cast<std::string>(statement->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::any AstPrinter::visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) {
    return parenthesize(";", {stmt->expression});
}

std::any AstPrinter::visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) {
    std::stringstream ss;
    ss << "(func " << stmt->name.lexeme << " (";
    for (const auto& param : stmt->params) {
        ss << " " << param.lexeme;
    }
    ss << ") ";
    for (const auto& body_stmt : stmt->body) {
        ss << std::any_cast<std::string>(body_stmt->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::any AstPrinter::visitIfStmt(std::shared_ptr<IfStmt> stmt) {
    std::stringstream ss;
    ss << "(if " << std::any_cast<std::string>(stmt->condition->accept(*this)) << " "
       << std::any_cast<std::string>(stmt->thenBranch->accept(*this));
    if (stmt->elseBranch != nullptr) {
        ss << " " << std::any_cast<std::string>(stmt->elseBranch->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::any AstPrinter::visitLetStmt(std::shared_ptr<LetStmt> stmt) {
    if (stmt->initializer == nullptr) {
        return "(let " + stmt->name.lexeme + ")";
    }
    return parenthesize("let " + stmt->name.lexeme, {stmt->initializer});
}

std::any AstPrinter::visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
    if (stmt->value == nullptr) {
        return "(return)";
    }
    return parenthesize("return", {stmt->value});
}

std::any AstPrinter::visitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
    std::stringstream ss;
    ss << "(while " << std::any_cast<std::string>(stmt->condition->accept(*this)) << " "
       << std::any_cast<std::string>(stmt->body->accept(*this)) << ")";
    return ss.str();
}

std::string AstPrinter::parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " " << std::any_cast<std::string>(expr->accept(*this));
    }
    ss << ")";
    return ss.str();
}

} // namespace chtholly
