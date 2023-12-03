//
// Created by user on 19.11.2023.
//

#include "CodeGen.hpp"
#include "CodeGenContext.hpp"
#include "parser/AST.hpp"

namespace Slangc {
    Value* typeCast(Value* value, Type* type, CodeGenContext &context, std::vector<ErrorMessage> &errors, SourceLoc loc) {
        if (value->getType() == type) return value;
        if (value->getType()->isIntegerTy() && type->isIntegerTy()) {
            return context.builder->CreateIntCast(value, type, true);
        }
        if (value->getType()->isIntegerTy() && type->isFloatingPointTy()) {
            return context.builder->CreateSIToFP(value, type);
        }
        if (value->getType()->isFloatingPointTy() && type->isIntegerTy()) {
            return context.builder->CreateFPToSI(value, type);
        }
        if (value->getType()->isFloatingPointTy() && type->isFloatingPointTy()) {
            return context.builder->CreateFPCast(value, type);
        }
        if (value->getType()->isPointerTy() && type->isIntegerTy()) {
            return context.builder->CreatePtrToInt(value, type);
        }
        std::string type_str = "";
        raw_string_ostream rso(type_str);
        value->getType()->print(rso);
        rso << " to ";
        type->print(rso);
        errors.emplace_back(context.context.filename, "Cannot cast from " + type_str + " .", loc, false, true);
        return nullptr;
    }

    Type* getIRType(const std::string& type, CodeGenContext& context) {
        if (type == "integer")
            return Type::getInt32Ty(*context.llvmContext);
        if (type == "real")
            return Type::getDoubleTy(*context.llvmContext);
        if (type == "float")
            return Type::getFloatTy(*context.llvmContext);
        if (type == "boolean")
            return Type::getInt1Ty(*context.llvmContext);
        if (type == "character")
            return Type::getInt8Ty(*context.llvmContext);
        if (type == "void" || type == "")
            return Type::getVoidTy(*context.llvmContext);
        if (context.allocatedClasses.contains(type))
            return context.allocatedClasses[type]->getPointerTo();
        return nullptr;
    }

    Type* getIRTypeForSize(const std::string& type, CodeGenContext& context) {
        if (type == "integer")
            return Type::getInt32Ty(*context.llvmContext);
        if (type == "real")
            return Type::getDoubleTy(*context.llvmContext);
        if (type == "float")
            return Type::getFloatTy(*context.llvmContext);
        if (type == "boolean")
            return Type::getInt1Ty(*context.llvmContext);
        if (type == "character")
            return Type::getInt8Ty(*context.llvmContext);
        if (type == "void" || type == "")
            return Type::getVoidTy(*context.llvmContext);
        if (context.allocatedClasses.contains(type))
            return context.allocatedClasses[type];
        return nullptr;
    }

    Type* getIRPtrType(const std::string& type, CodeGenContext& context) {
        return getIRType(type, context)->getPointerTo();
    }

    Type* getIRType(const ExprPtrVariant& expr, CodeGenContext& context) {
        if (auto type = std::get_if<TypeExprPtr>(&expr)) {
            return getIRType(type->get()->type, context);
        }
        if (auto arr = std::get_if<ArrayExprPtr>(&expr)) {
            return getIRType(arr->get()->type, context)->getPointerTo();
        }
        if (auto func = std::get_if<FuncExprPtr>(&expr)) {
            return getFuncType(*func, context)->getPointerTo();
        }
        return nullptr;
    }

    Type* getIRTypeForSize(const ExprPtrVariant& expr, CodeGenContext& context) {
        if (auto type = std::get_if<TypeExprPtr>(&expr)) {
            return getIRTypeForSize(type->get()->type, context);
        }
        if (auto arr = std::get_if<ArrayExprPtr>(&expr)) {
            return getIRTypeForSize(arr->get()->type, context)->getPointerTo();
        }
        if (auto func = std::get_if<FuncExprPtr>(&expr)) {
            return getFuncType(*func, context)->getPointerTo();
        }
        return nullptr;
    }

    Value* createMalloc(const std::string &type, Value* var, CodeGenContext &context) {
        auto intType = Type::getInt32Ty(*context.llvmContext);
        auto structType = context.allocatedClasses[type];
        auto mallocCall = context.builder->CreateMalloc(intType, structType, ConstantInt::get(intType, context.module->getDataLayout().getTypeAllocSize(structType)), nullptr);
        context.builder->CreateStore(mallocCall, var);
        return var;
    }

    void createMallocLoops(int i, ArrayExprPtr &array, int indicesCount, Value *var, std::vector<Value*> jvars, std::vector<Value*> sizes, CodeGenContext &context, std::vector<ErrorMessage> &errors) {
        if (i == indicesCount) return;
        auto intType = Type::getInt32Ty(*context.llvmContext);
        auto func = context.builder->GetInsertBlock()->getParent();
        auto whileCheck = BasicBlock::Create(*context.llvmContext, "whilecheck", func);
        auto whileIter = BasicBlock::Create(*context.llvmContext, "whileiter", func);
        auto whileEnd = BasicBlock::Create(*context.llvmContext, "whileend", func);
        context.builder->CreateBr(whileCheck);
        context.pushBlock(whileCheck);
        context.builder->SetInsertPoint(whileCheck);

        auto jvar = context.builder->CreateLoad(intType, jvars[i]);
        auto arraySize = sizes[i - 1];
        auto cmp = context.builder->CreateICmpSLT(jvar, arraySize, "j_less_arrSz");
        context.builder->CreateCondBr(cmp, whileIter, whileEnd);
        context.popBlock();
        context.pushBlock(whileIter);
        context.builder->SetInsertPoint(whileIter);

        auto arrayType = getIRType(array->type, context);
        auto loadArrType = getIRType(array, context);

        auto allocSize = context.builder->CreateMul(sizes[i], ConstantInt::get(intType, context.module->getDataLayout().getTypeAllocSize(getIRTypeForSize(array->type, context))));
        auto mallocCall = context.builder->CreateMalloc(intType, arrayType, allocSize, nullptr);

        auto arrLoad = context.builder->CreateLoad(loadArrType, var);
        Value* arrPtr = nullptr;
        for (int k = 1; k <= i; ++k) {
            jvar = context.builder->CreateLoad(intType, jvars[k]);
            auto sext = context.builder->CreateSExt(jvar, Type::getInt64Ty(*context.llvmContext));
            arrPtr = context.builder->CreateInBoundsGEP(loadArrType, arrLoad, sext);
            arrLoad = context.builder->CreateLoad(loadArrType, arrPtr);
        }
        context.builder->CreateStore(mallocCall, arrPtr);

        if (i == indicesCount - 1) {
            jvar = context.builder->CreateLoad(intType, jvars[i]);
            auto add = context.builder->CreateAdd(jvar, ConstantInt::get(intType, 1));
            context.builder->CreateStore(add, jvars[i]);
        }
        else context.builder->CreateStore(ConstantInt::get(intType, 0), jvars[i + 1]);

        auto nextArray = std::get_if<ArrayExprPtr>(&array->type);
        createMallocLoops(i + 1, *nextArray, indicesCount, var, jvars, sizes, context, errors);

        context.builder->CreateBr(whileCheck);
        context.popBlock();
        context.ret(whileEnd);
        context.builder->SetInsertPoint(whileEnd);

        if (i > 1) {
            auto endJvar = context.builder->CreateLoad(intType, jvars[i - 1]);
            auto endAdd = context.builder->CreateAdd(endJvar, ConstantInt::get(intType, 1));
            context.builder->CreateStore(endAdd, jvars[i - 1]);
        }
    }

    Value* createArrayMalloc(ArrayExprPtr& array, Value* var, CodeGenContext &context, std::vector<ErrorMessage> &errors) {
        auto intType = Type::getInt32Ty(*context.llvmContext);
        auto structType = getIRTypeForSize(array->type, context);
        auto temp = context.loadAsRvalue;
        context.loadAsRvalue = true;
        auto arraySize = processNode(array->size, context, errors);
        context.loadAsRvalue = temp;
        auto allocSize = context.builder->CreateMul(arraySize, ConstantInt::get(intType, context.module->getDataLayout().getTypeAllocSize(structType)));
        auto mallocCall = context.builder->CreateMalloc(intType, structType, allocSize, nullptr);
        auto indicesCount = array->getIndicesCount();
        context.builder->CreateStore(mallocCall, var);
        if (indicesCount == 1) return var;
        std::vector<Value*> sizes;
        sizes.reserve(indicesCount);
        sizes.push_back(arraySize);
        auto currArray = array->type;
        context.loadAsRvalue = true;
        while (auto arr = std::get_if<ArrayExprPtr>(&currArray)) {
            sizes.push_back(processNode(arr->get()->size, context, errors));
            currArray = arr->get()->type;
        }
        context.loadAsRvalue = temp;
        std::vector<Value*> jvars;
        jvars.reserve(indicesCount);
        for (int i = 0; i < indicesCount; ++i) {
            jvars.push_back(context.builder->CreateAlloca(intType, nullptr, "j" + std::to_string(i)));
            context.builder->CreateStore(ConstantInt::get(intType, 0), jvars[i]);
        }
        int i = 1;
        createMallocLoops(i, std::get<ArrayExprPtr>(array->type), indicesCount, var, jvars, sizes, context, errors);
        return var;
    }

    Function* createDefaultConstructor(TypeDecStatementNode* type, CodeGenContext &context, std::vector<ErrorMessage>& errors, bool isImported) {
        auto structType = getIRType(type->name, context);
        auto intType = Type::getInt32Ty(*context.llvmContext);
        auto constructorType = FunctionType::get(Type::getVoidTy(*context.llvmContext), {structType->getPointerTo()}, false);
        auto constructor = Function::Create(constructorType, Function::ExternalLinkage, type->name + "._default_constructor", context.module.get());
        if (isImported) return constructor;
        auto block = BasicBlock::Create(*context.llvmContext, "entry", constructor);
        context.builder->SetInsertPoint(block);
        context.pushBlock(block);

        auto typeAlloc = context.builder->CreateAlloca(structType);
        auto storeThis = context.builder->CreateStore(constructor->getArg(0), typeAlloc);
        auto loadThis = context.builder->CreateLoad(structType->getPointerTo(), typeAlloc);
        context.currentTypeLoad = loadThis;
        for (auto &&field : type->fields) {
            processNode(field, context, errors);
        }
        context.builder->CreateRetVoid();
        context.popBlock();
        return constructor;
    }

    FunctionType* getFuncType(const FuncExprPtr& funcExpr, CodeGenContext &context) {
        std::vector<Type*> params;
        for (auto& param : funcExpr->params) {
            if (param->parameterType == In || param->parameterType == None)
                params.push_back(getIRType(param->type, context));
            else
                params.push_back(getIRType(param->type, context)->getPointerTo());
        }
        return FunctionType::get(getIRType(funcExpr->type, context), params, false);
    }

    std::string typeToMangledString(const ExprPtrVariant& type, ParameterType parameterType, bool newType = false) {
        std::string result;
        if (newType) result += "%";
        switch (parameterType) {
            case Out:
            case Var:
                result += "*";
                break;
            default:
                break;
        }
        if (auto typePtr = std::get_if<TypeExprPtr>(&type)) {
            return result + typePtr->get()->type;
        }
        if (auto typePtr = std::get_if<ArrayExprPtr>(&type)) {
            result += "arr[]";
            return result + typeToMangledString(typePtr->get()->type, None);
        }
        if (std::holds_alternative<FuncExprPtr>(type)) {
            auto funcExpr = std::get<FuncExprPtr>(type);
            result += "func(";
            for (int i = 0; i < funcExpr->params.size(); i++) {
                result += typeToMangledString(funcExpr->params[i]->type, funcExpr->params[i]->parameterType);
                if (i != funcExpr->params.size() - 1) result += ", ";
            }
            result += "):" + typeToMangledString(funcExpr->type, None);
            return result;
        }
        return "unknown";
    }

    std::string getMangledFuncName(const FuncExprPtr& funcExpr) {
        std::string result = "_";
        for (auto& param : funcExpr->params) {
            result += typeToMangledString(param->type, param->parameterType, true);
        }
        result += "_";
        result += typeToMangledString(funcExpr->type, None, true);
        return result;
    }

    Function* getFuncFromExpr(const DeclPtrVariant& funcExpr, CodeGenContext &context) {
        if (auto func = std::get_if<FuncDecStatementPtr>(&funcExpr)) {
            if (func->get()->isExtern) return context.module->getFunction(func->get()->name);
            return context.module->getFunction(func->get()->name + "." + getMangledFuncName(func->get()->expr));
        }
        if (auto method = std::get_if<MethodDecPtr>(&funcExpr)) {
            return context.module->getFunction(method->get()->name + "." + getMangledFuncName(method->get()->expr));
        }
        return nullptr;
    }
}
