#include "Parser.h"
#include "Token.h"
#include <stdexcept>

std::map<char, int> Parser::bin_op_precedence_ = {
    {'=', 2},
    {'<', 10},
    {'>', 10},
    {'+', 20},
    {'-', 20},
    {'*', 40},
    {'/', 40}
};

Parser::Parser(Lexer& lexer) : lexer_(lexer) {
    // Prime the pump
    get_next_token();
}

void Parser::get_next_token() {
    current_token_ = lexer_.get_token();
}

int Parser::get_token_precedence() {
    if (get_current_token() != Token::Operator)
        return -1;

    char op = lexer_.get_operator();
    if (bin_op_precedence_.count(op) == 0)
        return -1;

    return bin_op_precedence_[op];
}

std::unique_ptr<ExprAST> Parser::parse_number_expression() {
    auto result = std::make_unique<NumberExprAST>(lexer_.get_number());
    get_next_token(); // consume the number
    return std::move(result);
}

std::unique_ptr<ExprAST> Parser::parse_paren_expression() {
    get_next_token(); // eat (.
    auto v = parse_expression();
    if (!v)
        return nullptr;

    if (get_current_token() != Token::RightParen)
        throw std::runtime_error("expected ')'");
    get_next_token(); // eat ).
    return v;
}

std::unique_ptr<ExprAST> Parser::parse_identifier_expression() {
    std::string id_name = lexer_.get_identifier();
    get_next_token(); // eat identifier.

    if (get_current_token() == Token::LeftParen) {
        // Function call.
        get_next_token(); // eat '('.
        std::vector<std::unique_ptr<ExprAST>> args;
        if (get_current_token() != Token::RightParen) {
            while (true) {
                if (auto arg = parse_expression())
                    args.push_back(std::move(arg));
                else
                    return nullptr;

                if (get_current_token() == Token::RightParen)
                    break;

                if (get_current_token() != Token::Comma)
                    throw std::runtime_error("Expected ')' or ',' in argument list");
                get_next_token();
            }
        }
        get_next_token(); // eat the ')'.
        return std::make_unique<CallExprAST>(id_name, std::move(args));
    } else if (get_current_token() == Token::LeftBrace) {
        // Struct instantiation.
        get_next_token(); // eat '{'.
        std::vector<StructFieldInitializer> fields;
        while (get_current_token() != Token::RightBrace) {
            if (get_current_token() != Token::Identifier)
                throw std::runtime_error("expected identifier in struct field initializer");
            std::string field_name = lexer_.get_identifier();
            get_next_token(); // eat the identifier.

            if (get_current_token() != Token::Colon)
                throw std::runtime_error("expected ':' after field name in initializer");
            get_next_token(); // eat the ':'.

            auto value = parse_expression();
            if (!value)
                return nullptr;
            fields.push_back({field_name, std::move(value)});

            if (get_current_token() == Token::Comma) {
                get_next_token(); // eat the ','.
            } else if (get_current_token() != Token::RightBrace) {
                throw std::runtime_error("expected ',' or '}' after field initializer");
            }
        }
        get_next_token(); // eat the '}'.
        return std::make_unique<StructInstantiationExprAST>(id_name, std::move(fields));
    }

    // Simple variable ref.
    return std::make_unique<VariableExprAST>(id_name);
}

std::shared_ptr<Type> Parser::parse_type() {
    if (get_current_token() != Token::Identifier) {
        throw std::runtime_error("Expected a type identifier");
    }

    std::string type_name = lexer_.get_identifier();
    get_next_token(); // consume type identifier

    if (type_name == "int") {
        return std::make_shared<IntType>();
    } else if (type_name == "double") {
        return std::make_shared<DoubleType>();
    } else if (type_name == "string") {
        return std::make_shared<StringType>();
    } else if (type_name == "array") {
        if (get_current_token() != Token::LeftBracket) {
            throw std::runtime_error("Expected '[' after 'array'");
        }
        get_next_token(); // consume '['

        auto element_type = parse_type();

        int size = -1;
        if (get_current_token() == Token::Semicolon) {
            get_next_token(); // consume ';'
            if (get_current_token() != Token::Number) {
                throw std::runtime_error("Expected a number for array size");
            }
            size = lexer_.get_number();
            get_next_token(); // consume number
        }

        if (get_current_token() != Token::RightBracket) {
            throw std::runtime_error("Expected ']' after array element type");
        }
        get_next_token(); // consume ']'

        return std::make_shared<ArrayType>(element_type, size);
    } else {
        throw std::runtime_error("Unknown type: " + type_name);
    }
}

std::unique_ptr<ExprAST> Parser::parse_variable_declaration() {
    bool is_mutable = get_current_token() == Token::Mut;
    get_next_token(); // consume 'let' or 'mut'

    if (get_current_token() != Token::Identifier) {
        throw std::runtime_error("expected identifier after let/mut");
    }

    std::string name = lexer_.get_identifier();
    get_next_token(); // consume identifier

    std::shared_ptr<Type> type;
    if (get_current_token() == Token::Colon) {
        get_next_token(); // consume ':'
        type = parse_type();
    }

    if (get_current_token() != Token::Operator || lexer_.get_operator() != '=') {
        throw std::runtime_error("expected '=' in variable declaration");
    }
    get_next_token(); // consume '='

    auto init = parse_expression();
    if (!init) {
        return nullptr;
    }

    auto var_decl = std::make_unique<VarDeclAST>(name, std::move(init), is_mutable);
    var_decl->type = type;
    return std::move(var_decl);
}

std::unique_ptr<ExprAST> Parser::parse_if_expression() {
    get_next_token(); // eat the if.

    if (get_current_token() != Token::LeftParen)
        throw std::runtime_error("expected '(' after if");
    get_next_token(); // eat the '('.

    // condition.
    auto cond = parse_expression();
    if (!cond)
        return nullptr;

    if (get_current_token() != Token::RightParen)
        throw std::runtime_error("expected ')' after if condition");
    get_next_token(); // eat the ')'.

    if (get_current_token() != Token::LeftBrace)
        throw std::runtime_error("expected '{' after if condition");
    get_next_token(); // eat the '{'.

    auto then = parse_block();
    if (!then)
        return nullptr;

    if (get_current_token() != Token::RightBrace)
        throw std::runtime_error("expected '}' after then block");
    get_next_token(); // eat the '}'.

    std::unique_ptr<ExprAST> else_expr;
    if (get_current_token() == Token::Else) {
        get_next_token(); // eat the else.
        if (get_current_token() == Token::If) {
            else_expr = parse_if_expression();
        } else {
            if (get_current_token() != Token::LeftBrace)
                throw std::runtime_error("expected '{' after else");
            get_next_token(); // eat the '{'.
            else_expr = parse_block();
            if (get_current_token() != Token::RightBrace)
                throw std::runtime_error("expected '}' after else block");
            get_next_token(); // eat the '}'.
        }
    }

    return std::make_unique<IfExprAST>(std::move(cond), std::move(then), std::move(else_expr));
}

std::unique_ptr<ExprAST> Parser::parse_struct_declaration() {
    get_next_token(); // eat the struct.

    if (get_current_token() != Token::Identifier)
        throw std::runtime_error("expected identifier after struct");
    std::string name = lexer_.get_identifier();
    get_next_token(); // eat the identifier.

    std::vector<std::string> impls;
    if (get_current_token() == Token::Impl) {
        get_next_token(); // eat the impl.
        while (get_current_token() == Token::Identifier) {
            std::string trait_name = lexer_.get_identifier();
            get_next_token(); // eat the identifier.
            // TODO: Handle `::` for nested traits.
            impls.push_back(trait_name);
            if (get_current_token() == Token::Comma) {
                get_next_token(); // eat the ','.
            } else {
                break;
            }
        }
    }

    if (get_current_token() != Token::LeftBrace)
        throw std::runtime_error("expected '{' after struct name");
    get_next_token(); // eat the '{'.

    std::vector<StructField> fields;
    while (get_current_token() != Token::RightBrace) {
        if (get_current_token() != Token::Identifier)
            throw std::runtime_error("expected identifier in struct field");
        std::string field_name = lexer_.get_identifier();
        get_next_token(); // eat the identifier.

        if (get_current_token() != Token::Colon)
            throw std::runtime_error("expected ':' after field name");
        get_next_token(); // eat the ':'.

        auto field_type = parse_type();
        fields.push_back({field_name, field_type});

        if (get_current_token() == Token::Comma) {
            get_next_token(); // eat the ','.
        } else if (get_current_token() != Token::RightBrace) {
            throw std::runtime_error("expected ',' or '}' after field");
        }
    }

    get_next_token(); // eat the '}'.

    return std::make_unique<StructDeclAST>(name, std::move(fields), std::move(impls));
}

std::unique_ptr<ExprAST> Parser::parse_while_expression() {
    get_next_token(); // eat the while.

    if (get_current_token() != Token::LeftParen)
        throw std::runtime_error("expected '(' after while");
    get_next_token(); // eat the '('.

    auto cond = parse_expression();
    if (!cond)
        return nullptr;

    if (get_current_token() != Token::RightParen)
        throw std::runtime_error("expected ')' after while condition");
    get_next_token(); // eat the ')'.

    if (get_current_token() != Token::LeftBrace)
        throw std::runtime_error("expected '{' after while condition");
    get_next_token(); // eat the '{'.

    auto body = parse_block();
    if (!body)
        return nullptr;

    if (get_current_token() != Token::RightBrace)
        throw std::runtime_error("expected '}' after while body");
    get_next_token(); // eat the '}'.

    return std::make_unique<WhileExprAST>(std::move(cond), std::move(body));
}

std::unique_ptr<ExprAST> Parser::parse_for_expression() {
    get_next_token(); // eat the for.

    if (get_current_token() != Token::LeftParen)
        throw std::runtime_error("expected '(' after for");
    get_next_token(); // eat the '('.

    auto start = parse_expression();
    if (!start)
        return nullptr;
    if (get_current_token() != Token::Semicolon)
        throw std::runtime_error("expected ';' after for start value");
    get_next_token(); // eat the ';'.

    auto cond = parse_expression();
    if (!cond)
        return nullptr;
    if (get_current_token() != Token::Semicolon)
        throw std::runtime_error("expected ';' after for condition");
    get_next_token(); // eat the ';'.

    auto step = parse_expression();
    if (!step)
        return nullptr;
    if (get_current_token() != Token::RightParen)
        throw std::runtime_error("expected ')' after for step value");
    get_next_token(); // eat the ')'.

    if (get_current_token() != Token::LeftBrace)
        throw std::runtime_error("expected '{' after for");
    get_next_token(); // eat the '{'.

    auto body = parse_block();
    if (!body)
        return nullptr;

    if (get_current_token() != Token::RightBrace)
        throw std::runtime_error("expected '}' after for body");
    get_next_token(); // eat the '}'.

    return std::make_unique<ForExprAST>(std::move(start), std::move(cond), std::move(step), std::move(body));
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
    switch (get_current_token()) {
    default:
        throw std::runtime_error("unknown token when expecting an expression");
    case Token::Identifier:
        return parse_identifier_expression();
    case Token::Number:
        return parse_number_expression();
    case Token::LeftParen:
        return parse_paren_expression();
    case Token::If:
        return parse_if_expression();
    case Token::While:
        return parse_while_expression();
    case Token::For:
        return parse_for_expression();
    case Token::Struct:
        return parse_struct_declaration();
    case Token::Let:
    case Token::Mut:
        return parse_variable_declaration();
    }
}

std::unique_ptr<ExprAST> Parser::parse_bin_op_rhs(int expr_prec,
                                                std::unique_ptr<ExprAST> lhs) {
    while (true) {
        int tok_prec = get_token_precedence();

        if (tok_prec < expr_prec)
            return lhs;

        char bin_op = lexer_.get_operator();
        get_next_token(); // eat binop

        auto rhs = parse_primary();
        if (!rhs)
            return nullptr;

        int next_prec = get_token_precedence();
        if (tok_prec < next_prec) {
            rhs = parse_bin_op_rhs(tok_prec + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        bool is_assignment = bin_op == '=';
        lhs = std::make_unique<BinaryExprAST>(bin_op, std::move(lhs),
                                              std::move(rhs), is_assignment);
    }
}

std::unique_ptr<ExprAST> Parser::parse_expression() {
    auto lhs = parse_primary();
    if (!lhs)
        return nullptr;

    return parse_bin_op_rhs(0, std::move(lhs));
}

std::unique_ptr<BlockExprAST> Parser::parse_block() {
    std::vector<std::unique_ptr<ExprAST>> expressions;
    while (get_current_token() != Token::RightBrace && get_current_token() != Token::Eof) {
        auto expr = parse_expression();
        if (!expr) {
            return nullptr;
        }
        expressions.push_back(std::move(expr));

        if (get_current_token() != Token::Semicolon) {
            throw std::runtime_error("Expected ';' after expression");
        }
        get_next_token(); // eat ';'
    }

    return std::make_unique<BlockExprAST>(std::move(expressions));
}

std::unique_ptr<PrototypeAST> Parser::parse_prototype() {
    if (get_current_token() != Token::Identifier) {
        throw std::runtime_error("Expected function name in prototype");
    }

    std::string fn_name = lexer_.get_identifier();
    get_next_token();

    if (get_current_token() != Token::LeftParen) {
        throw std::runtime_error("Expected '(' in prototype");
    }

    get_next_token();
    std::vector<std::string> arg_names;
    while (get_current_token() == Token::Identifier) {
        arg_names.push_back(lexer_.get_identifier());
        get_next_token();
    }

    if (get_current_token() != Token::RightParen) {
        throw std::runtime_error("Expected ')' in prototype");
    }

    get_next_token(); // eat ')'.

    return std::make_unique<PrototypeAST>(fn_name, std::move(arg_names));
}

std::unique_ptr<ExprAST> Parser::parse_definition() {
    get_next_token(); // eat func.
    auto proto = parse_prototype();
    if (!proto)
        return nullptr;

    if (auto e = parse_expression()) {
        return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }

    return nullptr;
}

std::unique_ptr<ExprAST> Parser::parse_import_statement() {
    get_next_token(); // eat the import.

    if (get_current_token() != Token::Identifier)
        throw std::runtime_error("expected identifier after import");
    std::string module_name = lexer_.get_identifier();
    get_next_token(); // eat the identifier.

    if (get_current_token() != Token::Semicolon)
        throw std::runtime_error("expected ';' after import statement");
    get_next_token(); // eat the ';'.

    return std::make_unique<ImportAST>(module_name);
}

std::unique_ptr<ModuleAST> Parser::parse_module() {
    std::vector<std::unique_ptr<ExprAST>> expressions;
    while (get_current_token() != Token::Eof) {
        if (get_current_token() == Token::Import) {
            expressions.push_back(parse_import_statement());
        } else if (get_current_token() == Token::Func) {
            expressions.push_back(parse_definition());
        } else if (get_current_token() == Token::Struct) {
            expressions.push_back(parse_struct_declaration());
        } else {
            expressions.push_back(parse_expression());
        }
    }
    return std::make_unique<ModuleAST>(std::move(expressions));
}
