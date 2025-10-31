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

    std::stringstream m_out;
};

} // namespace Chtholly

#endif // CHTHOLLY_CODEGEN_H
