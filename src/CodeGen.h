#ifndef CHTHOLLY_CODEGEN_H
#define CHTHOLLY_CODEGEN_H

#include "AST.h"
#include <string>
#include <sstream>

namespace Chtholly {

class CodeGen {
public:
    std::string generate(Program* program);

private:
    void visit(Node* node);
    void visit(ExpressionStatement* node);
    void visit(LetStatement* node);
    void visit(ReturnStatement* node);
    void visit(Identifier* node);
    void visit(IntegerLiteral* node);
    void visit(Boolean* node);
    void visit(PrefixExpression* node);
    void visit(InfixExpression* node);
    void visit(BlockStatement* node);
    void visit(IfExpression* node);
    void visit(FunctionLiteral* node);
    void visit(CallExpression* node);
    void visit(TypeName* node);
    void visit(StringLiteral* node);
    void visit(StructStatement* node);
    void visit(Field* node);
    void visit(Method* node);
    void visit(GenericInstantiation* node);
    void visit(StructLiteral* node);
    void visit(MemberAccessExpression* node);
    void visit(EnumStatement* node);
    void visit(TraitStatement* node);
    void visit(ImplStatement* node);
    void visit(Constraint* node);

    std::stringstream m_out;
    Program* m_program = nullptr;
};

} // namespace Chtholly

#endif // CHTHOLLY_CODEGEN_H
