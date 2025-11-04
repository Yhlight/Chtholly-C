#include <CodeGen.h>
#include <iostream>

std::string CodeGen::generate(const std::vector<std::unique_ptr<Stmt>>& statements) {
    result = "#include <iostream>\n\n";
    for (const auto& statement : statements) {
        statement->accept(*this);
    }
    return result;
}

void CodeGen::visit(const BinaryExpr& expr) {
    result += "(";
    expr.left->accept(*this);
    result += " " + expr.op.lexeme + " ";
    expr.right->accept(*this);
    result += ")";
}

void CodeGen::visit(const GroupingExpr& expr) {
    result += "(";
    expr.expression->accept(*this);
    result += ")";
}

void CodeGen::visit(const LiteralExpr& expr) {
    result += expr.value.lexeme;
}

void CodeGen::visit(const UnaryExpr& expr) {
    result += expr.op.lexeme;
    expr.right->accept(*this);
}

void CodeGen::visit(const VariableExpr& expr) {
    result += expr.name.lexeme;
}

void CodeGen::visit(const AssignmentExpr& expr) {
    result += expr.name.lexeme + " = ";
    expr.value->accept(*this);
}

void CodeGen::visit(const CallExpr& expr) {
    expr.callee->accept(*this);
    result += "(";
    for (size_t i = 0; i < expr.arguments.size(); ++i) {
        expr.arguments[i]->accept(*this);
        if (i < expr.arguments.size() - 1) {
            result += ", ";
        }
    }
    result += ")";
}

void CodeGen::visit(const GetExpr& expr) {
    expr.object->accept(*this);
    result += "." + expr.name.lexeme;
}

void CodeGen::visit(const SetExpr& expr) {
    expr.object->accept(*this);
    result += "." + expr.name.lexeme + " = ";
    expr.value->accept(*this);
}

void CodeGen::visit(const SelfExpr& expr) {
    result += "this";
}

void CodeGen::visit(const BlockStmt& stmt) {
    result += "{\n";
    for (const auto& statement : stmt.statements) {
        statement->accept(*this);
    }
    result += "}\n";
}

void CodeGen::visit(const ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
    result += ";\n";
}

void CodeGen::visit(const FunctionStmt& stmt) {
    result += "auto " + stmt.name.lexeme + "(";
    for (size_t i = 0; i < stmt.params.size(); ++i) {
        result += "auto " + stmt.params[i].lexeme;
        if (i < stmt.params.size() - 1) {
            result += ", ";
        }
    }
    result += ") ";
    stmt.body->accept(*this);
}

void CodeGen::visit(const StructStmt& stmt) {
    isInsideStruct = true;
    result += "struct " + stmt.name.lexeme + " {\n";
    for (const auto& field : stmt.fields) {
        field->accept(*this);
    }
    for (const auto& method : stmt.methods) {
        method->accept(*this);
    }
    result += "};\n";
    isInsideStruct = false;
}

void CodeGen::visit(const IfStmt& stmt) {
    result += "if (";
    stmt.condition->accept(*this);
    result += ") ";
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        result += " else ";
        stmt.elseBranch->accept(*this);
    }
}

void CodeGen::visit(const ReturnStmt& stmt) {
    result += "return ";
    if (stmt.value) {
        stmt.value->accept(*this);
    }
    result += ";\n";
}

void CodeGen::visit(const VarDeclStmt& stmt) {
    std::string type = "auto";
    if (stmt.type.type != TokenType::UNKNOWN) {
        type = stmt.type.lexeme;
    }

    if (stmt.isMutable || isInsideStruct) {
        result += type + " " + stmt.name.lexeme;
    } else {
        result += "const " + type + " " + stmt.name.lexeme;
    }
    if (stmt.initializer) {
        result += " = ";
        stmt.initializer->accept(*this);
    }
    result += ";\n";
}

void CodeGen::visit(const WhileStmt& stmt) {
    result += "while (";
    stmt.condition->accept(*this);
    result += ") ";
    stmt.body->accept(*this);
}
