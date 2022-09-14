// AVM HEADER
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
#include <stdarg.h>
#include <math.h>

//-----------AVM STRUCTS-----------------

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
    retval_a
};

enum vmopcode{
    add_v,
    sub_v,
    mul_v,
    div_v,
    mod_v,
    newtable_v,
    tablegetelem_v,
    tablesetelem_v,
    assign_v,
    jump_v,
    jeq_v,
    jne_v,
    jgt_v,
    jge_v,
    jlt_v,
    jle_v,
    pusharg_v,
    call_v,
    funcenter_v,
    funcexit_v
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

struct userfunc_array{
    char* id;
    unsigned int address;
    unsigned int localSize;
};

enum avm_memcell_t{
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
};

struct avm_table_bucket{
    struct avm_memcell *key;
    struct avm_memcell *value;
    struct avm_table_bucket *next;
};

#define AVM_TABLE_HASHSIZE 211
struct avm_table{ 
    unsigned int refCounter;
    struct avm_table_bucket* strIndexed[AVM_TABLE_HASHSIZE];
    struct avm_table_bucket* numIndexed[AVM_TABLE_HASHSIZE];
    struct avm_table_bucket* userfuncIndexed[AVM_TABLE_HASHSIZE];
    struct avm_table_bucket* libfuncIndexed[AVM_TABLE_HASHSIZE];
    struct avm_table_bucket* boolIndexed[AVM_TABLE_HASHSIZE];
    unsigned int total;
};

struct avm_memcell {
    enum avm_memcell_t type;
    union{
        double numVal;
        char* strVal;
        unsigned char boolVal;
        struct avm_table* tableVal;
        unsigned int funcVal;
        char* libfuncVal;
    } data;
};


//-----------AVM GLOBAL VALS-------------

#define AVM_ENDING_PC codeSize
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_STACKENV_SIZE 4
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1
#define AVM_MAX_INSTRUCTIONS 20
#define AVM_STANDARD_STRING_SIZE 50
#define PI 3.14159265359

unsigned char executionFinished;
unsigned int pc;
unsigned int currLine;
unsigned int codeSize;

struct instruction* code;
char** const_string_array;
double* const_number_array;
struct userfunc_array* userfunc_array;
char** libfunc_array;

unsigned int const_string_array_size;
unsigned int const_number_array_size;
unsigned int userfunc_array_size;
unsigned int libfunc_array_size;

unsigned int total_global_vals;

unsigned int totalActuals;

char* typeString[8];

struct avm_memcell stack[AVM_STACKSIZE];
struct avm_memcell ax, bx, cx;
struct avm_memcell retval;
unsigned top, topsp;

//-----------AVM FUNC DEFS---------------

void init_avm();
void read_file_and_create_tables(FILE* fd);
void execute_cycle();
void execute_code(enum vmopcode opcode, struct instruction* instr);

struct avm_memcell* avm_translate_operand(struct vmarg* arg, struct avm_memcell* reg);
void execute_arithmetic(struct instruction* instr);
void execute_assign(struct instruction* instr);
void execute_call(struct instruction* instr);
void execute_funcenter(struct instruction* instr);
void execute_funcexit(struct instruction* instr);
void execute_pusharg(struct instruction* instr);
void execute_jeq(struct instruction* instr);
void execute_jne(struct instruction* instr);
void execute_jle(struct instruction* instr);
void execute_jge(struct instruction* instr);
void execute_jlt(struct instruction* instr);
void execute_jgt(struct instruction* instr);
void execute_newtable(struct instruction* instr);
void execute_tablegetelem(struct instruction* instr);
void execute_tablesetelem(struct instruction* instr);
void execute_jump(struct instruction* instr);

double consts_getnumber(unsigned int index);
char* consts_getstring(unsigned int index);
char* libfuncs_getused(unsigned int index);

struct avm_table* avm_tablenew();
struct avm_memcell* avm_tablegetelem(struct avm_table* table, struct avm_memcell* index);
void avm_tablesetelem(struct avm_table* table, struct avm_memcell *index, struct avm_memcell *content);

unsigned int hash_function(char* string);
void avm_tabledestroy(struct avm_table* t);
void avm_tableincrefcounter(struct avm_table* t);
void avm_tabledecrefcounter(struct avm_table* t);
void avm_tablebucketsinit(struct avm_table_bucket** p);
void avm_tablebucketsdestroy(struct avm_table_bucket** p);
void avm_assign(struct avm_memcell* lv, struct avm_memcell* rv);
unsigned int avm_totalactuals();
struct avm_memcell* avm_getactual(unsigned int i);
unsigned char avm_tobool(struct avm_memcell* m);

char* avm_tostring(struct avm_memcell* m);
char* number_tostring(struct avm_memcell* m);
char* string_tostring(struct avm_memcell* m);
char* bool_tostring(struct avm_memcell* m);
char* table_tostring(struct avm_memcell* m);
char* userfunc_tostring(struct avm_memcell* m);
char* libfunc_tostring(struct avm_memcell* m);
char* nil_tostring(struct avm_memcell* m);
char* undef_tostring(struct avm_memcell* m);
void bucket_runner(struct avm_table_bucket** tmp, char* result);
void key_and_value_print(struct avm_table_bucket* tmp, char* result);
void avm_calllibfunc(char* funcname);
void libfunc_print();
void libfunc_input();
void libfunc_objectmemberkeys();
void libfunc_objecttotalmembers();
void libfunc_objectcopy();
void libfunc_totalarguments();
void libfunc_arguments();
void libfunc_typeof();
void libfunc_strtonum();
void libfunc_sqrt();
void libfunc_cos();
void libfunc_sin();
struct userfunc_array* avm_getfuncinfo(unsigned int address);

void avm_initstack();
void avm_memcellclear(struct avm_memcell* m);
void avm_dec_top();
void avm_callsaveenviroment();
void avm_push_envvalue(unsigned int val);
unsigned int avm_get_envvalue(unsigned int i);

void read_from_file(FILE* file, char* buffer, int max_input, int abort_on_newline);