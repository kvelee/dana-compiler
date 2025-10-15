%{
#include "lexer.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include "ast.hpp"

extern int yylineno;
extern char* yytext;

extern void init_indent_stack();

typedef struct StackNode {
    char *keyword;
    int position;
    int line;
    struct StackNode *next;
} StackNode;

StackNode *stack_top = NULL;

void push_guide(char *keyword, int position, int line) {
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    
    node->keyword = strdup(keyword);
    node->position = position;
    node->line = line;
    node->next = stack_top;
    stack_top = node;
}

void pop_guide() {
    if (stack_top == NULL) return;
    
    StackNode *temp = stack_top;
    stack_top = stack_top->next;
    
    free(temp->keyword);
    free(temp);
}

void free_guide_stack() {
    while (stack_top != NULL) {
        pop_guide();
    }
}

void print_guide_stack() {
    StackNode *temp = stack_top;
    printf("Guide stack (top to bottom):\n");
    while (temp != NULL) {
        printf("  %s at position %d (line %d)\n", temp->keyword, temp->position, temp->line);
        temp = temp->next;
    }
}
%}

%define parse.error verbose
%define parse.lac full
%define parse.trace

%expect 1

%token T_and  "and"
%token T_as  "as"
%token T_begin  "begin"
%token T_break  "break"
%token T_byte  "byte"
%token T_continue  "continue"
%token T_dec  "decl"
%token T_def  "def"
%token T_elif  "elif"
%token T_else  "else"
%token T_end  "end"
%token T_exit  "exit"
%token T_false  "false"
%token T_if  "if"
%token T_is  "is"
%token T_int  "int"
%token T_loop  "loop"
%token T_not  "not"
%token T_or  "or"
%token T_ref  "ref"
%token T_return  "return"
%token T_skip  "skip"
%token T_true  "true"
%token T_var  "var"
%token T_sep  ":="
%token T_ge  ">=" 
%token T_le  "<="
%token T_ne  "<>"


%union {
    AST *ast;
    Header *hd;
    DataType type;
    Type *tp;
    FparType *fpt;
    Id_List *idlst;
    Opt_Fpar *optfp;
    StmtList *st_list;
    LoopBlock *lb;
    LocalDef *lcldef;
    Lvalue *lval;
    IfBlock *ifblck;
    Expr *expr;
    Cond *cnd;
    ExprList *exlst;
    Opt_Elif_Else *oee;
    LocalDefList *ldl;
    Stmt *st;
    FuncDefBlock *fdf;
    FuncDecl *fdcl;
    Func_Call *fcall;
    Proc_Call *pcall;
    std::string *str; 
    int num;
}

%token<str> T_const
%token<str> T_id  
%token<num> T_number  
%token<str> T_string
%token auto_end




%type<ast> program  
%type<fdf> func_def
%type<st> stmt 
%type<ldl> local_def_list
%type<st_list> stmt_list
%type<hd> header
%type<type> data_type ref_data_type
%type<tp> type 
%type<fpt> fpar_type array_type
%type<idlst> id_list
%type<optfp> opt_fpar
%type<lb> loop
%type<oee> opt_elif_else
%type<lcldef> local_def 
%type<fdcl> func_decl
%type<expr> expr 
%type<fcall> func_call
%type<pcall> proc_call
%type<cnd> cond
%type<lval> l_value
%type<exlst> expr_list
%type<ifblck> if_stmts

%nonassoc "def" "if" "loop" "break" "continue" "return"
%nonassoc '=' "<>" '<' '>' "<=" ">=" '!'
%left "or"
%left "and"
%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%' 
%nonassoc "not"  '(' ')'

%%
program:
    func_def { std::cout<< "AST: " << *$1 <<std::endl;  }
;

func_def:
    "def" header local_def_list stmt_list auto_end { $$ = new FuncDefBlock($2,$3,$4); }
;

func_decl:
    "decl" header { $$ = new FuncDecl($2);}
;

header:
    T_id "is" type ':' opt_fpar { $$ = new Header(new Id($1),$3,$5); }
|   T_id "is" type { $$ = new Header(new Id($1),$3); }
|   T_id ':' opt_fpar { $$ = new Header(new Id($1),nullptr,$3);}
|   T_id { $$ = new Header(new Id($1)); }
;

type:
    type '[' T_number ']' { $1->add_dim($3); $$ = $1; }
|   data_type { $$ = new Type($1); }
;

data_type:
    "int"  { $$ = TYPE_int;}
|   "byte" { $$ = TYPE_byte;}
;

opt_fpar:
    id_list "as" ref_data_type { $$ = new Opt_Fpar($1,$3); }
|   id_list "as" ref_data_type ',' opt_fpar { $$ = new Opt_Fpar($1,$3,$5); }
|   id_list "as" fpar_type {  $$ = new Opt_Fpar($1,$3); }
|   id_list "as" fpar_type ',' opt_fpar {$$ = new Opt_Fpar($1,$3,$5); }
;

ref_data_type:
    "ref" "int" { $$ = REF_int; }
|   "ref" "byte" { $$ = REF_byte;}
;

fpar_type:
    "int" { $$ = new FparType(TYPE_int); }
|   "byte" { $$ = new FparType(TYPE_byte); }
|   array_type { $$ = $1; }
;

array_type:
    "int" '[' ']' { $$ = new ArrayType(TYPE_int,0); }
|   "byte" '[' ']' { $$ = new ArrayType(TYPE_byte,0); }
|   "int" '[' T_number ']' { $$ = new ArrayType(TYPE_int, $3); }
|   "byte" '[' T_number ']' { $$ = new ArrayType(TYPE_byte, $3); }
|   array_type '[' T_number ']' { $1->add_dim($3) ; $$=$1; }
;

id_list:
      T_id { $$ = new Id_List(*$1); }
    | id_list T_id { $1->append(*$2); $$ = $1; }
;

stmt_list:
      stmt { $$ = new StmtList($1); }
    | stmt_list stmt { $1->append($2); $$ = $1; }
;

local_def_list: 
      /*nothing*/                    { $$ = new LocalDefList();}
    | local_def_list local_def     { $1->append($2); $$=$1; }
;

local_def: 
    func_def    {$$ = new LocalDef($1); }
|   func_decl   {$$ = new LocalDef($1); }
|   "var" id_list "is" type { $$ = new LocalDef($2,$4); } 
;

stmt:
      "skip"                                        { $$ = new SkipStmt(); }
    | l_value ":=" expr                             { $$ = new LvalueStmt($1,$3); }
    | func_call                                     { $$ = new FuncStmt($1); }
    | proc_call                                     { $$ = new ProcStmt($1); }
    | "exit"                                        { $$ = new ExitStmt();}
    | "return" ':' expr                             { $$ = new ReturnStmt($3); }
    | if_stmts                                      { $$ = $1; }
    | loop                                          { $$ = $1; }
    | "break"                                       { $$ = new BreakStmt();}
    | "break" ':' T_id                              { $$ = new BreakStmt($3);}
    | "continue"                                    { $$ = new ContStmt();}
    | "continue" ':' T_id                           { $$ = new ContStmt($3);}
;

if_stmts:
   "if" cond ':' stmt_list auto_end opt_elif_else {
        $$ = new IfBlock($2,$4,$6);
}
//|  "if" cond ':' stmt_list auto_end "else" ':' stmt_list auto_end { 
//        
//        $$ = new IfBlock($2,$4,$8);
//    }
//|   "if" cond ':' stmt_list auto_end "elif" cond ':' stmt_list auto_end opt_elif_else {
//        $$ = new IfBlock($2,$4,$7,$9,$11);}
;

opt_elif_else:
      /* empty */ { $$ = nullptr;}
    | "else" ':' stmt_list auto_end { $$ = new Opt_Elif_Else($3); } 
    | "elif" cond ':' stmt_list auto_end opt_elif_else { $$ = new Opt_Elif_Else($2,$4,$6); }
;

//Lvalues can also appear as -expr- (see below)
l_value:
      T_id     { $$ = new Lvalue (new Id($1)); }
    | T_string { $$ = new Lvalue($1); }
    | l_value '[' expr ']' { $1->append($3); $$=$1; }
;

loop:
    "loop" T_id ':' stmt_list auto_end { $$ = new LoopBlock($2,$4);}
|   "loop" ':' stmt_list auto_end { $$ = new LoopBlock($3); }
;

proc_call:
      T_id                      { $$ = new Proc_Call(new Id($1));}
    | T_id ':' expr_list        { $$ = new Proc_Call(new Id($1),$3);}
;

//Func-calls can also appear as -expr- (see bellow)
func_call:
      T_id '('')'            { $$ = new Func_Call(new Id($1));}
    | T_id '(' expr_list ')' { $$ = new Func_Call(new Id($1),$3);}
;

expr:
      T_number                { $$ = new Number($1); }
    | T_const                 { $$ = new Const($1); }
    | l_value                 { $$ = $1; }
    | '(' expr ')'            { $$ = $2; }
    | func_call               { $$ = $1; }
    | '+' expr                { $$ = new UniOp('+', $2); }
    | '-' expr                { $$ = new UniOp('-', $2); }
    | expr '+' expr           { $$ = new BinOp($1,'+', $3); }
    | expr '-' expr           { $$ = new BinOp($1,'-', $3); }
    | expr '*' expr           { $$ = new BinOp($1,'*', $3); }
    | expr '/' expr           { $$ = new BinOp($1,'/', $3); }
    | expr '%' expr           { $$ = new BinOp($1,'%', $3); }
    | "true"                  { $$ = new Bool(true); }
    | "false"                 { $$ = new Bool(false); }
    | '!' expr                { $$ = new NotExpr($2); }
    | expr '&' expr           { $$ = new BoolBinOp($1, '&', $3); }
    | expr '|' expr           { $$ = new BoolBinOp($1, '|', $3); }
;

cond:
      expr '>' expr               { $$ = new Cond($1,">",$3); }
    | expr '<' expr               { $$ = new Cond($1,"<",$3); }
    | expr ">=" expr              { $$ = new Cond($1,">=",$3); }
    | expr "<=" expr              { $$ = new Cond($1,"<=",$3); }
    | expr '=' expr               { $$ = new Cond($1,"=",$3); }
    | expr "<>" expr              { $$ = new Cond($1,"<>",$3); }
    | cond "and" cond             { $$ = new Cond($1,"and",$3); }
    | cond "or" cond              { $$ = new Cond($1,"or",$3); }
    | "not" cond                  { $$ = new NotCond($2); }
    | '(' cond ')'                { $$ = $2;}
    | expr                        { $$ = new Cond($1) ;}
;

expr_list:
    expr { $$ = new ExprList($1); }
|   expr_list ',' expr { $1 ->append($3);  $$ = $1; }
;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Error: %s at line %d, near '%s'\n", msg, yylineno, yytext);
}

int main() {
    init_indent_stack();
    int result = yyparse();
    if(result == 0)
        printf("Success.\n");
    else
        printf("Parsing failed.\n");
    
    free_guide_stack();
    return result;
}