//
// Created by f0xeri on 05.05.2023.
//

#ifndef SLANGCREFACTORED_CODEGENCONTEXT_HPP
#define SLANGCREFACTORED_CODEGENCONTEXT_HPP

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Pass.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/IR/DIBuilder.h>
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "check/Context.hpp"
#include "parser/ASTFwdDecl.hpp"
#include "DebugInfoBuilder.hpp"

namespace Slangc {
    using namespace llvm;
    struct CodeGenBlock {
        BasicBlock* block;
        std::map<std::string, Value*> locals;
        std::map<std::string, DeclPtrVariant> localsDecls;
        explicit CodeGenBlock(BasicBlock* block) : block(block) {}
    };

    class CodeGenContext {
    public:
        std::unique_ptr<LLVMContext> llvmContext;
        std::unique_ptr<Module> module;
        std::unique_ptr<TargetMachine> targetMachine;
        std::unique_ptr<IRBuilder<>> builder;
        //std::unique_ptr<DIBuilder> debugBuilder;
        std::unique_ptr<DebugInfoBuilder> debugBuilder;
        std::vector<CodeGenBlock*> blocks;
        std::map<std::string, Value*> globalVars;
        std::map<std::string, DeclPtrVariant> globalVarsDecls;
        Context& context;

        std::map<std::string, llvm::StructType *> allocatedClasses;
        std::map<std::string, TypeDecStmtPtr> allocatedClassesDecls;
        LoadInst* currentTypeLoad = nullptr;
        std::optional<FuncExprPtr> currentFuncSignature = std::nullopt;
        Type* currentReturnType = nullptr;
        bool loadValue = false;
        bool currentDeclImported = false;
        bool debug = true;

        explicit CodeGenContext(Context &context) : context(context) {
            InitializeAllTargetInfos();
            InitializeAllTargets();
            InitializeAllTargetMCs();
            InitializeAllAsmParsers();
            InitializeAllAsmPrinters();

            // only for x86
            //LLVMInitializeX86TargetInfo();
            //LLVMInitializeX86Target();
            //LLVMInitializeX86TargetMC();
            //LLVMInitializeX86AsmParser();
            //LLVMInitializeX86AsmPrinter();
            std::string targetTriple;
#ifdef _WIN32
            targetTriple = "x86_64-w64-windows-gnu";
#else
            targetTriple = sys::getDefaultTargetTriple();
#endif
            std::string error;
            auto target = TargetRegistry::lookupTarget(targetTriple, error);

            llvmContext = std::make_unique<LLVMContext>();
            module = std::make_unique<Module>(context.moduleName, *llvmContext);
            module->setSourceFileName(context.filename);
            module->setTargetTriple(targetTriple);
            TargetOptions opt;
            targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(targetTriple, "generic", "", opt, Reloc::PIC_));
            module->setDataLayout(targetMachine->createDataLayout());
            builder = std::make_unique<IRBuilder<>>(*llvmContext);
            if (debug) {
                debugBuilder = std::make_unique<DebugInfoBuilder>(module->getDataLayout(), context.filename, std::make_unique<DIBuilder>(*module), *builder);
                module->addModuleFlag(Module::Warning, "Debug Info Version", DEBUG_METADATA_VERSION);
            }

        }
        ~CodeGenContext() = default;

        auto startMainFunc() -> llvm::Function* {
            auto mainFuncType = llvm::FunctionType::get(Type::getInt32Ty(*llvmContext), false);
            auto mainFunc = llvm::Function::Create(mainFuncType, Function::ExternalLinkage, "main", module.get());
            auto mainBlock = BasicBlock::Create(*llvmContext, "entry", mainFunc);
            builder->SetInsertPoint(mainBlock);
            pushBlock(mainBlock);
            if (debug) {
                auto dbgFunc = debugBuilder->createMainFunction(*this);
                mainFunc->setSubprogram(dbgFunc);
                debugBuilder->lexicalBlocks.push_back(dbgFunc);
            }
            return mainFunc;
        }

        void endMainFunc() {
            builder->CreateRet(ConstantInt::get(Type::getInt32Ty(*llvmContext), 0));
            popBlock();
            if (debug) {
                debugBuilder->lexicalBlocks.pop_back();
                debugBuilder->finalize();
            }
        }

        void pushBlock(BasicBlock *block) {
            blocks.push_back(new CodeGenBlock(block));
        }

        void popBlock() {
            auto top = blocks.back();
            blocks.pop_back();
            delete top;
        }

        void ret(BasicBlock* block) {
            blocks.back()->block = block;
        }

        BasicBlock* currentBlock() {
            return blocks.back()->block;
        }

        Value* localsLookup(const std::string &name) const {
            Value *value = nullptr;
            for (auto &b : blocks) {
                if (b->locals.contains(name)) {
                    value = b->locals[name];
                    break;
                }
            }
            return value;
        }

        std::optional<DeclPtrVariant> localsDeclsLookup(const std::string &name) const {
            std::optional<DeclPtrVariant> value = std::nullopt;
            for (auto &b : blocks) {
                if (b->localsDecls.contains(name)) {
                    value = b->localsDecls[name];
                    break;
                }
            }
            return value;
        }

        std::map<std::string, Value*>& locals() {
            return blocks.back()->locals;
        }

        std::map<std::string, DeclPtrVariant>& localsDecls() {
            return blocks.back()->localsDecls;
        }

        std::map<std::string, llvm::Value*>& globals() {
            return globalVars;
        }
        std::map<std::string, DeclPtrVariant>& globalsDecls() {
            return globalVarsDecls;
        }

        std::optional<DeclPtrVariant> globalsDeclsLookup(const std::string &name) const {
            return globalVarsDecls.contains(name) ? std::make_optional(globalVarsDecls.at(name)) : std::nullopt;
        }
    };
    Type* getIRType(const std::string& type, CodeGenContext& context);
    Type* getIRPtrType(const std::string& type, CodeGenContext& context);
    Type* getIRType(const ExprPtrVariant& expr, CodeGenContext& context);
    Type* getIRPtrType(const ExprPtrVariant& expr, CodeGenContext& context);
    Type* getIRTypeForSize(const std::string& type, CodeGenContext& context);
    Type* getIRTypeForSize(const ExprPtrVariant& expr, CodeGenContext& context);
    DIType* getDebugType(const std::string& type, CodeGenContext& context);
    DIType* getDebugType(const ExprPtrVariant& expr, CodeGenContext& context, std::vector<ErrorMessage> &errors);
    Value* createMalloc(const std::string &type, Value* var, CodeGenContext &context);
    Function* createDefaultConstructor(TypeDecStatementNode* type, CodeGenContext &context, std::vector<ErrorMessage>& errors, bool isImported);
    Value* typeCast(Value* value, Type* type, CodeGenContext &context, std::vector<ErrorMessage> &errors, SourceLoc loc);
    Value* createArrayMalloc(ArrayExprPtr& array, Value* var, CodeGenContext &context, std::vector<ErrorMessage> &errors);
    void callArrayElementsConstructors(ArrayExprPtr& array, Value* var, Value* size, CodeGenContext &context, std::vector<ErrorMessage> &errors);
    void createMallocLoops(int i, ArrayExprPtr &array, int indicesCount, Value *var, std::vector<Value*> jvars, std::vector<Value*> sizes, CodeGenContext &context, std::vector<ErrorMessage> &errors);
    void createVTable(TypeDecStatementNode* type, CodeGenContext &context, std::vector<ErrorMessage>& errors);
    FunctionType* getFuncType(const FuncExprPtr& funcExpr, CodeGenContext &context);
    std::string typeToMangledString(const ExprPtrVariant& type, ParameterType parameterType, bool newType);
    std::string getMangledFuncName(const FuncExprPtr& funcExpr);
    Function* getFuncFromExpr(const DeclPtrVariant& funcExpr, CodeGenContext &context);

}

#endif //SLANGCREFACTORED_CODEGENCONTEXT_HPP
