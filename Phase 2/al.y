%{
    #include "Utilities.h"

    int yyerror(char* Message);
    int yylex(void);
%}

%start program

%union{
    char* stringValue;
    int intValue;
    float floatValue;
    int boolValue;
    struct SymbolTableEntry* symbolValue;
}

%token IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR TOK_LOCAL DOUBLE_COLON DOUBLE_DOT EQUAL NOT_EQUAL PLUS_PLUS MINUS_MINUS BIGGER_EQUAL SMALLER_EQUAL
%token <stringValue> STRING
%token <stringValue> ID
%token <intValue> INT
%token <floatValue> FLOAT
%token <intValue> NIL
%token <boolValue> BOOL

%type <symbolValue> lvalue
%type <symbolValue> member
%type <symbolValue> call
%type <symbolValue> '('

%defines

%output "parser.c"

%right '='
%left OR
%left AND
%nonassoc EQUAL NOT_EQUAL
%nonassoc '>' BIGGER_EQUAL '<' SMALLER_EQUAL
%left '+' '-'
%left '*' '/' '%'
%right NOT PLUS_PLUS MINUS_MINUS UMINUS
%left '.' DOUBLE_DOT
%left '[' ']'
%left '(' ')'


%%

program:        stmt program
                |
                ;

stmt:           expr ';'
                | ifstmt
                | whilestmt
                | forstmt
                | returnstmt
                | BREAK ';' { if(loop_counter == 0) yyerror("cannot BREAK outside of a loop"); }
                | CONTINUE ';' { if(loop_counter == 0) yyerror("cannot CONTINUE outside of a loop"); }
                | block
                | funcdef
                | ';'
                | error ';' {yyerrok;}
                ;

expr:           assignexpr
                | expr '+' expr
                | expr '-' expr
                | expr '*' expr
                | expr '/' expr
                | expr '%' expr
                | expr '>' expr
                | expr BIGGER_EQUAL expr
                | expr '<' expr
                | expr SMALLER_EQUAL expr
                | expr EQUAL expr
                | expr NOT_EQUAL expr
                | expr AND expr
                | expr OR expr
                | term
                ;

term:           '(' expr ')'
                | '-' expr %prec UMINUS
                | NOT expr
                | PLUS_PLUS lvalue { if($2 != NULL){if( ($2->type == LIBFUNC) || ($2->type == USERFUNC) ) yyerror("use of function as variable (++)");} }
                | lvalue PLUS_PLUS { if($1 != NULL){if( ($1->type == LIBFUNC) || ($1->type == USERFUNC) ) yyerror("use of function as variable (++)");} }
                | MINUS_MINUS lvalue { if($2 != NULL){if( ($2->type == LIBFUNC) || ($2->type == USERFUNC) ) yyerror("use of function as variable (--)");} }
                | lvalue MINUS_MINUS { if($1 != NULL){if( ($1->type == LIBFUNC) || ($1->type == USERFUNC) ) yyerror("use of function as variable (--)");} }
                | primary
                ;

assignexpr:     lvalue '=' { if($1 != NULL){if( ($1->type == LIBFUNC) || ($1->type == USERFUNC) ) yyerror("assignment to function");} } expr 
                ;

primary:        lvalue
                | call
                | objectdef
                | '(' funcdef ')'
                | constdef
                ;

constdef:       INT
                | FLOAT
                | STRING
                | NIL
                | BOOL
                ;

lvalue:         ID {$$ = lookup($1, "DESCENDING"); if($$ == NULL){ insert_to_symbol_table($1, LOCAL); $$ = lookup($1, "CURRENT");}  }
                | TOK_LOCAL ID { $$ = lookup($2, "CURRENT"); if($$ == NULL){ insert_to_symbol_table($2, LOCAL); $$ = lookup($2, "CURRENT");} }
                | DOUBLE_COLON ID { $$ = lookup($2, "GLOBAL"); if($$ == NULL) yyerror(concat_ss("no global variable with name ", strup($2))); }
                | member {$$ = $1;}
                ;

member:         lvalue '[' expr ']'
                | lvalue '.' ID { $$ = lookup($3, "DESCENDING"); }
                | call '[' expr ']'
                | call '.' ID { $$ = lookup($3, "DESCENDING"); }
                ;

call:           call '(' elist ')'
                | lvalue callsuffix
                | '(' funcdef ')' '(' elist ')'
                ;

callsuffix:     normcall
                | methodcall
                ;

normcall:       '(' elist ')'
                ;

methodcall:     DOUBLE_DOT ID { lookup($2, "DESCENDING"); } '(' elist ')'
                ;

elist:          expr elist_cont
                |
                ;

elist_cont:     ',' expr elist_cont
                |
                ;

objectdef:      '[' elist ']'
                | '[' indexed ']'
                ;

indexed:        indexedelem indexed_cont
                ;

indexed_cont:   ',' indexedelem indexed_cont
                |
                ;      

indexedelem:    '{' expr ':' expr '}'
                ;

block:          '{' {current_scope++;} block_inner '}' {hide(current_scope); current_scope--;}
                ;

block_inner:    stmt block_inner
                |
                ;

funcdef:        FUNCTION func_id '(' {function_scope = current_scope; push_function_scope_stack(function_scope); current_scope++;} idlist ')' '{' block_inner '}' {hide(current_scope); current_scope--; function_scope = pop_function_scope_stack();}
                ;

func_id:        ID { if(lookup($1, "CURRENT") == NULL) insert_to_symbol_table($1,USERFUNC); else{ if(!check_in_libfunc($1)) yyerror(concat_ss("id ", concat_ss(strup($1), " already taken in current scope"))); } }
                | { insert_to_symbol_table(concat_si("_func", unnamed_function++),USERFUNC); }
                ;

idlist:         ID { if(lookup($1, "CURRENT") == NULL) insert_to_symbol_table($1,FORMAL); } idlist_cont
                |
                ;

idlist_cont:    ',' ID { if(lookup($2, "CURRENT") == NULL) insert_to_symbol_table($2,FORMAL); else yyerror("formal redeclaration"); } idlist_cont
                |
                ;

ifstmt:         IF '(' expr ')' stmt elsestmt
                ;

elsestmt:       ELSE stmt
                |
                ;

whilestmt:      WHILE '(' expr ')' {loop_counter++;} stmt {loop_counter--;}
                ;

forstmt:        FOR '(' elist ';' expr ';' elist ')' {loop_counter++;} stmt {loop_counter--;}
                ;

returnstmt:     RETURN return_cont ';' { if(function_stack_is_empty()) yyerror("cannot RETURN outside of a function"); }
                ;

return_cont:    expr
                |
                ;

%%

int main(int argc, char** argv){
    int fd;

    if(argc > 1){
        yyin = fopen(argv[1], "r");
        if(!yyin){
            fprintf(stdout, "\nCannot read file: %s\n", argv[1]);
            return 1;
        }
    }else{
        fprintf(stderr, "\nYou haven\'t provide a file to translate!\n");
        exit(1);
    }

    if(argc == 3){
        fd  = open(argv[2], O_CREAT | O_RDWR, 0666);
        dup2(fd, 1);
    }

    if(argc > 3){
        fprintf(stderr, "\nTOO MANY ARGUMENTS!\n");
        exit(1);
    }

    init_lex();

    init_bison();
 
    yyparse();
    
    show_lex_errors();

    print_scopelist();

    return 0;
}