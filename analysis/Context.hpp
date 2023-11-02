//
// Created by f0xeri on 26.05.2023.
//

#ifndef SLANGCREFACTORED_CONTEXT_HPP
#define SLANGCREFACTORED_CONTEXT_HPP

#include <memory>
#include "parser/Scope.hpp"

namespace Slangc {
    class Context {
        std::shared_ptr<Scope> currScope = std::make_unique<Scope>();
    public:
        std::vector<TypeDecStatementPtr> types;
        std::vector<VarDecStatementPtr> global_vars;
        std::vector<FuncDecStatementPtr> funcs;
        std::vector<ExternFuncDecStatementPtr> extern_funcs;
        auto enterScope() -> void {
            currScope = std::make_unique<Scope>(std::move(currScope));
        }

        auto exitScope() -> void {
            currScope = currScope->parent;
        }

        auto insert(const std::string& declName, const DeclPtrVariant &declarationNode) -> void {
            currScope->insert(declName, declarationNode);
        }

        auto lookup(std::string_view name) const -> const DeclPtrVariant* {
            return currScope->lookup(name);
        }

        auto getVarDeclType(std::string_view name) const -> ExprPtrVariant {
            auto* node = currScope->lookup(name);
        }

        static bool isBuiltInType(std::string_view name) {
            return name == "integer" || name == "float" || name == "real" || name == "boolean" || name == "string" || name == "character";
        }
    };
}

#endif //SLANGCREFACTORED_CONTEXT_HPP