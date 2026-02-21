#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include "symbol.hpp"


extern int yylineno;

namespace llvm {
    class Value;
};

class AST {
public:
    virtual void printOn(std::ostream &out) const = 0;

    virtual void sem() {}

    virtual llvm::Value* codegen() = 0; // Implemented in: codegen.cpp
};

inline std::ostream &operator<<(std::ostream &out, const AST &ast) {
    ast.printOn(out);
    return out;
}

inline std::ostream &operator<<(std::ostream &out, SymbolType t) {
    switch (t) {
    case TYPE_INT: out << "int"; break;
    case TYPE_BYTE: out << "byte"; break;
    case TYPE_REF_INT: out<<"ref int"; break;
    case TYPE_REF_BYTE: out<<"ref byte"; break;
    case TYPE_FUNC: out << "function"; break;
    case LOOP: out << "loop"; break;
    case TYPE_PARAM: out << "param"; break;
    case TYPE_INVALID: out << "invalid"; break;
    case TYPE_BOOL: out << "bool"; break;
    default: out << "unknown"; break;
    }
    return out;
}


class Type : public AST {
public:
    // initialize dt to avoid garbage
    Type() : dt(TYPE_INVALID) {}     

    Type (SymbolType d) : dt(d), dim() {}

    void add_dim(int n) {
        dim.push_back(n);
    }

    std::vector<int> getDims() const { return dim; }

    void printOn(std::ostream &out) const override {
        out<< "Type: " << dt;
        if(!dim.empty()) {
            out << " with dimensions: ";
            for (int x : dim)
                out << x << ", ";
        }
        out<<")";
    }

    virtual void sem() override {

        for (int d : dim) {
            if (d <= 0) {
                yyerror("Semantic Error: Array dimension must be a positive integer (greater than 0).");
                exit(1);
            }
        }
    }   

    // make virtual so derived ArrayType resolves correctly
    virtual SymbolType getsymbol() {
        return dt;
    }

    virtual llvm::Value* codegen() override;

protected:
    SymbolType dt;
    std::vector<int> dim;
};

class ArrayType : public Type {
public:
    // use base Type to store the base type and base.dim for dimensions
    ArrayType(SymbolType dt,int d=0) : Type(dt) {
        dim.push_back(d);
    }

    void printOn(std::ostream &out) const override {
        out<< "ArrayType: "<< dt;
        for  (int d:dim){
            out<<"["<<d<<"]";
        }
    }

    void add_dim(int n) {
        dim.push_back(n);
    }

    virtual void sem() override {
    if (dt == TYPE_REF_INT || dt == TYPE_REF_BYTE)
        yyerror("Invalid base type for array (cannot be ref type)");

    for (int d : dim) {
        if (d <= 0) {
            yyerror("Semantic Error: Array dimension must be a positive integer.");
            exit(1);
        }
    }

    }

    // override to be safe (returns base dt)
    SymbolType getsymbol() override {
        return dt;
    }

    virtual llvm::Value* codegen() override;

private:
    /* inherits them from Type */
};

class Expr : public AST {
public:
    virtual void printOn(std::ostream &out) const  =0;

    virtual void type_check(SymbolType symbolt){
        sem();
        if(symboltype!=symbolt) {

            // --- Επιτρέπουμε μίξη int <-> byte ---
            if ((symbolt == TYPE_INT && symboltype == TYPE_BYTE) ||
                (symbolt == TYPE_BYTE && symboltype == TYPE_INT)) return;

            // Επιτρέπουμε ref types
            if ((symbolt == TYPE_REF_INT && symboltype == TYPE_INT) || 
                (symbolt == TYPE_INT && symboltype == TYPE_REF_INT)) return;

            if((symbolt == TYPE_REF_BYTE && symboltype == TYPE_BYTE) \
              || (symbolt == TYPE_BYTE && symboltype ==TYPE_REF_BYTE)) return;
            
            // bool -> byte (one way)
            if (symbolt == TYPE_BYTE && symboltype == TYPE_BOOL) return;

            if(symboltype == TYPE_FUNC) return;

            auto sym_to_cstr = [](SymbolType t) -> const char* {
                switch (t) {
                    case TYPE_INT: return "int";
                    case TYPE_BYTE: return "byte";
                    case TYPE_REF_INT: return "ref int";
                    case TYPE_REF_BYTE: return "ref byte";
                    case TYPE_FUNC: return "function";
                    case LOOP : return "loop";
                    case TYPE_PARAM : return "param";
                    case TYPE_INVALID : return "invalid";
                    case TYPE_BOOL: return "bool";
                    default: return "unknown";
                }
            };
            std::string msg = std::string("Type mismatch: expected ") + sym_to_cstr(symbolt)
                            + ", got " + sym_to_cstr(symboltype);
            this->printOn(std::cerr);
            
            yyerror(msg.c_str());
            exit(1);
        }
    }

    virtual SymbolType getsymbol() {return symboltype;}

protected:
    SymbolType symboltype;

};

class Id : public Expr {
public:
    Id(std::string *s) : id(*s) {}

    void printOn(std::ostream &out) const override {
        out << "ID : "<<id;
    }

    std::string getName(){
        return id;
    }

    virtual void sem() {
    SymbolEntry *e = st.lookup(id);
    if(e!=nullptr) {
        
        symboltype = e->return_value;
    } else {
        yyerror(("Identifier " + id + " not declared (Line " + std::to_string(yylineno) + ")").c_str()); 
        symboltype = TYPE_INVALID;
    }
}

    virtual llvm::Value* codegen() override;

private:
    std::string id;
};



class Id_List : public AST {
public:
    Id_List(Id* s) {
        id_list.push_back(s);
    }

    void printOn(std::ostream &out) const override {
        out<< "Id_List (";
        bool first = true;
        for (Id* s : id_list){
            if(!first) out<<",";
            first=false;
            s->printOn(out);
        }

        out<<")";
    }

    void append(Id* s) {
        id_list.push_back(s);
    }

    std::vector<Id *> getIds() {
        return id_list;
    }

    virtual llvm::Value* codegen() override;

private:
    std::vector<Id *> id_list;
};

class Opt_Fpar : public AST {
public:
    Opt_Fpar(Id_List *idlst, ArrayType *atype, Opt_Fpar *opt = nullptr) : 
    id_list(*idlst), array_type(atype) {
        if(opt!=nullptr) next = new Opt_Fpar(*opt);
        else next = nullptr;
    }

    Opt_Fpar(Id_List *idlst, Type *dt, Opt_Fpar *opt = nullptr) : 
    id_list(*idlst), data_type(dt->getsymbol()) {
        // Check if dt is actually an ArrayType and set array_type accordingly
        array_type = dynamic_cast<ArrayType*>(dt);

        if(opt!=nullptr) next = new Opt_Fpar(*opt);
        else next = nullptr;
    }

    Opt_Fpar(Id_List *idlst, SymbolType dt, Opt_Fpar *opt = nullptr) : 
    id_list(*idlst), data_type(dt) {
        array_type =nullptr;

        if(opt!=nullptr) next = new Opt_Fpar(*opt);
        else next = nullptr;
    }

    virtual void printOn(std::ostream &out) const override {
        out << "Opt_Fpar (" << id_list << " as ";
        if (array_type)
            out << *array_type;
        else
            out << data_type;

        if (next != nullptr) {
            out << ", ";
            next->printOn(out);
        }
        out << ")\n";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        std::vector<Id *> temp = id_list.getIds();
        for(Id *idd : temp) {
            if (array_type != nullptr){
                st.insert(idd->getName(), TYPE_PARAM, array_type->getsymbol(), {}, true);
            }
            else {
                st.insert(idd->getName(), TYPE_PARAM, data_type, {}, false);
            }
        }
        if(next!=nullptr) next->sem();
    }

    // Extract parameter IDs and their types for codegen
    std::vector<std::pair<std::string, SymbolType>> getParams() const {
        std::vector<std::pair<std::string, SymbolType>> result;
        std::vector<Id *> ids = const_cast<Id_List&>(id_list).getIds();
        for (Id *id : ids) {
            SymbolType type = (array_type != nullptr) ? array_type->getsymbol() : data_type;
            // Convert array types to reference types for function parameters
            if (array_type != nullptr) {
                if (type == TYPE_BYTE) type = TYPE_REF_BYTE;
                else if (type == TYPE_INT) type = TYPE_REF_INT;
            }
            result.push_back({id->getName(), type});
        }
        if (next != nullptr) {
            auto nextParams = next->getParams();
            result.insert(result.end(), nextParams.begin(), nextParams.end());
        }
        return result;
    }

    // Extract array parameters and their dimensions
    std::vector<std::pair<std::string, std::vector<int>>> getArrayParams() const {
        std::vector<std::pair<std::string, std::vector<int>>> result;
        std::vector<Id *> ids = const_cast<Id_List&>(id_list).getIds();
        for (Id *id : ids) {
            if (array_type) result.push_back({id->getName(), array_type->getDims()});
        }
        if (next != nullptr) {
            auto nextRes = next->getArrayParams();
            result.insert(result.end(), nextRes.begin(), nextRes.end());
        }
        return result;
    }

private:
    Id_List id_list; 
    SymbolType data_type;
    ArrayType *array_type;
    Opt_Fpar *next;
};



class Header : public AST {
public:
    Header (Id *id, Type *t = nullptr, Opt_Fpar *oftpar = nullptr) 
        : Id_val(id) { 
            if(oftpar) oft = new Opt_Fpar(*oftpar);
            if(t) tp = new Type(*t);
        }

    void printOn(std::ostream &out) const override {
        out << "Header ("<< Id_val->getName();
        if (tp) out << *tp;
        if(oft) out << *oft;
        out << ")";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {

        std::vector<SymbolType> paramTypes;
        if (oft != nullptr) {
            auto params = oft->getParams();
            for (auto &p : params) {
                paramTypes.push_back(p.second);
            }
        }

        if (tp != nullptr) {
            tp->sem();
            st.insert(Id_val->getName(), TYPE_FUNC, tp->getsymbol(), paramTypes);
        }
        else {
            st.insert(Id_val->getName(), TYPE_FUNC, TYPE_FUNC, paramTypes);
        }

        if (oft != nullptr) oft->sem();
    }

    std::string getName(){
        return Id_val->getName();
    }

    Type *gettype() const {
        return tp;
    }

    Opt_Fpar *getOpt_Fpar() const {
        return oft;
    }

private:
    Id *Id_val;
    Type *tp;
    Opt_Fpar *oft;
};


// =============== Stmts ===================
class Stmt : public AST{
public:
    Stmt() {}

    virtual ~Stmt() = default;

    virtual void printOn(std::ostream &out) const override {
        out << "Stmt Statement";
    }

    virtual llvm::Value* codegen() override;
};

class StmtList : public Stmt {
    public:
        StmtList(Stmt *stmt) {
            stmt_list.push_back(stmt);  
        }

        virtual llvm::Value* codegen() override;
        
        void printOn(std::ostream &out) const override {
            out << "Stmt List: (";
            bool first = true;
            for(Stmt* s : stmt_list){
                if(!first) out << ", ";
                first = false;
                out << *s ;
                
            }
            out << ")";
        }
    
        void append(Stmt *s) {
            stmt_list.push_back(s);
        }
    
        ~StmtList() {
            for (Stmt* s : stmt_list) {
                delete s;
            }
        }
        
        virtual void sem() override {
            for (Stmt *st : stmt_list)
                st->sem();
        }
    private:
        std::vector<Stmt*> stmt_list;
    };
    
class SkipStmt : public Stmt {
public:
    SkipStmt(){}

    void printOn(std::ostream &out) const override {
        out << "Skip Statement" ;
    }

    virtual llvm::Value* codegen() override;

};

class ExitStmt : public Stmt {
public:
    ExitStmt() {}

    void printOn(std::ostream &out) const override {
        out << "Exit Statement";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        // Διαβάζουμε το (hidden) σημάδι της τρέχουσας συνάρτησης
        SymbolEntry *current_function = st.lookup("$$ret$$");
        if (!current_function) {
            yyerror("Exit statement not inside a function/procedure\n");
            return;
        }

        if (current_function->return_value != TYPE_FUNC && current_function->return_value != LOOP) {
            yyerror("Exit statement not allowed in functions with return type\n");
            return;
        }    
    }
};

class LoopBlock : public Stmt {
public:
    LoopBlock(std::string *id, StmtList *s) : ID(*id), loop_stmt_list(s) {};
    
    LoopBlock(StmtList *s) : loop_stmt_list(s) {};

    void printOn(std::ostream &out) const override {
        out<<"Loop BLock :";
        if(!ID.empty()) out<<"ID: "<<ID;
        loop_stmt_list->printOn(out);
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        st.openScope();
        if(ID.empty()) st.insert("",LOOP);
        else st.insert(ID, SymbolType::LOOP);
        if (loop_stmt_list) loop_stmt_list->sem();
        st.closeScope();
    }

private:
    std::string ID;
    StmtList *loop_stmt_list;
};

class BreakStmt : public Stmt {
public:
    BreakStmt() {}

    BreakStmt(std::string *s) : id(*s){}

    virtual llvm::Value* codegen() override;

    void printOn(std::ostream &out) const override {
        out << "Break Statement";
        if(id!="") out<<" with "<<id;
    }

    virtual void sem() override {
        if (!id.empty()) {
            SymbolEntry *e = st.lookup(id);
            if(e==nullptr || e->type != LOOP) yyerror("Invalid loop ip in break stmt");
        }
        else {
            if (!st.insideLoop())
                yyerror("Break not inside a loop");
        }
    }
private:
    std::string id;
};

class ContStmt : public Stmt {
public:
    ContStmt() {}

    ContStmt(std::string *s) : id(*s){}

    virtual llvm::Value* codegen() override;

    void printOn(std::ostream &out) const override {
        out << "Continue Statement";
        if(id!="") out<<" with "<<id;
    }

    virtual void sem() override {
        if(!id.empty()) {
        SymbolEntry *e = st.lookup(id);
        if(e==nullptr || e->type != LOOP) yyerror("Invalid loop ip in continue stmt");
        }
        else {
            if (!st.insideLoop()) yyerror("Continue not inside a loop");
        }
    }

private:
    std::string id;
    
};

// ==========================================


// Forward declaration of LocalDefList (used in FuncDefBlock)
class LocalDefList;

// At the FuncDefBlock, "inside" the LocalDefList, we define all the 
// variables, functions etc that we're gonna need. The LocalDefList
// corresponds to the decl_list from the lectures

// -------------------- FuncDefBlock --------------------
class FuncDefBlock : public AST {
public:
    FuncDefBlock(Header *h, LocalDefList *lst, StmtList *stlst)
        : header(h), localDefList(lst), stmt_list(stlst) {}

    // Declaration only - definition comes later, after LocalDefList is fully defined.
    void printOn(std::ostream &out) const override;

    virtual llvm::Value* codegen() override;

    // Declaration only - definition comes later, same as above
    virtual void sem() override ;

private:
    Header *header;
    LocalDefList *localDefList;
    StmtList *stmt_list;
};

// -------------------- FuncDecl --------------------
class FuncDecl : public AST {
public:
    FuncDecl(Header *h) : hd(h) {}

    void printOn(std::ostream &out) const override {
        out << "FuncDecl: " << *hd;
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        std::vector<SymbolType> paramTypes;
        if (hd && hd->getOpt_Fpar() != nullptr) {
            auto params = hd->getOpt_Fpar()->getParams();
            for (auto &p : params) {
                paramTypes.push_back(p.second);
            }
        }
        SymbolType ret = (hd && hd->gettype()) ? hd->gettype()->getsymbol() : TYPE_FUNC;
        st.insert(hd->getName(), TYPE_FUNC, ret, paramTypes);
    }

private:
    Header *hd;
};


// -------------------- LocalDef --------------------
class LocalDef : public AST {
public:
    LocalDef(FuncDefBlock *fdef) : func_decl(nullptr), func_def(fdef), idlst(nullptr), tp(nullptr) {}

    LocalDef(FuncDecl *fdecl) : func_decl(fdecl), func_def(nullptr), idlst(nullptr), tp(nullptr) {}

    LocalDef(Id_List *il, Type *t) : func_decl(nullptr), func_def(nullptr), idlst(il), tp(t) {}

    void printOn(std::ostream &out) const override {
        out << "Local Def: ";
        if (func_decl) {
            out << *func_decl;
        }
        else if (func_def) {
            out << *func_def;
        }
        else if (idlst) {
            out << "var " << *idlst << " is " << *tp;
        }
    }
    
    virtual llvm::Value* codegen() override;

    virtual void sem() override {

        if(func_def) func_def->sem();
        else if (func_decl) func_decl->sem();
        else if (idlst) {

            if (tp) tp->sem();

            std::vector<Id*> temp = idlst->getIds();
            for (Id* id : temp){
                SymbolType t = tp->getsymbol();
                bool isArr = (tp->getDims().size() > 0);
                st.insert(id->getName(), t, t, {}, isArr);
            }
        }
    }
    
private:
    FuncDecl *func_decl;
    FuncDefBlock *func_def;
    Id_List *idlst;
    Type *tp;
};

// -------------------- LocalDefList --------------------
class LocalDefList : public AST {
public:
    LocalDefList() {}

    LocalDefList (LocalDef *ld) {
        local_def_list.push_back(ld);
    } 

    void printOn(std::ostream &out) const override {
        
        if(!local_def_list.empty()) {
        out << "LocalDefList:";
        for (const auto& def : local_def_list) {
            out << " - " << *def ;
        }}
    }

    void append(LocalDef *s) {
        local_def_list.push_back(s);
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        for (LocalDef* ld : local_def_list)
        ld->sem();
    }

private:
    std::vector<LocalDef*> local_def_list;
};

// -------------------- FuncDefBlock::printOn --------------------
// At this point, LocalDefList is fully defined.
inline void FuncDefBlock::printOn(std::ostream &out) const {
    out << *header << ", Local Definitions: ";
    localDefList->printOn(out);
}

inline void FuncDefBlock::sem() {

    // Added some checks for the main function 
    bool isMain = !st.hasScope();
    
    if (isMain) {
        if (header->gettype() != nullptr) {
            yyerror("Semantic Error: The main program cannot return a result.");
            exit(1);
        }
        if (header->getOpt_Fpar() != nullptr) {
            yyerror("Semantic Error: The main program cannot accept parameters.");
            exit(1);
        }
    }

    if (!st.hasScope()) { 
        st.openScope(); 
    }

    std::vector<SymbolType> paramTypes;
    if (header && header->getOpt_Fpar() != nullptr) {
        auto params = header->getOpt_Fpar()->getParams();
        for (auto &p : params) {
            paramTypes.push_back(p.second);
        }
    }

    if (header) {
        SymbolType ret = header->gettype() ? header->gettype()->getsymbol() : TYPE_FUNC;
        st.insert(header->getName(), TYPE_FUNC, ret, paramTypes);
    }

    st.openScope(); // Άνοιγμα scope για το σώμα της συνάρτησης
    
    // Κρυφό σημάδι για να βρίσκουμε ΠΑΝΤΑ το σωστό return type της συνάρτησης!
    SymbolType currentRet = header->gettype() ? header->gettype()->getsymbol() : TYPE_FUNC;
    st.insert("$$ret$$", TYPE_FUNC, currentRet);

    if (header && header->getOpt_Fpar() != nullptr) {
        header->getOpt_Fpar()->sem();
    }
    
    if (localDefList) localDefList->sem();
    if (stmt_list) stmt_list->sem();
    st.closeScope();
}

//=========== Expressions & Stmts ============
//=== They are mixed cause of dependencies ===


class ExprList : public AST {
public:
    ExprList(Expr *e) {
        expr_list.push_back(e);
    }

    ExprList() {}

    void printOn(std::ostream &out) const override {
        out << "ExprList: ( ";
        bool first = true;
        for(Expr* e : expr_list){
            if(!first) out << ", ";
            first = false;
            e->printOn(out); 
            
        }
        out << ")";
    }

    void append(Expr *e){
        expr_list.push_back(e);
    }

    virtual void sem() override{
        for (Expr *e : expr_list){
            if(e) e->sem();
        }
    }

    const std::vector<Expr*>& getExprs() const { 
        return expr_list; 
    }

    virtual llvm::Value* codegen() override;

private:
    std::vector<Expr*> expr_list;
};

class Lvalue : public Expr {
public:
    Lvalue(Id *i) : id(i), str(), expr_list(nullptr) {}
    Lvalue(std::string *s) : id(nullptr), str(*s), expr_list(nullptr) {}

    void printOn(std::ostream &out) const override {
        out << "Lvalue: (";
        if (id) out << *id;
        else if (!str.empty()) out << "\"" << str << "\"";
        out << " ";
        if (expr_list != nullptr) expr_list->printOn(out);
        out << ")\n";
    }

    void append(Expr *e) {
        if (!expr_list) expr_list = new ExprList();
        expr_list->append(e);
    }

    virtual llvm::Value* codegen() override;
    
    virtual void sem() override {
        SymbolEntry *e = nullptr;

        if (id) {
            id->sem();
            e = st.lookup(id->getName());
            symboltype = id->getsymbol();
        } else if (!str.empty()) {
            symboltype = TYPE_REF_BYTE;
        } else {
            symboltype = TYPE_INVALID;
        }

        if (expr_list) {
            // 1. Έλεγχος αν η μεταβλητή είναι όντως πίνακας
            if (e != nullptr && !e->is_array && str.empty()) {
                yyerror(("Semantic Error: '" + id->getName() + "' is not an array.").c_str());
                exit(1);
            }

            expr_list->sem();

            // 2. Έλεγχος των array indexes
            for (Expr* indexExpr : expr_list->getExprs()) {
                SymbolType it = indexExpr->getsymbol();
                if (it != TYPE_INT && it != TYPE_BYTE && it != TYPE_REF_INT && it != TYPE_REF_BYTE) {
                    yyerror("Semantic Error: Array index must be an integer or byte.");
                    exit(1);
                }
            }

            // 3. Αποτίμηση του τελικού τύπου
            if (symboltype == TYPE_REF_INT) symboltype = TYPE_INT;
            else if (symboltype == TYPE_REF_BYTE) symboltype = TYPE_BYTE;
        }
    }

    std::string getIdName() { return id ? id->getName() : ""; }

    llvm::Value* getPointer();

    bool hasExprList() const { return expr_list != nullptr; }
    bool isStringLiteral() const { return !str.empty(); }

private:
    Id *id;
    std::string str;
    ExprList* expr_list;
};

class Number : public Expr {
public:
    Number (int n) : numb(n) {}

    void printOn(std::ostream &out) const override {
        out << "Number : "<<numb;
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        symboltype=TYPE_INT;
    }

private:
    int numb;
};

class CharConst: public Expr {
public:
    CharConst(char c) : character(c) { symboltype = TYPE_BYTE; }

    void printOn(std::ostream &out) const override {
        out << "CharConst ('" << character << "')";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {}

private:
    char character;

};

class UniOp : public Expr {
public:
    UniOp(char oper, Expr *e_input) : op(oper), e(e_input) {}

    void printOn(std::ostream &out) const override {
        out << "UniOp Expr: ( "<< op << " "<<*e;
    }

    virtual llvm::Value* codegen() override;

    //I assume that UniOp works only with integers
    virtual void sem() override {
        if(e) e->type_check(TYPE_INT);
    }

private:
    char op;
    Expr *e;
};

class BinOp : public Expr {
public:
    BinOp(Expr *e_input1, char oper, Expr *e_input2) : 
    left(e_input1), op(oper), right(e_input2) {}

    void printOn(std::ostream &out) const override {
        out << "BinOp Expr: ( " <<*left<<" "<< op << " "<<*right<< " )";
    }

    virtual llvm::Value* codegen() override;


    virtual void sem() override {
        if (left) left->sem();
        if (right) right->sem();

        SymbolType l = left ? left->getsymbol() : TYPE_INVALID;
        SymbolType r = right ? right->getsymbol() : TYPE_INVALID;

        // ref t ==> t
        SymbolType baseL = (l == TYPE_REF_INT) ? TYPE_INT : ((l == TYPE_REF_BYTE) ? TYPE_BYTE : l);
        SymbolType baseR = (r == TYPE_REF_INT) ? TYPE_INT : ((r == TYPE_REF_BYTE) ? TYPE_BYTE : r);

        if (baseL == TYPE_INT && baseR == TYPE_INT) {
            symboltype = TYPE_INT;
        } else if (baseL == TYPE_BYTE && baseR == TYPE_BYTE) {
            symboltype = TYPE_BYTE;
        } else {
            yyerror("Semantic Error: Arithmetic operands must be of the exact same numeric type (both int or both byte).");
            exit(1);
        }
    }

private:
    Expr *left;
    char op;
    Expr *right;
};

class Bool : public Expr {
public:
    Bool(bool b) :  flag(b) {}

    void printOn(std::ostream &out) const override {
        out << "Bool Expr: ( " << (flag ? "true" : "false" );
        out<<" )";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        // True or False have byte type
        symboltype=TYPE_BYTE;
    }
private:
    bool flag;
};

class BoolBinOp : public Expr {
public:
    BoolBinOp(Expr *e1_i, char o, Expr *e2_i) : left(e1_i), op(o), right(e2_i) {}

    void printOn(std::ostream &out) const override {
        out << "BoolBinOp Expr: ( " <<*left<<" "<< op << " "<<*right<< " )";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if (left) left->sem();
        if (right) right->sem();

        if (op != '&' && op != '|') {
            yyerror(("Unknown boolean operator: " + std::string(1, op)).c_str());
            symboltype = TYPE_INVALID;
            return;
        }

        // operands must be of byte type
        if (left) left->type_check(TYPE_BYTE);
        if (right) right->type_check(TYPE_BYTE);

        // result is an r-value of byte type
        symboltype = TYPE_BYTE;
    }

private:
    Expr *left;
    char op;
    Expr *right;
};

class Func_Call : public Expr {
public:
    Func_Call(Id *i, ExprList *lst = nullptr) : id(i) {
        if(lst) list = lst;
    }

    void printOn(std::ostream &out) const override {
        out << "Func Call: ("<< *id;
        if(list) out<<" with "<<*list;
        out<<")\n";

    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if (!id) {
            yyerror("Function call with null id");
            symboltype = TYPE_INVALID; 
            return; 
        }   
        SymbolEntry *e = st.lookup(id->getName());
        if (e == nullptr) {
            yyerror(("Function/Procedure " + id->getName() + " not declared").c_str());
            exit(1); 
        }
        if (e->type != TYPE_FUNC){
            yyerror((id->getName() + " is not a function").c_str());
            exit(1);
        }
        if (list) list->sem();

        std::vector<Expr*> argExprs;
        if (list) argExprs = list->getExprs();

        if (argExprs.size() != e->param_types.size()) {
            yyerror(("Function '" + id->getName() + "' expects " + 
                     std::to_string(e->param_types.size()) + " arguments, but " + 
                     std::to_string(argExprs.size()) + " were given.").c_str());
            exit(1);
        }

        for (size_t i = 0; i < argExprs.size(); ++i) {

            SymbolType expectedT = e->param_types[i];
            Expr* actualE = argExprs[i];

            if (expectedT == TYPE_REF_INT || expectedT == TYPE_REF_BYTE) {
                if (dynamic_cast<Lvalue*>(actualE) == nullptr) {
                    yyerror(("Semantic Error: Parameter " + std::to_string(i+1) + 
                             " of function " + id->getName() + 
                             " is passed by reference and requires an l-value.").c_str());
                    exit(1);
                }
            }
            actualE->type_check(expectedT);
            argExprs[i]->type_check(e->param_types[i]);
        }

        symboltype = e->return_value;
    }

private:
    Id *id;
    ExprList *list;
};
    
class Proc_Call : public Expr {
public:
    Proc_Call(Id *i, ExprList *lst = nullptr) : id(i) {
        if(lst) list = lst;
    }

    void printOn(std::ostream &out) const override {
        out << "Proc Call: ( "<< *id;
        if(list) out<<" with : "<<*list;
        out<<" )";
        
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if (!id) {
            yyerror("Procedure call with null id"); 
            return; 
        }
        SymbolEntry *e = st.lookup(id->getName());
        if (e == nullptr) {
            yyerror(("Function/Procedure " + id->getName() + " not declared").c_str());
            exit(1);
        }
        if (e->type != TYPE_FUNC)
            yyerror((id->getName() + " is not a function").c_str());

        if (list) list->sem();

        std::vector<Expr*> argExprs;
        if (list) argExprs = list->getExprs();

        if (argExprs.size() != e->param_types.size()) {
            yyerror(("Function '" + id->getName() + "' expects " + 
                     std::to_string(e->param_types.size()) + " arguments, but " + 
                     std::to_string(argExprs.size()) + " were given.").c_str());
            exit(1);
        }

        for (size_t i = 0; i < argExprs.size(); ++i) {

            SymbolType expectedT = e->param_types[i];
            Expr* actualE = argExprs[i];
                    
            if (expectedT == TYPE_REF_INT || expectedT == TYPE_REF_BYTE) {
                if (dynamic_cast<Lvalue*>(actualE) == nullptr) {
                    yyerror(("Semantic Error: Parameter " + std::to_string(i+1) + 
                             " of function " + id->getName() + 
                             " is passed by reference and requires an l-value.").c_str());
                    exit(1);
                }
            }
            actualE->type_check(expectedT);
        }

        symboltype = e->return_value;
    }

private:
    Id *id;
    ExprList *list;
};

//---------------------------------------------------
        
class ReturnStmt : public Stmt {
public:
    ReturnStmt(Expr *rhs) : e(rhs) {}

    void printOn(std::ostream &out) const override {
        out << "Return Statement : " << *e;
    }

    virtual llvm::Value* codegen() override;

    //e must have the same type that the block/function returns
    virtual void sem() override {
        SymbolEntry *function_entry = st.lookup("$$ret$$"); 
        if(function_entry != nullptr) {
            if (e) {
                if (function_entry->return_value == TYPE_FUNC) {
                    yyerror("Semantic Error: Cannot return a value from a procedure (void function).");
                    exit(1);
                }
                e->type_check(function_entry->return_value);
            } else if (function_entry->return_value != TYPE_FUNC) {
                yyerror("Semantic Error: Return value missing in function that expects a result.");
                exit(1);
            }
        }
    }
private:
    Expr *e;
};      

class FuncStmt : public Stmt {
public:
    FuncStmt(Func_Call *f) : fc(f) {}

    void printOn(std::ostream &out) const override {
        out << "FuncStmt :" << *fc;
        out<<")";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if(fc) fc->sem();
    }
private:
    Func_Call *fc;
};
                
class ProcStmt : public Stmt {
public:
    ProcStmt(Proc_Call *p) : pc(p) {}

    void printOn(std::ostream &out) const override {
        out << "ProcStmt :" << *pc;
        out<<")";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if(pc) pc->sem();
    }
private:
    Proc_Call *pc;
};
    
class LvalueStmt : public Stmt {
public:
    LvalueStmt(Lvalue *lv, Expr *rhs) : l(lv), e(rhs) {}

    void printOn(std::ostream &out) const override {
        out << "LvalueStmt List: (";
        out<<*l<< " := "<<*e;
        out<<")";
    }
    
    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if(l) l->sem();
        if(e) e->sem();

        if (l->isStringLiteral()) {
            yyerror("Semantic Error: Cannot assign to a string literal constant.");
            exit(1);
        }
        
        SymbolEntry *leftEntry = st.lookup(l->getIdName());
        
        if (leftEntry && leftEntry->is_array && !l->hasExprList()) {
            yyerror("Semantic Error: Cannot assign directly to an array. Use strcpy for strings.");
            exit(1);
        }

        if (Lvalue *rval = dynamic_cast<Lvalue*>(e)) {
            SymbolEntry *rightEntry = st.lookup(rval->getIdName());
            if (rightEntry && rightEntry->is_array && !rval->hasExprList()) {
                yyerror("Semantic Error: Cannot assign from an array.");
                exit(1);
            }
            if (rval->isStringLiteral()) {
                yyerror("Semantic Error: Cannot assign from a string literal. Use strcpy.");
                exit(1);
            }
        }

        SymbolType lt = l->getsymbol();
        e->type_check(lt);
    }

    private:
    Lvalue *l;
    Expr *e;
};
                    
//===========================================

class NotExpr : public Expr {
public:
    NotExpr(Expr *e) : expr(e) {}

    void printOn(std::ostream &out) const override {
        out << "Not Expr: ( "<<*expr<<" )";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if(expr) expr->type_check(TYPE_BYTE);
        symboltype = TYPE_BYTE;
    }

private:
    Expr *expr;
};

// =============== Conditions ==============

class Cond : public Expr {
public:
        
    Cond(Expr* lhs, const std::string& op, Expr* rhs)
            : single(nullptr), left(lhs), oper(op), right(rhs) {}

    Cond(Expr* e) : single(e), left(nullptr), oper(""), right(nullptr) {}
    Cond() : single(nullptr), left(nullptr), oper(""), right(nullptr) {}

    virtual llvm::Value* codegen() override;

    void printOn(std::ostream& out) const override {
            out<<"Cond (";
            if(single) out<<*single<<" )";
            else {out << "(";
            left->printOn(out);
            out << " " << oper << " ";
            right->printOn(out);
            out << ")";}
        }


    virtual void sem() override {
        if (single != nullptr) {
            single->sem(); 
            if (single->getsymbol() != TYPE_BOOL) {
                yyerror("Semantic Error: Condition must be of boolean type");
                exit(1);
            }
            symboltype = TYPE_BOOL;
            return;
        }

        if (left) left->sem();
        if (right) right->sem();

        SymbolType lt = left ? left->getsymbol() : TYPE_INVALID;
        SymbolType rt = right ? right->getsymbol() : TYPE_INVALID;

        // Συγκριτικοί τελεστές μεταξύ αριθμών: <, >, <=, >=, =, <>
        if (oper == "<" || oper == ">" || oper == "<=" || oper == ">=" || oper == "=" || oper == "<>") {
            symboltype = TYPE_BOOL;
            return;
        }

        // Λογικοί τελεστές and/or
        if (oper == "and" || oper == "or") {
            if (lt != TYPE_BOOL || rt != TYPE_BOOL) {
                yyerror("Semantic Error: Operands of 'and' / 'or' must be conditions.");
                exit(1);
            }
            symboltype = TYPE_BOOL;
            return;
        }

        yyerror(("Unknown conditional operator: " + oper).c_str());
        symboltype = TYPE_INVALID;
    }

private:
    Expr *single;
    Expr* left;
    std::string oper;
    Expr* right;
};
    
class NotCond : public Cond {
public:
    NotCond (Cond *c) {
        con = new Cond(c);
    }

    void printOn(std::ostream& out) const override {
        out << " Not (";
        out << *con;
        out << ")";
    }

    virtual llvm::Value* codegen() override;

    virtual void sem() override {
        if(con) {
            con->sem();
            if (con->getsymbol() != TYPE_BOOL) {
                yyerror("Semantic Error: Operand of 'not' must be a condition.");
                exit(1);
            }
        }
        symboltype = TYPE_BOOL; 
    }

private:
    Cond *con;
};

class Opt_Elif_Else : public Stmt {
public:

    Opt_Elif_Else(Cond *c,StmtList *stlist, Opt_Elif_Else *opteliflese) :
    condition(c), stmts(stlist), elif_else(opteliflese) {};

    Opt_Elif_Else(StmtList *stlist) :
    condition (nullptr), stmts(stlist), elif_else(nullptr) {};

    virtual llvm::Value* codegen() override;

    void printOn(std::ostream &out) const override {
        if (condition != nullptr && elif_else!=nullptr){
            out<<"Elif ";
            condition->printOn(out);
            out<<"then ";
            stmts->printOn(out);
            elif_else->printOn(out);

        }
        else if (condition==nullptr && elif_else==nullptr){
            out<<"Else then ";
            stmts->printOn(out);
        }
        else {
            std::cout<<"[ERROR] condition and elif_else are not the same! What?! 0_0\n";
        }
    }

    virtual void sem() override {
        if(condition) condition->type_check(TYPE_BOOL);
        st.openScope();
        if(stmts) stmts->sem();
        if(elif_else!=nullptr) elif_else->sem();
        st.closeScope();
    }

private:
    Cond *condition;
    StmtList *stmts;
    Opt_Elif_Else *elif_else;

};

class IfBlock : public Stmt {
public:
    
    IfBlock(Cond *c, StmtList *stlist, Opt_Elif_Else *oee) : 
    condition(c), stmts (stlist), elif_else(oee) {};
    
    virtual llvm::Value* codegen() override;

    void printOn(std::ostream& out) const override {
            out << "If condition: ";
            condition->printOn(out);
            out << "\nThen block:\n";
            stmts->printOn(out);

            elif_else->printOn(out);
        }
    

    virtual void sem() override {
        if(condition) condition -> type_check(TYPE_BOOL);
        st.openScope();
        if(stmts) stmts->sem();
        if(elif_else!=nullptr) elif_else->sem();
        st.closeScope();
    }

private:
    Cond* condition;
    StmtList *stmts;
    Opt_Elif_Else *elif_else;
};
        

#endif