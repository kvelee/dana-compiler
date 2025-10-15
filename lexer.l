%{
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"
#include <stdio.h>
#include <string>

#define T_eof 0

int current_ind_level = 0;
unsigned int indent_stack[100] = {0};
unsigned int level = 0;

unsigned int current_indent = 0;

unsigned int white_count(char* line) {
    unsigned int count = 0;
    while (*line == ' ' || *line == '\t')
        count++, line++;
    return count;
}

int process_indent(char* line) {
    current_indent = white_count(line);
    
    if (*line == '\n' || *line == '\r' || *line == '#' || *line == '\0')
        return 0;

    int dedent_count = 0;
    
    while (level > 0 && current_indent <= indent_stack[level]) {
        --level;
        dedent_count++;
    }
    
    return dedent_count;
}

int is_guide_token = 0;
%}

L [A-Za-z]
D [0-9]


%option noyywrap
%option yylineno

%%

"and" { return T_and; } 
"as" { return T_as; } 
"begin" { return T_begin; } 
"break" { return T_break; } 
"byte" { return T_byte; } 
"continue" { return T_continue; } 
"decl" { return T_dec; }
"def" { is_guide_token = 1; indent_stack[++level] = current_indent; return T_def; } 
"elif" { is_guide_token = 1; indent_stack[++level] = current_indent; return T_elif; } 
"else" { is_guide_token = 1; indent_stack[++level] = current_indent; return T_else; } 
"end" { return T_end; } 
"exit" { return T_exit; } 
"false" { return T_false; } 
"if" { is_guide_token = 1; indent_stack[++level] = current_indent; return T_if; }
"is" { return T_is; } 
"int" { return T_int; } 
"loop" { is_guide_token = 1; indent_stack[++level] = current_indent; return T_loop; } 
"not" { return T_not; } 
"or" { return T_or; } 
"ref" { return T_ref; } 
"return" { return T_return; }
"skip" { return T_skip; } 
"true" { return T_true; } 
"var" { return T_var; }

\'({L}|{D}|\\[ntr0\\\'\"]\') { yylval.str = new std::string(yytext); return T_const; }
\'\\x[0-9a-fA-F]{2}\' { yylval.str = new std::string(yytext); return T_const; }

":=" { return T_sep; }
[\(\)\[\]\,\:] { return yytext[0]; }

{L}({L}|{D})* { yylval.str = new std::string(yytext); return T_id; }
{D}+ { yylval.num = atoi(yytext); return T_number; }

^[ \t]*\n { ; }

^[ \t]*#.*\n { ; }

^[ \t]*[^ \t\n]+ {
    char *yynew;
    int last = yyleng - 1;
    int indent = white_count(yytext);
    int dedent_count = process_indent(yytext);

    yynew = strdup(yytext);

    while ((last >= 0) && (yynew[last] != ' ' && yynew[last] != '\t')) {
        unput(yynew[last]);
        last--;
    }
    free(yynew);

    if (dedent_count > 0) {
        for (int i = 1; i < dedent_count; i++) {
            unput('}');
        }
        return auto_end;
    }
}

[ \t] { ; }

[\n\r] { 
    is_guide_token = 0;
}

"}" {
    return auto_end;
}
                    
\"([^\"\n]|\\.)*\" { 
    yylval.str = new std::string(yytext + 1, yyleng - 2); // Create a new std::string without quotes
    return T_string;
}

\#.*\n { ; }
\(\*(.|\n)*\*\) { ; }

[\+\-\*\/\%\!\&\|\=\<\>\:] { return yytext[0]; }
">=" { return T_ge; }
"<=" { return T_le; }
"<>" { return T_ne; }

<<EOF>> { 
    if (level > 0) {
        level--;
        return auto_end;
    }
    return 0; 
}

. { printf("Unrecognized character: %s\n", yytext); }
%%

void init_indent_stack() {
    indent_stack[0] = 0;
    level = 0;
    current_indent = 0;
    is_guide_token = 0;
}