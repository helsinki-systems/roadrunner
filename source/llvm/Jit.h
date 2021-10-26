//
// Created by Ciaran on 25/10/2021.
//

#ifndef ROADRUNNER_JIT_H
#define ROADRUNNER_JIT_H


#include <llvm/Analysis/TargetLibraryInfo.h>
#include "llvm/IR/IRBuilder.h"
#include "LLVMException.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Object/ObjectFile.h"

namespace rr {
    class ExecutableModel;

}

namespace rrllvm {

    class ModelResources;
    template <typename FunctionPtrType>
    class CodeGenBase;

    class Jit {
    public:

        Jit();

        /**
         * @brief adds sbml support function to the Jit engine
         * @details i.e. tan, arccsh, quotient. In MCJit (llvm 6) these
         * were "mapped" to sbml support functions. This API has changed in
         * later llvm's and so this functionality is made virtual.
         */
        virtual void addSupportFunctions() = 0;

        virtual std::uint64_t getFunctionAddress(const std::string& name) = 0;

        virtual llvm::TargetMachine* getTargetMachine() = 0;

        virtual void addObjectFile(llvm::object::OwningBinary<llvm::object::ObjectFile> owningObject) = 0;

        /**
         * MCJit needs this but might be a deprecated api. We have to include it anyway.
         */
        virtual void finalizeObject() = 0;

        virtual const llvm::DataLayout& getDataLayout() = 0;
        virtual void addModule(llvm::Module* M) = 0;



        /**
         * *Moves* objects over to ModelResources ptr
         */
        virtual void transferObjectsToResources(std::shared_ptr<rrllvm::ModelResources> rc);


        virtual void mapFunctionsToAddresses(std::shared_ptr<ModelResources>& rc, std::uint32_t options);


        /**
         * @brief returns a non owning pointer to the llvm::Module instance
         */
        virtual llvm::Module *getModuleNonOwning();

        /**
         * @brief returns a non owning pointer to the llvm::LLVMContext instance
         */
        virtual llvm::LLVMContext *getContextNonOwning();

        /**
         * @brief returns a non owning pointer to the llvm::LLVMContext instance
         */
        virtual llvm::IRBuilder<> *getBuilderNonOwning();

        virtual std::string emitToString();

//        virtual void setTargetTriple(llvm::Triple triple);
//
//        virtual void setDataLayout(llvm::DataLayout dataLayout);


    protected:

        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> module;
        llvm::Module* moduleNonOwning = nullptr;
        std::unique_ptr<llvm::IRBuilder<>> builder;
//        llvm::Triple triple;
//        llvm::DataLayout DataLayout;

    private:

        /**
         * todo note to self. I'm tempted to add a nested
         * class for C library functions (like abs or pow).
         * The idea is to have a factory function whereby
         * a enum is provided and a jit'd math func is returned.
         * However, I don't actually know how these math
         * functions should be used so I'll hold off for now.
         * Pointers can be access like so:
         */


        /**
         * @brief Add a function from the standard C library to the IR Module.
         * @example An example declaration is:
         *   declare double @pow(double, double)
         * @details the declaration is resolved with the standard C
         * library.
         * @code
                using powFn = double (*)(double x, double y);
                powFn pow = (powFn) executionEngine->getPointerToNamedFunction("pow");
                std::cout << pow(4, 2) << std::endl; // outputs 16
         * @endcode
         */
        virtual void createLibraryFunction(llvm::LibFunc funcId, llvm::FunctionType *funcType);

        /**
         * @brief declare all the necessary functions to
         * support sbml.
         * @details The following functions are declared:
         * pow, fabs, acos, asin, atan, ceil, cos, cosh, exp,
         * floor, log, log10, sin, sinh, tan, tanh, fmod
         */
        virtual void createLibraryFunctions();
    };

}

#endif //ROADRUNNER_JIT_H
