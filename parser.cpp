/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

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

#line 134 "parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_and = 3,                      /* "and"  */
  YYSYMBOL_T_as = 4,                       /* "as"  */
  YYSYMBOL_T_begin = 5,                    /* "begin"  */
  YYSYMBOL_T_break = 6,                    /* "break"  */
  YYSYMBOL_T_byte = 7,                     /* "byte"  */
  YYSYMBOL_T_continue = 8,                 /* "continue"  */
  YYSYMBOL_T_dec = 9,                      /* "decl"  */
  YYSYMBOL_T_def = 10,                     /* "def"  */
  YYSYMBOL_T_elif = 11,                    /* "elif"  */
  YYSYMBOL_T_else = 12,                    /* "else"  */
  YYSYMBOL_T_end = 13,                     /* "end"  */
  YYSYMBOL_T_exit = 14,                    /* "exit"  */
  YYSYMBOL_T_false = 15,                   /* "false"  */
  YYSYMBOL_T_if = 16,                      /* "if"  */
  YYSYMBOL_T_is = 17,                      /* "is"  */
  YYSYMBOL_T_int = 18,                     /* "int"  */
  YYSYMBOL_T_loop = 19,                    /* "loop"  */
  YYSYMBOL_T_not = 20,                     /* "not"  */
  YYSYMBOL_T_or = 21,                      /* "or"  */
  YYSYMBOL_T_ref = 22,                     /* "ref"  */
  YYSYMBOL_T_return = 23,                  /* "return"  */
  YYSYMBOL_T_skip = 24,                    /* "skip"  */
  YYSYMBOL_T_true = 25,                    /* "true"  */
  YYSYMBOL_T_var = 26,                     /* "var"  */
  YYSYMBOL_T_sep = 27,                     /* ":="  */
  YYSYMBOL_T_ge = 28,                      /* ">="  */
  YYSYMBOL_T_le = 29,                      /* "<="  */
  YYSYMBOL_T_ne = 30,                      /* "<>"  */
  YYSYMBOL_T_const = 31,                   /* T_const  */
  YYSYMBOL_T_id = 32,                      /* T_id  */
  YYSYMBOL_T_number = 33,                  /* T_number  */
  YYSYMBOL_T_string = 34,                  /* T_string  */
  YYSYMBOL_auto_end = 35,                  /* auto_end  */
  YYSYMBOL_36_ = 36,                       /* '='  */
  YYSYMBOL_37_ = 37,                       /* '<'  */
  YYSYMBOL_38_ = 38,                       /* '>'  */
  YYSYMBOL_39_ = 39,                       /* '!'  */
  YYSYMBOL_40_ = 40,                       /* '|'  */
  YYSYMBOL_41_ = 41,                       /* '&'  */
  YYSYMBOL_42_ = 42,                       /* '+'  */
  YYSYMBOL_43_ = 43,                       /* '-'  */
  YYSYMBOL_44_ = 44,                       /* '*'  */
  YYSYMBOL_45_ = 45,                       /* '/'  */
  YYSYMBOL_46_ = 46,                       /* '%'  */
  YYSYMBOL_47_ = 47,                       /* '('  */
  YYSYMBOL_48_ = 48,                       /* ')'  */
  YYSYMBOL_49_ = 49,                       /* ':'  */
  YYSYMBOL_50_ = 50,                       /* '['  */
  YYSYMBOL_51_ = 51,                       /* ']'  */
  YYSYMBOL_52_ = 52,                       /* ','  */
  YYSYMBOL_YYACCEPT = 53,                  /* $accept  */
  YYSYMBOL_program = 54,                   /* program  */
  YYSYMBOL_func_def = 55,                  /* func_def  */
  YYSYMBOL_func_decl = 56,                 /* func_decl  */
  YYSYMBOL_header = 57,                    /* header  */
  YYSYMBOL_type = 58,                      /* type  */
  YYSYMBOL_data_type = 59,                 /* data_type  */
  YYSYMBOL_opt_fpar = 60,                  /* opt_fpar  */
  YYSYMBOL_ref_data_type = 61,             /* ref_data_type  */
  YYSYMBOL_fpar_type = 62,                 /* fpar_type  */
  YYSYMBOL_array_type = 63,                /* array_type  */
  YYSYMBOL_id_list = 64,                   /* id_list  */
  YYSYMBOL_stmt_list = 65,                 /* stmt_list  */
  YYSYMBOL_local_def_list = 66,            /* local_def_list  */
  YYSYMBOL_local_def = 67,                 /* local_def  */
  YYSYMBOL_stmt = 68,                      /* stmt  */
  YYSYMBOL_if_stmts = 69,                  /* if_stmts  */
  YYSYMBOL_opt_elif_else = 70,             /* opt_elif_else  */
  YYSYMBOL_l_value = 71,                   /* l_value  */
  YYSYMBOL_loop = 72,                      /* loop  */
  YYSYMBOL_proc_call = 73,                 /* proc_call  */
  YYSYMBOL_func_call = 74,                 /* func_call  */
  YYSYMBOL_expr = 75,                      /* expr  */
  YYSYMBOL_cond = 76,                      /* cond  */
  YYSYMBOL_expr_list = 77                  /* expr_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
# define YYCOPY_NEEDED 1
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   390

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  25
/* YYNRULES -- Number of rules.  */
#define YYNRULES  90
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  169

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   290


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    39,     2,     2,     2,    46,    41,     2,
      47,    48,    44,    42,    52,    43,     2,    45,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    49,     2,
      37,    36,    38,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    50,     2,    51,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    40,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   171,   171,   175,   179,   183,   184,   185,   186,   190,
     191,   195,   196,   200,   201,   202,   203,   207,   208,   212,
     213,   214,   218,   219,   220,   221,   222,   226,   227,   231,
     232,   236,   237,   241,   242,   243,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   262,   274,
     275,   276,   281,   282,   283,   287,   288,   292,   293,   298,
     299,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     312,   313,   314,   315,   316,   317,   318,   319,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   337,
     338
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "\"and\"", "\"as\"",
  "\"begin\"", "\"break\"", "\"byte\"", "\"continue\"", "\"decl\"",
  "\"def\"", "\"elif\"", "\"else\"", "\"end\"", "\"exit\"", "\"false\"",
  "\"if\"", "\"is\"", "\"int\"", "\"loop\"", "\"not\"", "\"or\"",
  "\"ref\"", "\"return\"", "\"skip\"", "\"true\"", "\"var\"", "\":=\"",
  "\">=\"", "\"<=\"", "\"<>\"", "T_const", "T_id", "T_number", "T_string",
  "auto_end", "'='", "'<'", "'>'", "'!'", "'|'", "'&'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "'('", "')'", "':'", "'['", "']'", "','", "$accept",
  "program", "func_def", "func_decl", "header", "type", "data_type",
  "opt_fpar", "ref_data_type", "fpar_type", "array_type", "id_list",
  "stmt_list", "local_def_list", "local_def", "stmt", "if_stmts",
  "opt_elif_else", "l_value", "loop", "proc_call", "func_call", "expr",
  "cond", "expr_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-59)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-53)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      25,    16,    14,   -59,   -10,   -59,   -59,    15,    18,   324,
     -59,   -59,    11,   -59,   -59,   -59,    13,    20,    22,    16,
     -59,   194,    -3,    37,   -59,    18,    76,   -59,   -59,   -59,
      56,   -59,   -59,   -59,   -18,   -59,   -59,   -59,    18,    30,
      95,   -59,    46,    51,   -59,   -59,   194,   -59,   -59,     0,
     -59,   215,   215,   215,   194,    39,   -59,   344,     3,    38,
       4,   215,    -2,    34,   215,   -59,   -59,   215,   215,   -59,
      49,    61,    68,    35,    60,    67,    70,   -59,   -59,   -59,
     215,   279,    62,    62,   323,    10,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   194,
     194,     4,     4,   108,   279,    15,   -59,   279,    47,    69,
     279,   140,   -59,   -17,   -14,   -59,   -59,    18,    18,   105,
     104,   -59,   -59,   279,   279,   279,   279,   279,   279,   124,
     178,    62,    62,   -59,   -59,   -59,   -59,    98,   236,   258,
     -59,    78,   -59,   215,   -59,    86,   -59,    88,   -59,   -59,
     -59,    90,    29,   -59,   279,   -59,   -59,   -59,   194,   102,
     -59,     5,     4,     4,   272,   294,   -59,    29,   -59
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     8,    31,     1,     0,     0,     0,
      12,    11,     6,    10,    27,     7,     0,    44,    46,     0,
      40,     0,     0,     0,    36,     0,    57,    53,    33,    34,
       0,    32,    29,    42,     0,    43,    39,    38,     0,     0,
       0,    28,     0,     0,     4,    74,     0,    73,    62,    52,
      61,     0,     0,     0,     0,    63,    65,    88,     0,     0,
       0,     0,     0,     0,     0,     3,    30,     0,     0,     5,
       0,    20,    19,     0,    13,    15,    21,    45,    47,    86,
       0,    75,    66,    67,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    41,     0,    59,    89,     0,    58,
      37,     0,     9,     0,     0,    18,    17,     0,     0,     0,
       0,    64,    87,    80,    81,    83,    82,    79,    78,    77,
      76,    68,    69,    70,    71,    72,    84,    85,     0,     0,
      56,    35,    60,     0,    54,     0,    23,     0,    22,    14,
      16,     0,    49,    55,    90,    25,    24,    26,     0,     0,
      48,     0,     0,     0,     0,     0,    50,    49,    51
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -59,   -59,   163,   -59,   156,    71,   -59,   -33,   -59,   -59,
     -59,   154,   -58,   -59,   -59,   -29,   -59,    21,    -9,   -59,
     -59,    -5,   110,   -43,   123
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,    29,     5,    12,    13,    15,    74,    75,
      76,    16,    30,     9,    31,    32,    33,   160,    55,    35,
      36,    56,    57,    58,   108
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      34,    66,   103,    79,    37,    69,    99,     7,    99,    67,
      17,    85,    18,    99,     6,   105,   145,    40,    20,   147,
      21,    34,    10,    22,   100,    37,   100,    23,    24,    59,
      41,   100,    68,    11,   146,     1,    26,   148,    27,     8,
     158,   159,   115,   138,   139,    41,    60,    63,     4,    45,
      14,    34,   101,   116,   163,    37,   136,   137,   122,    47,
      38,    39,    17,    70,    18,    48,    49,    50,    27,    42,
      20,    43,    21,    51,    66,    22,    52,    53,    77,    23,
      24,    80,   106,    78,   149,   150,    61,   102,    26,    68,
      27,    65,    34,    34,    34,   142,    37,    37,    37,   143,
     112,    99,    71,   -52,   164,   165,    96,    97,    98,    66,
      66,   113,   117,    72,    17,   161,    18,    73,   114,   118,
     119,   143,    20,    63,    21,    64,   -52,    22,    39,    34,
      34,    23,    24,    37,    37,    66,    66,   155,   151,   156,
      26,   157,    27,   140,    92,    93,    94,    95,    96,    97,
      98,   162,   121,    34,    34,    34,    34,    37,    37,    37,
      37,    81,    82,    83,    84,    93,    94,    95,    96,    97,
      98,   104,    28,   107,   107,    44,   141,   110,   111,    62,
      92,    93,    94,    95,    96,    97,    98,   109,   168,     0,
     120,   144,     0,     0,     0,     0,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,    45,
       0,     0,     0,     0,    46,     0,     0,     0,     0,    47,
      94,    95,    96,    97,    98,    48,    49,    50,    27,     0,
      45,     0,     0,    51,     0,     0,    52,    53,     0,     0,
      47,    54,    17,     0,    18,     0,    48,    49,    50,    27,
      20,     0,    21,   154,    51,    22,     0,    52,    53,    23,
      24,     0,    80,     0,    17,     0,    18,     0,    26,     0,
      27,   152,    20,     0,    21,     0,     0,    22,    17,     0,
      18,    23,    24,     0,     0,     0,    20,     0,    21,     0,
      26,    22,    27,   153,     0,    23,    24,     0,     0,     0,
      17,     0,    18,     0,    26,     0,    27,   166,    20,     0,
      21,     0,     0,    22,     0,     0,     0,    23,    24,    92,
      93,    94,    95,    96,    97,    98,    26,     0,    27,   167,
      17,     0,    18,    19,     1,     0,     0,     0,    20,     0,
      21,     0,     0,    22,     0,     0,     0,    23,    24,     0,
      25,    86,    87,    88,     0,     0,    26,     0,    27,    89,
      90,    91,     0,    92,    93,    94,    95,    96,    97,    98,
       0,   121,    86,    87,    88,     0,     0,     0,     0,     0,
      89,    90,    91,     0,    92,    93,    94,    95,    96,    97,
      98
};

static const yytype_int16 yycheck[] =
{
       9,    30,    60,    46,     9,    38,     3,    17,     3,    27,
       6,    54,     8,     3,     0,    17,    33,     4,    14,    33,
      16,    30,     7,    19,    21,    30,    21,    23,    24,    32,
      32,    21,    50,    18,    51,    10,    32,    51,    34,    49,
      11,    12,     7,   101,   102,    32,    49,    47,    32,    15,
      32,    60,    49,    18,    49,    60,    99,   100,    48,    25,
      49,    50,     6,    33,     8,    31,    32,    33,    34,    49,
      14,    49,    16,    39,   103,    19,    42,    43,    32,    23,
      24,    47,    48,    32,   117,   118,    49,    49,    32,    50,
      34,    35,   101,   102,   103,    48,   101,   102,   103,    52,
      51,     3,     7,    27,   162,   163,    44,    45,    46,   138,
     139,    50,    52,    18,     6,   158,     8,    22,    50,    52,
      50,    52,    14,    47,    16,    49,    50,    19,    50,   138,
     139,    23,    24,   138,   139,   164,   165,    51,    33,    51,
      32,    51,    34,    35,    40,    41,    42,    43,    44,    45,
      46,    49,    48,   162,   163,   164,   165,   162,   163,   164,
     165,    51,    52,    53,    54,    41,    42,    43,    44,    45,
      46,    61,     9,    63,    64,    19,   105,    67,    68,    25,
      40,    41,    42,    43,    44,    45,    46,    64,   167,    -1,
      80,    51,    -1,    -1,    -1,    -1,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    15,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    25,
      42,    43,    44,    45,    46,    31,    32,    33,    34,    -1,
      15,    -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,
      25,    47,     6,    -1,     8,    -1,    31,    32,    33,    34,
      14,    -1,    16,   143,    39,    19,    -1,    42,    43,    23,
      24,    -1,    47,    -1,     6,    -1,     8,    -1,    32,    -1,
      34,    35,    14,    -1,    16,    -1,    -1,    19,     6,    -1,
       8,    23,    24,    -1,    -1,    -1,    14,    -1,    16,    -1,
      32,    19,    34,    35,    -1,    23,    24,    -1,    -1,    -1,
       6,    -1,     8,    -1,    32,    -1,    34,    35,    14,    -1,
      16,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,    40,
      41,    42,    43,    44,    45,    46,    32,    -1,    34,    35,
       6,    -1,     8,     9,    10,    -1,    -1,    -1,    14,    -1,
      16,    -1,    -1,    19,    -1,    -1,    -1,    23,    24,    -1,
      26,    28,    29,    30,    -1,    -1,    32,    -1,    34,    36,
      37,    38,    -1,    40,    41,    42,    43,    44,    45,    46,
      -1,    48,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
      36,    37,    38,    -1,    40,    41,    42,    43,    44,    45,
      46
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    10,    54,    55,    32,    57,     0,    17,    49,    66,
       7,    18,    58,    59,    32,    60,    64,     6,     8,     9,
      14,    16,    19,    23,    24,    26,    32,    34,    55,    56,
      65,    67,    68,    69,    71,    72,    73,    74,    49,    50,
       4,    32,    49,    49,    57,    15,    20,    25,    31,    32,
      33,    39,    42,    43,    47,    71,    74,    75,    76,    32,
      49,    49,    64,    47,    49,    35,    68,    27,    50,    60,
      33,     7,    18,    22,    61,    62,    63,    32,    32,    76,
      47,    75,    75,    75,    75,    76,    28,    29,    30,    36,
      37,    38,    40,    41,    42,    43,    44,    45,    46,     3,
      21,    49,    49,    65,    75,    17,    48,    75,    77,    77,
      75,    75,    51,    50,    50,     7,    18,    52,    52,    50,
      75,    48,    48,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    76,    76,    65,    65,
      35,    58,    48,    52,    51,    33,    51,    33,    51,    60,
      60,    33,    35,    35,    75,    51,    51,    51,    11,    12,
      70,    76,    49,    49,    65,    65,    35,    35,    70
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    53,    54,    55,    56,    57,    57,    57,    57,    58,
      58,    59,    59,    60,    60,    60,    60,    61,    61,    62,
      62,    62,    63,    63,    63,    63,    63,    64,    64,    65,
      65,    66,    66,    67,    67,    67,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    69,    70,
      70,    70,    71,    71,    71,    72,    72,    73,    73,    74,
      74,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    77,
      77
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     5,     2,     5,     3,     3,     1,     4,
       1,     1,     1,     3,     5,     3,     5,     2,     2,     1,
       1,     1,     3,     3,     4,     4,     4,     1,     2,     1,
       2,     0,     2,     1,     1,     4,     1,     3,     1,     1,
       1,     3,     1,     1,     1,     3,     1,     3,     6,     0,
       4,     6,     1,     1,     4,     5,     4,     1,     3,     3,
       4,     1,     1,     1,     3,     1,     2,     2,     3,     3,
       3,     3,     3,     1,     1,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     1,     1,
       3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        YY_LAC_DISCARD ("YYBACKUP");                              \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Given a state stack such that *YYBOTTOM is its bottom, such that
   *YYTOP is either its top or is YYTOP_EMPTY to indicate an empty
   stack, and such that *YYCAPACITY is the maximum number of elements it
   can hold without a reallocation, make sure there is enough room to
   store YYADD more elements.  If not, allocate a new stack using
   YYSTACK_ALLOC, copy the existing elements, and adjust *YYBOTTOM,
   *YYTOP, and *YYCAPACITY to reflect the new capacity and memory
   location.  If *YYBOTTOM != YYBOTTOM_NO_FREE, then free the old stack
   using YYSTACK_FREE.  Return 0 if successful or if no reallocation is
   required.  Return YYENOMEM if memory is exhausted.  */
static int
yy_lac_stack_realloc (YYPTRDIFF_T *yycapacity, YYPTRDIFF_T yyadd,
#if YYDEBUG
                      char const *yydebug_prefix,
                      char const *yydebug_suffix,
#endif
                      yy_state_t **yybottom,
                      yy_state_t *yybottom_no_free,
                      yy_state_t **yytop, yy_state_t *yytop_empty)
{
  YYPTRDIFF_T yysize_old =
    *yytop == yytop_empty ? 0 : *yytop - *yybottom + 1;
  YYPTRDIFF_T yysize_new = yysize_old + yyadd;
  if (*yycapacity < yysize_new)
    {
      YYPTRDIFF_T yyalloc = 2 * yysize_new;
      yy_state_t *yybottom_new;
      /* Use YYMAXDEPTH for maximum stack size given that the stack
         should never need to grow larger than the main state stack
         needs to grow without LAC.  */
      if (YYMAXDEPTH < yysize_new)
        {
          YYDPRINTF ((stderr, "%smax size exceeded%s", yydebug_prefix,
                      yydebug_suffix));
          return YYENOMEM;
        }
      if (YYMAXDEPTH < yyalloc)
        yyalloc = YYMAXDEPTH;
      yybottom_new =
        YY_CAST (yy_state_t *,
                 YYSTACK_ALLOC (YY_CAST (YYSIZE_T,
                                         yyalloc * YYSIZEOF (*yybottom_new))));
      if (!yybottom_new)
        {
          YYDPRINTF ((stderr, "%srealloc failed%s", yydebug_prefix,
                      yydebug_suffix));
          return YYENOMEM;
        }
      if (*yytop != yytop_empty)
        {
          YYCOPY (yybottom_new, *yybottom, yysize_old);
          *yytop = yybottom_new + (yysize_old - 1);
        }
      if (*yybottom != yybottom_no_free)
        YYSTACK_FREE (*yybottom);
      *yybottom = yybottom_new;
      *yycapacity = yyalloc;
    }
  return 0;
}

/* Establish the initial context for the current lookahead if no initial
   context is currently established.

   We define a context as a snapshot of the parser stacks.  We define
   the initial context for a lookahead as the context in which the
   parser initially examines that lookahead in order to select a
   syntactic action.  Thus, if the lookahead eventually proves
   syntactically unacceptable (possibly in a later context reached via a
   series of reductions), the initial context can be used to determine
   the exact set of tokens that would be syntactically acceptable in the
   lookahead's place.  Moreover, it is the context after which any
   further semantic actions would be erroneous because they would be
   determined by a syntactically unacceptable token.

   YY_LAC_ESTABLISH should be invoked when a reduction is about to be
   performed in an inconsistent state (which, for the purposes of LAC,
   includes consistent states that don't know they're consistent because
   their default reductions have been disabled).  Iff there is a
   lookahead token, it should also be invoked before reporting a syntax
   error.  This latter case is for the sake of the debugging output.

   For parse.lac=full, the implementation of YY_LAC_ESTABLISH is as
   follows.  If no initial context is currently established for the
   current lookahead, then check if that lookahead can eventually be
   shifted if syntactic actions continue from the current context.
   Report a syntax error if it cannot.  */
#define YY_LAC_ESTABLISH                                                \
do {                                                                    \
  if (!yy_lac_established)                                              \
    {                                                                   \
      YYDPRINTF ((stderr,                                               \
                  "LAC: initial context established for %s\n",          \
                  yysymbol_name (yytoken)));                            \
      yy_lac_established = 1;                                           \
      switch (yy_lac (yyesa, &yyes, &yyes_capacity, yyssp, yytoken))    \
        {                                                               \
        case YYENOMEM:                                                  \
          YYNOMEM;                                                      \
        case 1:                                                         \
          goto yyerrlab;                                                \
        }                                                               \
    }                                                                   \
} while (0)

/* Discard any previous initial lookahead context because of Event,
   which may be a lookahead change or an invalidation of the currently
   established initial context for the current lookahead.

   The most common example of a lookahead change is a shift.  An example
   of both cases is syntax error recovery.  That is, a syntax error
   occurs when the lookahead is syntactically erroneous for the
   currently established initial context, so error recovery manipulates
   the parser stacks to try to find a new initial context in which the
   current lookahead is syntactically acceptable.  If it fails to find
   such a context, it discards the lookahead.  */
#if YYDEBUG
# define YY_LAC_DISCARD(Event)                                           \
do {                                                                     \
  if (yy_lac_established)                                                \
    {                                                                    \
      YYDPRINTF ((stderr, "LAC: initial context discarded due to "       \
                  Event "\n"));                                          \
      yy_lac_established = 0;                                            \
    }                                                                    \
} while (0)
#else
# define YY_LAC_DISCARD(Event) yy_lac_established = 0
#endif

/* Given the stack whose top is *YYSSP, return 0 iff YYTOKEN can
   eventually (after perhaps some reductions) be shifted, return 1 if
   not, or return YYENOMEM if memory is exhausted.  As preconditions and
   postconditions: *YYES_CAPACITY is the allocated size of the array to
   which *YYES points, and either *YYES = YYESA or *YYES points to an
   array allocated with YYSTACK_ALLOC.  yy_lac may overwrite the
   contents of either array, alter *YYES and *YYES_CAPACITY, and free
   any old *YYES other than YYESA.  */
static int
yy_lac (yy_state_t *yyesa, yy_state_t **yyes,
        YYPTRDIFF_T *yyes_capacity, yy_state_t *yyssp, yysymbol_kind_t yytoken)
{
  yy_state_t *yyes_prev = yyssp;
  yy_state_t *yyesp = yyes_prev;
  /* Reduce until we encounter a shift and thereby accept the token.  */
  YYDPRINTF ((stderr, "LAC: checking lookahead %s:", yysymbol_name (yytoken)));
  if (yytoken == YYSYMBOL_YYUNDEF)
    {
      YYDPRINTF ((stderr, " Always Err\n"));
      return 1;
    }
  while (1)
    {
      int yyrule = yypact[+*yyesp];
      if (yypact_value_is_default (yyrule)
          || (yyrule += yytoken) < 0 || YYLAST < yyrule
          || yycheck[yyrule] != yytoken)
        {
          /* Use the default action.  */
          yyrule = yydefact[+*yyesp];
          if (yyrule == 0)
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
        }
      else
        {
          /* Use the action from yytable.  */
          yyrule = yytable[yyrule];
          if (yytable_value_is_error (yyrule))
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
          if (0 < yyrule)
            {
              YYDPRINTF ((stderr, " S%d\n", yyrule));
              return 0;
            }
          yyrule = -yyrule;
        }
      /* By now we know we have to simulate a reduce.  */
      YYDPRINTF ((stderr, " R%d", yyrule - 1));
      {
        /* Pop the corresponding number of values from the stack.  */
        YYPTRDIFF_T yylen = yyr2[yyrule];
        /* First pop from the LAC stack as many tokens as possible.  */
        if (yyesp != yyes_prev)
          {
            YYPTRDIFF_T yysize = yyesp - *yyes + 1;
            if (yylen < yysize)
              {
                yyesp -= yylen;
                yylen = 0;
              }
            else
              {
                yyesp = yyes_prev;
                yylen -= yysize;
              }
          }
        /* Only afterwards look at the main stack.  */
        if (yylen)
          yyesp = yyes_prev -= yylen;
      }
      /* Push the resulting state of the reduction.  */
      {
        yy_state_fast_t yystate;
        {
          const int yylhs = yyr1[yyrule] - YYNTOKENS;
          const int yyi = yypgoto[yylhs] + *yyesp;
          yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyesp
                     ? yytable[yyi]
                     : yydefgoto[yylhs]);
        }
        if (yyesp == yyes_prev)
          {
            yyesp = *yyes;
            YY_IGNORE_USELESS_CAST_BEGIN
            *yyesp = YY_CAST (yy_state_t, yystate);
            YY_IGNORE_USELESS_CAST_END
          }
        else
          {
            if (yy_lac_stack_realloc (yyes_capacity, 1,
#if YYDEBUG
                                      " (", ")",
#endif
                                      yyes, yyesa, &yyesp, yyes_prev))
              {
                YYDPRINTF ((stderr, "\n"));
                return YYENOMEM;
              }
            YY_IGNORE_USELESS_CAST_BEGIN
            *++yyesp = YY_CAST (yy_state_t, yystate);
            YY_IGNORE_USELESS_CAST_END
          }
        YYDPRINTF ((stderr, " G%d", yystate));
      }
    }
}

/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yy_state_t *yyesa;
  yy_state_t **yyes;
  YYPTRDIFF_T *yyes_capacity;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;

  int yyx;
  for (yyx = 0; yyx < YYNTOKENS; ++yyx)
    {
      yysymbol_kind_t yysym = YY_CAST (yysymbol_kind_t, yyx);
      if (yysym != YYSYMBOL_YYerror && yysym != YYSYMBOL_YYUNDEF)
        switch (yy_lac (yyctx->yyesa, yyctx->yyes, yyctx->yyes_capacity, yyctx->yyssp, yysym))
          {
          case YYENOMEM:
            return YYENOMEM;
          case 1:
            continue;
          default:
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
       In the first two cases, it might appear that the current syntax
       error should have been detected in the previous state when yy_lac
       was invoked.  However, at that time, there might have been a
       different syntax error that discarded a different initial context
       during error recovery, leaving behind the current lookahead.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      YYDPRINTF ((stderr, "Constructing syntax error message\n"));
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else if (yyn == 0)
        YYDPRINTF ((stderr, "No expected tokens.\n"));
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.  In order to see if a particular token T is a
   valid looakhead, invoke yy_lac (YYESA, YYES, YYES_CAPACITY, YYSSP, T).

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store or if
   yy_lac returned YYENOMEM.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    yy_state_t yyesa[20];
    yy_state_t *yyes = yyesa;
    YYPTRDIFF_T yyes_capacity = 20 < YYMAXDEPTH ? 20 : YYMAXDEPTH;

  /* Whether LAC context is established.  A Boolean.  */
  int yy_lac_established = 0;
  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    {
      YY_LAC_ESTABLISH;
      goto yydefault;
    }
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      YY_LAC_ESTABLISH;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  YY_LAC_DISCARD ("shift");
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  {
    int yychar_backup = yychar;
    switch (yyn)
      {
  case 2: /* program: func_def  */
#line 171 "parser.y"
             { std::cout<< "AST: " << *(yyvsp[0].fdf) <<std::endl;  }
#line 1863 "parser.cpp"
    break;

  case 3: /* func_def: "def" header local_def_list stmt_list auto_end  */
#line 175 "parser.y"
                                                   { (yyval.fdf) = new FuncDefBlock((yyvsp[-3].hd),(yyvsp[-2].ldl),(yyvsp[-1].st_list)); }
#line 1869 "parser.cpp"
    break;

  case 4: /* func_decl: "decl" header  */
#line 179 "parser.y"
                  { (yyval.fdcl) = new FuncDecl((yyvsp[0].hd));}
#line 1875 "parser.cpp"
    break;

  case 5: /* header: T_id "is" type ':' opt_fpar  */
#line 183 "parser.y"
                                { (yyval.hd) = new Header(new Id((yyvsp[-4].str)),(yyvsp[-2].tp),(yyvsp[0].optfp)); }
#line 1881 "parser.cpp"
    break;

  case 6: /* header: T_id "is" type  */
#line 184 "parser.y"
                   { (yyval.hd) = new Header(new Id((yyvsp[-2].str)),(yyvsp[0].tp)); }
#line 1887 "parser.cpp"
    break;

  case 7: /* header: T_id ':' opt_fpar  */
#line 185 "parser.y"
                      { (yyval.hd) = new Header(new Id((yyvsp[-2].str)),nullptr,(yyvsp[0].optfp));}
#line 1893 "parser.cpp"
    break;

  case 8: /* header: T_id  */
#line 186 "parser.y"
         { (yyval.hd) = new Header(new Id((yyvsp[0].str))); }
#line 1899 "parser.cpp"
    break;

  case 9: /* type: type '[' T_number ']'  */
#line 190 "parser.y"
                          { (yyvsp[-3].tp)->add_dim((yyvsp[-1].num)); (yyval.tp) = (yyvsp[-3].tp); }
#line 1905 "parser.cpp"
    break;

  case 10: /* type: data_type  */
#line 191 "parser.y"
              { (yyval.tp) = new Type((yyvsp[0].type)); }
#line 1911 "parser.cpp"
    break;

  case 11: /* data_type: "int"  */
#line 195 "parser.y"
           { (yyval.type) = TYPE_int;}
#line 1917 "parser.cpp"
    break;

  case 12: /* data_type: "byte"  */
#line 196 "parser.y"
           { (yyval.type) = TYPE_byte;}
#line 1923 "parser.cpp"
    break;

  case 13: /* opt_fpar: id_list "as" ref_data_type  */
#line 200 "parser.y"
                               { (yyval.optfp) = new Opt_Fpar((yyvsp[-2].idlst),(yyvsp[0].type)); }
#line 1929 "parser.cpp"
    break;

  case 14: /* opt_fpar: id_list "as" ref_data_type ',' opt_fpar  */
#line 201 "parser.y"
                                            { (yyval.optfp) = new Opt_Fpar((yyvsp[-4].idlst),(yyvsp[-2].type),(yyvsp[0].optfp)); }
#line 1935 "parser.cpp"
    break;

  case 15: /* opt_fpar: id_list "as" fpar_type  */
#line 202 "parser.y"
                           {  (yyval.optfp) = new Opt_Fpar((yyvsp[-2].idlst),(yyvsp[0].fpt)); }
#line 1941 "parser.cpp"
    break;

  case 16: /* opt_fpar: id_list "as" fpar_type ',' opt_fpar  */
#line 203 "parser.y"
                                        {(yyval.optfp) = new Opt_Fpar((yyvsp[-4].idlst),(yyvsp[-2].fpt),(yyvsp[0].optfp)); }
#line 1947 "parser.cpp"
    break;

  case 17: /* ref_data_type: "ref" "int"  */
#line 207 "parser.y"
                { (yyval.type) = REF_int; }
#line 1953 "parser.cpp"
    break;

  case 18: /* ref_data_type: "ref" "byte"  */
#line 208 "parser.y"
                 { (yyval.type) = REF_byte;}
#line 1959 "parser.cpp"
    break;

  case 19: /* fpar_type: "int"  */
#line 212 "parser.y"
          { (yyval.fpt) = new FparType(TYPE_int); }
#line 1965 "parser.cpp"
    break;

  case 20: /* fpar_type: "byte"  */
#line 213 "parser.y"
           { (yyval.fpt) = new FparType(TYPE_byte); }
#line 1971 "parser.cpp"
    break;

  case 21: /* fpar_type: array_type  */
#line 214 "parser.y"
               { (yyval.fpt) = (yyvsp[0].fpt); }
#line 1977 "parser.cpp"
    break;

  case 22: /* array_type: "int" '[' ']'  */
#line 218 "parser.y"
                  { (yyval.fpt) = new ArrayType(TYPE_int,0); }
#line 1983 "parser.cpp"
    break;

  case 23: /* array_type: "byte" '[' ']'  */
#line 219 "parser.y"
                   { (yyval.fpt) = new ArrayType(TYPE_byte,0); }
#line 1989 "parser.cpp"
    break;

  case 24: /* array_type: "int" '[' T_number ']'  */
#line 220 "parser.y"
                           { (yyval.fpt) = new ArrayType(TYPE_int, (yyvsp[-1].num)); }
#line 1995 "parser.cpp"
    break;

  case 25: /* array_type: "byte" '[' T_number ']'  */
#line 221 "parser.y"
                            { (yyval.fpt) = new ArrayType(TYPE_byte, (yyvsp[-1].num)); }
#line 2001 "parser.cpp"
    break;

  case 26: /* array_type: array_type '[' T_number ']'  */
#line 222 "parser.y"
                                { (yyvsp[-3].fpt)->add_dim((yyvsp[-1].num)) ; (yyval.fpt)=(yyvsp[-3].fpt); }
#line 2007 "parser.cpp"
    break;

  case 27: /* id_list: T_id  */
#line 226 "parser.y"
           { (yyval.idlst) = new Id_List(*(yyvsp[0].str)); }
#line 2013 "parser.cpp"
    break;

  case 28: /* id_list: id_list T_id  */
#line 227 "parser.y"
                   { (yyvsp[-1].idlst)->append(*(yyvsp[0].str)); (yyval.idlst) = (yyvsp[-1].idlst); }
#line 2019 "parser.cpp"
    break;

  case 29: /* stmt_list: stmt  */
#line 231 "parser.y"
           { (yyval.st_list) = new StmtList((yyvsp[0].st)); }
#line 2025 "parser.cpp"
    break;

  case 30: /* stmt_list: stmt_list stmt  */
#line 232 "parser.y"
                     { (yyvsp[-1].st_list)->append((yyvsp[0].st)); (yyval.st_list) = (yyvsp[-1].st_list); }
#line 2031 "parser.cpp"
    break;

  case 31: /* local_def_list: %empty  */
#line 236 "parser.y"
                                     { (yyval.ldl) = new LocalDefList();}
#line 2037 "parser.cpp"
    break;

  case 32: /* local_def_list: local_def_list local_def  */
#line 237 "parser.y"
                                   { (yyvsp[-1].ldl)->append((yyvsp[0].lcldef)); (yyval.ldl)=(yyvsp[-1].ldl); }
#line 2043 "parser.cpp"
    break;

  case 33: /* local_def: func_def  */
#line 241 "parser.y"
                {(yyval.lcldef) = new LocalDef((yyvsp[0].fdf)); }
#line 2049 "parser.cpp"
    break;

  case 34: /* local_def: func_decl  */
#line 242 "parser.y"
                {(yyval.lcldef) = new LocalDef((yyvsp[0].fdcl)); }
#line 2055 "parser.cpp"
    break;

  case 35: /* local_def: "var" id_list "is" type  */
#line 243 "parser.y"
                            { (yyval.lcldef) = new LocalDef((yyvsp[-2].idlst),(yyvsp[0].tp)); }
#line 2061 "parser.cpp"
    break;

  case 36: /* stmt: "skip"  */
#line 247 "parser.y"
                                                    { (yyval.st) = new SkipStmt(); }
#line 2067 "parser.cpp"
    break;

  case 37: /* stmt: l_value ":=" expr  */
#line 248 "parser.y"
                                                    { (yyval.st) = new LvalueStmt((yyvsp[-2].lval),(yyvsp[0].expr)); }
#line 2073 "parser.cpp"
    break;

  case 38: /* stmt: func_call  */
#line 249 "parser.y"
                                                    { (yyval.st) = new FuncStmt((yyvsp[0].fcall)); }
#line 2079 "parser.cpp"
    break;

  case 39: /* stmt: proc_call  */
#line 250 "parser.y"
                                                    { (yyval.st) = new ProcStmt((yyvsp[0].pcall)); }
#line 2085 "parser.cpp"
    break;

  case 40: /* stmt: "exit"  */
#line 251 "parser.y"
                                                    { (yyval.st) = new ExitStmt();}
#line 2091 "parser.cpp"
    break;

  case 41: /* stmt: "return" ':' expr  */
#line 252 "parser.y"
                                                    { (yyval.st) = new ReturnStmt((yyvsp[0].expr)); }
#line 2097 "parser.cpp"
    break;

  case 42: /* stmt: if_stmts  */
#line 253 "parser.y"
                                                    { (yyval.st) = (yyvsp[0].ifblck); }
#line 2103 "parser.cpp"
    break;

  case 43: /* stmt: loop  */
#line 254 "parser.y"
                                                    { (yyval.st) = (yyvsp[0].lb); }
#line 2109 "parser.cpp"
    break;

  case 44: /* stmt: "break"  */
#line 255 "parser.y"
                                                    { (yyval.st) = new BreakStmt();}
#line 2115 "parser.cpp"
    break;

  case 45: /* stmt: "break" ':' T_id  */
#line 256 "parser.y"
                                                    { (yyval.st) = new BreakStmt((yyvsp[0].str));}
#line 2121 "parser.cpp"
    break;

  case 46: /* stmt: "continue"  */
#line 257 "parser.y"
                                                    { (yyval.st) = new ContStmt();}
#line 2127 "parser.cpp"
    break;

  case 47: /* stmt: "continue" ':' T_id  */
#line 258 "parser.y"
                                                    { (yyval.st) = new ContStmt((yyvsp[0].str));}
#line 2133 "parser.cpp"
    break;

  case 48: /* if_stmts: "if" cond ':' stmt_list auto_end opt_elif_else  */
#line 262 "parser.y"
                                                  {
        (yyval.ifblck) = new IfBlock((yyvsp[-4].cnd),(yyvsp[-2].st_list),(yyvsp[0].oee));
}
#line 2141 "parser.cpp"
    break;

  case 49: /* opt_elif_else: %empty  */
#line 274 "parser.y"
                  { (yyval.oee) = nullptr;}
#line 2147 "parser.cpp"
    break;

  case 50: /* opt_elif_else: "else" ':' stmt_list auto_end  */
#line 275 "parser.y"
                                    { (yyval.oee) = new Opt_Elif_Else((yyvsp[-1].st_list)); }
#line 2153 "parser.cpp"
    break;

  case 51: /* opt_elif_else: "elif" cond ':' stmt_list auto_end opt_elif_else  */
#line 276 "parser.y"
                                                       { (yyval.oee) = new Opt_Elif_Else((yyvsp[-4].cnd),(yyvsp[-2].st_list),(yyvsp[0].oee)); }
#line 2159 "parser.cpp"
    break;

  case 52: /* l_value: T_id  */
#line 281 "parser.y"
               { (yyval.lval) = new Lvalue (new Id((yyvsp[0].str))); }
#line 2165 "parser.cpp"
    break;

  case 53: /* l_value: T_string  */
#line 282 "parser.y"
               { (yyval.lval) = new Lvalue((yyvsp[0].str)); }
#line 2171 "parser.cpp"
    break;

  case 54: /* l_value: l_value '[' expr ']'  */
#line 283 "parser.y"
                           { (yyvsp[-3].lval)->append((yyvsp[-1].expr)); (yyval.lval)=(yyvsp[-3].lval); }
#line 2177 "parser.cpp"
    break;

  case 55: /* loop: "loop" T_id ':' stmt_list auto_end  */
#line 287 "parser.y"
                                       { (yyval.lb) = new LoopBlock((yyvsp[-3].str),(yyvsp[-1].st_list));}
#line 2183 "parser.cpp"
    break;

  case 56: /* loop: "loop" ':' stmt_list auto_end  */
#line 288 "parser.y"
                                  { (yyval.lb) = new LoopBlock((yyvsp[-1].st_list)); }
#line 2189 "parser.cpp"
    break;

  case 57: /* proc_call: T_id  */
#line 292 "parser.y"
                                { (yyval.pcall) = new Proc_Call(new Id((yyvsp[0].str)));}
#line 2195 "parser.cpp"
    break;

  case 58: /* proc_call: T_id ':' expr_list  */
#line 293 "parser.y"
                                { (yyval.pcall) = new Proc_Call(new Id((yyvsp[-2].str)),(yyvsp[0].exlst));}
#line 2201 "parser.cpp"
    break;

  case 59: /* func_call: T_id '(' ')'  */
#line 298 "parser.y"
                             { (yyval.fcall) = new Func_Call(new Id((yyvsp[-2].str)));}
#line 2207 "parser.cpp"
    break;

  case 60: /* func_call: T_id '(' expr_list ')'  */
#line 299 "parser.y"
                             { (yyval.fcall) = new Func_Call(new Id((yyvsp[-3].str)),(yyvsp[-1].exlst));}
#line 2213 "parser.cpp"
    break;

  case 61: /* expr: T_number  */
#line 303 "parser.y"
                              { (yyval.expr) = new Number((yyvsp[0].num)); }
#line 2219 "parser.cpp"
    break;

  case 62: /* expr: T_const  */
#line 304 "parser.y"
                              { (yyval.expr) = new Const((yyvsp[0].str)); }
#line 2225 "parser.cpp"
    break;

  case 63: /* expr: l_value  */
#line 305 "parser.y"
                              { (yyval.expr) = (yyvsp[0].lval); }
#line 2231 "parser.cpp"
    break;

  case 64: /* expr: '(' expr ')'  */
#line 306 "parser.y"
                              { (yyval.expr) = (yyvsp[-1].expr); }
#line 2237 "parser.cpp"
    break;

  case 65: /* expr: func_call  */
#line 307 "parser.y"
                              { (yyval.expr) = (yyvsp[0].fcall); }
#line 2243 "parser.cpp"
    break;

  case 66: /* expr: '+' expr  */
#line 308 "parser.y"
                              { (yyval.expr) = new UniOp('+', (yyvsp[0].expr)); }
#line 2249 "parser.cpp"
    break;

  case 67: /* expr: '-' expr  */
#line 309 "parser.y"
                              { (yyval.expr) = new UniOp('-', (yyvsp[0].expr)); }
#line 2255 "parser.cpp"
    break;

  case 68: /* expr: expr '+' expr  */
#line 310 "parser.y"
                              { (yyval.expr) = new BinOp((yyvsp[-2].expr),'+', (yyvsp[0].expr)); }
#line 2261 "parser.cpp"
    break;

  case 69: /* expr: expr '-' expr  */
#line 311 "parser.y"
                              { (yyval.expr) = new BinOp((yyvsp[-2].expr),'-', (yyvsp[0].expr)); }
#line 2267 "parser.cpp"
    break;

  case 70: /* expr: expr '*' expr  */
#line 312 "parser.y"
                              { (yyval.expr) = new BinOp((yyvsp[-2].expr),'*', (yyvsp[0].expr)); }
#line 2273 "parser.cpp"
    break;

  case 71: /* expr: expr '/' expr  */
#line 313 "parser.y"
                              { (yyval.expr) = new BinOp((yyvsp[-2].expr),'/', (yyvsp[0].expr)); }
#line 2279 "parser.cpp"
    break;

  case 72: /* expr: expr '%' expr  */
#line 314 "parser.y"
                              { (yyval.expr) = new BinOp((yyvsp[-2].expr),'%', (yyvsp[0].expr)); }
#line 2285 "parser.cpp"
    break;

  case 73: /* expr: "true"  */
#line 315 "parser.y"
                              { (yyval.expr) = new Bool(true); }
#line 2291 "parser.cpp"
    break;

  case 74: /* expr: "false"  */
#line 316 "parser.y"
                              { (yyval.expr) = new Bool(false); }
#line 2297 "parser.cpp"
    break;

  case 75: /* expr: '!' expr  */
#line 317 "parser.y"
                              { (yyval.expr) = new NotExpr((yyvsp[0].expr)); }
#line 2303 "parser.cpp"
    break;

  case 76: /* expr: expr '&' expr  */
#line 318 "parser.y"
                              { (yyval.expr) = new BoolBinOp((yyvsp[-2].expr), '&', (yyvsp[0].expr)); }
#line 2309 "parser.cpp"
    break;

  case 77: /* expr: expr '|' expr  */
#line 319 "parser.y"
                              { (yyval.expr) = new BoolBinOp((yyvsp[-2].expr), '|', (yyvsp[0].expr)); }
#line 2315 "parser.cpp"
    break;

  case 78: /* cond: expr '>' expr  */
#line 323 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].expr),">",(yyvsp[0].expr)); }
#line 2321 "parser.cpp"
    break;

  case 79: /* cond: expr '<' expr  */
#line 324 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].expr),"<",(yyvsp[0].expr)); }
#line 2327 "parser.cpp"
    break;

  case 80: /* cond: expr ">=" expr  */
#line 325 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].expr),">=",(yyvsp[0].expr)); }
#line 2333 "parser.cpp"
    break;

  case 81: /* cond: expr "<=" expr  */
#line 326 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].expr),"<=",(yyvsp[0].expr)); }
#line 2339 "parser.cpp"
    break;

  case 82: /* cond: expr '=' expr  */
#line 327 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].expr),"=",(yyvsp[0].expr)); }
#line 2345 "parser.cpp"
    break;

  case 83: /* cond: expr "<>" expr  */
#line 328 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].expr),"<>",(yyvsp[0].expr)); }
#line 2351 "parser.cpp"
    break;

  case 84: /* cond: cond "and" cond  */
#line 329 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].cnd),"and",(yyvsp[0].cnd)); }
#line 2357 "parser.cpp"
    break;

  case 85: /* cond: cond "or" cond  */
#line 330 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[-2].cnd),"or",(yyvsp[0].cnd)); }
#line 2363 "parser.cpp"
    break;

  case 86: /* cond: "not" cond  */
#line 331 "parser.y"
                                  { (yyval.cnd) = new NotCond((yyvsp[0].cnd)); }
#line 2369 "parser.cpp"
    break;

  case 87: /* cond: '(' cond ')'  */
#line 332 "parser.y"
                                  { (yyval.cnd) = (yyvsp[-1].cnd);}
#line 2375 "parser.cpp"
    break;

  case 88: /* cond: expr  */
#line 333 "parser.y"
                                  { (yyval.cnd) = new Cond((yyvsp[0].expr)) ;}
#line 2381 "parser.cpp"
    break;

  case 89: /* expr_list: expr  */
#line 337 "parser.y"
         { (yyval.exlst) = new ExprList((yyvsp[0].expr)); }
#line 2387 "parser.cpp"
    break;

  case 90: /* expr_list: expr_list ',' expr  */
#line 338 "parser.y"
                       { (yyvsp[-2].exlst) ->append((yyvsp[0].expr));  (yyval.exlst) = (yyvsp[-2].exlst); }
#line 2393 "parser.cpp"
    break;


#line 2397 "parser.cpp"

        default: break;
      }
    if (yychar_backup != yychar)
      YY_LAC_DISCARD ("yychar change");
  }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yyesa, &yyes, &yyes_capacity, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        if (yychar != YYEMPTY)
          YY_LAC_ESTABLISH;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  /* If the stack popping above didn't lose the initial context for the
     current lookahead token, the shift below will for sure.  */
  YY_LAC_DISCARD ("error recovery");

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yyes != yyesa)
    YYSTACK_FREE (yyes);
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 341 "parser.y"


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
