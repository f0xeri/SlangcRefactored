//
// Created by user on 05.11.2023.
//

#include "Check.hpp"

namespace Slangc::Check {
    auto checkDecl(const DeclPtrVariant &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool;

    auto checkDecl(const ArrayDecStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return checkStmt(decl, context, errors);
    }

    auto checkDecl(const ExternFuncDecStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return checkStmt(decl, context, errors);
    }

    auto checkDecl(const FieldArrayVarDecPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const FieldVarDecPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const FieldFuncPointerStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const FuncDecStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        result = checkExpr(decl->expr->type, context, errors);
        if (result) {
            for (const auto &param: decl->expr->params) {
                result &= checkDecl(param, context, errors);
            }
        }
        if (result) {
            auto foundFunc = context.lookupFunc(decl->name, decl->expr);
            if (foundFunc) {
                auto func = std::get<FuncDecStatementPtr>(*foundFunc);
                auto errorMsg = std::string("Function '") + decl->name + "' with the same signature already exists.";
                if (!compareTypes(func->expr->type, decl->expr->type)) {
                    errorMsg += " Functions that differ only in their return type cannot be overloaded.";
                }
                errors.emplace_back(errorMsg, decl->loc, false, false);
                result = false;
            }
            context.insert(decl->name, decl);
            context.enterScope();
            for (const auto &param: decl->expr->params) {
                context.insert(param->name, param);
            }
            result &= checkBlockStmt(decl->block.value(), context, errors);
        }

        return result;
    }

    auto checkDecl(const FuncParamDecStmtPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        result &= checkExpr(decl->type, context, errors);
                /*if (context.lookup(stmt->name)) {
            errors.emplace_back("Variable with name '" + stmt->name + "' already exists.", stmt->loc, false, false);
            result = false;
        }
        context.insert(stmt->name, stmt);*/
        return result;
    }

    auto checkDecl(const FuncPointerStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const MethodDecPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const TypeDecStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const VarDecStatementPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        return result;
    }

    auto checkDecl(const ModuleDeclPtr &decl, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        for (auto &stmt : decl->block->statements) {
            result &= checkStmt(stmt, context, errors);
        }
        return result;
    }

    auto checkDecl(const DeclPtrVariant &expr, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return std::visit([&](const auto &expr) {
            return checkDecl(expr, context, errors);
        }, expr);
    }
}