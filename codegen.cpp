#include "ast.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <vector>

/*
    Global LLVM Variables
*/

llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string,llvm::Value *> NamedValues;

// Helper function for extracting the LLVM-Type
llvm::Type *getLLVMType(SymbolType t) {
    switch (t) {
        case TYPE_INT: return llvm::Type::getInt16Ty(TheContext);
        case TYPE_BYTE: return llvm::Type::getInt8Ty(TheContext);
        case TYPE_BOOL: return llvm::Type::getInt8Ty(TheContext); // 1 bit, αλλά συνήθως αποθηκεύεται ως byte (i8)
        case TYPE_FUNC: return llvm::Type::getVoidTy(TheContext); // Void functions
        default: return llvm::Type::getVoidTy(TheContext);
    }
}

void initializeLLVM() {
    TheModule = std::make_unique<llvm::Module>("DanaModule", TheContext);
    
    // Δήλωση της writeString (είναι built-in)
    // decl writeString: s as byte []
    // Στο LLVM: declare void @writeString(i8*)
    std::vector<llvm::Type*> args;
    args.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))); // byte [] -> i8*
    llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), args, false);
    llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "writeString", TheModule.get());
    llvm::FunctionType *readIntFT = llvm::FunctionType::get(llvm::Type::getInt16Ty(TheContext), false);
    llvm::Function::Create(readIntFT, llvm::Function::ExternalLinkage, "readInteger", TheModule.get());
}

// ==========================================
// Code Generation Implementations
// ==========================================

// --- Number (Int Constants) ---
llvm::Value* Number::codegen(){
    // Dana ints are 16-bit
    return llvm::ConstantInt::get(TheContext, llvm::APInt(16, numb, true));
}

// --- CharConst (Byte Constants) ---
llvm::Value* CharConst::codegen() {
    // Dana bytes are 8-bit
    return llvm::ConstantInt::get(TheContext, llvm::APInt(8, character, false));
}

// --- StmtList (Block execution) ---
llvm::Value* StmtList::codegen() {
    llvm::Value *lastVal = nullptr;
    for (Stmt *s : stmt_list) {
        if (s)
            lastVal = s->codegen();
    }
    return lastVal;
}

// --- Func_Call (Κλήση συνάρτησης) ---
llvm::Value* Func_Call::codegen() {
    // 1. Βρες τη συνάρτηση
    llvm::Function *CalleeF = TheModule->getFunction(id->getName());
    if (!CalleeF) {
        std::cerr << "Unknown function referenced: " << id->getName() << "\n";
        return nullptr;
    }

    // 2. Ετοίμασε τα ορίσματα (Args)
    std::vector<llvm::Value *> ArgsV;
    if (list) {
        for (Expr *arg : list->getExprs()) { 
            ArgsV.push_back(arg->codegen());
            if (!ArgsV.back()) return nullptr;
        }
    }

    if (CalleeF->arg_size() != ArgsV.size()) {
        std::cerr << "Incorrect # arguments passed\n";
        return nullptr;
    }

    return Builder.CreateCall(CalleeF, ArgsV);
}


// --- LocalDef (Δήλωση μεταβλητής var x is int) ---
llvm::Value* LocalDef::codegen() {
    if (func_def) { 
        // Δήλωση συνάρτησης με σώμα
        return func_def->codegen();
    } 
    else if (func_decl) { 
        // Δήλωση συνάρτησης χωρίς σώμα
        return func_decl->codegen();
    } 
    else if (idlst) { 
        // var x, y is int
        std::vector<Id*> ids = idlst->getIds();
        llvm::Type *llvmType = getLLVMType(tp->getsymbol()); // Π.χ. i16
        
        // H Alloca γίνεται ΠΑΝΤΑ στην αρχή της συνάρτησης (Entry Block)
        llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
        llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());

        for (Id* id : ids) {
            // 1. Insert variable into symbol table
            st.insert(id->getName(), tp->getsymbol());
            
            // 2. Create Alloca
            llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(llvmType, nullptr, id->getName());
            
            // 3. Ενημέρωση Symbol Table!
            SymbolEntry *e = st.lookup(id->getName());
            if (e) {
                e->alloca_ptr = Alloca; // Αποθηκεύουμε τον pointer!
            }
        }
    }
    return nullptr; // Declarations don't return a value
}

// --- ExprList ---
llvm::Value* ExprList::codegen() {
    llvm::Value *lastVal = nullptr;
    for (Expr *e : getExprs()) {
        if (e) lastVal = e->codegen();
    }
    return lastVal;
}

// --- Id (Variable reference) ---
llvm::Value* Id::codegen() {
    SymbolEntry *e = st.lookup(id);
    if (!e) {
        std::cerr << "Unknown variable: " << id << "\n";
        return nullptr;
    }
    
    if (e->alloca_ptr) {
        // Load from allocated memory
        return Builder.CreateLoad(getLLVMType(e->return_value), e->alloca_ptr, id);
    }
    return nullptr;
}

// --- Lvalue (Assignment target) ---
llvm::Value* Lvalue::codegen() {
    // 1. Handle string literal
    if (!str.empty()) {
        return Builder.CreateGlobalStringPtr(str);
    }

    // 2. Handle variable (ID) - return the pointer for assignment
    if (id != nullptr) {
        SymbolEntry *entry = st.lookup(id->getName());
        if (entry != nullptr && entry->alloca_ptr != nullptr) {
            // Return the alloca pointer for storing value
            return entry->alloca_ptr;
        }
    }
    return nullptr;
}
// --- LvalueStmt (Assignment) ---
llvm::Value* LvalueStmt::codegen() {
    // Get the lvalue (should be a pointer to allocated memory)
    llvm::Value *LVal = l->codegen();
    if (!LVal) return nullptr;
    
    // Get the rvalue
    llvm::Value *RVal = e->codegen();
    if (!RVal) return nullptr;
    
    // Store the value
    Builder.CreateStore(RVal, LVal);
    return RVal;
}

// --- UniOp ---
llvm::Value* UniOp::codegen() {
    llvm::Value *val = e->codegen();
    if (!val) return nullptr;
    
    switch (op) {
        case '-':
            return Builder.CreateNeg(val, "neg");
        case '~':
            return Builder.CreateNot(val, "not");
        default:
            std::cerr << "Unknown unary operator: " << op << "\n";
            return nullptr;
    }
}

// --- BinOp ---
llvm::Value* BinOp::codegen() {
    llvm::Value *lhs = left->codegen();
    llvm::Value *rhs = right->codegen();
    
    if (!lhs || !rhs) return nullptr;
    
    switch (op) {
        case '+':
            return Builder.CreateAdd(lhs, rhs, "add");
        case '-':
            return Builder.CreateSub(lhs, rhs, "sub");
        case '*':
            return Builder.CreateMul(lhs, rhs, "mul");
        case '/':
            return Builder.CreateSDiv(lhs, rhs, "div");
        case '%':
            return Builder.CreateSRem(lhs, rhs, "rem");
        default:
            std::cerr << "Unknown binary operator: " << op << "\n";
            return nullptr;
    }
}

// --- ReturnStmt ---
llvm::Value* ReturnStmt::codegen() {
    llvm::Value *val = e->codegen();
    if (!val) return nullptr;
    return Builder.CreateRet(val);
}

// --- IfBlock ---
llvm::Value* IfBlock::codegen() {

    llvm::Value *condVal = condition->codegen();
    if (!condVal) return nullptr;
    
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    
    // Create blocks for then, else, and merge
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(TheContext, "then", TheFunction);
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(TheContext, "merge", TheFunction);
    
    // Convert condition to i1 - use the correct bit width for comparison
    llvm::Constant *Zero = llvm::ConstantInt::get(condVal->getType(), 0);
    condVal = Builder.CreateICmpNE(condVal, Zero, "cond");
    
    Builder.CreateCondBr(condVal, ThenBB, MergeBB);
    
    // Emit then block
    Builder.SetInsertPoint(ThenBB);
    if (stmts) stmts->codegen();
    Builder.CreateBr(MergeBB);
    
    // Emit merge block
    Builder.SetInsertPoint(MergeBB);
    
    return nullptr;
}

// --- FuncDefBlock (Η καρδιά του Code Generation) ---
llvm::Value* FuncDefBlock::codegen() {
    // 0. Open a new scope for the function
    st.openScope();
    
    // 1. Build argument types from Opt_Fpar
    std::vector<llvm::Type*> argTypes;
    std::vector<std::pair<std::string, SymbolType>> paramInfo;
    
    if (header->getOpt_Fpar() != nullptr) {
        paramInfo = header->getOpt_Fpar()->getParams();
        for (const auto& param : paramInfo) {
            argTypes.push_back(getLLVMType(param.second));
        }
    }

    // 2. Create function type
    llvm::Type *returnType = (header->gettype()) ? getLLVMType(header->gettype()->getsymbol()) : llvm::Type::getVoidTy(TheContext);
    llvm::FunctionType *FT = llvm::FunctionType::get(returnType, argTypes, false);

    // 3. Create the function in the module
    llvm::Function *TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, header->getName(), TheModule.get());

    // 4. Create entry block
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    // 5. Handle function parameters: create alloca for each param and store the argument
    llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    unsigned argIndex = 0;
    for (auto& arg : TheFunction->args()) {
        if (argIndex < paramInfo.size()) {
            // Insert parameter into symbol table
            st.insert(paramInfo[argIndex].first, paramInfo[argIndex].second);
            
            llvm::Type *paramType = getLLVMType(paramInfo[argIndex].second);
            llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(paramType, nullptr, paramInfo[argIndex].first);
            TmpBuilder.CreateStore(&arg, Alloca);
            
            // Update symbol table with alloca pointer
            SymbolEntry *e = st.lookup(paramInfo[argIndex].first);
            if (e) {
                e->alloca_ptr = Alloca;
            }
        }
        argIndex++;
    }

    // Set insertion point back to end of entry block
    Builder.SetInsertPoint(BB);

    // 6. Generate code for local definitions
    if (localDefList) {
        localDefList->codegen();
    }

    // 7. Generate code for statements
    if (stmt_list) {
        stmt_list->codegen();
    }

    // 8. Final return statement
    if (returnType->isVoidTy()) {
        Builder.CreateRetVoid();
    } else {
        if (!Builder.GetInsertBlock()->getTerminator()) {
            Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(16, 0, true)));
        }
    }

    // Verify function
    llvm::verifyFunction(*TheFunction);

    // Close the function scope
    st.closeScope();

    return TheFunction;
}

llvm::Value* LocalDefList::codegen() {
    for (LocalDef* ld : local_def_list) {
        if (ld) ld->codegen();
    }
    return nullptr;
}

llvm::Value* ProcStmt::codegen() {
    if (pc) return pc->codegen(); // pc είναι το Proc_Call*
    return nullptr;
}

llvm::Value* Proc_Call::codegen() {
    // 1. Find the function
    llvm::Function *CalleeF = TheModule->getFunction(id->getName());
    if (!CalleeF) {
        std::cerr << "Unknown function referenced: " << id->getName() << "\n";
        return nullptr;
    }

    // 2. Prepare the arguments (Args)
    std::vector<llvm::Value *> ArgsV;
    if (list) {
        for (Expr *arg : list->getExprs()) { 
            ArgsV.push_back(arg->codegen());
            if (!ArgsV.back()) return nullptr;
        }
    }

    if (CalleeF->arg_size() != ArgsV.size()) {
        std::cerr << "Incorrect # arguments passed\n";
        return nullptr;
    }

    return Builder.CreateCall(CalleeF, ArgsV);
}

// ==========================================
// Stub implementations for remaining classes
// ==========================================

llvm::Value* Type::codegen() { return nullptr; }
llvm::Value* ArrayType::codegen() { return nullptr; }
llvm::Value* Id_List::codegen() { return nullptr; }
llvm::Value* Opt_Fpar::codegen() { return nullptr; }
llvm::Value* Header::codegen() { return nullptr; }
llvm::Value* Stmt::codegen() { return nullptr; }
llvm::Value* SkipStmt::codegen() { return nullptr; }
llvm::Value* ExitStmt::codegen() { return nullptr; }
llvm::Value* LoopBlock::codegen() {
    // Open scope for the loop
    st.openScope();
    
    // Mark this scope as inside a loop
    st.insert("__loop__", LOOP);
    
    // Generate code for the loop body
    llvm::Value *result = nullptr;
    if (loop_stmt_list) {
        result = loop_stmt_list->codegen();
    }
    
    // Close the loop scope
    st.closeScope();
    
    return result;
}
llvm::Value* BreakStmt::codegen() { return nullptr; }
llvm::Value* ContStmt::codegen() { return nullptr; }
llvm::Value* FuncDecl::codegen() { return nullptr; }

llvm::Value* Bool::codegen() { return nullptr; }
llvm::Value* BoolBinOp::codegen() { return nullptr; }


llvm::Value* FuncStmt::codegen() { return nullptr; }

llvm::Value* NotExpr::codegen() { return nullptr; }
llvm::Value* Cond::codegen() {
    // Handle single condition (just a boolean expression)
    if (single != nullptr) {
        return single->codegen();
    }

    // Handle binary comparison
    llvm::Value *lhsVal = left->codegen();
    llvm::Value *rhsVal = right->codegen();
    
    if (!lhsVal || !rhsVal) return nullptr;

    // Ensure both operands have the same type before comparison
    // If types differ, promote to the larger type
    if (lhsVal->getType() != rhsVal->getType()) {
        if (lhsVal->getType()->getPrimitiveSizeInBits() < rhsVal->getType()->getPrimitiveSizeInBits()) {
            lhsVal = Builder.CreateSExt(lhsVal, rhsVal->getType(), "sext_lhs");
        } else {
            rhsVal = Builder.CreateSExt(rhsVal, lhsVal->getType(), "sext_rhs");
        }
    }

    if (oper == ">") {
        return Builder.CreateICmpSGT(lhsVal, rhsVal, "cmp_gt");
    } else if (oper == "<") {
        return Builder.CreateICmpSLT(lhsVal, rhsVal, "cmp_lt");
    } else if (oper == ">=") {
        return Builder.CreateICmpSGE(lhsVal, rhsVal, "cmp_ge");
    } else if (oper == "<=") {
        return Builder.CreateICmpSLE(lhsVal, rhsVal, "cmp_le");
    } else if (oper == "=" || oper == "==") {
        return Builder.CreateICmpEQ(lhsVal, rhsVal, "cmp_eq");
    } else if (oper == "<>" || oper == "!=") {
        return Builder.CreateICmpNE(lhsVal, rhsVal, "cmp_ne");
    } else if (oper == "and") {
        return Builder.CreateAnd(lhsVal, rhsVal, "and");
    } else if (oper == "or") {
        return Builder.CreateOr(lhsVal, rhsVal, "or");
    } else {
        std::cerr << "Unknown condition operator: " << oper << "\n";
        return nullptr;
    }
}
llvm::Value* NotCond::codegen() { return nullptr; }
llvm::Value* Opt_Elif_Else::codegen() { return nullptr; }



