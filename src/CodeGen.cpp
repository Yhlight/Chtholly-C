#include "CodeGen.h"
#include "Token.h"

namespace chtholly {

CodeGen::CodeGen() {}

std::string CodeGen::generate(const BlockStmtAST& ast) {
    for (const auto& stmt : ast.getStatements()) {
        visit(*stmt);
    }
    return ss.str();
}

void CodeGen::visit(const StmtAST& stmt) {
    if (auto* varDecl = dynamic_cast<const VarDeclAST*>(&stmt)) {
        visit(*varDecl);
    } else if (auto* funcDecl = dynamic_cast<const FuncDeclAST*>(&stmt)) {
        visit(*funcDecl);
    } else if (auto* structDecl = dynamic_cast<const StructDeclAST*>(&stmt)) {
        visit(*structDecl);
    } else if (auto* ifStmt = dynamic_cast<const IfStmtAST*>(&stmt)) {
        visit(*ifStmt);
    } else if (auto* switchStmt = dynamic_cast<const SwitchStmtAST*>(&stmt)) {
        visit(*switchStmt);
    } else if (auto* caseBlock = dynamic_cast<const CaseBlockAST*>(&stmt)) {
        visit(*caseBlock);
    } else if (auto* enumDecl = dynamic_cast<const EnumDeclAST*>(&stmt)) {
        visit(*enumDecl);
    } else if (auto* whileStmt = dynamic_cast<const WhileStmtAST*>(&stmt)) {
        visit(*whileStmt);
    } else if (auto* forStmt = dynamic_cast<const ForStmtAST*>(&stmt)) {
        visit(*forStmt);
    } else if (auto* importStmt = dynamic_cast<const ImportStmtAST*>(&stmt)) {
        visit(*importStmt);
    } else if (auto* block = dynamic_cast<const BlockStmtAST*>(&stmt)) {
        visit(*block);
    } else if (auto* exprStmt = dynamic_cast<const ExprStmtAST*>(&stmt)) {
        visit(*exprStmt);
    }
}

void CodeGen::visit(const VarDeclAST& stmt) {
    if (stmt.getIsMutable()) {
        ss << "auto ";
    } else {
        ss << "const auto ";
    }
    ss << stmt.getName() << " = ";
    visit(*stmt.getInit());
    if (inForInit) {
        ss << ";";
    } else {
        ss << ";\n";
    }
}

void CodeGen::visit(const FuncDeclAST& stmt) {
    ss << stmt.getReturnTypeName() << " " << stmt.getName() << "(";
    const auto& params = stmt.getParams();
    for (size_t i = 0; i < params.size(); ++i) {
        ss << params[i].typeName << " " << params[i].name;
        if (i < params.size() - 1) {
            ss << ", ";
        }
    }
    ss << ") ";
    visit(stmt.getBody());
}

void CodeGen::visit(const StructDeclAST& stmt) {
    ss << "struct " << stmt.getName() << " {\n";
    for (const auto& member : stmt.getMembers()) {
        if (member.isPublic) {
            ss << "public: ";
        } else {
            ss << "private: ";
        }
        ss << member.typeName << " " << member.name << ";\n";
    }
    ss << "};\n";
}

void CodeGen::visit(const ExprStmtAST& stmt) {
    visit(stmt.getExpr());
    ss << ";\n";
}

void CodeGen::visit(const ImportStmtAST& stmt) {
    if (stmt.getModuleName() == "iostream") {
        ss << "#include <iostream>\n";
        ss << "#include <string>\n";
    } else {
        ss << "#include \"" << stmt.getModuleName() << ".h\"\n";
    }
}

void CodeGen::visit(const ForStmtAST& stmt) {
    ss << "for (";
    if (stmt.getInit()) {
        inForInit = true;
        visit(*stmt.getInit());
        inForInit = false;
    }

    if (stmt.getCond()) {
        visit(*stmt.getCond());
    }
    ss << ";";

    if (stmt.getInc()) {
        visit(*stmt.getInc());
    }
    ss << ") ";
    visit(stmt.getBody());
}

void CodeGen::visit(const IfStmtAST& stmt) {
    ss << "if (";
    visit(stmt.getCond());
    ss << ") ";
    visit(stmt.getThen());
    if (stmt.getElse()) {
        ss << "else ";
        visit(*stmt.getElse());
    }
}

void CodeGen::visit(const SwitchStmtAST& stmt) {
    ss << "switch (";
    visit(stmt.getExpr());
    ss << ") {\n";
    for (const auto& caseBlock : stmt.getCases()) {
        visit(*caseBlock);
    }
    ss << "}\n";
}

void CodeGen::visit(const CaseBlockAST& stmt) {
    if (stmt.getExpr()) {
        ss << "case ";
        visit(*stmt.getExpr());
        ss << ": ";
    } else {
        ss << "default: ";
    }
    visit(stmt.getBody());
}

void CodeGen::visit(const EnumDeclAST& stmt) {
    ss << "enum class " << stmt.getName() << " {\n";
    for (size_t i = 0; i < stmt.getMembers().size(); ++i) {
        ss << "    " << stmt.getMembers()[i];
        if (i < stmt.getMembers().size() - 1) {
            ss << ",";
        }
        ss << "\n";
    }
    ss << "};\n";
}

void CodeGen::visit(const WhileStmtAST& stmt) {
    ss << "while (";
    visit(stmt.getCond());
    ss << ") ";
    visit(stmt.getBody());
}

void CodeGen::visit(const BlockStmtAST& stmt) {
    ss << "{\n";
    for (const auto& s : stmt.getStatements()) {
        visit(*s);
    }
    ss << "}\n";
}

void CodeGen::visit(const ExprAST& expr) {
    if (auto* numExpr = dynamic_cast<const NumberExprAST*>(&expr)) {
        visit(*numExpr);
    } else if (auto* strExpr = dynamic_cast<const StringExprAST*>(&expr)) {
        visit(*strExpr);
    } else if (auto* varExpr = dynamic_cast<const VariableExprAST*>(&expr)) {
        visit(*varExpr);
    } else if (auto* binExpr = dynamic_cast<const BinaryExprAST*>(&expr)) {
        visit(*binExpr);
    } else if (auto* callExpr = dynamic_cast<const CallExprAST*>(&expr)) {
        visit(*callExpr);
    } else if (auto* boolExpr = dynamic_cast<const BoolExprAST*>(&expr)) {
        visit(*boolExpr);
    } else if (auto* assignExpr = dynamic_cast<const AssignExprAST*>(&expr)) {
        visit(*assignExpr);
    }
}

void CodeGen::visit(const NumberExprAST& expr) {
    if (std::holds_alternative<int>(expr.getVal())) {
        ss << std::get<int>(expr.getVal());
    } else if (std::holds_alternative<double>(expr.getVal())) {
        ss << std::get<double>(expr.getVal());
    }
}

void CodeGen::visit(const StringExprAST& expr) {
    ss << "\"" << expr.getVal() << "\"";
}

void CodeGen::visit(const VariableExprAST& expr) {
    ss << expr.getName();
}

static std::string toCppOp(TokenType op) {
    switch (op) {
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Less: return "<";
        case TokenType::LessEqual: return "<=";
        case TokenType::Greater: return ">";
        case TokenType::GreaterEqual: return ">=";
        default: return "";
    }
}

void CodeGen::visit(const BinaryExprAST& expr) {
    ss << "(";
    visit(expr.getLHS());
    ss << " " << toCppOp(expr.getOp()) << " ";
    visit(expr.getRHS());
    ss << ")";
}

void CodeGen::visit(const CallExprAST& expr) {
    if (auto* varExpr = dynamic_cast<const VariableExprAST*>(&expr.getCallee())) {
        if (varExpr->getName() == "print") {
            ss << "std::cout << ";
            visit(*expr.getArgs()[0]);
            ss << " << std::endl";
            return;
        }
    }

    visit(expr.getCallee());
    ss << "(";
    const auto& args = expr.getArgs();
    for (size_t i = 0; i < args.size(); ++i) {
        visit(*args[i]);
        if (i < args.size() - 1) {
            ss << ", ";
        }
    }
    ss << ")";
}

void CodeGen::visit(const BoolExprAST& expr) {
    ss << (expr.getVal() ? "true" : "false");
}

void CodeGen::visit(const AssignExprAST& expr) {
    visit(expr.getTarget());
    ss << " = ";
    visit(expr.getValue());
}

} // namespace chtholly
