#include "Expr.h"
#include "Stmt.h"

// Stmt destructors
Stmt::~Stmt() = default;
ExpressionStmt::~ExpressionStmt() = default;
LetStmt::~LetStmt() = default;
BlockStmt::~BlockStmt() = default;
IfStmt::~IfStmt() = default;
WhileStmt::~WhileStmt() = default;
FunctionStmt::~FunctionStmt() = default;
ReturnStmt::~ReturnStmt() = default;
StructStmt::~StructStmt() = default;
TraitStmt::~TraitStmt() = default;
ImplStmt::~ImplStmt() = default;
ImportStmt::~ImportStmt() = default;
BreakStmt::~BreakStmt() = default;
FallthroughStmt::~FallthroughStmt() = default;
CaseStmt::~CaseStmt() = default;
SwitchStmt::~SwitchStmt() = default;
EnumStmt::~EnumStmt() = default;

// Stmt constructors
ExpressionStmt::ExpressionStmt(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
LetStmt::LetStmt(Token name, std::optional<TypeInfo> type, std::unique_ptr<Expr> initializer, bool isMutable, bool is_public) : name(name), type(std::move(type)), initializer(std::move(initializer)), isMutable(isMutable), is_public(is_public) {}
BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
IfStmt::IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch) : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
WhileStmt::WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body) : condition(std::move(condition)), body(std::move(body)) {}
FunctionStmt::FunctionStmt(Token name, std::vector<Token> generics, std::vector<std::pair<Token, TypeInfo>> params, std::vector<std::unique_ptr<Stmt>> body, std::optional<TypeInfo> returnType) : name(name), generics(std::move(generics)), params(std::move(params)), body(std::move(body)), returnType(std::move(returnType)) {}
ReturnStmt::ReturnStmt(Token keyword, std::unique_ptr<Expr> value) : keyword(keyword), value(std::move(value)) {}
StructStmt::StructStmt(Token name, std::vector<std::unique_ptr<LetStmt>> fields) : name(name), fields(std::move(fields)) {}
TraitStmt::TraitStmt(Token name, std::vector<Token> generics, std::vector<std::unique_ptr<Stmt>> raw_methods) : name(name), generics(std::move(generics)) {
    for (auto& stmt : raw_methods) {
        auto func_stmt = dynamic_cast<FunctionStmt*>(stmt.get());
        if (func_stmt) {
            stmt.release();
            methods.emplace_back(func_stmt);
        } else {
            throw std::runtime_error("Trait method is not a FunctionStmt");
        }
    }
}
ImplStmt::ImplStmt(Token structName, std::optional<Token> traitName, std::vector<TypeInfo> generics, std::vector<std::unique_ptr<Stmt>> raw_methods) : structName(structName), traitName(traitName), generics(std::move(generics)) {
    for (auto& stmt : raw_methods) {
        auto func_stmt = dynamic_cast<FunctionStmt*>(stmt.get());
        if (func_stmt) {
            stmt.release();
            methods.emplace_back(func_stmt);
        } else {
            throw std::runtime_error("Impl method is not a FunctionStmt");
        }
    }
}
ImportStmt::ImportStmt(Token path, bool is_std) : path(path), is_std(is_std) {}
BreakStmt::BreakStmt(Token keyword) : keyword(keyword) {}
FallthroughStmt::FallthroughStmt(Token keyword) : keyword(keyword) {}
CaseStmt::CaseStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body) : condition(std::move(condition)), body(std::move(body)) {}
SwitchStmt::SwitchStmt(std::unique_ptr<Expr> expression, std::vector<CaseStmt> cases, std::optional<CaseStmt> defaultCase) : expression(std::move(expression)), cases(std::move(cases)), defaultCase(std::move(defaultCase)) {}
EnumStmt::EnumStmt(Token name, std::vector<Token> members) : name(name), members(std::move(members)) {}

// Expr destructors
Expr::~Expr() = default;
BinaryExpr::~BinaryExpr() = default;
GroupingExpr::~GroupingExpr() = default;
LiteralExpr::~LiteralExpr() = default;
UnaryExpr::~UnaryExpr() = default;
VariableExpr::~VariableExpr() = default;
AssignExpr::~AssignExpr() = default;
CallExpr::~CallExpr() = default;
GetExpr::~GetExpr() = default;
SetExpr::~SetExpr() = default;
BorrowExpr::~BorrowExpr() = default;
LambdaExpr::~LambdaExpr() = default;
StructInitializerExpr::~StructInitializerExpr() = default;

// Expr constructors
BinaryExpr::BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) : left(std::move(left)), op(op), right(std::move(right)) {}
GroupingExpr::GroupingExpr(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
LiteralExpr::LiteralExpr(std::variant<std::monostate, std::string, double, bool> value) : value(value) {}
UnaryExpr::UnaryExpr(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right)) {}
VariableExpr::VariableExpr(Token name) : name(name) {}
AssignExpr::AssignExpr(Token name, std::unique_ptr<Expr> value) : name(name), value(std::move(value)) {}
CallExpr::CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments, std::vector<TypeInfo> generic_args) : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)), generic_args(std::move(generic_args)) {}
GetExpr::GetExpr(std::unique_ptr<Expr> object, Token name) : object(std::move(object)), name(name) {}
SetExpr::SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value) : object(std::move(object)), name(name), value(std::move(value)) {}
BorrowExpr::BorrowExpr(std::unique_ptr<Expr> expression, bool isMutable) : expression(std::move(expression)), isMutable(isMutable) {}
LambdaExpr::LambdaExpr(std::vector<std::pair<Token, TypeInfo>> params, std::vector<std::unique_ptr<Stmt>> body, std::optional<TypeInfo> returnType) : params(std::move(params)), body(std::move(body)), returnType(std::move(returnType)) {}
StructInitializerExpr::StructInitializerExpr(Token name, std::map<Token, std::unique_ptr<Expr>> initializers) : name(name), initializers(std::move(initializers)) {}
