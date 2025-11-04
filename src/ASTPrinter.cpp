#include <ASTPrinter.h>

std::string ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& statements) {
    result = "";
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
    return result;
}

void ASTPrinter::visit(const BinaryExpr& expr) {
    result += "(";
    result += expr.op.lexeme;
    result += " ";
    expr.left->accept(*this);
    result += " ";
    expr.right->accept(*this);
    result += ")";
}

void ASTPrinter::visit(const GroupingExpr& expr) {
    result += "(group ";
    expr.expression->accept(*this);
    result += ")";
}

void ASTPrinter::visit(const LiteralExpr& expr) {
    result += expr.value.lexeme;
}

void ASTPrinter::visit(const UnaryExpr& expr) {
    result += "(";
    result += expr.op.lexeme;
    result += " ";
    expr.right->accept(*this);
    result += ")";
}

void ASTPrinter::visit(const VariableExpr& expr) {
    result += expr.name.lexeme;
}

void ASTPrinter::visit(const AssignmentExpr& expr) {
    result += "(= ";
    result += expr.name.lexeme;
    result += " ";
    expr.value->accept(*this);
    result += ")";
}

void ASTPrinter::visit(const CallExpr& expr) {
    result += "(call ";
    expr.callee->accept(*this);
    for (const auto& arg : expr.arguments) {
        result += " ";
        arg->accept(*this);
    }
    result += ")";
}

void ASTPrinter::visit(const GetExpr& expr) {
    result += "(. ";
    expr.object->accept(*this);
    result += " " + expr.name.lexeme + ")";
}

void ASTPrinter::visit(const SetExpr& expr) {
    result += "(= ";
    expr.object->accept(*this);
    result += "." + expr.name.lexeme + " ";
    expr.value->accept(*this);
    result += ")";
}

void ASTPrinter::visit(const SelfExpr& expr) {
    result += "self";
}

void ASTPrinter::visit(const BlockStmt& stmt) {
    result += "(block ";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    result += ")";
}

void ASTPrinter::visit(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    result += ";";
}

void ASTPrinter::visit(const FunctionStmt& stmt) {
    result += "(fun " + stmt.name.lexeme + " (";
    for (const auto& param : stmt.params) {
        result += " " + param.lexeme;
    }
    result += ") ";
    stmt.body->accept(*this);
    result += ")";
}

void ASTPrinter::visit(const StructStmt& stmt) {
    result += "(struct " + stmt.name.lexeme;
    for (const auto& field : stmt.fields) {
        result += " ";
        field->accept(*this);
    }
    for (const auto& method : stmt.methods) {
        result += " ";
        method->accept(*this);
    }
    result += ")";
}

void ASTPrinter::visit(const IfStmt& stmt) {
    result += "(if ";
    stmt.condition->accept(*this);
    result += " ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        result += " ";
        stmt.elseBranch->accept(*this);
    }
    result += ")";
}

void ASTPrinter::visit(const ReturnStmt& stmt) {
    result += "(return ";
    if (stmt.value) {
        stmt.value->accept(*this);
    }
    result += ")";
}

void ASTPrinter::visit(const VarDeclStmt& stmt) {
    result += "(var " + stmt.name.lexeme;
    if (stmt.initializer) {
        result += " ";
        stmt.initializer->accept(*this);
    }
    result += ");";
}

void ASTPrinter::visit(const WhileStmt& stmt) {
    result += "(while ";
    stmt.condition->accept(*this);
    result += " ";
    stmt.body->accept(*this);
    result += ")";
}
