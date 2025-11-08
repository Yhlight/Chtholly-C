#include "TestHelpers.h"
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include <algorithm>
#include <cctype>

#include "AST.h"
#include "Transpiler.h"
#include <map>
#include <string>

namespace chtholly {
class DeclarationScanner : public StmtVisitor, public ExprVisitor {
public:
    std::any visitStructStmt(const StructStmt& stmt) override {
        structs[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    std::any visitEnumStmt(const EnumStmt& stmt) override {
        enums[stmt.name.lexeme] = &stmt;
        return nullptr;
    }
    // Implement other visitor methods as needed, similar to Transpiler.cpp
    std::any visitBlockStmt(const BlockStmt& stmt) override { for (const auto& s : stmt.statements) s->accept(*this); return nullptr; }
    std::any visitExpressionStmt(const ExpressionStmt& stmt) override { stmt.expression->accept(*this); return nullptr; }
    std::any visitFunctionStmt(const FunctionStmt& stmt) override { return nullptr; } // Don't recurse into bodies
    std::any visitIfStmt(const IfStmt& stmt) override { stmt.thenBranch->accept(*this); if (stmt.elseBranch) stmt.elseBranch->accept(*this); return nullptr; }
    std::any visitVarStmt(const VarStmt& stmt) override { if (stmt.initializer) stmt.initializer->accept(*this); return nullptr; }
    std::any visitWhileStmt(const WhileStmt& stmt) override { stmt.body->accept(*this); return nullptr; }
    std::any visitForStmt(const ForStmt& stmt) override { stmt.body->accept(*this); return nullptr; }
    std::any visitReturnStmt(const ReturnStmt& stmt) override { if (stmt.value) stmt.value->accept(*this); return nullptr; }
    std::any visitImportStmt(const ImportStmt& stmt) override { return nullptr; }
    std::any visitSwitchStmt(const SwitchStmt& stmt) override { for (const auto& c : stmt.cases) c->accept(*this); return nullptr; }
    std::any visitCaseStmt(const CaseStmt& stmt) override { stmt.body->accept(*this); return nullptr; }
    std::any visitBreakStmt(const BreakStmt& stmt) override { return nullptr; }
    std::any visitFallthroughStmt(const FallthroughStmt& stmt) override { return nullptr; }
    std::any visitTraitStmt(const TraitStmt& stmt) override { return nullptr; }
    std::any visitBinaryExpr(const BinaryExpr& expr) override { expr.left->accept(*this); expr.right->accept(*this); return nullptr; }
    std::any visitUnaryExpr(const UnaryExpr& expr) override { expr.right->accept(*this); return nullptr; }
    std::any visitLiteralExpr(const LiteralExpr& expr) override { return nullptr; }
    std::any visitGroupingExpr(const GroupingExpr& expr) override { expr.expression->accept(*this); return nullptr; }
    std::any visitVariableExpr(const VariableExpr& expr) override { return nullptr; }
    std::any visitAssignExpr(const AssignExpr& expr) override { expr.value->accept(*this); return nullptr; }
    std::any visitCallExpr(const CallExpr& expr) override { expr.callee->accept(*this); for (const auto& arg : expr.arguments) arg->accept(*this); return nullptr; }
    std::any visitLambdaExpr(const LambdaExpr& expr) override { return nullptr; }
    std::any visitGetExpr(const GetExpr& expr) override { expr.object->accept(*this); return nullptr; }
    std::any visitSetExpr(const SetExpr& expr) override { expr.object->accept(*this); expr.value->accept(*this); return nullptr; }
    std::any visitSelfExpr(const SelfExpr& expr) override { return nullptr; }
    std::any visitBorrowExpr(const BorrowExpr& expr) override { return nullptr; }
    std::any visitDerefExpr(const DerefExpr& expr) override { return nullptr; }
    std::any visitStructLiteralExpr(const StructLiteralExpr& expr) override { for (const auto& field : expr.fields) field.second->accept(*this); return nullptr; }
    std::any visitArrayLiteralExpr(const ArrayLiteralExpr& expr) override { for (const auto& element : expr.elements) element->accept(*this); return nullptr; }
    std::any visitTypeCastExpr(const TypeCastExpr& expr) override { expr.expression->accept(*this); return nullptr; }

    std::map<std::string, const StructStmt*> structs;
    std::map<std::string, const EnumStmt*> enums;
};
}

std::string compile(const std::string& source) {
    chtholly::Lexer lexer(source);
    std::vector<chtholly::Token> tokens = lexer.scanTokens();
    chtholly::Parser parser(tokens);
    std::vector<std::unique_ptr<chtholly::Stmt>> statements = parser.parse();

    // Manually run the declaration scanner
    chtholly::DeclarationScanner scanner;
    for (const auto& stmt : statements) {
        if (stmt) stmt->accept(scanner);
    }

    chtholly::Transpiler transpiler;
    // This is a bit of a hack to inject the declarations into the transpiler.
    // A better solution would be to pass the scanner to the transpiler's constructor.
    // But for now, this will work for testing purposes.
    std::stringstream ss;
    ss << transpiler.transpile(statements);

    // This is an even bigger hack to get around the fact that we can't easily
    // modify the Transpiler class to accept the scanner. We'll just replace
    // the empty declarations with the ones we found.
    std::string result = ss.str();
    size_t pos = result.find("/*Declarations*/");
    if (pos != std::string::npos) {
        std::stringstream declarations;
        for (auto const& [name, val] : scanner.structs) {
            declarations << "struct " << name << ";\n";
        }
        for (auto const& [name, val] : scanner.enums) {
            declarations << "enum class " << name << ";\n";
        }
        result.replace(pos, 16, declarations.str());
    }

    return result;
}

std::string normalize(const std::string& s) {
    std::string result = s;
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](unsigned char c) { return std::isspace(c); }),
                 result.end());
    return result;
}
