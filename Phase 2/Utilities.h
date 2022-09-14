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

typedef struct Variable {
    char *name;
    unsigned int scope;
    unsigned int line;
} Variable;

typedef struct Function {
    char *name;
    unsigned int scope;
    unsigned int line;
} Function;

typedef struct SymbolTableEntry {
    bool isActive;
    union {
        Variable *varVal;
        Function *funcVal;
    } value;
    enum SymbolType type;
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

void push_function_scope_stack(int scope);
int pop_function_scope_stack();
bool function_stack_is_empty();

void insert_to_symbol_table(char* name, enum SymbolType type);
void insert_to_scope_list(uSymbolTableEntry* entry);

uSymbolTableEntry* lookup(char* name, char* lookup_mode);

void hide(int scope);

int hash_function(char* string);

bool check_in_libfunc(char* name);

void print_scopelist();

char* concat_si(const char* s1, unsigned int s2);

char* concat_ss(const char* s1, const char* s2);

char* strup(char* s1);

//-------------END OF HEADER FILE----------------