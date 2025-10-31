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
    } else if (auto* ifStmt = dynamic_cast<const IfStmtAST*>(&stmt)) {
        visit(*ifStmt);
    } else if (auto* switchStmt = dynamic_cast<const SwitchStmtAST*>(&stmt)) {
        visit(*switchStmt);
    } else if (auto* caseBlock = dynamic_cast<const CaseBlockAST*>(&stmt)) {
        visit(*caseBlock);
    } else if (auto* block = dynamic_cast<const BlockStmtAST*>(&stmt)) {
        visit(*block);
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
    ss << ";\n";
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

} // namespace chtholly
