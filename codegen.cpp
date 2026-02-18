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

// --- Loops ---
std::vector<llvm::BasicBlock*> LoopMergeBlocks;
std::vector<llvm::BasicBlock*> LoopHeaderBlocks;

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
    if (!id) {
        std::cerr << "Error: Func_Call has null id\n";
        return nullptr;
    }
    if (!TheModule) {
        std::cerr << "Error: TheModule is not initialized\n";
        return nullptr;
    }
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
        std::vector<Id*> ids = idlst->getIds();
        llvm::Type *llvmType = getLLVMType(tp->getsymbol());
        
        // 1. Υπολογισμός του μεγέθους του πίνακα (αν είναι πίνακας)
        llvm::Value *ArraySize = nullptr;
        std::vector<int> dims = tp->getDims();
        if (!dims.empty()) {
            int totalSize = 1;
            for (int d : dims) {
                totalSize *= d;
            }
            // Λέμε στο LLVM πόσα στοιχεία θέλουμε συνολικά (ως 32-bit ακέραιο)
            ArraySize = llvm::ConstantInt::get(llvm::Type::getInt32Ty(TheContext), totalSize);
        }

        // 2. Ασφαλής ανάκτηση της τρέχουσας συνάρτησης
        llvm::BasicBlock *InsertBlock = Builder.GetInsertBlock();
        if (!InsertBlock) {
            std::cerr << "Error: No insert block in LocalDef for variable declaration\n";
            return nullptr;
        }
        llvm::Function *TheFunction = InsertBlock->getParent();
        if (!TheFunction) {
            std::cerr << "Error: Cannot get parent function in LocalDef\n";
            return nullptr;
        }

        // Δημιουργία TmpBuilder στην αρχή του entry block
        llvm::IRBuilder<> TmpBuilder(&TheFunction->getEntryBlock(), 
                                     TheFunction->getEntryBlock().begin());

        // 3. Δέσμευση της μνήμης για κάθε μεταβλητή
        for (Id* id : ids) {
            st.insert(id->getName(), tp->getsymbol());
            
            llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(llvmType, ArraySize, id->getName());
            
            SymbolEntry *e = st.lookup(id->getName());
            if (e) e->alloca_ptr = Alloca;
        }
    }return nullptr; // Declarations don't return a value
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
    SymbolEntry *e = st.lookup(id); // Αναζήτηση στον πίνακα συμβόλων 
    if (!e) {
        std::cerr << "Unknown variable: " << id << "\n";
        return nullptr;
    }
    
    if (e->alloca_ptr) {
        // Παίρνουμε τον σωστό τύπο LLVM από τον τύπο που αποθηκεύσαμε στο Symbol Table 
        llvm::Type *llvmType = getLLVMType(e->type);
        
        // Εκτελούμε Load χρησιμοποιώντας ρητά τον τύπο (απαραίτητο για LLVM 18) 
        if (!llvmType) {
            std::cerr << "Error: Could not get LLVM type for variable " << id << "\n";
            return nullptr;
        }
        return Builder.CreateLoad(llvmType, e->alloca_ptr, id.c_str());
    }
    
    std::cerr << "Variable " << id << " has no allocated memory.\n";
    return nullptr;
}

// --- Lvalue (Assignment target) ---
llvm::Value* Lvalue::codegen() {
    llvm::Value *Ptr = getPointer();
    if (!Ptr) return nullptr;

    // Αν είναι String Literal, επιστρέφουμε τη διεύθυνσή του απευθείας
    if (!str.empty()) return Ptr;

    SymbolEntry *entry = st.lookup(id->getName());
    if (!entry) return nullptr;

    // Ελέγχουμε αν η μεταβλητή είναι τοπικός πίνακας (έχει γίνει alloca με μέγεθος > 1)
    bool isLocalArray = false;
    if (llvm::AllocaInst *AI = llvm::dyn_cast<llvm::AllocaInst>(Ptr)) {
        if (AI->isArrayAllocation()) isLocalArray = true;
    }

    // Αν ΔΕΝ ζητάμε συγκεκριμένο στοιχείο ΚΑΙ είναι πίνακας (είτε ref είτε local array)
    if (!expr_list && (entry->type == TYPE_REF_INT || entry->type == TYPE_REF_BYTE || 
        (entry->type == TYPE_PARAM && (entry->return_value == TYPE_REF_INT || entry->return_value == TYPE_REF_BYTE)) || 
        isLocalArray)) {
        return Ptr;
    }

    // Διαφορετικά, κάνουμε Load την τιμή (π.χ. a[i] ή x)
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
    
    // Ensure operands are not pointers (must be loaded values)
    if (lhs->getType()->isPointerTy()) {
        std::cerr << "Error: BinOp left operand is a pointer, should be a loaded value\n";
        return nullptr;
    }
    if (rhs->getType()->isPointerTy()) {
        std::cerr << "Error: BinOp right operand is a pointer, should be a loaded value\n";
        return nullptr;
    }
    
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
        std::cerr << "Error: FuncDefBlock has null header\\n";
        return nullptr;
    }
    
    // 1. Άνοιγμα νέου scope για τη συνάρτηση (Rescoping) 
    st.openScope();

    // 2. Προετοιμασία τύπων παραμέτρων 
    std::vector<llvm::Type*> argTypes;
    std::vector<std::pair<std::string, SymbolType>> paramInfo;
    
    if (header->getOpt_Fpar() != nullptr) {
        paramInfo = header->getOpt_Fpar()->getParams();
        for (const auto& param : paramInfo) {
            // Έλεγχος: αν είναι TYPE_INT, χρησιμοποίησε i16
            if (param.second == TYPE_INT) {
                argTypes.push_back(llvm::Type::getInt16Ty(TheContext));
            } else if (param.second == TYPE_BYTE) {
                // Αν είναι TYPE_BYTE, χρησιμοποίησε i8
                argTypes.push_back(llvm::Type::getInt8Ty(TheContext));
            } else if (param.second == TYPE_REF_INT) {
                // Reference to int: pass as i16*
                argTypes.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt16Ty(TheContext)));
            } else if (param.second == TYPE_REF_BYTE) {
                // Reference to byte: pass as i8*
                argTypes.push_back(llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(TheContext)));
            } else {
                llvm::Type *paramType = getLLVMType(param.second);
                if (!paramType) {
                    std::cerr << "Error: Invalid parameter type\n";
                    return nullptr;
                }
                argTypes.push_back(paramType);
            }
        }
    }

    // 3. Δημιουργία τύπου συνάρτησης και της ίδιας της συνάρτησης 
    llvm::Type *returnType = (header->gettype()) ? getLLVMType(header->gettype()->getsymbol()) : llvm::Type::getVoidTy(TheContext);
    if (!returnType) {
        std::cerr << "Error: Could not get return type for function " << header->getName() << "\n";
        return nullptr;
    }
    llvm::FunctionType *FT = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function *TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, header->getName(), TheModule.get());

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

            // For references/arrays (byte[], ref int), store the pointer directly without intermediate alloca
            if (pType == TYPE_REF_BYTE || pType == TYPE_REF_INT) {
                // For references, the arg IS the pointer we want to use
                if (e) e->alloca_ptr = &arg;
            } else {
                // For numeric parameters, create alloca and store the value
                llvm::AllocaInst *Alloca = TmpBuilder.CreateAlloca(arg.getType(), nullptr, pName);
                TmpBuilder.CreateStore(&arg, Alloca);
                if (e) e->alloca_ptr = Alloca;
            }
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

    // 9. Διαχείριση Return 
    if (returnType->isVoidTy()) {
        if (!Builder.GetInsertBlock()->getTerminator()) Builder.CreateRetVoid();
    } else if (!Builder.GetInsertBlock()->getTerminator()) {
        Builder.CreateRet(llvm::ConstantInt::get(TheContext, llvm::APInt(16, 0, true)));
    }

    // Κλείσιμο scope 
    st.closeScope();

    // Επαναφορά στον προηγούμενο Builder για να συνεχίσει η εξωτερική συνάρτηση 
    if (prevBB) Builder.SetInsertPoint(prevBB);

    llvm::verifyFunction(*TheFunction);
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
    // 1. Find the function
    llvm::Function *CalleeF = TheModule->getFunction(id->getName());
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
            
            // ΕΞΥΠΝΟ ΚΟΛΠΟ: Αν η συνάρτηση ζητάει pointer (π.χ. ref int ή byte[]),
            // αλλά εμείς έχουμε φορτωμένη τιμή (i16/i8), δώσε της τη διεύθυνση!
            if (i < CalleeF->arg_size() && CalleeF->getArg(i)->getType()->isPointerTy() && !argVal->getType()->isPointerTy()) {
                if (Lvalue *lval = dynamic_cast<Lvalue*>(arg)) {
                    argVal = lval->getPointer(); // Περάσαμε τη διεύθυνση (pass-by-reference)!
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
llvm::Value* ExitStmt::codegen() { return nullptr; }

llvm::Value* LoopBlock::codegen() {
    
    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
    if (!TheFunction) return nullptr;

    // Δημιουργία των Basic Blocks
    llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(TheContext, "loop", TheFunction);
    llvm::BasicBlock *BodyBB = llvm::BasicBlock::Create(TheContext, "body", TheFunction);
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(TheContext, "loop_merge", TheFunction);

    // Αποθήκευση στη στοίβα για τα break/continue
    LoopHeaderBlocks.push_back(LoopBB);
    LoopMergeBlocks.push_back(MergeBB);

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

    // Επιστροφή στην αρχή του loop
    Builder.CreateBr(LoopBB);

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
    // Πήδα στο Merge (τέλος του loop)
    Builder.CreateBr(LoopMergeBlocks.back());
    
    // Φτιάχνουμε ένα νεκρό block για να μην κρασάρει ο Builder αν υπάρχουν εντολές μετά το break
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
    // Πήδα στο Header (αρχή του loop)
    Builder.CreateBr(LoopHeaderBlocks.back());
    
    // Νεκρό block
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
        if (lhsVal->getType()->getPrimitiveSizeInBits() < rhsVal->getType()->getPrimitiveSizeInBits()) { 
            lhsVal = Builder.CreateSExt(lhsVal, rhsVal->getType(), "sext_lhs"); 
        } else { 
            rhsVal = Builder.CreateSExt(rhsVal, lhsVal->getType(), "sext_rhs"); 
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

// --- Lvalue GEP ---
llvm::Value* Lvalue::getPointer() {
    if (!str.empty()) {
        // Καθαρίζουμε το string πριν το δώσουμε στον LLVM
        std::string cleanStr = unescapeString(str);
        return Builder.CreateGlobalStringPtr(cleanStr);
    }
    if (!id) return nullptr;

    SymbolEntry *entry = st.lookup(id->getName());
    if (!entry || !entry->alloca_ptr) return nullptr;

    llvm::Value *Ptr = entry->alloca_ptr;
    llvm::Type *BaseTy = getLLVMType(entry->type);

    // Αν είναι δείκτης (π.χ. byte[] σαν παράμετρος), κάνουμε Load τη διεύθυνση
    if (entry->type == TYPE_REF_BYTE || entry->type == TYPE_REF_INT || 
       (entry->type == TYPE_PARAM && BaseTy->isPointerTy())) {
        Ptr = Builder.CreateLoad(BaseTy, Ptr, id->getName() + "_base");
        
        // --- ΝΕΟΣ ΕΛΕΓΧΟΣ: Ελέγχουμε αν η βάση είναι BYTE ή REF_BYTE ---
        bool isByte = (entry->type == TYPE_BYTE) || (entry->type == TYPE_REF_BYTE) || 
                      (entry->type == TYPE_PARAM && (entry->return_value == TYPE_BYTE || entry->return_value == TYPE_REF_BYTE));
        BaseTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt16Ty(TheContext);
    }

    // Εξασφαλίζουμε ότι το BaseTy είναι σωστό ΠΡΙΝ υπολογίσουμε τη θέση [i]
    bool isByte = (entry->type == TYPE_BYTE) || (entry->type == TYPE_REF_BYTE) || 
                  (entry->type == TYPE_PARAM && (entry->return_value == TYPE_BYTE || entry->return_value == TYPE_REF_BYTE));
                  
    BaseTy = isByte ? llvm::Type::getInt8Ty(TheContext) : llvm::Type::getInt16Ty(TheContext);

    // Διαχείριση των θέσεων [i] (Array Indexing)
    if (expr_list) {
        for (Expr *idx_expr : expr_list->getExprs()) {
            llvm::Value *IdxVal = idx_expr->codegen();
            if (!IdxVal) continue;
            IdxVal = Builder.CreateSExt(IdxVal, llvm::Type::getInt64Ty(TheContext), "idx_ext");
            
            // Το BaseTy εδώ θα είναι i8 για τα strings και i16 για τα ints!
            Ptr = Builder.CreateInBoundsGEP(BaseTy, Ptr, {IdxVal}, "arrayidx");
        }
    }

    return Ptr;
}