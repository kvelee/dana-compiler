#include "ast.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>
#include <vector>
#include <utility>

/*
    Global LLVM Variables
*/

llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string,llvm::Value *> NamedValues;

// --- Loops ---
std::vector<std::pair<std::string, llvm::BasicBlock*>> LoopMergeBlocks;
std::vector<std::pair<std::string, llvm::BasicBlock*>> LoopHeaderBlocks;

std::map<SymbolEntry*, llvm::Function*> FuncPointers;

std::map<SymbolEntry*, std::vector<int>> ArrayDimensions;

// Active functions for the nested function implentation
std::vector<SymbolEntry*> active_functions;

bool isOutermostFunction = true;

// Helper function for extracting the LLVM-Type
llvm::Type *getLLVMType(SymbolType t) {
    switch (t) {
        case TYPE_INT: 
            return llvm::Type::getInt32Ty(TheContext);
        case TYPE_BYTE: 
            return llvm::Type::getInt8Ty(TheContext);
        case TYPE_BOOL: 
            return llvm::Type::getInt8Ty(TheContext);
        case TYPE_PARAM:
            // TYPE_PARAM defaults to i16 for numeric parameters
            return llvm::Type::getInt32Ty(TheContext);
        case TYPE_REF_INT: 
            return llvm::PointerType::getUnqual(llvm::Type::getInt32Ty(TheContext));
        case TYPE_REF_BYTE:
            return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext));
        case TYPE_FUNC:
            return llvm::Type::getVoidTy(TheContext);
        case TYPE_INVALID:
        default: 
            std::cerr << "Warning: getLLVMType called with invalid/unknown type. Returning Int16Ty as fallback." << std::endl;
            return llvm::Type::getInt32Ty(TheContext);
    }
}

void initializeLLVM() {
    TheModule = std::make_unique<llvm::Module>("DanaModule", TheContext);
    
    // writeInteger: n as int
    std::vector<llvm::Type*> writeIntArgs = {llvm::Type::getInt32Ty(TheContext)};
    llvm::FunctionType *writeIntFT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), writeIntArgs, false);
    llvm::Function::Create(writeIntFT, llvm::Function::ExternalLinkage, "writeInteger", TheModule.get());

    // writeByte: b as byte (εκτυπώνει αριθμητική τιμή)
    // writeChar: b as byte (εκτυπώνει ASCII χαρακτήρα)
    std::vector<llvm::Type*> writeByteArgs = {llvm::Type::getInt8Ty(TheContext)};
    llvm::FunctionType *writeByteFT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), writeByteArgs, false);
    llvm::Function::Create(writeByteFT, llvm::Function::ExternalLinkage, "writeByte", TheModule.get());
    llvm::Function::Create(writeByteFT, llvm::Function::ExternalLinkage, "writeChar", TheModule.get());

    // writeString: s as byte []
    std::vector<llvm::Type*> writeStrArgs = {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))};
    llvm::FunctionType *writeStrFT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), writeStrArgs, false);
    llvm::Function::Create(writeStrFT, llvm::Function::ExternalLinkage, "writeString", TheModule.get());
    
    // readInteger is int
    llvm::FunctionType *readIntFT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), false);
    llvm::Function::Create(readIntFT, llvm::Function::ExternalLinkage, "readInteger", TheModule.get());

    // readByte is byte / readChar is byte
    llvm::FunctionType *readByteFT = llvm::FunctionType::get(llvm::Type::getInt8Ty(TheContext), false);
    llvm::Function::Create(readByteFT, llvm::Function::ExternalLinkage, "readByte", TheModule.get());
    llvm::Function::Create(readByteFT, llvm::Function::ExternalLinkage, "readChar", TheModule.get());

    // readString: n as int, s as byte []
    std::vector<llvm::Type*> readStrArgs = {
        llvm::Type::getInt32Ty(TheContext), 
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))
    };
    llvm::FunctionType *readStrFT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), readStrArgs, false);
    llvm::Function::Create(readStrFT, llvm::Function::ExternalLinkage, "readString", TheModule.get());
    
    // extend: b as byte -> int (i8 to i16)
    std::vector<llvm::Type*> extArgs = {llvm::Type::getInt8Ty(TheContext)};
    llvm::FunctionType *extFT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), extArgs, false);
    llvm::Function::Create(extFT, llvm::Function::ExternalLinkage, "extend", TheModule.get());

    // shrink: i as int -> byte (i16 to i8)
    std::vector<llvm::Type*> shrArgs = {llvm::Type::getInt32Ty(TheContext)};
    llvm::FunctionType *shrFT = llvm::FunctionType::get(llvm::Type::getInt8Ty(TheContext), shrArgs, false);
    llvm::Function::Create(shrFT, llvm::Function::ExternalLinkage, "shrink", TheModule.get());

    // strlen is int: s as byte []
    std::vector<llvm::Type*> strlenArgs = {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))};
    llvm::FunctionType *strlenFT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), strlenArgs, false);
    llvm::Function::Create(strlenFT, llvm::Function::ExternalLinkage, "strlen", TheModule.get());

    // strcmp is int: s1 s2 as byte []
    std::vector<llvm::Type*> strcmpArgs = {
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext)),
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))
    };
    llvm::FunctionType *strcmpFT = llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), strcmpArgs, false);
    llvm::Function::Create(strcmpFT, llvm::Function::ExternalLinkage, "strcmp", TheModule.get());

    // strcpy / strcat: trg src as byte []
    std::vector<llvm::Type*> strCopyArgs = {
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext)),
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))
    };
    llvm::FunctionType *strCopyFT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), strCopyArgs, false);
    llvm::Function::Create(strCopyFT, llvm::Function::ExternalLinkage, "strcpy", TheModule.get());
    llvm::Function::Create(strCopyFT, llvm::Function::ExternalLinkage, "strcat", TheModule.get());
}


// unescapeString moved to --> parser.y / lexer.hpp

// Code Generation Implementations
// ==========================================

// --- Number (Int Constants) ---
llvm::Value* Number::codegen(){
    // Dana ints are now 32-bit
    return llvm::ConstantInt::get(TheContext, llvm::APInt(32, numb, true));
}

// --- CharConst (Byte Constants) ---
llvm::Value* CharConst::codegen() {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(TheContext), (uint8_t)character, false);
}

// --- StmtList ---
llvm::Value* StmtList::codegen() {
    llvm::Value *lastVal = nullptr;
    for (Stmt *s : stmt_list) {
        if (s)
            lastVal = s->codegen();
    }
    return lastVal;
}

// --- Func_Call ---
llvm::Value* Func_Call::codegen() {
    if (!id) {
        std::cerr << "Error: Proc_Call has null id\n";
        return nullptr;
    }
    if (!TheModule) {
        std::cerr << "Error: TheModule is not initialized\n";
        return nullptr;
    }
    // 1. Βρες τη συνάρτηση
    SymbolEntry *e = st.lookup(id->getName());
    llvm::Function *CalleeF = nullptr;
    if (e && FuncPointers.count(e)) {
        CalleeF = FuncPointers[e];
    } else {
        CalleeF = TheModule->getFunction(id->getName()); // Fallback
    }

    if (!CalleeF) {
        std::cerr << "Unknown function referenced: " << id->getName() << "\n";
        return nullptr;
    }

    // 2. Prepare the arguments
    std::vector<llvm::Value *> ArgsV;
    if (list) {
        unsigned i = 0;
        for (Expr *arg : list->getExprs()) { 
            llvm::Value *argVal = arg->codegen();
            
            if (!argVal) return nullptr;
            
            if (i < CalleeF->arg_size() && CalleeF->getArg(i)->getType()->isPointerTy() && !argVal->getType()->isPointerTy()) {
                if (Lvalue *lval = dynamic_cast<Lvalue*>(arg)) {
                    argVal = lval->getPointer();
                } else if (Id *id_arg = dynamic_cast<Id*>(arg)) {
                    SymbolEntry *entry = st.lookup(id_arg->getName());
                    if (entry && entry->alloca_ptr) {
                        llvm::Value *Ptr = entry->alloca_ptr;
                        bool isRef = (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE ||
                                     (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)));
                        if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
                            Ptr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id_arg->getName() + "_ref_ptr");
                        }
                        argVal = Ptr;
                    }
                } else {
                    // Δημιουργούμε μια κρυφή (προσωρινή) μεταβλητή
                    llvm::BasicBlock *InsertBB = Builder.GetInsertBlock();
                    llvm::Function *TheFunction = InsertBB->getParent();
                    llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
                    
                    llvm::AllocaInst *tempAlloca = TmpBuilder.CreateAlloca(argVal->getType(), nullptr, "temp_ref_arg");
                    Builder.CreateStore(argVal, tempAlloca);
                    argVal = tempAlloca;
                }
            }
            
            ArgsV.push_back(argVal);
            if (!ArgsV.back()) return nullptr;
            i++;
        }
    }

    if (e && !e->captured_vars.empty()) {
        for (const std::string& capName : e->captured_vars) {
            SymbolEntry *capE = st.lookup(capName);
            llvm::Value *ptrToPass = capE->alloca_ptr;
            
            bool isRef = (capE->type == TYPE_REF_INT || capE->type == TYPE_REF_BYTE ||
                         (capE->type == TYPE_PARAM && (capE->return_value == TYPE_REF_INT || capE->return_value == TYPE_REF_BYTE)));
            
            if (isRef && !llvm::isa<llvm::GlobalVariable>(ptrToPass)) {
                ptrToPass = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), ptrToPass, capName + "_fwd_ptr");
            }
            
            ArgsV.push_back(ptrToPass);
        }
    }
    
    if (CalleeF->arg_size() != ArgsV.size()) {
        std::cerr << "Incorrect # arguments passed\n";
        return nullptr;
    }

    return Builder.CreateCall(CalleeF, ArgsV);
}


// --- LocalDef ---
llvm::Value* LocalDef::codegen() {
    if (func_def) { 
        return func_def->codegen();
    } 
    else if (func_decl) { 
        return func_decl->codegen();
    } 
    else if (idlst) { 
        std::vector<Id*> ids = idlst->getIds();
        llvm::Type *llvmType = getLLVMType(tp->getsymbol());
        
        int totalSize = 1;
        std::vector<int> dims = tp->getDims();
        for (int d : dims) totalSize *= d;

        llvm::BasicBlock *InsertBlock = Builder.GetInsertBlock();
        llvm::Function *TheFunction = InsertBlock->getParent();

        llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), 
                                     TheFunction->getEntryBlock().begin());

        bool isMain = (TheFunction->getName() == "main");

        for (Id* id : ids) {
            st.insert(id->getName(), tp->getsymbol());
            SymbolEntry *e = st.lookup(id->getName());
            
            // ---- Αποθήκευση διαστάσεων ---
            std::vector<int> currentDims = tp->getDims();
            if (e && !currentDims.empty()) {
                ArrayDimensions[e] = currentDims;
            }

            if (isMain) {
                // Προαγωγή σε Global Variable
                llvm::Type *allocType = llvmType;
                if (totalSize > 1) allocType = llvm::ArrayType::get(llvmType, totalSize);
                
                llvm::GlobalVariable *GV = new llvm::GlobalVariable(
                    *TheModule, allocType, false, llvm::GlobalValue::InternalLinkage,
                    llvm::Constant::getNullValue(allocType), id->getName()
                );
                if (e) e->alloca_ptr = GV;
            } else {
                // Τοπικό Alloca
                llvm::Value *ArraySize = nullptr;
                if (totalSize > 1) ArraySize = llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), totalSize);
                
                llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(llvmType, ArraySize, id->getName());
                if (e) e->alloca_ptr = Alloca;
            }
        }
    }
    return nullptr;
}

// --- ExprList ---
llvm::Value* ExprList::codegen() {
    llvm::Value *lastVal = nullptr;
    for (Expr *e : getExprs()) {
        if (e) lastVal = e->codegen();
    }
    return lastVal;
}

// --- Id ---
llvm::Value* Id::codegen() {
    SymbolEntry *e = st.lookup(id); 
    if (!e || !e->alloca_ptr) return nullptr;
    
    llvm::Value *Ptr = e->alloca_ptr;

    bool isRef = (e->type == TYPE_REF_INT || e->type == TYPE_REF_BYTE) ||
                 (e->type == TYPE_PARAM && (e->return_value == TYPE_REF_INT || e->return_value == TYPE_REF_BYTE));

    if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
        llvm::Value *ActualAddress = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id + "_addr");
        
        bool isByte = (e->type == TYPE_REF_BYTE) || (e->type == TYPE_PARAM && e->return_value == TYPE_REF_BYTE);
        llvm::Type *ValueTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt32Ty(TheContext);
        
        return Builder.CreateLoad(ValueTy, ActualAddress, id.c_str());
    }
    
    // Κανονική μεταβλητή (όχι ref/captured)
    SymbolType actualType = (e->type == TYPE_PARAM) ? e->return_value : e->type;
    return Builder.CreateLoad(getLLVMType(actualType), Ptr, id.c_str());
}
// --- Lvalue ---
llvm::Value* Lvalue::codegen() {
    llvm::Value *Ptr = getPointer();
    if (!Ptr) return nullptr;
    if (!str.empty()) {
        if (expr_list != nullptr) {
            return Builder.CreateLoad(llvm::Type::getInt8Ty(TheContext), Ptr, "str_char_val");
        }
        return Ptr;
    }

    SymbolEntry *entry = st.lookup(id->getName());
    if (!entry) return nullptr;

    bool isArray = false;
    if (auto *GV = llvm::dyn_cast<llvm::GlobalVariable>(Ptr)) {
        if (GV->getValueType()->isArrayTy()) isArray = true;
    } else if (auto *AI = llvm::dyn_cast<llvm::AllocaInst>(Ptr)) {
        if (AI->getAllocatedType()->isArrayTy() || AI->isArrayAllocation()) isArray = true;
    }

    if (!expr_list && isArray) return Ptr;

    bool isByte = (entry->type == TYPE_BYTE) || (entry->type == TYPE_REF_BYTE) || 
                  (entry->type == TYPE_PARAM && (entry->return_value == TYPE_BYTE || entry->return_value == TYPE_REF_BYTE));
    llvm::Type *ValTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt32Ty(TheContext);

    return Builder.CreateLoad(ValTy, Ptr, id->getName() + "_val");
}


// --- LvalueStmt ---
llvm::Value* LvalueStmt::codegen() {
    if (!l) return nullptr;

    // Βρίσκουμε τον σωστό δείκτη
    llvm::Value *LValPtr = l->getPointer(); 
    if (!LValPtr) {
        std::cerr << "Error: Could not get pointer for assignment\n";
        return nullptr;
    }
    
    llvm::Value *RVal = e->codegen();
    if (!RVal) return nullptr;
    
    // Αρχικοποιούμε το ExpectedTy από το δικό μας Symbol Table!
    llvm::Type *ExpectedTy = llvm::Type::getInt32Ty(TheContext); // Fallback
    
    SymbolEntry *entry = st.lookup(l->getIdName());
    if (entry) {
        bool isByte = (entry->type == TYPE_BYTE) || (entry->type == TYPE_REF_BYTE) || 
                      (entry->type == TYPE_PARAM && (entry->return_value == TYPE_BYTE || entry->return_value == TYPE_REF_BYTE));
        ExpectedTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt32Ty(TheContext);
    }
    if (RVal->getType() != ExpectedTy && RVal->getType()->isIntegerTy() && ExpectedTy->isIntegerTy()) {
        if (RVal->getType()->getIntegerBitWidth() < ExpectedTy->getIntegerBitWidth()) {
            RVal = Builder.CreateZExt(RVal, ExpectedTy, "store_ext");
        } else {
            RVal = Builder.CreateTrunc(RVal, ExpectedTy, "store_trunc");
        }
    }

    // Αποθηκεύουμε την τιμή στη μνήμη
    return Builder.CreateStore(RVal, LValPtr);
}


// --- UniOp ---
llvm::Value* UniOp::codegen() {
    llvm::Value *val = e->codegen();
    if (!val) return nullptr;
    
    switch (op) {
        case '+': 
            return val;
        case '-': 
            return Builder.CreateNeg(val, "neg");
        case '!': {
            llvm::Value *Zero = llvm::ConstantInt::get(val->getType(), 0);
            llvm::Value *Cmp = Builder.CreateICmpEQ(val, Zero, "not_byte_cmp");
            return Builder.CreateZExt(Cmp, llvm::Type::getInt8Ty(TheContext), "not_byte_res");
        }
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

    SymbolType leftTy = left->getsymbol();
    SymbolType rightTy = right->getsymbol();

    // Αν διαφέρουν τα μεγέθη
    if (lhs->getType() != rhs->getType()) {
        if (lhs->getType()->getPrimitiveSizeInBits() < rhs->getType()->getPrimitiveSizeInBits()) {
            // Check for type_byte, if so it is unsigned
            if (leftTy == TYPE_BYTE || leftTy == TYPE_REF_BYTE)
                lhs = Builder.CreateZExt(lhs, rhs->getType(), "lhs_zext");
            else
                lhs = Builder.CreateSExt(lhs, rhs->getType(), "lhs_sext");
        }
        else {
            // The same checking as above for the rhs
            if (rightTy == TYPE_BYTE || rightTy == TYPE_REF_BYTE)
                rhs = Builder.CreateZExt(rhs, lhs->getType(), "rhs_zext");
            else
                rhs = Builder.CreateSExt(rhs, lhs->getType(), "rhs_sext");
        }
    }

    switch (op) {
        case '+': return Builder.CreateAdd(lhs, rhs, "addtmp");
        case '-': return Builder.CreateSub(lhs, rhs, "subtmp");
        case '*': return Builder.CreateMul(lhs, rhs, "multmp");
        case '/': 
            if (symboltype == TYPE_BYTE) return Builder.CreateUDiv(lhs, rhs, "divtmp");
            else return Builder.CreateSDiv(lhs, rhs, "divtmp");
        case '%': 
            if (symboltype == TYPE_BYTE) return Builder.CreateURem(lhs, rhs, "remtmp");
            else return Builder.CreateSRem(lhs, rhs, "remtmp");
        default: return nullptr;
    }
}

// --- ReturnStmt ---
llvm::Value* ReturnStmt::codegen() {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::Type *retType = TheFunction->getReturnType();

    if (!e) { 
        // Αν η συνάρτηση είναι η "main" -> 0
        if (TheFunction->getName() == "main") {
            return Builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), 0));
        }
        return Builder.CreateRetVoid(); 
    }
    
    llvm::Value *val = e->codegen();
    if (!val) return nullptr;

    // return type == συνάρτηση
    if (val->getType() != retType && !retType->isVoidTy()) {
        if (val->getType()->getIntegerBitWidth() < retType->getIntegerBitWidth()) {
            if (e->getsymbol() == TYPE_BYTE || e->getsymbol() == TYPE_REF_BYTE)
                val = Builder.CreateZExt(val, retType, "ret_zext");
            else
                val = Builder.CreateSExt(val, retType, "ret_sext");
        }
        else
            val = Builder.CreateTrunc(val, retType, "ret_trunc");
    }
    return Builder.CreateRet(val);
}

// --- IfBlock ---
llvm::Value* IfBlock::codegen() {
    llvm::Value *condVal = condition->codegen();
    if (!condVal) return nullptr;
    
    llvm::BasicBlock *InsertBlock = Builder.GetInsertBlock();
    if (!InsertBlock) return nullptr;
    llvm::Function *TheFunction = InsertBlock->getParent();
    
    // Μετατροπή της συνθήκης σε i1 (boolean)
    llvm::Constant *Zero = llvm::ConstantInt::get(condVal->getType(), 0);
    condVal = Builder.CreateICmpNE(condVal, Zero, "cond");
    
    // Δημιουργία των Blocks
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(TheContext, "then", TheFunction);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(TheContext, "else"); // Δεν το κάνουμε insert ακόμα
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(TheContext, "merge");
    
    bool hasElse = (elif_else != nullptr);
    Builder.CreateCondBr(condVal, ThenBB, hasElse ? ElseBB : MergeBB);
    
    // 1. Then Block
    Builder.SetInsertPoint(ThenBB);
    if (stmts) stmts->codegen();
    // Αν δεν υπάρχει ήδη return/break, βάλε branch στο merge
    if (!Builder.GetInsertBlock()->getTerminator()) Builder.CreateBr(MergeBB);
    
    // 2. Else Block
    if (hasElse) {
        TheFunction->insert(TheFunction->end(), ElseBB);
        Builder.SetInsertPoint(ElseBB);
        elif_else->codegen();
        if (!Builder.GetInsertBlock()->getTerminator()) Builder.CreateBr(MergeBB);
    }
    
    // 3. Merge Block
    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder.SetInsertPoint(MergeBB);
    
    return nullptr;
}

// --- FuncDefBlock (Η καρδιά του Code Generation) ---
llvm::Value* FuncDefBlock::codegen() {
    if (!header) {
        std::cerr << "Error: FuncDefBlock has null header\n";
        return nullptr;
    }
    
    std::string originalName = header->getName();
    std::string llvmName = originalName;
    
    // Αν είναι η πρώτη συνάρτηση, την βαφτίζουμε "main"
    if (isOutermostFunction) {
        llvmName = "main";
        isOutermostFunction = false;
    } else {
        SymbolType retTypeSym = header->gettype() ? header->gettype()->getsymbol() : TYPE_FUNC;
        st.insert(originalName, TYPE_FUNC, retTypeSym);
    }

    SymbolEntry *funcEntry = st.lookup(originalName);

    if (funcEntry) {
        funcEntry->captured_vars = this->saved_captured_vars;
        funcEntry->captured_types = this->saved_captured_types;
    }

    // 1. Άνοιγμα νέου scope για τη συνάρτηση 
    st.openScope();

    // 2. Προετοιμασία τύπων παραμέτρων
    std::vector<llvm::Type*> argTypes;
    std::vector<std::pair<std::string, SymbolType>> paramInfo;
    
    if (header->getOpt_Fpar() != nullptr) {
        paramInfo = header->getOpt_Fpar()->getParams();
        for (const auto& param : paramInfo) {
            if (param.second == TYPE_INT) {
                argTypes.push_back(llvm::Type::getInt32Ty(TheContext));
            } else if (param.second == TYPE_BYTE) {
                argTypes.push_back(llvm::Type::getInt8Ty(TheContext));
            } else if (param.second == TYPE_REF_INT) {
                argTypes.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt32Ty(TheContext)));
            } else if (param.second == TYPE_REF_BYTE) {
                argTypes.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext)));
            } else {
                llvm::Type *paramType = getLLVMType(param.second);
                argTypes.push_back(paramType);
            }
        }

    }

    if (funcEntry) {
            for (SymbolType capType : funcEntry->captured_types) {
                llvm::Type *baseTy = getLLVMType(capType);
                argTypes.push_back(llvm::PointerType::getUnqual(baseTy));
            }
    }

    // --- ΒΗΜΑ 3: Δημιουργία συνάρτησης ---
    llvm::Type *returnType = (llvmName == "main") ? llvm::Type::getInt32Ty(TheContext) : 
        (header->gettype() ? getLLVMType(header->gettype()->getsymbol()) : llvm::Type::getVoidTy(TheContext));

    llvm::FunctionType *FT = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function *TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, llvmName, TheModule.get());

    // Συνδέουμε το αρχικό όνομα
    if (funcEntry) {
        FuncPointers[funcEntry] = TheFunction;
    }

    unsigned idx = 0;
    for (auto &Arg : TheFunction->args()) {
        if (idx < paramInfo.size()) Arg.setName(paramInfo[idx].first);
        idx++;
    }

    // 4. Αποθήκευση του τρέχοντος Insert Point
    llvm::BasicBlock *prevBB = Builder.GetInsertBlock();

    // 5. Δημιουργία Entry Block 
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    // 6. Δέσμευση μνήμης (alloca) για παραμέτρους στην αρχή του entry block 
    llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());

    std::map<std::string, std::vector<int>> paramDimsMap;
    if (header->getOpt_Fpar() != nullptr) {
        auto arrParams = header->getOpt_Fpar()->getArrayParams();
        for (auto &ap : arrParams) paramDimsMap[ap.first] = ap.second;
    }

    unsigned argIndex = 0;
    for (auto& arg : TheFunction->args()) {
        if (argIndex < paramInfo.size()) {
            std::string pName = paramInfo[argIndex].first;
            SymbolType pType = paramInfo[argIndex].second;

            // Επανεισαγωγή παραμέτρου στον Symbol Table 
            st.insert(pName, TYPE_PARAM, pType);
            SymbolEntry *e = st.lookup(pName);

            if (e && paramDimsMap.count(pName)) {
                ArrayDimensions[e] = paramDimsMap[pName];
            }

            llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(arg.getType(), nullptr, pName);
            TmpBuilder.CreateStore(&arg, Alloca);
            if (e) e->alloca_ptr = Alloca;
        }
        argIndex++;
    }

    if (funcEntry) {
        for (size_t c = 0; c < funcEntry->captured_vars.size(); ++c) {
            llvm::Argument *capArg = TheFunction->getArg(paramInfo.size() + c);
            std::string capName = funcEntry->captured_vars[c];
            capArg->setName(capName + "_captured");
            
            llvm::AllocaInst *capAlloca = TmpBuilder.CreateAlloca(capArg->getType(), nullptr, capName + "_ptr");
            TmpBuilder.CreateStore(capArg, capAlloca);
            
            SymbolType refType = (funcEntry->captured_types[c] == TYPE_BYTE) ? TYPE_REF_BYTE : TYPE_REF_INT;
            st.insert(capName, TYPE_PARAM, refType);
            SymbolEntry *localCapE = st.lookup(capName);
            if (localCapE) localCapE->alloca_ptr = capAlloca; 
        }
    }

    // Αποθήκευση insert point
    llvm::BasicBlock *SavedBB = Builder.GetInsertBlock();

    // 7. Παραγωγή κώδικα για τοπικούς ορισμούς (εδώ μπαίνουν οι nested functions) 
    if (localDefList) localDefList->codegen();

    // Επαναφορά του Builder
    Builder.SetInsertPoint(SavedBB);

    // 8. Παραγωγή κώδικα για τις εντολές της συνάρτησης 
    if (stmt_list) stmt_list->codegen();

    // --- ΒΗΜΑ 9: Διαχείριση Return ---
    if (!Builder.GetInsertBlock()->getTerminator()) {
        if (originalName == "main") {
            Builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), 0));
        } else if (returnType->isVoidTy()) {
            Builder.CreateRetVoid();
        } else {
            Builder.CreateRet(llvm::ConstantInt::get(returnType, 0));
        }
    }

    st.closeScope();

    // Επαναφορά στον προηγούμενο Builder
    if (prevBB) Builder.SetInsertPoint(prevBB);

    if (llvm::verifyFunction(*TheFunction, &llvm::errs())) {
        std::cerr << "LLVM Verification failed for function: " << originalName << "\n";
    }
    return TheFunction;
}

llvm::Value* LocalDefList::codegen() {
    for (LocalDef* ld : local_def_list) {
        if (ld) ld->codegen();
    }
    return nullptr;
}

llvm::Value* ProcStmt::codegen() {
    if (pc) return pc->codegen();
    return nullptr;
}

llvm::Value* Proc_Call::codegen() {
    if (!id) {
        std::cerr << "Error: Proc_Call has null id\n";
        return nullptr;
    }
    if (!TheModule) {
        std::cerr << "Error: TheModule is not initialized\n";
        return nullptr;
    }
    // 1. Βρες τη συνάρτηση
    SymbolEntry *e = st.lookup(id->getName());
    llvm::Function *CalleeF = nullptr;
    if (e && FuncPointers.count(e)) {
        CalleeF = FuncPointers[e];
    } else {
        CalleeF = TheModule->getFunction(id->getName()); // Fallback
    }

    if (!CalleeF) {
        std::cerr << "Unknown function referenced: " << id->getName() << "\n";
        return nullptr;
    }

    // 2. Prepare the arguments
    std::vector<llvm::Value *> ArgsV;
    if (list) {
        unsigned i = 0;
        for (Expr *arg : list->getExprs()) { 
            llvm::Value *argVal = arg->codegen();
            
            if (!argVal) return nullptr;
            
            if (i < CalleeF->arg_size() && CalleeF->getArg(i)->getType()->isPointerTy() && !argVal->getType()->isPointerTy()) {
                if (Lvalue *lval = dynamic_cast<Lvalue*>(arg)) {
                    argVal = lval->getPointer();
                } else if (Id *id_arg = dynamic_cast<Id*>(arg)) {
                    SymbolEntry *entry = st.lookup(id_arg->getName());
                    if (entry && entry->alloca_ptr) {
                        llvm::Value *Ptr = entry->alloca_ptr;
                        bool isRef = (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE ||
                                     (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)));
                        if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
                            Ptr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id_arg->getName() + "_ref_ptr");
                        }
                        argVal = Ptr;
                    }
                } else {
                    // Δημιουργούμε μια κρυφή (προσωρινή) μεταβλητή
                    llvm::BasicBlock *InsertBB = Builder.GetInsertBlock();
                    llvm::Function *TheFunction = InsertBB->getParent();
                    llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
                    
                    llvm::AllocaInst *tempAlloca = TmpBuilder.CreateAlloca(argVal->getType(), nullptr, "temp_ref_arg");
                    Builder.CreateStore(argVal, tempAlloca);
                    argVal = tempAlloca;
                }
            }
            
            ArgsV.push_back(argVal);
            if (!ArgsV.back()) return nullptr;
            i++;
        }
    }

    if (e && !e->captured_vars.empty()) {
        for (const std::string& capName : e->captured_vars) {
            SymbolEntry *capE = st.lookup(capName);
            llvm::Value *ptrToPass = capE->alloca_ptr;
            
            bool isRef = (capE->type == TYPE_REF_INT || capE->type == TYPE_REF_BYTE ||
                         (capE->type == TYPE_PARAM && (capE->return_value == TYPE_REF_INT || capE->return_value == TYPE_REF_BYTE)));
            
            if (isRef && !llvm::isa<llvm::GlobalVariable>(ptrToPass)) {
                ptrToPass = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), ptrToPass, capName + "_fwd_ptr");
            }
            
            ArgsV.push_back(ptrToPass);
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

// ===========================================

llvm::Value* ExitStmt::codegen() {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::Type *retType = TheFunction->getReturnType();

    if (retType->isVoidTy()) {
        Builder.CreateRetVoid();
    } else {
        Builder.CreateRet(llvm::ConstantInt::get(retType, 0));
    }

    llvm::BasicBlock *DeadBB = llvm::BasicBlock::Create(TheContext, "dead_after_exit", TheFunction);
    Builder.SetInsertPoint(DeadBB);

    return nullptr;
}

llvm::Value* LoopBlock::codegen() {
    
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    if (!TheFunction) return nullptr;

    // Δημιουργία των Basic Blocks
    llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(TheContext, "loop", TheFunction);
    llvm::BasicBlock *BodyBB = llvm::BasicBlock::Create(TheContext, "body", TheFunction);
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(TheContext, "loop_merge", TheFunction);

    // Αποθήκευση στη στοίβα για τα break/continue
    LoopHeaderBlocks.push_back({ID, LoopBB});
    LoopMergeBlocks.push_back({ID, MergeBB});

    // Μετάβαση στο header του loop
    Builder.CreateBr(LoopBB);
    Builder.SetInsertPoint(LoopBB);

    // Πάντα μπαίνουμε στο body
    Builder.CreateBr(BodyBB);
    Builder.SetInsertPoint(BodyBB);

    // Open scope for the loop variables
    st.openScope();
    st.insert("__loop__", LOOP);
    
    // Generate code for the loop body
    if (loop_stmt_list) loop_stmt_list->codegen();
    
    // Close the loop scope
    st.closeScope();

    if (!Builder.GetInsertBlock()->getTerminator()) {
        Builder.CreateBr(LoopBB);
    }

    Builder.SetInsertPoint(MergeBB);

    // Αφαίρεση από τη στοίβα
    LoopHeaderBlocks.pop_back();
    LoopMergeBlocks.pop_back();

    return nullptr;
}

llvm::Value* BreakStmt::codegen() {
    if (LoopMergeBlocks.empty()) {
        std::cerr << "Error: Break statement not inside a loop\n";
        return nullptr;
    }

    llvm::BasicBlock *TargetBB = nullptr;
    if (id.empty()) {
        TargetBB = LoopMergeBlocks.back().second;
    } else {
        for (auto it = LoopMergeBlocks.rbegin(); it != LoopMergeBlocks.rend(); ++it) {
            if (it->first == id) { TargetBB = it->second; break; }
        }
    }

    if (!TargetBB) return nullptr;
    Builder.CreateBr(TargetBB);
    
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *DeadBB = llvm::BasicBlock::Create(TheContext, "dead", TheFunction);
    Builder.SetInsertPoint(DeadBB);
    return nullptr;
}

llvm::Value* ContStmt::codegen() {
    if (LoopHeaderBlocks.empty()) {
        std::cerr << "Error: Continue statement not inside a loop\n";
        return nullptr;
    }

    llvm::BasicBlock *TargetBB = nullptr;
    if (id.empty()) {
        TargetBB = LoopHeaderBlocks.back().second;
    } else {
        for (auto it = LoopHeaderBlocks.rbegin(); it != LoopHeaderBlocks.rend(); ++it) {
            if (it->first == id) { TargetBB = it->second; break; }
        }
    }

    if (!TargetBB) return nullptr;

    Builder.CreateBr(TargetBB);
    
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *DeadBB = llvm::BasicBlock::Create(TheContext, "dead", TheFunction);
    Builder.SetInsertPoint(DeadBB);
    return nullptr;
}
llvm::Value* FuncDecl::codegen() { return nullptr; }

llvm::Value* Bool::codegen() {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(TheContext), flag ? 1 : 0);
}

llvm::Value* BoolBinOp::codegen() { 
    llvm::Value *L = left->codegen();
    llvm::Value *R = right->codegen();
    if (!L || !R) return nullptr;

    // Μετατροπή των byte σε boolean
    llvm::Value *ZeroL = llvm::ConstantInt::get(L->getType(), 0);
    llvm::Value *LBool = Builder.CreateICmpNE(L, ZeroL, "l_bool");

    llvm::Value *ZeroR = llvm::ConstantInt::get(R->getType(), 0);
    llvm::Value *RBool = Builder.CreateICmpNE(R, ZeroR, "r_bool");

    llvm::Value *Res;
    if (op == '&') {
        Res = Builder.CreateAnd(LBool, RBool, "and_res");
    } else if (op == '|') {
        Res = Builder.CreateOr(LBool, RBool, "or_res");
    } else {
        return nullptr;
    }

    return Builder.CreateZExt(Res, llvm::Type::getInt8Ty(TheContext), "bool_zext");
}

llvm::Value* FuncStmt::codegen() { 
    if(fc) return fc->codegen();
    return nullptr;
}

llvm::Value* NotExpr::codegen() {
    if (!expr) return nullptr;
    llvm::Value *val = expr->codegen();
    if (!val) return nullptr;
    
    llvm::Constant *Zero = llvm::ConstantInt::get(val->getType(), 0);
    llvm::Value *cmp = Builder.CreateICmpEQ(val, Zero, "not_cmp");
    return Builder.CreateZExt(cmp, val->getType(), "not_val");
}

llvm::Value* Cond::codegen() {
    if (single != nullptr) { 
        llvm::Value *val = single->codegen();
        if (!val) return nullptr;
        
        if (val->getType()->isIntegerTy(1)) {
            return val;
        } else {
            llvm::Constant *Zero = llvm::ConstantInt::get(val->getType(), 0);
            return Builder.CreateICmpNE(val, Zero, "cond_single");
        }
    }



    if (oper == "and" || oper == "or") {
        llvm::Value *lhsVal = left->codegen(); 
        if (!lhsVal) return nullptr; 

        llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
        
        llvm::BasicBlock *LhsBB = Builder.GetInsertBlock();
        
        llvm::BasicBlock *EvalRightBB = llvm::BasicBlock::Create(TheContext, "eval_right", TheFunction);
        llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(TheContext, "merge_cond");

        if (oper == "and") {
            Builder.CreateCondBr(lhsVal, EvalRightBB, MergeBB);
        } else {
            Builder.CreateCondBr(lhsVal, MergeBB, EvalRightBB);
        }

        Builder.SetInsertPoint(EvalRightBB);
        llvm::Value *rhsVal = right->codegen();
        if (!rhsVal) return nullptr;
        
        llvm::BasicBlock *RhsBB = Builder.GetInsertBlock(); 
        Builder.CreateBr(MergeBB);

        TheFunction->insert(TheFunction->end(), MergeBB);
        Builder.SetInsertPoint(MergeBB);
        
        llvm::PHINode *PN = Builder.CreatePHI(llvm::Type::getInt1Ty(TheContext), 2, "cond_res");

        if (oper == "and") {
            PN->addIncoming(llvm::ConstantInt::getFalse(TheContext), LhsBB);
            PN->addIncoming(rhsVal, RhsBB);
        } else {
            PN->addIncoming(llvm::ConstantInt::getTrue(TheContext), LhsBB);
            PN->addIncoming(rhsVal, RhsBB);
        }

        return PN;
    }

    llvm::Value *lhsVal = left->codegen(); 
    llvm::Value *rhsVal = right->codegen(); 

    SymbolType lt = left->getsymbol();
    SymbolType rt = right->getsymbol();
    
    if (!lhsVal || !rhsVal) return nullptr; 

    if (lhsVal->getType() != rhsVal->getType()) { 
        if (lhsVal->getType()->getIntegerBitWidth() < rhsVal->getType()->getIntegerBitWidth()) { 
            if (lt == TYPE_BYTE || lt == TYPE_REF_BYTE)
                lhsVal = Builder.CreateZExt(lhsVal, rhsVal->getType(), "zext_lhs"); 
            else
                lhsVal = Builder.CreateSExt(lhsVal, rhsVal->getType(), "sext_lhs");
        } else { 
            if (rt == TYPE_BYTE || rt == TYPE_REF_BYTE)
                rhsVal = Builder.CreateZExt(rhsVal, lhsVal->getType(), "zext_rhs"); 
            else
                rhsVal = Builder.CreateSExt(rhsVal, lhsVal->getType(), "sext_rhs");
        }
    }
 
    if (lt == TYPE_REF_BYTE) lt = TYPE_BYTE;
    else if (lt == TYPE_REF_INT) lt = TYPE_INT;

    if (rt == TYPE_REF_BYTE) rt = TYPE_BYTE;
    else if (rt == TYPE_REF_INT) rt = TYPE_INT;

    if (!lhsVal || !rhsVal) return nullptr;

    bool bothUnsigned = (lt == TYPE_BYTE && rt == TYPE_BYTE);

    if (oper == "=")  return Builder.CreateICmpEQ(lhsVal, rhsVal, "cmp_eq"); 
    if (oper == "<>") return Builder.CreateICmpNE(lhsVal, rhsVal, "cmp_ne"); 

    if (bothUnsigned) {
        if (oper == ">")  return Builder.CreateICmpUGT(lhsVal, rhsVal, "cmp_ugt"); 
        if (oper == "<")  return Builder.CreateICmpULT(lhsVal, rhsVal, "cmp_ult"); 
        if (oper == ">=") return Builder.CreateICmpUGE(lhsVal, rhsVal, "cmp_uge"); 
        if (oper == "<=") return Builder.CreateICmpULE(lhsVal, rhsVal, "cmp_ule"); 
    } else {
        if (oper == ">")  return Builder.CreateICmpSGT(lhsVal, rhsVal, "cmp_sgt"); 
        if (oper == "<")  return Builder.CreateICmpSLT(lhsVal, rhsVal, "cmp_slt"); 
        if (oper == ">=") return Builder.CreateICmpSGE(lhsVal, rhsVal, "cmp_sge"); 
        if (oper == "<=") return Builder.CreateICmpSLE(lhsVal, rhsVal, "cmp_sle");
    }

    return nullptr;
}

llvm::Value* NotCond::codegen() {
    if (!con) return nullptr;
    llvm::Value *condVal = con->codegen();
    if (!condVal) return nullptr;
    
    if (condVal->getType()->isIntegerTy(1)) {
        return Builder.CreateNot(condVal, "not_cond");
    } else {
        llvm::Constant *Zero = llvm::ConstantInt::get(condVal->getType(), 0);
        return Builder.CreateICmpEQ(condVal, Zero, "not_cond_cmp"); // 0 γίνεται true (1)
    }
}
llvm::Value* Opt_Elif_Else::codegen() {
    if (condition == nullptr) {
        if (stmts) stmts->codegen();
        return nullptr;
    }

    // Elif συνθήκη
    llvm::Value *condVal = condition->codegen();
    if (!condVal) return nullptr;

    llvm::Constant *Zero = llvm::ConstantInt::get(condVal->getType(), 0);
    condVal = Builder.CreateICmpNE(condVal, Zero, "elif_cond");

    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(TheContext, "elif_then", TheFunction);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(TheContext, "elif_else");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(TheContext, "elif_merge");

    bool hasNextElse = (elif_else != nullptr);
    Builder.CreateCondBr(condVal, ThenBB, hasNextElse ? ElseBB : MergeBB);

    // -- Elif Then Block --
    Builder.SetInsertPoint(ThenBB);
    if (stmts) stmts->codegen();
    if (!Builder.GetInsertBlock()->getTerminator()) Builder.CreateBr(MergeBB);

    // -- Elif Else Block --
    if (hasNextElse) {
        TheFunction->insert(TheFunction->end(), ElseBB);
        Builder.SetInsertPoint(ElseBB);
        elif_else->codegen();
        if (!Builder.GetInsertBlock()->getTerminator()) Builder.CreateBr(MergeBB);
    }

    // -- Merge Block --
    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder.SetInsertPoint(MergeBB);

    return nullptr;
}

// --- Lvalue GEP  ---
llvm::Value* Lvalue::getPointer() {
    if (!str.empty()) {
        llvm::Value *StringPtr = Builder.CreateGlobalStringPtr(str);
        if (!expr_list) return StringPtr;
        
        llvm::Value *IdxVal = expr_list->getExprs()[0]->codegen();
        if (expr_list->getExprs()[0]->getsymbol() == TYPE_BYTE || expr_list->getExprs()[0]->getsymbol() == TYPE_REF_BYTE) {
            IdxVal = Builder.CreateZExt(IdxVal, llvm::Type::getInt64Ty(TheContext), "str_idx_zext");
        } else {
            IdxVal = Builder.CreateSExt(IdxVal, llvm::Type::getInt64Ty(TheContext), "str_idx_sext");
        }
        return Builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(TheContext), StringPtr, {IdxVal}, "stridx");
    }

    if (!id) return nullptr;

    SymbolEntry *entry = st.lookup(id->getName());
    if (!entry || !entry->alloca_ptr) return nullptr;

    llvm::Value *Ptr = entry->alloca_ptr;

    bool isRef = (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE ||
                 (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)));

    if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
        Ptr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id->getName() + "_actual_ptr");
    }

    if (expr_list) {
        llvm::Value *FlatIdx = nullptr;
        std::vector<int> actual_dims = ArrayDimensions[entry];
        
        std::vector<int> multipliers;
        if (actual_dims.size() > 1) {
            for (size_t i = 1; i < actual_dims.size(); ++i) multipliers.push_back(actual_dims[i]);
        }

        auto exprs = expr_list->getExprs();
        for (size_t i = 0; i < exprs.size(); ++i) {
            llvm::Value *IdxVal = exprs[i]->codegen();
            IdxVal = Builder.CreateSExt(IdxVal, llvm::Type::getInt64Ty(TheContext), "idx_sext");

            if (!FlatIdx) {
                FlatIdx = IdxVal;
            } else {
                int currentDimSize = (i - 1 < multipliers.size()) ? multipliers[i - 1] : 1;
                llvm::Value *DimVal = llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), currentDimSize);
                FlatIdx = Builder.CreateMul(FlatIdx, DimVal, "flat_mul");
                FlatIdx = Builder.CreateAdd(FlatIdx, IdxVal, "flat_add");
            }
        }

        int remaining_mult = 1;
        for (size_t i = exprs.size(); i < actual_dims.size(); ++i) {
            remaining_mult *= actual_dims[i];
        }
        
        if (remaining_mult > 1) {
            llvm::Value *RemVal = llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), remaining_mult);
            FlatIdx = Builder.CreateMul(FlatIdx, RemVal, "flat_rem_mul");
        }

        llvm::Type *BaseTy = (entry->type == TYPE_BYTE || entry->type == TYPE_REF_BYTE || 
                     (entry->type == TYPE_PARAM && (entry->return_value == TYPE_BYTE || entry->return_value == TYPE_REF_BYTE))) 
                     ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt32Ty(TheContext);

        if (auto *GV = llvm::dyn_cast<llvm::GlobalVariable>(Ptr)) {
            if (GV->getValueType()->isArrayTy()) {
                llvm::Value *Zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), 0);
                return Builder.CreateInBoundsGEP(GV->getValueType(), Ptr, {Zero, FlatIdx}, "arrayidx");
            }
        }
        return Builder.CreateInBoundsGEP(BaseTy, Ptr, {FlatIdx}, "arrayidx");
    }

    return Ptr;
}
