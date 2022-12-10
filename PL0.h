#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "evl.h"
#include "set.h"

#define OPTM    1
#define OPTM_CY 0
#define STEP       1
#define MAXDIMLEN  10
#define NRW        25     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       13     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers
#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array
#define MAXINS   10

#define MAXSYM     30     // maximum number of symbols  
#define STACKSIZE  1000   // maximum storage
symset phi, decleration_begin_sys, statement_begin_sys, factor_begin_sys, rel_set;
enum symtype {
    SYM_NULL,
    SYM_IDENTIFIER,
    SYM_NUMBER,
    SYM_PLUS,
    SYM_MINUS,
    SYM_TIMES,
    SYM_SLASH,
    SYM_ODD,
    SYM_EQU,
    SYM_NEQ,
    SYM_LES,
    SYM_LEQ,
    SYM_GTR,
    SYM_GEQ,
    SYM_LPAREN,
    SYM_RPAREN,
    SYM_COMMA,
    SYM_SEMICOLON,
    SYM_PERIOD,
    SYM_BECOMES,
    SYM_BEGIN,
    SYM_END,
    SYM_IF,
    SYM_THEN,
    SYM_WHILE,
    SYM_DO,
    SYM_CALL,
    SYM_CONST,
    SYM_VAR,
    SYM_PROCEDURE,
    SYM_WRITE,
    SYM_READ,
    SYM_FOR,
    SYM_DOWNTO,
    SYM_TO,
    SYM_ELSE,
    SYM_WRITELN,
    SYM_REPEAT,
    SYM_UNTIL,
    SYM_EXIT,
    SYM_AND,
    SYM_OR,
    SYM_NOT,
    SYM_LSQUARE,
    SYM_RSQUARE,
    SYM_BREAK,
    SYM_ADDEQU,
    SYM_SUBEQU,
    SYM_ADDADD,
    SYM_SUBSUB,
    SYM_DIVEQU,
    SYM_MULEQU,
    SYM_MODEQU
};

enum idtype {
    ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE, ID_ARRAY
};

enum opcode {
    LIT, OPR, LOD, STO, CAL, INT, JMP, JPC, STA, LAD
};
int optime[100] =
        {
                1, 0, 2, 2, 1, 1, 1, 1, 2, 2
        };

enum oprcode {
    OPR_RET,
    OPR_NEG,
    OPR_ADD,
    OPR_MIN,
    OPR_MUL,
    OPR_DIV,
    OPR_ODD,
    OPR_EQU,
    OPR_NEQ,
    OPR_LES,
    OPR_LEQ,
    OPR_GTR,
    OPR_GEQ,
    OPR_WTL,
    OPR_PRT,
    OPR_RED,
    OPR_AND,
    OPR_OR,
    OPR_NOT,
    OPR_MOD
};

int oprtime[100] =
        {
                1, 1, 1, 1,
                5, 10, 1, 1,
                1, 1, 1, 1,
                1, 3, 3, 3,
                1, 1, 1
        };
typedef struct {
    int f; // function code
    int l; // level
    int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char *err_msg[] =
        {
                /*  0 */"",
                /*  1 */"Found ':=' when expecting '='.",
                /*  2 */"There must be a number to follow '='.",
                /*  3 */"There must be an '=' to follow the identifier.",
                /*  4 */"There must be an identifier to follow 'for', 'const', 'var', or 'procedure'.",
                /*  5 */"Missing ',' or ';'.",
                /*  6 */"Incorrect procedure name.",
                /*  7 */"Statement expected.",
                /*  8 */"Follow the statement is an incorrect symbol.",
                /*  9 */"'.' expected.",
                /* 10 */"';' expected.",
                /* 11 */"Undeclared identifier.",
                /* 12 */"Illegal assignment.",
                /* 13 */"':=' '+=','-=','*=','/=','%='expected.",
                /* 14 */"There must be an identifier to follow the 'call'.",
                /* 15 */"A constant or variable can not be called.",
                /* 16 */"'then' expected.",
                /* 17 */"';' or 'end' expected.",
                /* 18 */"'do' expected.",
                /* 19 */"Incorrect symbol.",
                /* 20 */"Relative operators expected.",
                /* 21 */"Procedure identifier can not be in an expression.",
                /* 22 */"Missing ')'.",
                /* 23 */"The symbol can not be followed by a factor.",
                /* 24 */"The symbol can not be as the beginning of an expression.",
                /* 25 */"The number is too great.",
                /* 26 */"Dimension of the array is not correct.",
                /* 27 */"']' expected.",
                /* 28 */"'[' expected.",
                /* 29 */"the symbol can not be as the beginning of the condition",
                /* 30 */"'to' or 'downto' expected.",
                /* 31 */"missing until",
                /* 32 */"There are too many levels.",
                /* 33 */"')' expected.",
                /* 34 */"The number of segments doesn't match.",
                /* 35 */"'break' is not the suitable place.",
                /* 36 */"Declaration is overlap"
                        /* 37 */"Dimension of the array must be determined."};

//////////////////////////////////////////////////////////////////////
char last_char_read; // last character read
int last_sym_read; // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
int last_num_read; // last number read
int char_count; // character count
int line_length; // line length
int kk;
int err;
int current_instruction_index; // index of current instruction to be generated.
int level = 0;
int loop_level = 0;
int block_level = 0;
int block_num = 1;
int block_level_count[MAXLEVEL]; //block_level_count
int tx = 0;
int tx_[100]; //每递归调用一次block开始时table的tx位置
int dim = 0;
int array_size = 1;
int latit[MAXDIMLEN];
int proth[100]; //cy_quote
typedef struct cxlink {
    int break_code_index;
    struct cxlink *next;
} *break_code_index_list;

typedef struct procedure_link {
    int table_adr;
    int start;
    int end;
    struct procedure_list *next;
} procedure_list;

typedef struct { //cy
    int flag;
    int sign;
    break_code_index_list then;
} break_code_block; //存放break代码地址

break_code_block break_code_index;
char line[80];

instruction code[CXMAX];

char *word[NRW + 1] = {"", /* place holder */
                       "begin", "call", "const", "do", "end", "if", //6
                       "odd", "procedure", "then", "var", "while", //11
                       "write", "read", "for", "downto", "to", "else", //17
                       "writeln", "repeat", "until", "exit", "and", //22
                       "or", "not", "break"
};

int wsym[NRW + 1] = {SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
                     SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE, SYM_WRITE,
                     SYM_READ, SYM_FOR, SYM_DOWNTO, SYM_TO, SYM_ELSE, SYM_WRITELN,
                     SYM_REPEAT, SYM_UNTIL, SYM_EXIT, SYM_AND, SYM_OR, SYM_NOT, SYM_BREAK};

int ssym[NSYM + 1] = {SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
                      SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON,
                      SYM_LSQUARE, SYM_RSQUARE, SYM_MODEQU};

char csym[NSYM + 1] = {' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';',
                       '[', ']', '%'};

char *mnemonic[MAXINS] = {"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP",
                          "JPC", "STA", "LAD"};

typedef struct dim {
    int dim_len;
    struct dim *next;
} p_dim;

typedef struct {
    char name[MAXIDLEN + 1];
    int kind;
    int value;
    int numOfPar;
    int quote; //是否引用过;0未引用;1引用过
    p_dim *next;
    evl evl;
    int cnt;
    int lpl;//loop_level
    int blkNum;
} var_obj;

var_obj table[TXMAX];

typedef struct {
    char name[MAXIDLEN + 1];
    int kind;
    short level;
    short address;
    int numOfPar;
    int quote; //是否引用过;0未引用;1引用过
    p_dim *next;
    evl evl;
    int cnt;
    int lpl;//loop_level
    int blkNum;
} mask;

typedef struct {
    char name[MAXIDLEN + 1];
    short kind;
    short dim_n;
    short level;
    short address;
    int numOfPar;
    int quote; //是否引用过;0未引用;1引用过
    p_dim *next;
    evl evl;
    int cnt;
    int lpl;//loop_level
    int blkNum;
} array;

FILE *infile;
int jdgok(int curBlkNum, int saveBlkNum);
void print_table();
void in_block();
void out_block(int saveBlkNum, int saveBlkLvl);
void error(int n);
void getch(void);
void get_next_symbol(void);
void gen_instruction(int x, int y, int z);
void enter_obj_2_table(int kind);
void test(symset s1, symset s2, int n);
void enter_par();
void modify_table(int numOfPar);
void print_table();
int position(char *id);
void const_declaration(symset fsys);
void var_declaration(void);
void list_code(int from, int to);
int const_factor(symset fsys);
int const_term(symset fsys);
int const_expression(symset fsys);
void dim_declaration(void);
void optimize(mask *mk, int saveCx);
void optimize_term(mask *mk, int saveCx);
mask *factor(symset fsys);
mask *term(symset fsys);
mask *expression(symset fsys);
void condition_factor(symset fsys);
void condition_term(symset fsys);
void condition(symset fsys);
void statement(symset fsys);
int get_array_size(int i);
int search_var(int len, int from);
int search_pro(int n);
void cut_code(int start, int end);
int locate(int n, int star_tx, int end_tx, int f);
void cut_pro_var_code(int star_cx, int end_cx, int star_tx, int end_tx);
void block(symset fsys);
int get_base_addr(int stack[], int currentLevel, int levelDiff);
void interpret();
// EOF PL0.h
