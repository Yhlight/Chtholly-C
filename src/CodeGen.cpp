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

void CodeGen::visit(const ExprAST& expr) {
    if (auto* numExpr = dynamic_cast<const NumberExprAST*>(&expr)) {
        visit(*numExpr);
    } else if (auto* strExpr = dynamic_cast<const StringExprAST*>(&expr)) {
        visit(*strExpr);
    } else if (auto* varExpr = dynamic_cast<const VariableExprAST*>(&expr)) {
        visit(*varExpr);
    } else if (auto* binExpr = dynamic_cast<const BinaryExprAST*>(&expr)) {
        visit(*binExpr);
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

} // namespace chtholly
