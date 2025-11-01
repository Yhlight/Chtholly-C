#include "Transpiler.h"
#include "Type.h"

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    out_ << "#include <iostream>\n";
    out_ << "#include <string>\n\n";

    bool in_main = false;
    for (const auto& statement : statements) {
        if (dynamic_cast<FuncStmt*>(statement.get())) {
            if (in_main) {
                dedent();
                write_indent();
                out_ << "return 0;\n";
                write_indent();
                out_ << "}\n";
                in_main = false;
            }
            statement->accept(*this);
        } else {
            if (!in_main) {
                out_ << "int main(int argc, char* argv[]) {\n";
                indent();
                in_main = true;
            }
            write_indent();
            statement->accept(*this);
        }
    }

    if (in_main) {
        write_indent();
        out_ << "return 0;\n";
        dedent();
        write_indent();
        out_ << "}\n";
    }

    return out_.str();
}

void Transpiler::visitAssignExpr(const Assign& expr) {
    out_ << expr.name.lexeme << " = ";
    expr.value->accept(*this);
}

void Transpiler::visitBinaryExpr(const Binary& expr) {
    out_ << "(";
    expr.left->accept(*this);
    out_ << " " << expr.op.lexeme << " ";
    expr.right->accept(*this);
    out_ << ")";
}

void Transpiler::visitCallExpr(const Call& expr) {
    expr.callee->accept(*this);
    out_ << "(";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        expr.arguments[i]->accept(*this);
        if (i < expr.arguments.size() - 1) {
            out_ << ", ";
        }
    }
    out_ << ")";
}

void Transpiler::visitGroupingExpr(const Grouping& expr) {
    out_ << "(";
    expr.expression->accept(*this);
    out_ << ")";
}

void Transpiler::visitLiteralExpr(const Literal& expr) {
    out_ << expr.value.lexeme;
}

void Transpiler::visitUnaryExpr(const Unary& expr) {
    out_ << expr.op.lexeme;
    expr.right->accept(*this);
}

void Transpiler::visitVariableExpr(const Variable& expr) {
    out_ << expr.name.lexeme;
}

void Transpiler::visitBlockStmt(const Block& stmt) {
    out_ << "{\n";
    indent();
    for (const auto& statement : stmt.statements) {
        write_indent();
        statement->accept(*this);
    }
    dedent();
    write_indent();
    out_ << "}\n";
}

void Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    out_ << ";\n";
}

void Transpiler::write_indent() {
    for (int i = 0; i < indent_level_; ++i) {
        out_ << "    ";
    }
}

void Transpiler::visitFuncStmt(const FuncStmt& stmt) {
    if (stmt.returnType) {
        out_ << stmt.returnType->toString();
    } else {
        out_ << "void";
    }
    out_ << " " << stmt.name.lexeme << "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        out_ << stmt.params[i].second->toString() << " " << stmt.params[i].first.lexeme;
        if (i < stmt.params.size() - 1) {
            out_ << ", ";
        }
    }
    out_ << ") ";
    stmt.body->accept(*this);
}

void Transpiler::visitLetStmt(const LetStmt& stmt) {
    if (stmt.isMutable) {
        out_ << "";
    } else {
        out_ << "const ";
    }

    if (stmt.type) {
        out_ << stmt.type->toString() << " " << stmt.name.lexeme;
    } else {
        out_ << "auto " << stmt.name.lexeme;
    }

    if (stmt.initializer) {
        out_ << " = ";
        stmt.initializer->accept(*this);
    }
    out_ << ";\n";
}

void Transpiler::visitIfStmt(const IfStmt& stmt) {
    out_ << "if (";
    stmt.condition->accept(*this);
    out_ << ") ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        out_ << "else ";
        stmt.elseBranch->accept(*this);
    }
}

void Transpiler::visitWhileStmt(const WhileStmt& stmt) {
    out_ << "while (";
    stmt.condition->accept(*this);
    out_ << ") ";
    stmt.body->accept(*this);
}

void Transpiler::visitPrintStmt(const PrintStmt& stmt) {
    out_ << "    std::cout << ";
    stmt.expression->accept(*this);
    out_ << " << std::endl;\n";
}

void Transpiler::visitReturnStmt(const ReturnStmt& stmt) {
    out_ << "return";
    if (stmt.value) {
        out_ << " ";
        stmt.value->accept(*this);
    }
    out_ << ";\n";
}
