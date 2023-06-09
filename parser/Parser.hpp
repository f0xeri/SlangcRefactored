//
// Created by f0xeri on 04.05.2023.
//

#ifndef SLANGCREFACTORED_PARSER_HPP
#define SLANGCREFACTORED_PARSER_HPP

#include <map>
#include <algorithm>
#include <vector>
#include "lexer/TokenType.hpp"
#include "Scope.hpp"
#include "lexer/Lexer.hpp"
#include "AST.hpp"
#include "analysis/BasicAnalysis.hpp"
#include <CompilerOptions.hpp>

namespace Slangc {

    using ParserExprResult = std::optional<ExprPtrVariant>;
    using ParserStmtResult = std::optional<StmtPtrVariant>;
    using ParserDeclResult = std::optional<DeclPtrVariant>;

    class Parser {
        BasicAnalysis &analysis;
    public:
        explicit Parser(std::vector<Token> tokens, std::vector<ErrorMessage> &errors, const CompilerOptions &options, BasicAnalysis &analysis)
                : analysis(analysis), options(options), errors(errors), tokens(std::move(tokens)) {
            token = this->tokens.cbegin();
            currentScope = std::make_shared<Scope>();

        }

        const CompilerOptions &options;
        std::vector<ErrorMessage> &errors;
        ModuleDeclPtr moduleAST;
        bool hasError = false;

        auto parse() -> bool;
        auto parseImports() -> bool;
        auto parseTypeName() -> std::optional<std::string>;
        auto parseType() -> std::optional<ExprPtrVariant>;

        auto parseExpr() -> std::optional<ExprPtrVariant>;
        auto parseOr() -> std::optional<ExprPtrVariant>;
        auto parseAnd() -> std::optional<ExprPtrVariant>;
        auto parseEquality() -> std::optional<ExprPtrVariant>;
        auto parseCmp() -> std::optional<ExprPtrVariant>;
        auto parseAddSub() -> std::optional<ExprPtrVariant>;
        auto parseMulDiv() -> std::optional<ExprPtrVariant>;
        auto parseUnary() -> std::optional<ExprPtrVariant>;
        auto parseCall() -> std::optional<ExprPtrVariant>;
        auto parsePrimary() -> std::optional<ExprPtrVariant>;
        auto parseVar() -> std::optional<ExprPtrVariant>;

        auto parseVarExpr() -> std::optional<ExprPtrVariant>;
        auto parseAccess() -> std::optional<ExprPtrVariant>;

        auto parseModuleDecl() -> std::optional<ModuleDeclPtr>;
        auto parseBlockStmt(const std::string& name, std::vector<FuncParamDecStmtPtr> *args = nullptr) -> std::optional<BlockStmtPtr>;
        auto parseVarStmt() -> std::optional<StmtPtrVariant>;
        auto parseIfStmt() -> std::optional<StmtPtrVariant>;
        auto parseWhileStmt() -> std::optional<StmtPtrVariant>;
        auto parseOutputStmt() -> std::optional<StmtPtrVariant>;
        auto parseInputStmt() -> std::optional<StmtPtrVariant>;
        auto parseLetStmt() -> std::optional<StmtPtrVariant>;
        auto parseReturnStmt() -> std::optional<StmtPtrVariant>;
        auto parseCallStmt() -> std::optional<StmtPtrVariant>;
        auto parseDeleteStmt() -> std::optional<StmtPtrVariant>;

        auto parseVarDecl(bool isGlobal) -> std::optional<DeclPtrVariant>;
        auto parseFuncParams(bool named = true) -> std::optional<std::vector<FuncParamDecStmtPtr>>;
        auto parseFuncDecl() -> std::optional<DeclPtrVariant>;

    private:
        std::vector<Token> tokens;
        std::shared_ptr<Scope> currentScope;
        std::vector<Token>::const_iterator token;

        auto advance() -> Token {
            if (token->type == TokenType::EndOfFile) {
                errors.emplace_back("Unexpected EOF token.", token->location);
                hasError = true;
                return *token;
            }
            token++;
            return *token;
        }

        void error() {
            errors.emplace_back("Unexpected token " + std::string(Lexer::getTokenName(*token)) + ".", token->location);
            hasError = true;
        }

        bool expect(TokenType tokenType) {
            if (token->type != tokenType) {
                errors.emplace_back(std::string("Unexpected token ") + std::string(Lexer::getTokenName(*token)) + std::string(", expected ") + std::string(Lexer::getTokenName(tokenType)) + ".", token->location);
                hasError = true;
                return false;
            }
            return true;
        }

        bool expect(std::initializer_list<TokenType> tokenTypes) {
            if (std::none_of(tokenTypes.begin(), tokenTypes.end(), [this](TokenType tokenType) { return token->type == tokenType; })) {
                std::string err = "Unexpected token ";
                err += Lexer::getTokenName(*token);
                err += ", expected one of: ";
                for (auto it = tokenTypes.begin(); it != tokenTypes.end(); ++it) {
                    err += Lexer::getTokenName(*it);
                    if (it != tokenTypes.end() - 1) {
                        err += ", ";
                    }
                }
                err += ".";
                errors.emplace_back(err, token->location);
                hasError = true;
                return false;
            }
            return true;
        }

        bool match(TokenType tokenType) {
            if (token->type == tokenType) {
                advance();
                return true;
            }
            return false;
        }

        bool match(std::initializer_list<TokenType> tokenTypes) {
            if (std::any_of(tokenTypes.begin(), tokenTypes.end(), [this](TokenType tokenType) { return token->type == tokenType; })) {
                advance();
                return true;
            }
            return false;
        }

        auto prevToken() -> Token {
            return *(token - 1);
        }

        auto consume(TokenType tokenType) -> Token {
            expect(tokenType);
            Token tok = *token;
            advance();
            return tok;
        }

        static auto oneOfDefaultTypes(std::string_view name) -> bool {
            return (name == "integer" || name == "float" || name == "real" || name == "boolean" || name == "character");
        }
    };

} // Slangc

#endif //SLANGCREFACTORED_PARSER_HPP
