#include "ASTPrinter.h"
#include <sstream>
#include <variant>

std::string ASTPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::string ASTPrinter::print(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::any ASTPrinter::visitAssignExpr(const AssignExpr& expr) {
    return parenthesize("= " + expr.name.lexeme, {expr.value.get()});
}

std::any ASTPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

std::any ASTPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return parenthesize("group", {expr.expression.get()});
}

std::any ASTPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::string(std::get<bool>(expr.value) ? "true" : "false");
    }
    return std::string("nil");
}

std::any ASTPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()});
}

std::any ASTPrinter::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any ASTPrinter::visitCallExpr(const CallExpr& expr) {
    std::vector<const Expr*> exprs;
    for (const auto& arg : expr.arguments) {
        exprs.push_back(arg.get());
    }
    return parenthesize("call " + print(*expr.callee), exprs);
}

std::any ASTPrinter::visitBlockStmt(const BlockStmt& stmt) {
    std::vector<const Stmt*> stmts;
    for (const auto& s : stmt.statements) {
        stmts.push_back(s.get());
    }
    return parenthesizeStmts("block", stmts);
}

std::any ASTPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    return parenthesize(";", {stmt.expression.get()});
}

std::any ASTPrinter::visitIfStmt(const IfStmt& stmt) {
    std::stringstream builder;
    if (stmt.elseBranch) {
        builder << "(if-else " << print(*stmt.condition) << " " << print(*stmt.thenBranch) << " " << print(*stmt.elseBranch) << ")";
    } else {
        builder << "(if " << print(*stmt.condition) << " " << print(*stmt.thenBranch) << ")";
    }
    return builder.str();
}

std::any ASTPrinter::visitPrintStmt(const PrintStmt& stmt) {
    return parenthesize("print", {stmt.expression.get()});
}

std::any ASTPrinter::visitLetStmt(const LetStmt& stmt) {
    if (stmt.initializer) {
        return parenthesize("let " + stmt.name.lexeme + " =", {stmt.initializer.get()});
    }
    return std::string("(let " + stmt.name.lexeme + ")");
}

std::any ASTPrinter::visitWhileStmt(const WhileStmt& stmt) {
    std::stringstream builder;
    builder << "(while " << print(*stmt.condition) << " " << print(*stmt.body) << ")";
    return builder.str();
}

std::any ASTPrinter::visitSwitchStmt(const SwitchStmt& stmt) {
    std::stringstream builder;
    builder << "(switch " << print(*stmt.expression);
    for (const auto& caseStmt : stmt.cases) {
        builder << " " << print(*caseStmt);
    }
    builder << ")";
    return builder.str();
}

std::any ASTPrinter::visitCaseStmt(const CaseStmt& stmt) {
    std::stringstream builder;
    if (stmt.condition) {
        builder << "(case " << print(*stmt.condition) << " " << print(*stmt.body) << ")";
    } else {
        builder << "(default " << print(*stmt.body) << ")";
    }
    return builder.str();
}

std::any ASTPrinter::visitBreakStmt(const BreakStmt& stmt) {
    return std::string("break");
}

std::any ASTPrinter::visitFallthroughStmt(const FallthroughStmt& stmt) {
    return std::string("fallthrough");
}

std::any ASTPrinter::visitFuncStmt(const FuncStmt& stmt) {
    std::stringstream builder;
    builder << "(func " << stmt.name.lexeme << "(";
    for (const auto& param : stmt.params) {
        builder << " " << param.lexeme;
    }
    builder << ") " << print(*stmt.body) << ")";
    return builder.str();
}

std::any ASTPrinter::visitReturnStmt(const ReturnStmt& stmt) {
    if (stmt.value) {
        return parenthesize("return", {stmt.value.get()});
    }
    return std::string("(return)");
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& expr : exprs) {
        if(expr) builder << " " << print(*expr);
    }
    builder << ")";
    return builder.str();
}

std::string ASTPrinter::parenthesizeStmts(const std::string& name, const std::vector<const Stmt*>& stmts) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& stmt : stmts) {
        if(stmt) builder << " " << print(*stmt);
    }
    builder << ")";
    return builder.str();
}
