// UTILITIES HEADER
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "parser.h"

#define TABLE_SIZE 20

extern int yylineno;
extern char* yytext;
extern FILE* yyin;

//-------------LEX STRUCTS-----------------------

struct comment_stack_node{
    unsigned int start_line;
    struct comment_stack_node* next;
};

struct error_stack_node{
    char * text;
    struct error_stack_node* next;
};

//-------------LEX GLOBAL VARS-------------------

unsigned int comment_counter;

struct comment_stack_node* comment_stack;
struct error_stack_node* error_stack;

int tmp_int;

char* buffer;
char* error_buffer;

//-------------LEX FUNCTION DEFS-----------------

void init_lex();
void show_lex_errors();

void Push_to_comment_stack(int line);
int Pop_from_comment_stack();

void Push_to_error_stack(char* text);
char* Pop_from_error_stack();

char* filter_string(char* input_string);

void Add_to_buffer();
void Clear_buffer();

//-------------YACC STRUCTS----------------------

enum SymbolType {
    GLOBAL, 
    LOCAL, 
    FORMAL,
    USERFUNC, 
    LIBFUNC
};

enum scopespace_t{
    programvar,
    functionlocal,
    formalarg
};

typedef struct Variable {
    char *name;
    unsigned int scope;
    unsigned int line;
} Variable;

typedef struct Function {
    char *name;
    unsigned int scope;
    unsigned int line;
    unsigned int localVars;
    unsigned int startingQuad;
    unsigned int taddress;
} Function;

typedef struct SymbolTableEntry {
    bool isActive;
    union {
        Variable *varVal;
        Function *funcVal;
    } value;
    enum SymbolType type;
    enum scopespace_t space;
    unsigned int offset;
    struct SymbolTableEntry *next;
} uSymbolTableEntry;

typedef struct ScopeNodeValue {
    uSymbolTableEntry *value;
    struct ScopeNodeValue *next;
} ScopeNodeValue;

typedef struct ScopeNode {
    unsigned int scope;
    ScopeNodeValue *value;
    struct ScopeNode *next;
} ScopeNode;

typedef struct function_scope_stack{
    unsigned int scope;
    struct function_scope_stack* next;
} functionScopeStack;

//-------------YACC GLOBAL VARS------------------

ScopeNode *scope_list;
uSymbolTableEntry* SymbolTable[TABLE_SIZE];
struct function_scope_stack* function_scope_stack;

unsigned int current_scope;
unsigned int function_scope;
unsigned int unnamed_function;
unsigned int loop_counter;

//-------------YACC FUNCTION DEFS----------------

void init_bison();

int yyerror(char* Message);

void push_function_scope_stack(int scope);
int pop_function_scope_stack();
bool function_stack_is_empty();

void insert_to_symbol_table(char* name, enum SymbolType type);
void insert_to_scope_list(uSymbolTableEntry* entry);

uSymbolTableEntry* lookup(char* name, char* lookup_mode);

void hide(int scope);

unsigned int hash_function(char* string);

bool check_in_libfunc(char* name);

void print_scopelist();

char* concat_si(const char* s1, unsigned int s2);

char* concat_ss(const char* s1, const char* s2);

char* strup(char* s1);

//-------------QUAD STRUCTS----------------------
enum iopcode_t{
    assign,
    add,
    sub,
    mul,
    div_e,
    mod,
    uminus,
    if_eq,
    if_noteq,
    if_lesseq,
    if_greatereq,
    if_less,
    if_greater,
    call,
    param,
    ret,
    getretval,
    funcstart,
    funcend,
    tablecreate,
    tablegetelem,
    tablesetelem,
    jump,
    and,
    or,
    not
};

enum expr_t{
    var_e,
    tableitem_e,

    programfunc_e,
    libraryfunc_e,

    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,

    constint_e,
    constfloat_e,
    constbool_e,
    conststring_e,

    nil_e
};

struct expr{
    enum expr_t type;
    uSymbolTableEntry *sym;
    struct expr *index;
    float numConst;
    char *strConst;
    unsigned int boolConst;
    struct expr *next;
};

struct quad{
    enum iopcode_t op;
    struct expr *result;
    struct expr *arg1;
    struct expr *arg2;
    unsigned int taddress;
    unsigned int label;
    unsigned int line;
};

struct offset_node{
    int offset;
    struct offset_node *next;
};

struct for_node{
    int firstQuad;
    int quad1;
    int quad2;
};

struct breaknode{
    unsigned int quad;
    unsigned int loopNo;
    struct breaknode *next;
};

struct continuenode{
    unsigned int quad;
    unsigned int loopNo;
    struct continuenode *next;
};

struct returnnode{
    unsigned int quad;
    unsigned int scope;
    struct returnnode *next;
};

struct call_node{
    int normalCall;
    struct expr *elist;
    struct expr *name;
};

//-------------QUAD GLOBAL VARS------------------

#define EXPAND_SIZE 256
#define CURR_SIZE (max_quads * sizeof(struct quad))
#define NEW_SIZE ((EXPAND_SIZE * (sizeof(struct quad))) + CURR_SIZE)

unsigned int max_quads;
unsigned int quad_index;
struct quad *quads;

unsigned int temp_counter;
unsigned int scope_space_counter;

unsigned int programm_var_offset;
unsigned int function_local_offset;
unsigned int formal_argument_offset;

uSymbolTableEntry *temp_symbol;

struct expr *temp_expr;
struct expr *true_temp_expr;
struct expr *false_temp_expr;
struct expr *number_one_expr;
struct expr *number_minus_one_expr;

struct offset_node *offset_stack;

struct breaknode *break_stack;
struct continuenode *continue_stack;
struct returnnode *return_stack;

int succeeded;

//-------------QUAD FUNCTION DEFS----------------

void init_quads();

enum scopespace_t current_scopespace();

void expand_table();
void emmit(enum iopcode_t iopcode, struct expr *result, struct expr *arg1, struct expr *arg2, unsigned int label, unsigned int line);
int check_expr(enum expr_t type, struct expr *first, struct expr *second);

char *new_temp_name();
uSymbolTableEntry *new_temp();
void reset_temps();

unsigned int current_scopespace_offset();
void inc_current_scopespace_offset();
void set_current_scopespace_offset(int offset);

struct expr *create_new_expression(enum expr_t type, uSymbolTableEntry *sym, struct expr *index, float numConst, char *strConst, unsigned int boolConst);

void push_to_offset_stack(int offset);
int pop_from_offset_stack();

void backpatch(int index, int label);

void add_to_breakstack(unsigned int quad);
void add_to_continuestack(unsigned int quad);
void add_to_returnstack(unsigned int scope);

void backpatch_break(unsigned int label);
void backpatch_continue(unsigned int label);
void backpatch_return(unsigned int label);

void add_function_info(uSymbolTableEntry *symbol, unsigned int StartingQuad, unsigned int totalLocalVars);

void param_creator(struct expr *elist);

struct call_node *create_call_node(int normalCall, struct expr *name, struct expr *elist);
void use_call_node(struct expr *lvalue, struct call_node *callNode);

void create_elist_table_elements(struct expr *table, struct expr *elist);
void create_indexed_table_elements(struct expr *table, struct expr *elist);

int is_temp_expr(struct expr *expr);

void create_quad_file();

void check_if_temp(struct expr *quad, struct expr *expr);

//-------------FINAL CODE STRUCTS----------------

enum vmarg_t{
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a,
    ignore_a
};

enum vmopcode{
    add_t,
    sub_t,
    mul_t,
    div_tc,
    mod_t,
    newtable,
    tablegetelem_t,
    tablesetelem_t,
    assign_t,
    jump_t,
    jeq,
    jne,
    jgt,
    jge,
    jlt,
    jle,
    pusharg,
    callfunc,
    enterfunc,
    exitfunc
};

struct incomplete_jump{
    unsigned int instrNo;
    unsigned int iaddress;
    struct incomplete_jump *next;
};

struct vmarg{
    enum vmarg_t type;
    unsigned int val;
};

struct instruction{
    enum vmopcode opcode;
    struct vmarg result;
    struct vmarg arg1;
    struct vmarg arg2;
    unsigned int srcLine;
};

struct const_string_list{
    unsigned int index;
    char* value;
    struct const_string_list *next;
};

struct const_number_list{
    unsigned int index;
    double value;
    struct const_number_list *next;  
};

struct userfunc_list{
    unsigned int index;
    uSymbolTableEntry *value;
    struct userfunc_list *next;
};

struct libfunc_list{
    unsigned int index;
    char* value;
    struct libfunc_list *next;
};

struct tcode_returnlist{
    unsigned int index;
    struct tcode_returnlist *next;
};

struct tcode_funcstack{
    struct tcode_returnlist *returnlist;
    struct tcode_funcstack *next;
}; 

//-------------FINAL CODE GLOBAL VARS------------

#define EXPAND_SIZE_IN 256
#define CURR_SIZE_IN (max_instructions * sizeof(struct instruction))
#define NEW_SIZE_IN ((EXPAND_SIZE_IN * (sizeof(struct instruction))) + CURR_SIZE_IN)

unsigned int max_instructions;
unsigned int instruction_index;
struct instruction *instructions;

struct tcode_funcstack *t_funcstack;

unsigned int global_generate_index;
unsigned int magic_number;

struct incomplete_jump *ij_head;
unsigned int ij_total;

struct const_string_list *string_list;
struct const_number_list *number_list;
struct userfunc_list *userfunc_list;
struct libfunc_list *libfunc_list;
unsigned int string_array_size;
unsigned int number_array_size;
unsigned int userfunc_array_size;
unsigned int libfunc_array_size;

FILE *tcode_txt_file;
FILE *tcode_abc_file;

//-------------FINAL CODE FUNCTION DEFS----------

void generate_ASSIGN(struct quad *quad);
void generate_ADD(struct quad *quad);
void generate_SUB(struct quad *quad);
void generate_MUL(struct quad *quad);
void generate_DIV(struct quad *quad);
void generate_MOD(struct quad *quad);
void generate_UMINUS(struct quad *quad);
void generate_IF_EQ(struct quad *quad);
void generate_IF_NOTEQ(struct quad *quad);
void generate_IF_LESSEQ(struct quad *quad);
void generate_IF_GREATEREQ(struct quad *quad);
void generate_IF_LESS(struct quad *quad);
void generate_IF_GREATER(struct quad *quad);
void generate_CALL(struct quad *quad);
void generate_PARAM(struct quad *quad);
void generate_RETURN(struct quad *quad);
void generate_GETRETVAL(struct quad *quad);
void generate_FUNCSTART(struct quad *quad);
void generate_FUNCEND(struct quad *quad);
void generate_NEWTABLE(struct quad *quad);
void generate_TABLEGETELEM(struct quad *quad);
void generate_TABLESETELEM(struct quad *quad);
void generate_JUMP(struct quad *quad);

void add_incomplete_jump(unsigned int instrNo, unsigned int iaddress);

void generate(enum vmopcode op, struct quad *quad);
void generate_relational(enum vmopcode op, struct quad *quad);
void generate_tcode(void);

unsigned int consts_newstring(char *s);
unsigned int consts_newnumber(double n);
unsigned int libfuncs_newused(char *s);
void add_userfunc(uSymbolTableEntry *symbol);

void make_operand(struct expr *e, struct vmarg *arg);
void make_numberoperand(struct vmarg *arg, double val);
void make_booloperand(struct vmarg* arg, unsigned int val);
void make_retvaloperand(struct vmarg *arg);

void emmit_instruction(struct instruction *instruction);

void patch_incomplete_jumps();

void expand_table_instructions();
void init_instructions();

void push_tcode_funcstack();
void append_tcode_returnlist(unsigned int index);
void backpatch_tcode_returnlist();

unsigned int nextinstructionlabel();

void create_tcode_txt_file();
void create_tcode_abc_file();
void print_register_b(struct vmarg arg);
void print_register(struct vmarg arg);

//-------------END OF HEADER FILE----------------