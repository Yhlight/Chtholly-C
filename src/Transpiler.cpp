#include "Transpiler.h"
#include <sstream>
#include <stdexcept>

std::string Transpiler::transpile(const std::vector<std::unique_ptr<Stmt>>& statements) {
    out.str("");
    out.clear();
    indent_level_ = 0;

    out << "#include <iostream>\n";
    out << "#include <string>\n";
    out << "#include <variant>\n\n";

    for (const auto& statement : statements) {
        if (statement && (dynamic_cast<FuncStmt*>(statement.get()) || dynamic_cast<StructStmt*>(statement.get()))) {
            out << transpile(*statement) << "\n";
        }
    }

    out << "int main() {\n";
    indent();

    for (const auto& statement : statements) {
        if (statement && !dynamic_cast<FuncStmt*>(statement.get()) && !dynamic_cast<StructStmt*>(statement.get())) {
            write_indent();
            out << transpile(*statement) << "\n";
        }
    }

    write_indent();
    out << "return 0;\n";
    dedent();
    write_indent();
    out << "}\n";
    return out.str();
}


std::string Transpiler::transpile(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::string Transpiler::transpile(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

// Indentation methods
void Transpiler::indent() {
    indent_level_++;
}

void Transpiler::dedent() {
    indent_level_--;
}

void Transpiler::write_indent() {
    for (int i = 0; i < indent_level_; ++i) {
        out << "    ";
    }
}


// Expression visitor methods
std::any Transpiler::visitAssignExpr(const AssignExpr& expr) {
    return expr.name.lexeme + " = " + transpile(*expr.value);
}

std::any Transpiler::visitBinaryExpr(const BinaryExpr& expr) {
    return "(" + transpile(*expr.left) + " " + expr.op.lexeme + " " + transpile(*expr.right) + ")";
}

std::any Transpiler::visitGroupingExpr(const GroupingExpr& expr) {
    return "(" + transpile(*expr.expression) + ")";
}

std::any Transpiler::visitLiteralExpr(const LiteralExpr& expr) {
    if (std::holds_alternative<std::string>(expr.value)) {
        return "std::string(\"" + std::get<std::string>(expr.value) + "\")";
    }
    if (std::holds_alternative<double>(expr.value)) {
        return std::to_string(std::get<double>(expr.value));
    }
    if (std::holds_alternative<bool>(expr.value)) {
        return std::get<bool>(expr.value) ? "true" : "false";
    }
    return "nullptr"; // Representing nil
}

std::any Transpiler::visitUnaryExpr(const UnaryExpr& expr) {
    return "(" + expr.op.lexeme + transpile(*expr.right) + ")";
}

std::any Transpiler::visitVariableExpr(const VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any Transpiler::visitCallExpr(const CallExpr& expr) {
    std::stringstream call_out;
    call_out << transpile(*expr.callee) << "(";
    for (int i = 0; i < expr.arguments.size(); ++i) {
        call_out << transpile(*expr.arguments[i]);
        if (i < expr.arguments.size() - 1) {
            call_out << ", ";
        }
    }
    call_out << ")";
    return call_out.str();
}

std::any Transpiler::visitGetExpr(const GetExpr& expr) {
    return transpile(*expr.object) + "." + expr.name.lexeme;
}

std::any Transpiler::visitSetExpr(const SetExpr& expr) {
    return transpile(*expr.object) + "." + expr.name.lexeme + " = " + transpile(*expr.value);
}

std::any Transpiler::visitThisExpr(const ThisExpr& expr) {
    return std::string("this");
}

// Statement visitor methods
std::any Transpiler::visitBlockStmt(const BlockStmt& stmt) {
    std::stringstream block_out;
    block_out << "{\n";
    indent();
    for (const auto& statement : stmt.statements) {
        for(int i = 0; i < indent_level_; ++i) block_out << "    ";
        block_out << transpile(*statement) << "\n";
    }
    dedent();
    for(int i = 0; i < indent_level_ + 1; ++i) block_out << "    ";
    block_out << "}";
    return block_out.str();
}

std::any Transpiler::visitExpressionStmt(const ExpressionStmt& stmt) {
    return transpile(*stmt.expression) + ";";
}

std::any Transpiler::visitIfStmt(const IfStmt& stmt) {
    std::stringstream if_out;
    if_out << "if (" << transpile(*stmt.condition) << ") " << transpile(*stmt.thenBranch);
    if (stmt.elseBranch) {
        if_out << " else " << transpile(*stmt.elseBranch);
    }
    return if_out.str();
}

std::any Transpiler::visitLetStmt(const LetStmt& stmt) {
    std::string value = "auto";
    if (stmt.initializer) {
        value += " " + stmt.name.lexeme + " = " + transpile(*stmt.initializer) + ";";
    } else {
        value += " " + stmt.name.lexeme + ";";
    }
    return value;
}

std::any Transpiler::visitPrintStmt(const PrintStmt& stmt) {
    return "std::cout << (" + transpile(*stmt.expression) + ") << std::endl;";
}

std::any Transpiler::visitWhileStmt(const WhileStmt& stmt) {
    std::stringstream while_out;
    while_out << "while (" << transpile(*stmt.condition) << ") " << transpile(*stmt.body);
    return while_out.str();
}

std::any Transpiler::visitSwitchStmt(const SwitchStmt& stmt) {
    std::stringstream switch_out;
    switch_out << "switch (" << transpile(*stmt.expression) << ") {\n";
    indent();
    for (const auto& caseStmt : stmt.cases) {
        write_indent();
        switch_out << transpile(*caseStmt);
    }
    dedent();
    write_indent();
    switch_out << "}";
    return switch_out.str();
}

std::any Transpiler::visitCaseStmt(const CaseStmt& stmt) {
    std::stringstream case_out;
    if (stmt.condition) {
        case_out << "case " << transpile(*stmt.condition) << ":\n";
    } else {
        case_out << "default:\n";
    }
    indent();
    write_indent();
    case_out << transpile(*stmt.body) << "\n";
    dedent();
    return case_out.str();
}

std::any Transpiler::visitBreakStmt(const BreakStmt& stmt) {
    return std::string("break;");
}

std::any Transpiler::visitFallthroughStmt(const FallthroughStmt& stmt) {
    return std::string("[[fallthrough]];");
}

std::any Transpiler::visitFuncStmt(const FuncStmt& stmt) {
    std::stringstream func_out;
    func_out << "auto " << stmt.name.lexeme << "(";
    for (int i = 0; i < stmt.params.size(); ++i) {
        func_out << "auto " << stmt.params[i].lexeme;
        if (i < stmt.params.size() - 1) {
            func_out << ", ";
        }
    }
    func_out << ") " << transpile(*stmt.body);
    return func_out.str();
}

std::any Transpiler::visitReturnStmt(const ReturnStmt& stmt) {
    if (stmt.value) {
        return "return " + transpile(*stmt.value) + ";";
    }
    return std::string("return;");
}

std::any Transpiler::visitStructStmt(const StructStmt& stmt) {
    std::stringstream struct_out;
    struct_out << "struct " << stmt.name.lexeme << " {\n";
    indent();
    for (const auto& field : stmt.fields) {
        write_indent();
        struct_out << transpile(*field) << "\n";
    }
    for (const auto& method : stmt.methods) {
        write_indent();
        struct_out << transpile(*method) << "\n";
    }
    dedent();
    write_indent();
    struct_out << "};";
    return struct_out.str();
}
