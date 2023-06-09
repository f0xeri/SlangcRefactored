//
// Created by f0xeri on 27.05.2023.
//

#ifndef SLANGCREFACTORED_ASTFWDDECL_HPP
#define SLANGCREFACTORED_ASTFWDDECL_HPP

#include <memory>
#include <variant>

namespace Slangc {

    struct ArrayExprNode;
    struct BooleanExprNode;
    struct CharExprNode;
    struct FloatExprNode;
    struct FuncExprNode;
    struct IntExprNode;
    struct NilExprNode;
    struct OperatorExprNode;
    struct RealExprNode;
    struct StringExprNode;
    struct UnaryOperatorExprNode;
    struct VarExprNode;
    struct IndexesExprNode;
    struct IndexExprNode;
    struct CallExprNode;
    struct AccessExprNode;
    struct TypeExprNode;

    using ArrayExprPtr = std::shared_ptr<ArrayExprNode>;
    using BooleanExprPtr = std::shared_ptr<BooleanExprNode>;
    using CharExprPtr = std::shared_ptr<CharExprNode>;
    using FloatExprPtr = std::shared_ptr<FloatExprNode>;
    using FuncExprPtr = std::shared_ptr<FuncExprNode>;
    using IntExprPtr = std::shared_ptr<IntExprNode>;
    using NilExprPtr = std::shared_ptr<NilExprNode>;
    using OperatorExprPtr = std::shared_ptr<OperatorExprNode>;
    using RealExprPtr = std::shared_ptr<RealExprNode>;
    using StringExprPtr = std::shared_ptr<StringExprNode>;
    using UnaryOperatorExprPtr = std::shared_ptr<UnaryOperatorExprNode>;
    using VarExprPtr = std::shared_ptr<VarExprNode>;
    using IndexesExprPtr = std::shared_ptr<IndexesExprNode>;
    using IndexExprPtr = std::shared_ptr<IndexExprNode>;
    using CallExprPtr = std::shared_ptr<CallExprNode>;
    using AccessExprPtr = std::shared_ptr<AccessExprNode>;
    using TypeExprPtr = std::shared_ptr<TypeExprNode>;

    using ExprPtrVariant
            = std::variant<ArrayExprPtr, BooleanExprPtr, CharExprPtr, FloatExprPtr,
            FuncExprPtr, IntExprPtr, NilExprPtr, OperatorExprPtr, RealExprPtr, StringExprPtr,
            UnaryOperatorExprPtr, VarExprPtr, IndexesExprPtr, IndexExprPtr, CallExprPtr, AccessExprPtr, TypeExprPtr>;

    using VarExprPtrVariant = std::variant<VarExprPtr, IndexesExprPtr, IndexExprPtr>;

    struct ArrayDecStatementNode;
    struct ExternFuncDecStatementNode;
    struct FieldArrayVarDecNode;
    struct FieldVarDecNode;
    struct FuncDecStatementNode;
    struct FuncParamDecStatementNode;
    struct FuncPointerStatementNode;
    struct MethodDecNode;
    struct TypeDecStatementNode;
    struct VarDecStatementNode;
    struct ModuleDeclNode;

    using ArrayDecStatementPtr = std::shared_ptr<ArrayDecStatementNode>;
    using ExternFuncDecStatementPtr = std::shared_ptr<ExternFuncDecStatementNode>;
    using FieldArrayVarDecPtr = std::shared_ptr<FieldArrayVarDecNode>;
    using FieldVarDecPtr = std::shared_ptr<FieldVarDecNode>;
    using FuncDecStatementPtr = std::shared_ptr<FuncDecStatementNode>;
    using FuncParamDecStmtPtr = std::shared_ptr<FuncParamDecStatementNode>;
    using FuncPointerStatementPtr = std::shared_ptr<FuncPointerStatementNode>;
    using MethodDecPtr = std::shared_ptr<MethodDecNode>;
    using TypeDecStatementPtr = std::shared_ptr<TypeDecStatementNode>;
    using VarDecStatementPtr = std::shared_ptr<VarDecStatementNode>;
    using ModuleDeclPtr = std::shared_ptr<ModuleDeclNode>;

    using DeclPtrVariant
            = std::variant<ArrayDecStatementPtr, ExternFuncDecStatementPtr, FieldArrayVarDecPtr, FieldVarDecPtr,
            FuncDecStatementPtr, FuncParamDecStmtPtr, FuncPointerStatementPtr, MethodDecPtr,
            TypeDecStatementPtr, VarDecStatementPtr, ModuleDeclPtr>;

    struct AssignExprNode;
    struct DeleteStmtNode;
    struct ElseIfStatementNode;
    struct IfStatementNode;
    struct InputStatementNode;
    struct OutputStatementNode;
    struct ReturnStatementNode;
    struct WhileStatementNode;
    struct BlockStmtNode;

    using AssignExprPtr = std::shared_ptr<AssignExprNode>;
    using DeleteStmtPtr = std::shared_ptr<DeleteStmtNode>;
    using ElseIfStatementPtr = std::shared_ptr<ElseIfStatementNode>;
    using IfStatementPtr = std::shared_ptr<IfStatementNode>;
    using InputStatementPtr = std::shared_ptr<InputStatementNode>;
    using OutputStatementPtr = std::shared_ptr<OutputStatementNode>;
    using ReturnStatementPtr = std::shared_ptr<ReturnStatementNode>;
    using WhileStatementPtr = std::shared_ptr<WhileStatementNode>;
    using BlockStmtPtr = std::shared_ptr<BlockStmtNode>;

    using StmtPtrVariant
            = std::variant<AssignExprPtr, CallExprPtr, ArrayDecStatementPtr, ExternFuncDecStatementPtr,
            FieldArrayVarDecPtr, FieldVarDecPtr, FuncDecStatementPtr, FuncParamDecStmtPtr, FuncPointerStatementPtr,
            MethodDecPtr, TypeDecStatementPtr, VarDecStatementPtr, DeleteStmtPtr, ElseIfStatementPtr, IfStatementPtr,
            InputStatementPtr, OutputStatementPtr, ReturnStatementPtr, VarExprPtr,
            IndexesExprPtr, IndexExprPtr, WhileStatementPtr, BlockStmtPtr>;

    struct Type {
        std::string_view typeName;
        bool isArray = false;
        bool isPointer = false;
        uint64_t indicesCount = 0;
    };
}

#endif //SLANGCREFACTORED_ASTFWDDECL_HPP
