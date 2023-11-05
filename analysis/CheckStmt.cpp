//
// Created by user on 02.11.2023.
//

#include "Check.hpp"

namespace Slangc::Check {
    auto checkStmt(const StmtPtrVariant &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool;
    auto checkBlockStmt(const BlockStmtPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        for (const auto &statement: stmt->statements) {
            result &= checkStmt(statement, context, errors);
        }
        return result;
    }

    auto checkStmt(const VarDecStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        if (!context.lookup(stmt->typeExpr.type) && !Context::isBuiltInType(stmt->typeExpr.type)) {
            errors.emplace_back("Type '" + stmt->typeExpr.type + "' does not exist.", stmt->loc, false, false);
            result = false;
        }
        if (context.lookup(stmt->name)) {
            errors.emplace_back("Variable with name '" + stmt->name + "' already exists.", stmt->loc, false, false);
            result = false;
        }
        context.insert(stmt->name, stmt);
        if (stmt->expr.has_value() && result) {
            if (!checkExpr(stmt->expr.value(), context, errors)) {
                return false;
            }
            auto exprType = std::get<TypeExprPtr>(getExprType(stmt->expr.value(), context))->type;
            // TODO: check if conversion is possible
            if (stmt->typeExpr.type != exprType) {
                errors.emplace_back("Type mismatch: cannot assign '" + exprType + "' to '" + stmt->typeExpr.type + "'.", stmt->loc, false, false);
                result = false;
            }
        }
        return result;
    }

    auto checkStmt(const ArrayDecStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        if (!checkExpr(stmt->expr, context, errors)) result = false;
        if (context.lookup(stmt->name)) {
            errors.emplace_back("Variable with name '" + stmt->name + "' already exists.", stmt->loc, false, false);
            result = false;
        }
        context.insert(stmt->name, stmt);
        return result;
    }

    auto checkStmt(const FuncPointerStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        if (context.lookup(stmt->name)) {
            errors.emplace_back("Variable with name '" + stmt->name + "' already exists.", stmt->loc, false, false);
            result = false;
        }
        result &= checkExpr(stmt->expr, context, errors);
        context.insert(stmt->name, stmt);
        if (stmt->assignExpr.has_value()) {
            result &= checkExpr(stmt->assignExpr.value(), context, errors);
            if (result) {
                auto leftType = stmt->expr->getType(context);
                auto rightType = getExprType(stmt->assignExpr.value(), context);
                if (leftType != rightType) {
                    errors.emplace_back("Type mismatch: cannot assign.", stmt->loc, false, false);
                    result = false;
                }
            }
        }
        return result;
    }

    auto checkStmt(const IfStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const WhileStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const OutputStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const InputStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const AssignExprPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const ReturnStatementPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const CallExprPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const DeleteStmtPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return true;
    }

    auto checkStmt(const FuncParamDecStmtPtr &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        bool result = true;
        result &= checkExpr(stmt->type, context, errors);
        return result;
    }

    auto checkStmt(const StmtPtrVariant &stmt, Context &context, std::vector<ErrorMessage> &errors) -> bool {
        return std::visit([&](const auto &stmt) {
            return checkStmt(stmt, context, errors);
        }, stmt);
    }
};