#include "ASTPrinter.h"
#include <vector>
#include <iostream>

std::string ASTPrinter::print(Expr* expr) {
    if (!expr) return "";
    return expr->accept(*this);
}

std::string ASTPrinter::print(Stmt* stmt) {
    if (!stmt) return "";
    return stmt->accept(*this);
}

std::string ASTPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

std::string ASTPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return parenthesize("group", {expr.expression.get()});
}

std::string ASTPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    } else if (std::holds_alternative<double>(expr.value)) {
        std::string s = std::to_string(std::get<double>(expr.value));
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') {
            s.pop_back();
        }
        return s;
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return "nil";
}

std::string ASTPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()});
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<Expr*>& exprs) {
    std::string result = "(" + name;
    for (Expr* expr : exprs) {
        result += " ";
        result += expr->accept(*this);
    }
    result += ")";
    return result;
}

// Implement other visitor methods...
std::string ASTPrinter::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::string ASTPrinter::visitAssignExpr(const AssignExpr& expr) {
    return parenthesize("= " + expr.name.lexeme, {expr.value.get()});
}

std::string ASTPrinter::visitCallExpr(const CallExpr& expr) {
    std::vector<Expr*> exprs;
    exprs.push_back(expr.callee.get());
    for(const auto& arg : expr.arguments) {
        exprs.push_back(arg.get());
    }
    return parenthesize("call", exprs);
}

std::string ASTPrinter::visitGetExpr(const GetExpr& expr) {
    return parenthesize("." + expr.name.lexeme, {expr.object.get()});
}

std::string ASTPrinter::visitSetExpr(const SetExpr& expr) {
    return parenthesize("= ." + expr.name.lexeme, {expr.object.get(), expr.value.get()});
}

std::string ASTPrinter::visitBorrowExpr(const BorrowExpr& expr) {
    return parenthesize(expr.isMutable ? "&mut" : "&", {expr.expression.get()});
}

std::string ASTPrinter::visitLambdaExpr(const LambdaExpr& expr) {
    // This is a simplified representation
    return "(lambda)";
}

std::string ASTPrinter::visitStructInitializerExpr(const StructInitializerExpr& expr) {
    // This is a simplified representation
    return "(struct-init " + expr.name.lexeme + ")";
}


std::string ASTPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    return print(stmt.expression.get());
}

std::string ASTPrinter::visitLetStmt(const LetStmt& stmt) {
    std::string result = "(let " + stmt.name.lexeme;
    if (stmt.initializer) {
        result += " = " + print(stmt.initializer.get());
    }
    result += ")";
    return result;
}

// ... other Stmt visitors
std::string ASTPrinter::visitBlockStmt(const BlockStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitIfStmt(const IfStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitWhileStmt(const WhileStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName) { return std::string(""); }
std::string ASTPrinter::visitReturnStmt(const ReturnStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitStructStmt(const StructStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitTraitStmt(const TraitStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitImplStmt(const ImplStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitImportStmt(const ImportStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitSwitchStmt(const SwitchStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitBreakStmt(const BreakStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitFallthroughStmt(const FallthroughStmt& stmt) { return std::string(""); }
std::string ASTPrinter::visitEnumStmt(const EnumStmt& stmt) { return std::string(""); }
