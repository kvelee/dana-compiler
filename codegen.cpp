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

// άρτης που συνδέει το Scope Symbol με τη μετονομασμένη LLVM συνάρτηση (π.χ. @f.2)
std::map<SymbolEntry*, llvm::Function*> FuncPointers;

bool isOutermostFunction = true;

// Helper function for extracting the LLVM-Type
llvm::Type *getLLVMType(SymbolType t) {
    switch (t) {
        case TYPE_INT: 
            return llvm::Type::getInt16Ty(TheContext);
        case TYPE_BYTE: 
            return llvm::Type::getInt8Ty(TheContext);
        case TYPE_BOOL: 
            return llvm::Type::getInt8Ty(TheContext);
        case TYPE_PARAM:
            // TYPE_PARAM defaults to i16 for numeric parameters
            return llvm::Type::getInt16Ty(TheContext);
        case TYPE_REF_INT: 
            return llvm::PointerType::getUnqual(llvm::Type::getInt16Ty(TheContext));
        case TYPE_REF_BYTE:
            return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext));
        case TYPE_FUNC:    // Functions should never need alloca, but return void as fallback
            return llvm::Type::getVoidTy(TheContext);
        case TYPE_INVALID:
        default: 
            std::cerr << "Warning: getLLVMType called with invalid/unknown type. Returning Int16Ty as fallback." << std::endl;
            return llvm::Type::getInt16Ty(TheContext);
    }
}

void initializeLLVM() {
    TheModule = std::make_unique<llvm::Module>("DanaModule", TheContext);
    
    // writeInteger: n as int
    std::vector<llvm::Type*> writeIntArgs = {llvm::Type::getInt16Ty(TheContext)};
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
    llvm::FunctionType *readIntFT = llvm::FunctionType::get(llvm::Type::getInt16Ty(TheContext), false);
    llvm::Function::Create(readIntFT, llvm::Function::ExternalLinkage, "readInteger", TheModule.get());

    // readByte is byte / readChar is byte
    llvm::FunctionType *readByteFT = llvm::FunctionType::get(llvm::Type::getInt8Ty(TheContext), false);
    llvm::Function::Create(readByteFT, llvm::Function::ExternalLinkage, "readByte", TheModule.get());
    llvm::Function::Create(readByteFT, llvm::Function::ExternalLinkage, "readChar", TheModule.get());

    // readString: n as int, s as byte []
    std::vector<llvm::Type*> readStrArgs = {
        llvm::Type::getInt16Ty(TheContext), 
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))
    };
    llvm::FunctionType *readStrFT = llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), readStrArgs, false);
    llvm::Function::Create(readStrFT, llvm::Function::ExternalLinkage, "readString", TheModule.get());
    
    // extend: b as byte -> int (i8 to i16)
    std::vector<llvm::Type*> extArgs = {llvm::Type::getInt8Ty(TheContext)};
    llvm::FunctionType *extFT = llvm::FunctionType::get(llvm::Type::getInt16Ty(TheContext), extArgs, false);
    llvm::Function::Create(extFT, llvm::Function::ExternalLinkage, "extend", TheModule.get());

    // shrink: i as int -> byte (i16 to i8)
    std::vector<llvm::Type*> shrArgs = {llvm::Type::getInt16Ty(TheContext)};
    llvm::FunctionType *shrFT = llvm::FunctionType::get(llvm::Type::getInt8Ty(TheContext), shrArgs, false);
    llvm::Function::Create(shrFT, llvm::Function::ExternalLinkage, "shrink", TheModule.get());

    // strlen is int: s as byte []
    std::vector<llvm::Type*> strlenArgs = {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))};
    llvm::FunctionType *strlenFT = llvm::FunctionType::get(llvm::Type::getInt16Ty(TheContext), strlenArgs, false);
    llvm::Function::Create(strlenFT, llvm::Function::ExternalLinkage, "strlen", TheModule.get());

    // strcmp is int: s1 s2 as byte []
    std::vector<llvm::Type*> strcmpArgs = {
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext)),
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext))
    };
    llvm::FunctionType *strcmpFT = llvm::FunctionType::get(llvm::Type::getInt16Ty(TheContext), strcmpArgs, false);
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

// Βοηθητική συνάρτηση για μετατροπή escape sequences
std::string unescapeString(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        // Αν βρούμε backslash και υπάρχει επόμενος χαρακτήρας
        if (input[i] == '\\' && i + 1 < input.length()) {
            switch (input[i+1]) {
                case 'n': result += '\n'; break; // Αλλαγή γραμμής
                case 't': result += '\t'; break; // Tab
                case 'r': result += '\r'; break; // Carriage return
                case '0': result += '\0'; break; // Null terminator
                case '\\': result += '\\'; break; // Απλό backslash
                default: 
                    result += '\\'; 
                    result += input[i+1]; 
                    break;
            }
            i++; // Προσπερνάμε τον χαρακτήρα που μόλις μετατρέψαμε (π.χ. το 'n')
        } else {
            result += input[i];
        }
    }
    return result;
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
    uint8_t val = (uint8_t)character;

    // Αν ο Lexer έστειλε ASCII 92 (backslash), σημαίνει ότι διάβασε το '\' 
    // από το '\n' και έχασε το 'n'. Το μετατρέπουμε χειροκίνητα σε 10 (newline).
    if (val == 92) {
        val = 10; 
    } 
    // Αν ο Lexer έστειλε το γράμμα 'n' (ASCII 110) ενώ περιμέναμε escape sequence
    else if (val == 'n') {
        val = 10;
    }
    // Προσθήκη για tab αν χρειαστεί (αν και το πρόβλημά σου είναι το newline)
    else if (val == 't') {
        val = 9;
    }

    // Dana bytes are 8-bit
    return llvm::ConstantInt::get(TheContext, llvm::APInt(8, val, false));
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
    if (!id) {
        std::cerr << "Error: Func_Call has null id\n";
        return nullptr;
    }
    if (!TheModule) {
        std::cerr << "Error: TheModule is not initialized\n";
        return nullptr;
    }

    // 1. Βρες τη συνάρτηση (αντικατέστησε το TheModule->getFunction...)
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

    // 2. Ετοίμασε τα ορίσματα (Args)
    std::vector<llvm::Value *> ArgsV;
    if (list) {
        unsigned i = 0;
        for (Expr *arg : list->getExprs()) { 
            llvm::Value *argVal = arg->codegen();
            
            // ΑΣΦΑΛΕΙΑ 1: Αν η παραγωγή του ορίσματος απέτυχε, σταμάτα ομαλά αντί για SegFault!
            if (!argVal) return nullptr;
            
            // --- ΤΟ ΑΝΑΒΑΘΜΙΣΜΕΝΟ ΕΞΥΠΝΟ ΚΟΛΠΟ ---
            if (i < CalleeF->arg_size() && CalleeF->getArg(i)->getType()->isPointerTy() && !argVal->getType()->isPointerTy()) {
                if (Lvalue *lval = dynamic_cast<Lvalue*>(arg)) {
                    argVal = lval->getPointer();
                } else if (Id *id_arg = dynamic_cast<Id*>(arg)) {
                    SymbolEntry *entry = st.lookup(id_arg->getName());
                    if (entry && entry->alloca_ptr) {
                        llvm::Value *Ptr = entry->alloca_ptr;
                        // Ελέγχουμε αν είναι pass-by-reference
                        bool isRef = (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE ||
                                     (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)));
                        if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
                            Ptr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id_arg->getName() + "_ref_ptr");
                        }
                        argVal = Ptr;
                    }
                }
            }
            
            ArgsV.push_back(argVal);
            if (!ArgsV.back()) return nullptr;
            i++;
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

// --- Id (Variable reference) ---
llvm::Value* Id::codegen() {
    SymbolEntry *e = st.lookup(id); 
    if (!e || !e->alloca_ptr) return nullptr;
    
    // Έλεγχος: Μην κάνεις load αν πρόκειται για Πίνακα
    if (auto *GV = llvm::dyn_cast<llvm::GlobalVariable>(e->alloca_ptr)) {
        if (GV->getValueType()->isArrayTy()) return e->alloca_ptr;
    } else if (auto *AI = llvm::dyn_cast<llvm::AllocaInst>(e->alloca_ptr)) {
        if (AI->getAllocatedType()->isArrayTy() || AI->isArrayAllocation()) return e->alloca_ptr;
    }

    // ΣΩΣΤΟΣ ΕΛΕΓΧΟΣ: Πιάνουμε και τις παραμέτρους (TYPE_PARAM) που είναι reference!
    bool isRef = (e->type == TYPE_REF_INT || e->type == TYPE_REF_BYTE) ||
                 (e->type == TYPE_PARAM && (e->return_value == TYPE_REF_INT || e->return_value == TYPE_REF_BYTE));

    if (isRef && !llvm::isa<llvm::GlobalVariable>(e->alloca_ptr)) {
        llvm::Value *PtrToPtr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), e->alloca_ptr, id + "_ptr");
        
        bool isByte = (e->type == TYPE_REF_BYTE) || (e->type == TYPE_PARAM && e->return_value == TYPE_REF_BYTE);
        llvm::Type *ValueTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt16Ty(TheContext);
        
        return Builder.CreateLoad(ValueTy, PtrToPtr, id.c_str());
    }
    
    llvm::Type *llvmType = getLLVMType(e->type);
    return Builder.CreateLoad(llvmType, e->alloca_ptr, id.c_str());
}

// --- Lvalue (Assignment target) ---
llvm::Value* Lvalue::codegen() {
    llvm::Value *Ptr = getPointer();
    if (!Ptr) return nullptr;
    if (!str.empty()) return Ptr;

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
    llvm::Type *ValTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt16Ty(TheContext);

    return Builder.CreateLoad(ValTy, Ptr, id->getName() + "_val");
}


// --- LvalueStmt (Assignment) ---
llvm::Value* LvalueStmt::codegen() {
    if (!l) return nullptr;

    // Βρίσκουμε τον σωστό δείκτη (είτε είναι απλή μεταβλητή είτε στοιχείο πίνακα a[i])
    llvm::Value *LValPtr = l->getPointer(); 
    if (!LValPtr) {
        std::cerr << "Error: Could not get pointer for assignment\n";
        return nullptr;
    }
    
    llvm::Value *RVal = e->codegen();
    if (!RVal) return nullptr;
    
    // Αποθηκεύουμε την τιμή στη μνήμη
    return Builder.CreateStore(RVal, LValPtr);
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

    // Promotion: Αν διαφέρουν τα μεγέθη, επέκτεινε το μικρότερο
    if (lhs->getType() != rhs->getType()) {
        if (lhs->getType()->getPrimitiveSizeInBits() < rhs->getType()->getPrimitiveSizeInBits())
            lhs = Builder.CreateSExt(lhs, rhs->getType(), "lhs_ext");
        else
            rhs = Builder.CreateSExt(rhs, lhs->getType(), "rhs_ext");
    }

    switch (op) {
        case '+': return Builder.CreateAdd(lhs, rhs, "addtmp");
        case '-': return Builder.CreateSub(lhs, rhs, "subtmp");
        case '*': return Builder.CreateMul(lhs, rhs, "multmp");
        case '/': return Builder.CreateSDiv(lhs, rhs, "divtmp");
        case '%': return Builder.CreateSRem(lhs, rhs, "remtmp");
        default: return nullptr;
    }
}

// --- ReturnStmt ---
llvm::Value* ReturnStmt::codegen() {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::Type *retType = TheFunction->getReturnType();

    if (!e) { 
        // Αν η συνάρτηση είναι η "main" που επιστρέφει i32, δώσε 0 με ασφάλεια
        if (TheFunction->getName() == "main") {
            return Builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), 0));
        }
        return Builder.CreateRetVoid(); 
    }
    
    llvm::Value *val = e->codegen();
    if (!val) return nullptr;

    // Εξασφάλισε ότι το return type ταιριάζει με τη συνάρτηση
    if (val->getType() != retType && !retType->isVoidTy()) {
        if (val->getType()->getIntegerBitWidth() < retType->getIntegerBitWidth())
            val = Builder.CreateSExt(val, retType, "ret_ext");
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
    
    // Η ΜΑΓΕΙΑ: Αν είναι η πρώτη συνάρτηση, την βαφτίζουμε "main" για τον LLVM
    if (isOutermostFunction) {
        llvmName = "main";
        isOutermostFunction = false; // Όλες οι επόμενες φωλιασμένες παραμένουν ως έχουν
    } else {
        // Εισαγωγή στο scope μόνο για τις εσωτερικές (η εξωτερική έχει ήδη μπει από τη sem)
        SymbolType retTypeSym = header->gettype() ? header->gettype()->getsymbol() : TYPE_FUNC;
        st.insert(originalName, TYPE_FUNC, retTypeSym);
    }

    SymbolEntry *funcEntry = st.lookup(originalName);

    // 1. Άνοιγμα νέου scope για τη συνάρτηση (Rescoping) 
    st.openScope();

    // 2. Προετοιμασία τύπων παραμέτρων
    std::vector<llvm::Type*> argTypes;
    std::vector<std::pair<std::string, SymbolType>> paramInfo;
    
    if (header->getOpt_Fpar() != nullptr) {
        paramInfo = header->getOpt_Fpar()->getParams();
        for (const auto& param : paramInfo) {
            if (param.second == TYPE_INT) {
                argTypes.push_back(llvm::Type::getInt16Ty(TheContext));
            } else if (param.second == TYPE_BYTE) {
                argTypes.push_back(llvm::Type::getInt8Ty(TheContext));
            } else if (param.second == TYPE_REF_INT) {
                argTypes.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt16Ty(TheContext)));
            } else if (param.second == TYPE_REF_BYTE) {
                argTypes.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext)));
            } else {
                llvm::Type *paramType = getLLVMType(param.second);
                argTypes.push_back(paramType);
            }
        }
    }

    // --- ΒΗΜΑ 3: Δημιουργία συνάρτησης ---
    // ΠΡΟΣΟΧΗ: Χρησιμοποιούμε το llvmName εδώ για να παραχθεί η @main!
    llvm::Type *returnType = (llvmName == "main") ? llvm::Type::getInt32Ty(TheContext) : 
        (header->gettype() ? getLLVMType(header->gettype()->getsymbol()) : llvm::Type::getVoidTy(TheContext));

    llvm::FunctionType *FT = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function *TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, llvmName, TheModule.get());

    // Συνδέουμε το αρχικό όνομα (π.χ. "solve") με το νέο function
    if (funcEntry) {
        FuncPointers[funcEntry] = TheFunction;
    }

    unsigned idx = 0;
    for (auto &Arg : TheFunction->args()) {
        if (idx < paramInfo.size()) Arg.setName(paramInfo[idx].first);
        idx++;
    }

    // 4. Αποθήκευση του τρέχοντος Insert Point (για nested functions) 
    llvm::BasicBlock *prevBB = Builder.GetInsertBlock();

    // 5. Δημιουργία Entry Block 
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
    Builder.SetInsertPoint(BB);

    // 6. Δέσμευση μνήμης (alloca) για παραμέτρους στην αρχή του entry block 
    llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
    unsigned argIndex = 0;
    for (auto& arg : TheFunction->args()) {
        if (argIndex < paramInfo.size()) {
            std::string pName = paramInfo[argIndex].first;
            SymbolType pType = paramInfo[argIndex].second;

            // Επανεισαγωγή παραμέτρου στον Symbol Table 
            st.insert(pName, TYPE_PARAM, pType);
            SymbolEntry *e = st.lookup(pName);

            // Treat ALL parameters uniformly: create alloca and store the parameter
            llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(arg.getType(), nullptr, pName);
            TmpBuilder.CreateStore(&arg, Alloca);
            if (e) e->alloca_ptr = Alloca;
        }
        argIndex++;
    }

    // Save the current insert point (entry block for this function)
    llvm::BasicBlock *SavedBB = Builder.GetInsertBlock();

    // 7. Παραγωγή κώδικα για τοπικούς ορισμούς (εδώ μπαίνουν οι nested functions) 
    if (localDefList) localDefList->codegen();

    // ΚΡΙΣΙΜΟ: Επαναφορά του Builder στη δική μας συνάρτηση μετά τους τοπικούς ορισμούς 
    // Χωρίς αυτό, οι φωλιασμένες συναρτήσεις (π.χ. move) θα μπερδέψουν το insert point
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

    // Κλείσιμο scope 
    st.closeScope();

    // Επαναφορά στον προηγούμενο Builder για να συνεχίσει η εξωτερική συνάρτηση 
    if (prevBB) Builder.SetInsertPoint(prevBB);

    // Αντί για σκέτο llvm::verifyFunction(*TheFunction);
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
    if (pc) return pc->codegen(); // pc είναι το Proc_Call*
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
    // 1. Βρες τη συνάρτηση (αντικατέστησε το TheModule->getFunction...)
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

    // 2. Prepare the arguments (Args)
    std::vector<llvm::Value *> ArgsV;
    if (list) {
        unsigned i = 0;
        for (Expr *arg : list->getExprs()) { 
            llvm::Value *argVal = arg->codegen();
            
            // ΑΣΦΑΛΕΙΑ 1: Αν η παραγωγή του ορίσματος απέτυχε, σταμάτα ομαλά αντί για SegFault!
            if (!argVal) return nullptr;
            
            // --- ΤΟ ΑΝΑΒΑΘΜΙΣΜΕΝΟ ΕΞΥΠΝΟ ΚΟΛΠΟ ---
            if (i < CalleeF->arg_size() && CalleeF->getArg(i)->getType()->isPointerTy() && !argVal->getType()->isPointerTy()) {
                if (Lvalue *lval = dynamic_cast<Lvalue*>(arg)) {
                    argVal = lval->getPointer();
                } else if (Id *id_arg = dynamic_cast<Id*>(arg)) {
                    SymbolEntry *entry = st.lookup(id_arg->getName());
                    if (entry && entry->alloca_ptr) {
                        llvm::Value *Ptr = entry->alloca_ptr;
                        // Ελέγχουμε αν είναι pass-by-reference
                        bool isRef = (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE ||
                                     (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)));
                        if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
                            Ptr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id_arg->getName() + "_ref_ptr");
                        }
                        argVal = Ptr;
                    }
                }
            }
            
            ArgsV.push_back(argVal);
            if (!ArgsV.back()) return nullptr;
            i++;
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

llvm::Value* ExitStmt::codegen() {
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    llvm::Type *retType = TheFunction->getReturnType();

    // Αν η συνάρτηση είναι void (κανονικές διαδικασίες Dana), κάνε ret void
    if (retType->isVoidTy()) {
        Builder.CreateRetVoid();
    } else {
        // Αν δεν είναι void (π.χ. είναι η main που την κάναμε αναγκαστικά i32), 
        // επέστρεψε την τιμή 0 στον σωστό τύπο.
        Builder.CreateRet(llvm::ConstantInt::get(retType, 0));
    }

    // Επειδή φεύγουμε από τη συνάρτηση, φτιάχνουμε ένα "νεκρό" block 
    // για εντολές που τυχόν υπάρχουν από κάτω, ώστε να μην "σκάσει" ο LLVM Builder.
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

    // Μετάβαση στο header (αρχή) του loop
    Builder.CreateBr(LoopBB);
    Builder.SetInsertPoint(LoopBB);

    // Πάντα μπαίνουμε στο body (τα άπειρα loops ελέγχονται εσωτερικά)
    Builder.CreateBr(BodyBB);
    Builder.SetInsertPoint(BodyBB);

    // Open scope for the loop variables
    st.openScope();
    st.insert("__loop__", LOOP);
    
    // Generate code for the loop body
    if (loop_stmt_list) loop_stmt_list->codegen();
    
    // Close the loop scope
    st.closeScope();

    // Επιστροφή στην αρχή του loop ΜΟΝΟ αν το block δεν έχει ήδη τερματιστεί
    // (π.χ. αν δεν υπήρχε κάποιο break ή return ακριβώς πριν)
    if (!Builder.GetInsertBlock()->getTerminator()) {
        Builder.CreateBr(LoopBB);
    }

    // Συνεχίζουμε μετά το loop
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
    // Αν δεν έχει label (id), πάρε το τελευταίο (back)
    if (id.empty()) {
        TargetBB = LoopMergeBlocks.back().second;
    } else {
        // Διαφορετικά, ψάξε προς τα πίσω στη στοίβα για να βρεις το σωστό label
        for (auto it = LoopMergeBlocks.rbegin(); it != LoopMergeBlocks.rend(); ++it) {
            if (it->first == id) { TargetBB = it->second; break; }
        }
    }

    if (!TargetBB) return nullptr; // Ασφάλεια σε περίπτωση λάθους

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
    // Στην Dana τα booleans είναι bytes (i8), όπου 1 = true, 0 = false
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(TheContext), flag ? 1 : 0);
}

llvm::Value* BoolBinOp::codegen() { return nullptr; }


llvm::Value* FuncStmt::codegen() { return nullptr; }

llvm::Value* NotExpr::codegen() {
    if (!expr) return nullptr;
    llvm::Value *val = expr->codegen();
    if (!val) return nullptr;
    
    // Συγκρίνουμε με το 0 και μετατρέπουμε το αποτέλεσμα (i1) στον αρχικό τύπο
    llvm::Constant *Zero = llvm::ConstantInt::get(val->getType(), 0);
    llvm::Value *cmp = Builder.CreateICmpEQ(val, Zero, "not_cmp");
    return Builder.CreateZExt(cmp, val->getType(), "not_val");
}

llvm::Value* Cond::codegen() {
    // Διαχείριση απλής έκφρασης ως συνθήκη 
    if (single != nullptr) { 
        return single->codegen(); 
    }

    // Παραγωγή κώδικα για τα δύο σκέλη της σύγκρισης 
    llvm::Value *lhsVal = left->codegen(); 
    llvm::Value *rhsVal = right->codegen(); 
    
    if (!lhsVal || !rhsVal) return nullptr; 

    // Έλεγχος ώστε να μην συγκρίνουμε pointers (πρέπει να είναι loaded values) 
    if (lhsVal->getType()->isPointerTy() || rhsVal->getType()->isPointerTy()) { 
        std::cerr << "Error: Cond operands must be loaded values, not pointers.\n"; 
        return nullptr; 
    }

    // Εξίσωση τύπων (Promotion) αν διαφέρουν (π.χ. i8 με i16) 
    if (lhsVal->getType() != rhsVal->getType()) { 
        if (lhsVal->getType()->getIntegerBitWidth() < rhsVal->getType()->getIntegerBitWidth()) { 
            lhsVal = Builder.CreateZExt(lhsVal, rhsVal->getType(), "zext_lhs"); 
        } else { 
            rhsVal = Builder.CreateZExt(rhsVal, lhsVal->getType(), "zext_rhs"); 
        }
    }

    // Δημιουργία της εντολής σύγκρισης βάσει του τελεστή 
    if (oper == ">") { 
        return Builder.CreateICmpSGT(lhsVal, rhsVal, "cmp_gt"); 
    } else if (oper == "<") { 
        return Builder.CreateICmpSLT(lhsVal, rhsVal, "cmp_lt"); 
    } else if (oper == ">=") { 
        return Builder.CreateICmpSGE(lhsVal, rhsVal, "cmp_ge"); 
    } else if (oper == "<=") { 
        return Builder.CreateICmpSLE(lhsVal, rhsVal, "cmp_le"); 
    } else if (oper == "=") { 
        return Builder.CreateICmpEQ(lhsVal, rhsVal, "cmp_eq"); 
    } else if (oper == "<>") { 
        return Builder.CreateICmpNE(lhsVal, rhsVal, "cmp_ne"); 
    } else if (oper == "and") { 
        return Builder.CreateAnd(lhsVal, rhsVal, "and_res"); 
    } else if (oper == "or") { 
        return Builder.CreateOr(lhsVal, rhsVal, "or_res"); 
    } else { 
        std::cerr << "Unknown condition operator: " << oper << "\n"; 
        return nullptr; 
    }
}

llvm::Value* NotCond::codegen() {
    if (!con) return nullptr;
    llvm::Value *condVal = con->codegen();
    if (!condVal) return nullptr;
    
    // Αν είναι i1 (από ICmp), κάνουμε Not. Αν είναι i8/i16, συγκρίνουμε με 0.
    if (condVal->getType()->isIntegerTy(1)) {
        return Builder.CreateNot(condVal, "not_cond");
    } else {
        llvm::Constant *Zero = llvm::ConstantInt::get(condVal->getType(), 0);
        return Builder.CreateICmpEQ(condVal, Zero, "not_cond_cmp"); // 0 γίνεται true (1)
    }
}
llvm::Value* Opt_Elif_Else::codegen() {
    // Περίπτωση 1: Είναι απλό 'else' (δεν έχει συνθήκη)
    if (condition == nullptr) {
        if (stmts) stmts->codegen();
        return nullptr;
    }

    // Περίπτωση 2: Είναι 'elif' (έχει συνθήκη)
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

    // -- Elif Else Block (Αναδρομή στο επόμενο elif/else) --
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

// --- Lvalue GEP (Συμβατό με LLVM 18 / Opaque Pointers) ---
llvm::Value* Lvalue::getPointer() {
    if (!str.empty()) {
        std::string cleanStr = unescapeString(str);
        return Builder.CreateGlobalStringPtr(cleanStr);
    }
    if (!id) return nullptr;

    SymbolEntry *entry = st.lookup(id->getName());
    if (!entry || !entry->alloca_ptr) return nullptr;

    llvm::Value *Ptr = entry->alloca_ptr;

    // 1. Καθορισμός BaseTy (i8 ή i16)
    bool isByte = (entry->type == TYPE_BYTE) || (entry->type == TYPE_REF_BYTE) || 
                  (entry->type == TYPE_PARAM && (entry->return_value == TYPE_BYTE || entry->return_value == TYPE_REF_BYTE));
    llvm::Type *BaseTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt16Ty(TheContext);

    // 2. Χειρισμός Reference (Double Pointer) - LLVM 18 style
    bool isRef = (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE ||
                 (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)));

    if (isRef && !llvm::isa<llvm::GlobalVariable>(Ptr)) {
        // Στον LLVM 18, πρέπει να δώσουμε ρητά τον τύπο που φορτώνουμε (pointer)
        Ptr = Builder.CreateLoad(llvm::PointerType::getUnqual(TheContext), Ptr, id->getName() + "_ref_ptr");
    }

    // 3. Διαχείριση Array Indexing [i]
    if (expr_list) {
        for (Expr *idx_expr : expr_list->getExprs()) {
            llvm::Value *IdxVal = idx_expr->codegen();
            if (!IdxVal) continue;
            IdxVal = Builder.CreateSExt(IdxVal, llvm::Type::getInt64Ty(TheContext), "idx_ext");

            // Έλεγχος για Global Variable (Πίνακες της main)
            if (auto *GV = llvm::dyn_cast<llvm::GlobalVariable>(Ptr)) {
                // Χρησιμοποιούμε getValueType() αντί για getPointerElementType()
                if (GV->getValueType()->isArrayTy()) {
                    llvm::Value *Zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(TheContext), 0);
                    // Στο GEP δίνουμε τον τύπο του πίνακα (getValueType)
                    Ptr = Builder.CreateInBoundsGEP(GV->getValueType(), Ptr, {Zero, IdxVal}, "arrayidx");
                    continue; // Προχωράμε στο επόμενο index αν υπάρχει
                }
            }
            
            // Για τοπικούς pointers ή παραμέτρους
            Ptr = Builder.CreateInBoundsGEP(BaseTy, Ptr, {IdxVal}, "arrayidx");
        }
    }

    return Ptr;
}