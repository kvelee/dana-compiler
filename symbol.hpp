#ifndef __SYMBOL_HPP__
#define __SYMBOL_HPP__

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <fstream> // should remove it later is for testing the symbol table
#include <llvm/IR/Value.h>


void yyerror(const char *msg);


//Temporary enumaration to implement the sem()
enum SymbolType {   
    TYPE_INT,
    TYPE_BYTE,
    TYPE_BOOL,
    TYPE_REF_INT,      
    TYPE_REF_BYTE,
    LOOP,  
    TYPE_FUNC,
    TYPE_PARAM,
    TYPE_INVALID
};

extern std::ostream &operator<<(std::ostream &out, SymbolType t);

struct SymbolEntry {
    SymbolType type;
    SymbolType return_value;
    //=====
    llvm::Value *alloca_ptr = nullptr;
    //=====
    SymbolEntry () {}
    SymbolEntry(SymbolType t, SymbolType rv = TYPE_INVALID) {
        switch (t) {
            case TYPE_FUNC:
                if (rv == TYPE_INT || rv == TYPE_BYTE || rv == TYPE_BOOL\
                 || rv == TYPE_FUNC  // In this case, function is VOID
                 || rv==TYPE_INVALID) {
                    type = TYPE_FUNC;
                    return_value = rv;
                } else {
                    //std::cout<<t<<" "<<rv<<"\n";
                    //sleep(10);
                    yyerror("Function return type must be int, byte or bool");
                    type = TYPE_INVALID;
                    return_value = TYPE_INVALID;
                }
                break;

            case TYPE_REF_INT:
                if (rv == TYPE_INT) {
                    type = TYPE_REF_INT;
                    return_value = TYPE_INT;
                } else {
                    yyerror("ref int must refer to int");
                    type = TYPE_INVALID;
                    return_value = TYPE_INVALID;
                }
                break;

            case TYPE_REF_BYTE:
                if (rv == TYPE_BYTE) {
                    type = TYPE_REF_BYTE;
                    return_value = TYPE_BYTE;
                } else {
                    yyerror("ref byte must refer to byte");
                    type = TYPE_INVALID;
                    return_value = TYPE_INVALID;
                }
                break;

            case TYPE_INT:
            case TYPE_BYTE:
            case TYPE_BOOL:
            case LOOP:
            case TYPE_PARAM:
            case TYPE_INVALID:
                // simple entries: if no rv supplied, use the type itself
                type = t;
                return_value = (rv == TYPE_INVALID ? t : rv);
                break;

            default:
                yyerror("Invalid SymbolEntry kind");
                type = TYPE_INVALID;
                return_value = TYPE_INVALID;
                break;
        }
    }

    int value;
    std::string name;
};

static SymbolEntry BUILTIN_FUNC_INT = SymbolEntry(TYPE_FUNC,TYPE_INT);
static SymbolEntry BUILTIN_FUNC_BYTE = SymbolEntry(TYPE_FUNC,TYPE_BYTE);


class Scope{
public:
    Scope() {}  

    SymbolEntry *lookup(std::string s){
        //std::cout<<"Inside locals, looking for "<<s<<"\n";//<<"==\n"<<locals[s].type<<"\n";
        //sleep(1);
        if(locals.find(s)==locals.end()) return nullptr;
        return &locals[s];
    }

    void insert(std::string s, SymbolType stt) {
        if(locals.find(s)!=locals.end()) yyerror(("Duplicate variable! " + s).c_str());
        locals[s]=SymbolEntry(stt,stt);
        locals[s].name = s;
    }
    void insert(std::string s, SymbolType stt, SymbolType rv) {
        if(locals.find(s)!=locals.end()) yyerror(("Duplicate variable! " + s).c_str());
        locals[s]=SymbolEntry(stt,rv);
        locals[s].name=s;
    }

    std::map<std::string, SymbolEntry> getlocals() {
        return locals;
    }


    SymbolEntry *function() {
        for (auto i=locals.begin(); i!=locals.end();++i){
            if (i->second.type==TYPE_FUNC) return &i->second;
        }
        return nullptr;
    }

private:
    std::map<std::string,SymbolEntry> locals;
};

class SymbolTable {
public:

    SymbolTable () {};

    SymbolEntry *lookup(std::string s) {
        
        //if(scopes.empty()) return nullptr;
        
        if(s=="writeString" || s=="writeChar" || s=="readString") return &BUILTIN_FUNC_BYTE;
        if(s=="readInteger" || s=="writeInteger" || s=="extend" \
        || s=="strlen" || s=="strcmp") return &BUILTIN_FUNC_INT;

        //printSymbolTable();
        //std::cout << "\nPress Enter to continue..." << std::flush;
        //std::ifstream tty("/dev/tty");
        //std::string dummy;
        //std::getline(tty, dummy);

        for (auto i = scopes.rbegin(); i!=scopes.rend(); ++i){
            SymbolEntry *e = i->lookup(s);
           
            if(e!=nullptr) { return e;}
        }

        yyerror(("Variable <" +s+ "> not found!").c_str());
        printSymbolTable();
        sleep(10);
        return nullptr;
    }

    void printSymbolTable() {
    std::cout << "\n========== SYMBOL TABLE ==========\n";

    if (scopes.empty()) {
        std::cout << "(empty)\n";
        return;
    }

    // Για κάθε scope
    for (size_t i = 0; i < scopes.size(); ++i) {
        std::cout << "\n--- Scope Level " << i << " ---\n";

        // Από το Scope παίρνουμε το map (locals)
        const auto locals = scopes[i].getlocals();

        if (locals.empty()) {
            std::cout << "  (no symbols)\n";
            continue;
        }

        // Στήλες
        std::cout << std::left
                  << "  " << std::setw(20) << "Identifier"
                  << std::setw(20) << "Type"
                  << std::setw(15) << "Return"
                  << "\n";
        std::cout << "  " << std::string(55, '-') << "\n";

        for (const auto &pair : locals) {
            const std::string &name = pair.first;
            const SymbolEntry &entry = pair.second;

            std::string typeStr;
            switch (entry.type) {
                case TYPE_INT: typeStr = "int"; break;
                case TYPE_BYTE: typeStr = "byte"; break;
                case TYPE_BOOL: typeStr = "bool"; break;
                case TYPE_REF_INT: typeStr = "ref int"; break;
                case TYPE_REF_BYTE: typeStr = "ref byte"; break;
                case TYPE_FUNC: typeStr = "function"; break;
                case TYPE_PARAM: typeStr = "param"; break;
                case LOOP: typeStr = "loop"; break;
                case TYPE_INVALID: typeStr = "invalid"; break;
                default: typeStr = "unknown";
            }

            std::string returnStr;
            switch (entry.return_value) {
                case TYPE_INT: returnStr = "int"; break;
                case TYPE_BYTE: returnStr = "byte"; break;
                case TYPE_BOOL: returnStr = "bool"; break;
                case TYPE_REF_INT: returnStr = "ref int"; break;
                case TYPE_REF_BYTE: returnStr = "ref byte"; break;
                case TYPE_FUNC: returnStr = "void"; break; // used to denote void
                case TYPE_PARAM: returnStr = "param"; break;
                case LOOP: returnStr = "loop"; break;
                case TYPE_INVALID: returnStr = "-"; break;
                default: returnStr = "unknown";
            }

            std::cout << "  " << std::setw(20) << name
                      << std::setw(20) << typeStr
                      << std::setw(15) << returnStr
                      << "\n";
        }
    }

    std::cout << "==================================\n";
}


    void insert(std::string s, SymbolType st) {
        //std::cout<<"Inserting "<<s<<"!\n";
        scopes.back().insert(s,st);
    }
    void insert(std::string s, SymbolType stt, SymbolType rv){
        scopes.back().insert(s,stt,rv);
    }

    void openScope() {
        scopes.push_back(Scope());
    }

    void closeScope() {
        if (scopes.empty()) yyerror("callScope() called but no open scopes!");
        else scopes.pop_back();
    }

    bool insideLoop() {
        //if (scopes.empty()) printSymbolTable();
        
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            const auto locals = it->getlocals();
            for (const auto &p : locals) {
                if (p.second.type == LOOP) return true;
            }
        }
        return false;
    }

    SymbolEntry *function(){
        // Get the most recent function
        if (scopes.empty()) return nullptr;

        for (auto i = scopes.rbegin(); i!=scopes.rend(); ++i){
            SymbolEntry *e = i->function();
            if(e!=nullptr) return e;
        }

        yyerror("Not inside a function!");
        return nullptr;
        
    }

    bool hasScope() {
        return !scopes.empty();
    }

private:
    std::vector<Scope> scopes;
};

extern SymbolTable st;

#endif