#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include <map>

enum DataType { TYPE_int, TYPE_byte, REF_int, REF_byte };

class AST {
public:
    virtual void printOn(std::ostream &out) const = 0;
};

inline std::ostream &operator<<(std::ostream &out, const AST &ast) {
    ast.printOn(out);
    return out;
}

inline std::ostream &operator<<(std::ostream &out, DataType t) {
    switch (t) {
      case TYPE_int: out << "int"; break;
      case TYPE_byte: out << "byte"; break;
      case REF_byte: out<<"ref byte"; break;
      case REF_int: out<<"ref int"; break;
    }
    return out;
  }

class Type : public AST {
public:
    Type (DataType d) : dt(d), dim() {}

    void add_dim(int n) {
        dim.push_back(n);
    }

    void printOn(std::ostream &out) const override {
        out<< "Type: " << dt;
        if(!dim.empty()) {
            out << " with dimensions: ";
            for (int x : dim)
                out << x << ", ";
        }
        out<<")";
    }
private:
    DataType dt;
    //Only for pretty-printing
    std::vector<int> dim;
};

class FparType : public AST {
public:
    FparType(DataType t) : tp(t) {}

    void printOn(std::ostream &out) const override  {
        out<< "FparType ( "<<tp<<" )"; 
    }

    virtual void add_dim(int n) {};
protected:
    DataType tp;
};

class ArrayType : public FparType {
public:
    ArrayType(DataType dt,int d) : FparType(dt) {
        dim.push_back(d);
    }

    void printOn(std::ostream &out) const override {
        out<< "ArrayType: "<<tp;
        for  (int d:dim){
            out<<"["<<d<<"]";
        }
    }

    void add_dim(int n) override {
        dim.push_back(n);
    }
    
private:
    std::vector<int> dim;
};

class Id_List : public AST {
public:
    Id_List(std::string s) {
        lst.push_back(s);
    }

    void printOn(std::ostream &out) const override {
        out<< "Id_List (";
        bool first = true;
        for (std::string s : lst){
            if(!first) out<<",";
            first=false;
            out<<s;
        }

        out<<")";
    }

    void append(std::string s) {
        lst.push_back(s);
    }
private:
    std::vector<std::string> lst;
};

class Opt_Fpar : public AST {
public:
    Opt_Fpar(Id_List *idlst, FparType *fpr, Opt_Fpar *opt = nullptr) : id_list(*idlst), fpar(fpr) {
        if(opt!=nullptr) next = new Opt_Fpar(*opt);
        else next = nullptr;
    }

    Opt_Fpar(Id_List *idlst, DataType ref, Opt_Fpar *opt = nullptr) : id_list(*idlst), ref_type(ref) {
        fpar =nullptr;
        if(ref == REF_byte || ref == REF_int) yyerror("Invalid ref type int opt_fpar!\n");
        if(opt!=nullptr) next = new Opt_Fpar(*opt);
        else next = nullptr;
    }

    virtual void printOn(std::ostream &out) const override {
        out << "Opt_Fpar (" << id_list << " as ";
        if (dynamic_cast<const ArrayType*>(fpar)) {  //Adding runtime overhead... ISOS OXI POLY KALO
            out << *fpar; 
        } else {
            out << (fpar ? *fpar : ref_type);
        }
        if (next != nullptr) out << ", " << *next;
        out << ")";
    }

private:
    Id_List id_list;
    DataType ref_type;
    FparType *fpar;
    Opt_Fpar *next;
};

class Id : public AST {
public:
    Id(std::string *s) : id(*s) {}

    void printOn(std::ostream &out) const override {
        out << "ID : "<<id;
    }
private:
    std::string id;
};

class Header : public AST {
public:
    Header (Id *id, Type *t = nullptr, Opt_Fpar *oftpar = nullptr) 
        : Id_val(*id) { 
            if(oftpar) oft = new Opt_Fpar(*oftpar);
            if(t) tp = new Type(*t);
        }

    void printOn(std::ostream &out) const override {
        out << "Header ("<< Id_val;
        if (tp) out << *tp;
        if(oft) out << *oft;
        out << ")";
    }
private:
    Id Id_val;
    Type *tp;
    Opt_Fpar *oft;
};


// =============== Stmts ===================
class Stmt : public AST{
public:
    Stmt() {}

    virtual void printOn(std::ostream &out) const override {
        out << "Stmt Statement";
    }
    //Implementation of run()/exec()

    virtual void run() const = 0;
};

class StmtList : public Stmt {
    public:
        StmtList(Stmt *stmt) {
            lst.push_back(stmt);  // pointer instead of object
        }
        
        virtual void run() const {
            for (Stmt *s : lst)
            s->run();
        }

        void printOn(std::ostream &out) const override {
            out << "Stmt List: (";
            bool first = true;
            for(Stmt* s : lst){
                if(!first) out << ", ";
                first = false;
                out << *s ;
                
            }
            out << ")";
        }
    
        void append(Stmt *s) {
            lst.push_back(s);
        }
    
        ~StmtList() {
            for (Stmt* s : lst) {
                delete s;
            }
        }
    
    private:
        std::vector<Stmt*> lst;
    };
    
class SkipStmt : public Stmt {
public:
    SkipStmt(){}

    void printOn(std::ostream &out) const override {
        out << "Skip Statement" ;
    }
};

class ExitStmt : public Stmt {
public:
    ExitStmt() {}

    void printOn(std::ostream &out) const override {
        out << "Exit Statement";
    }
};

class LoopBlock : public Stmt {
public:
    LoopBlock(std::string *id, StmtList *s) : ID(*id), lst(s) {};
    
    LoopBlock(StmtList *s) : lst(s) {};

    void printOn(std::ostream &out) const override {
        out<<"Loop BLock :";
        if(!ID.empty()) out<<"ID: "<<ID;
        out<<*lst;
    }

    virtual void run() const {
        lst->run();
    }

private:
    std::string ID; // Check this
    StmtList *lst;
};

class BreakStmt : public Stmt {
public:
    BreakStmt() : id("") {}

    BreakStmt(std::string *s) : id(*s){}

    void printOn(std::ostream &out) const override {
        out << "Break Statement";
        if(id!="") out<<" with "<<id;
    }

private:
    std::string id;
};

class ContStmt : public Stmt {
public:
    ContStmt() : id("") {}

    ContStmt(std::string *s) : id(*s){}

    void printOn(std::ostream &out) const override {
        out << "Continue Statement";
        if(id!="") out<<" with "<<id;
    }

private:
    std::string id;
    
};

// ==========================================


// Forward declaration of LocalDefList (used in FuncDefBlock)
class LocalDefList;

// -------------------- FuncDefBlock --------------------
class FuncDefBlock : public AST {
public:
    FuncDefBlock(Header *h, LocalDefList *lst, StmtList *stlst)
        : header(h), localDefList(lst), stmt_list(stlst) {}

    // Declaration only - definition comes later, after LocalDefList is fully defined.
    void printOn(std::ostream &out) const override;

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

private:
    Header *hd;
};

// -------------------- LocalDef --------------------
class LocalDef : public AST {
public:
    LocalDef(FuncDefBlock *fdef) : func_def(fdef), func_decl(nullptr), idlst(nullptr), tp(nullptr) {}

    LocalDef(FuncDecl *fdecl) : func_decl(fdecl), func_def(nullptr), idlst(nullptr), tp(nullptr) {}

    LocalDef(Id_List *il, Type *t) : idlst(il), tp(t), func_decl(nullptr), func_def(nullptr) {}

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
        v.push_back(ld);
    }

    void printOn(std::ostream &out) const override {
        
        if(!v.empty()) {
        out << "LocalDefList:";
        for (const auto& def : v) {
            out << " - " << *def ;
        }}
    }

    void append(LocalDef *s) {
        
        v.push_back(s);
        
                         //if (v.empty())
    //  When it was      //    v.push_back(s);
    //  right-recursive  //else
                         //    v.insert(v.begin(), s);
    }

private:
    std::vector<LocalDef*> v;
};

// -------------------- Now define FuncDefBlock::printOn --------------------
// At this point, LocalDefList is fully defined.
inline void FuncDefBlock::printOn(std::ostream &out) const {
    out << *header << ", Local Definitions: ";
    localDefList->printOn(out);
}

//=========== Expressions & Stmts ============
//=== They are mixed cause of dependencies ===

class Expr : public AST {
public:
    virtual void printOn(std::ostream &out) const  {
    out << "Expr: ( )";
    }

    //Implementation of eval()
    virtual int eval() const = 0;
};

class ExprList : public AST {
public:
    ExprList(Expr *e) {
        v.push_back(e);
    }

    ExprList() {}

    void printOn(std::ostream &out) const override {
        out << "ExprList: ( ";
        bool first = true;
        for(Expr* e : v){
            if(!first) out << ", ";
            first = false;
            e->printOn(out); 
            
        }
        out << ")";
    }

    void append(Expr *e){
        v.push_back(e);
    }

    virtual int eval() const {
        for (Expr *e : v){
            e->eval();
        }
    }

private:
    std::vector<Expr*> v;
};

class Lvalue : public Expr {
public:
    Lvalue(Id *i) : id(i) {}

    Lvalue(std::string *s) : str(*s) {}

    void printOn(std::ostream &out) const override {
        out << "Lvalue: (";
        if(id) out<<*id;
        else if (!str.empty()) out<<"\""<<str<<"\"";
        out<<" ";
        for(Expr *x : v){
           out<<x<<", ";
        }

        out<<")";
    }

    void append(Expr *e) {
        v.push_back(e);
    }

private:
    Id *id;
    std::string str;
    std::vector<Expr*> v;
};

class Number : public Expr {
public:
    Number (int n) : numb(n) {}

    void printOn(std::ostream &out) const override {
        out << "Number : "<<numb;
    }

    virtual int eval() const {
        return numb;
    }

private:
    int numb;
};

class Const: public Expr {
public:
    Const (std::string *s) : str(*s) {}

    void printOn(std::ostream &out) const override {
        out << "Constant : "<<str;
        
    }



private:
    std::string str;

};

class UniOp : public Expr {
public:
    UniOp(char oper, Expr *e_input) : op(oper), e(e_input) {}

    void printOn(std::ostream &out) const override {
        out << "UniOp Expr: ( "<< op << " "<<*e;
    }

private:
    char op;
    Expr *e;
};

class BinOp : public Expr {
public:
        BinOp(Expr *e_input1, char oper, Expr *e_input2) : e1(e_input1), op(oper), e2(e_input2) {}

        void printOn(std::ostream &out) const override {
            out << "BinOp Expr: ( " <<*e1<<" "<< op << " "<<*e2<< " )";
        }

        virtual int eval() const {
            switch (op)
            {
            case '+':
            return e1->eval() + e2->eval();
            case '-':
            return e1->eval() - e2->eval();
            case '*':
            return e1->eval() * e2->eval();
            case '/':
            return e1->eval() / e2->eval();
            case '%':
            return e1->eval() % e2->eval();
            default:
            std::cerr << "Unknown operator: " << op << std::endl;
            return 0;
            }
        }

private:
        char op;
        Expr *e1;
        Expr *e2;
    };

class Bool : public Expr {
public:
    Bool(bool b) :  flag(b) {}

    void printOn(std::ostream &out) const override {
        out << "Bool Expr: ( " << (flag ? "true" : "false" );
        out<<" )";
    }

    virtual int eval() const {
        return flag;
    }
private:
    bool flag;
};

class BoolBinOp : public Expr {
public:
    BoolBinOp(Expr *e1_i, char o, Expr *e2_i) : e1(e1_i), op(o), e2(e2_i) {}

    void printOn(std::ostream &out) const override {
        out << "BoolBinOp Expr: ( " <<*e1<<" "<< op << " "<<*e2<< " )";
    }

    virtual int eval() const {
        if (op=='&') return e1->eval() && e2->eval();
        else if (op=='|') return e1->eval() || e2->eval();
    }

private:
    Expr *e1;
    Expr *e2;
    char op;
};

class Func_Call : public Expr {
    public:
        Func_Call(Id *i, ExprList *lst = nullptr) : id(i) {
            if(lst) list = lst;
        }
    
        void printOn(std::ostream &out) const override {
            out << "Func Call: ( "<< *id;
            if(list) out<<" with "<<*list;
            out<<" )";
    
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
                private:
                    Func_Call *fc;
                };
                
class ProcStmt : public Stmt {
                public:
                    ProcStmt(Proc_Call *f) : fc(f) {}
                
                    void printOn(std::ostream &out) const override {
                        out << "ProcStmt :" << *fc;
                        out<<")";
                    }
                private:
                    Proc_Call *fc;
                };
     
class LvalueStmt : public Stmt {
    public:
        LvalueStmt(Lvalue *lv, Expr *rhs) : l(lv), e(rhs) {}
    
        void printOn(std::ostream &out) const override {
            out << "LvalueStmt List: (";
            out<<*l<< " := "<<*e;
            out<<")";
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

    virtual int eval() {
        return !(expr->eval());
    }

private:
    Expr *expr;
};

// =============== Conditions ==============

class Cond : public Expr {
public:
        
    Cond(Expr* lhs, const std::string& op, Expr* rhs)
            : left(lhs), oper(op), right(rhs), single(nullptr) {}

    Cond(Expr* e) : single(e) {}
    Cond() {}

    void printOn(std::ostream& out) const override {
            out<<"Cond (";
            if(single) out<<*single<<" )";
            else {out << "(";
            left->printOn(out);
            out << " " << oper << " ";
            right->printOn(out);
            out << ")";}
        }
    
    virtual int eval() const {
        if (single != nullptr){
            single->eval();
        }
        else {
            if (oper == ">") return left->eval() > right->eval();
            else if (oper == "<") return left->eval() < right->eval();
            else if (oper == ">=") return left->eval() >= right->eval();
            else if (oper == "<=") return left->eval() <= right->eval();
            else if (oper == "=") return left->eval() == right->eval();
            else if (oper == "<>") return left->eval() != right->eval();
            else if (oper == "and") return left->eval() && right->eval();
            else if (oper == "or") return left->eval() || right->eval();
            else {
                std::cerr << "Unknown operator: " << oper << std::endl;
                return 0;
            }
        }
    }
private:
    Expr *single;
    Expr* left;
    Expr* right;
    std::string oper;
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

    virtual int eval(){
        return !(con->eval());
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

    virtual void run() {

        if(condition==nullptr && elif_else==nullptr) {
            stmts->run();
        }
        else if(condition!=nullptr && elif_else!=nullptr) {
        if(condition->eval() != 0) {
            stmts->run();
        }
        else if (elif_else!=nullptr){
            elif_else->run();
        }
        }
        else {
            std::cout<<"During run, condition and elif_else aren't the same!\n";
        }
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
        
        void printOn(std::ostream& out) const override {
                out << "If condition: ";
                condition->printOn(out);
                out << "\nThen block:\n";
                stmts->printOn(out);

                elif_else->printOn(out);
            }
        

        
        virtual void run() {
            if(condition->eval() != 0) {
                stmts->run();
            }
            else if (elif_else!=nullptr){
                elif_else->run();
            }
        }


    private:
        Cond* condition;
        StmtList *stmts;
        Opt_Elif_Else *elif_else;
};
        

#endif