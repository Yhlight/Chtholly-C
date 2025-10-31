#include <iostream>
#include <vector>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

// A simple visitor to print the AST
class AstPrinter : public ExprVisitor<std::string>, public StmtVisitor<std::string> {
public:
    std::string print(const std::vector<std::shared_ptr<Stmt>>& statements) {
        std::string result;
        for (const auto& stmt : statements) {
            if (stmt) {
                result += stmt->accept(*this) + "\n";
            }
        }
        return result;
    }

    std::string visitBinaryExpr(const std::shared_ptr<Binary>& expr) override {
        return parenthesize(expr->op.lexeme, {expr->left, expr->right});
    }

    std::string visitGroupingExpr(const std::shared_ptr<Grouping>& expr) override {
        return parenthesize("group", {expr->expression});
    }

    std::string visitLiteralExpr(const std::shared_ptr<Literal>& expr) override {
        return expr->value;
    }

    std::string visitUnaryExpr(const std::shared_ptr<Unary>& expr) override {
        return parenthesize(expr->op.lexeme, {expr->right});
    }

    std::string visitVariableExpr(const std::shared_ptr<Variable>& expr) override {
        return expr->name.lexeme;
    }

    std::string visitAssignExpr(const std::shared_ptr<Assign>& expr) override {
        return parenthesize("= " + expr->name.lexeme, {expr->value});
    }

    std::string visitExpressionStmt(const std::shared_ptr<Expression>& stmt) override {
        return parenthesize(";", {stmt->expression});
    }

    std::string visitPrintStmt(const std::shared_ptr<Print>& stmt) override {
        return parenthesize("print", {stmt->expression});
    }

    std::string visitVarStmt(const std::shared_ptr<Var>& stmt) override {
        if (stmt->initializer) {
            return parenthesize("var " + stmt->name.lexeme, {stmt->initializer});
        }
        return "(var " + stmt->name.lexeme + ")";
    }

    std::string visitBlockStmt(const std::shared_ptr<Block>& stmt) override {
        std::string result = "(block ";
        for (const auto& statement : stmt->statements) {
            result += statement->accept(*this);
        }
        result += ")";
        return result;
    }

private:
    std::string parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
        std::string builder = "(" + name;
        for (const auto& expr : exprs) {
            builder += " ";
            builder += expr->accept(*this);
        }
        builder += ")";
        return builder;
    }
};

int main() {
    std::string source = R"(
        let x = 10 + 5 * 2;
        let y = (10 + 5) * 2;
    )";

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser(tokens);
        std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

        if (!statements.empty()) {
            AstPrinter printer;
            std::cout << printer.print(statements) << std::endl;
        }


    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
