#include "ASTPrinter.h"
#include "Stmt.h"
#include <sstream>

std::string ASTPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::string ASTPrinter::print(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::any ASTPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, std::vector<const Expr*>{expr.left.get(), expr.right.get()});
}

std::any ASTPrinter::visitGroupingExpr(const GroupingExpr& expr) {
    return parenthesize("group", {expr.expression.get()});
}

std::any ASTPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return std::get<std::string>(expr.value);
    } else if (std::holds_alternative<double>(expr.value)) {
        std::ostringstream oss;
        oss << std::get<double>(expr.value);
        return oss.str();
    } else if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? std::string("true") : std::string("false");
    }
    return std::string("nil");
}

std::any ASTPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()});
}

std::any ASTPrinter::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any ASTPrinter::visitAssignExpr(const AssignExpr& expr) {
    return parenthesize("= " + expr.name.lexeme, {expr.value.get()});
}

std::any ASTPrinter::visitCallExpr(const CallExpr& expr) {
    return parenthesize("call", {expr.callee.get()});
}

std::any ASTPrinter::visitGetExpr(const GetExpr& expr) {
    return parenthesize("." + expr.name.lexeme, {expr.object.get()});
}

std::any ASTPrinter::visitSetExpr(const SetExpr& expr) {
    return parenthesize("= ." + expr.name.lexeme, std::vector<const Expr*>{expr.object.get(), expr.value.get()});
}

std::any ASTPrinter::visitBorrowExpr(const BorrowExpr& expr) {
    return parenthesize(expr.isMutable ? "&mut" : "&", {expr.expression.get()});
}

std::any ASTPrinter::visitLambdaExpr(const LambdaExpr& expr) {
    return "lambda";
}

std::any ASTPrinter::visitStructInitializerExpr(const StructInitializerExpr& expr) {
    std::vector<const Expr*> exprs;
    for (const auto& [name, value] : expr.initializers) {
        exprs.push_back(value.get());
    }
    return parenthesize("struct_init " + expr.name.lexeme, exprs);
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& expr : exprs) {
        builder << " ";
        builder << std::any_cast<std::string>(expr->accept(*this));
    }
    builder << ")";
    return builder.str();
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Stmt*>& stmts) {
    std::stringstream builder;
    builder << "(" << name;
    for (const auto& stmt : stmts) {
        builder << " ";
        builder << std::any_cast<std::string>(stmt->accept(*this));
    }
    builder << ")";
    return builder.str();
}

std::any ASTPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    return parenthesize("expr_stmt", {stmt.expression.get()});
}

std::any ASTPrinter::visitLetStmt(const LetStmt& stmt) {
    return parenthesize("let " + stmt.name.lexeme, {stmt.initializer.get()});
}

std::any ASTPrinter::visitBlockStmt(const BlockStmt& stmt) {
    std::vector<const Stmt*> stmts;
    for (const auto& s : stmt.statements) {
        stmts.push_back(s.get());
    }
    return parenthesize("block", stmts);
}

std::any ASTPrinter::visitIfStmt(const IfStmt& stmt) {
    return parenthesize("if", std::vector<const Stmt*>{stmt.thenBranch.get(), stmt.elseBranch.get()});
}

std::any ASTPrinter::visitWhileStmt(const WhileStmt& stmt) {
    return parenthesize("while", {stmt.body.get()});
}

std::any ASTPrinter::visitFunctionStmt(const FunctionStmt& stmt, std::optional<Token> structName) {
    return "func " + stmt.name.lexeme;
}

std::any ASTPrinter::visitReturnStmt(const ReturnStmt& stmt) {
    return parenthesize("return", {stmt.value.get()});
}

std::any ASTPrinter::visitStructStmt(const StructStmt& stmt) {
    return "struct " + stmt.name.lexeme;
}

std::any ASTPrinter::visitTraitStmt(const TraitStmt& stmt) {
    return "trait " + stmt.name.lexeme;
}

std::any ASTPrinter::visitImplStmt(const ImplStmt& stmt) {
    return "impl " + stmt.structName.lexeme;
}

std::any ASTPrinter::visitImportStmt(const ImportStmt& stmt) {
    return "import " + stmt.path.lexeme;
}

std::any ASTPrinter::visitSwitchStmt(const SwitchStmt& stmt) {
    return "switch";
}

std::any ASTPrinter::visitBreakStmt(const BreakStmt& stmt) {
    return "break";
}

std::any ASTPrinter::visitFallthroughStmt(const FallthroughStmt& stmt) {
    return "fallthrough";
}
