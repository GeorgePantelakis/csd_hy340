%{
    #include "Utilities.h"

    int yylex(void);
%}

%start program

%union{
    char* stringValue;
    int intValue;
    float floatValue;
    int boolValue;
    struct SymbolTableEntry* symbolValue;
    struct expr *expressionValue;
    struct quad *quadValue;
    struct for_node *forValue;
    struct call_node *callValue;
}

%token IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR TOK_LOCAL DOUBLE_COLON DOUBLE_DOT EQUAL NOT_EQUAL PLUS_PLUS MINUS_MINUS BIGGER_EQUAL SMALLER_EQUAL
%token  <stringValue>       STRING
%token  <stringValue>       ID
%token  <intValue>          INT
%token  <floatValue>        FLOAT
%token  <intValue>          NIL
%token  <boolValue>         BOOL

%type   <symbolValue>       func_id

%type   <expressionValue>   lvalue
%type   <expressionValue>   member
%type   <expressionValue>   primary
%type   <expressionValue>   term
%type   <expressionValue>   expr
%type   <expressionValue>   constdef
%type   <expressionValue>   assignexpr
%type   <expressionValue>   func_name
%type   <expressionValue>   elist
%type   <expressionValue>   elist_cont
%type   <expressionValue>   return_cont
%type   <expressionValue>   funcdef
%type   <expressionValue>   call
%type   <expressionValue>   methodcall_pre
%type   <expressionValue>   objectdef
%type   <expressionValue>   indexed
%type   <expressionValue>   indexed_cont
%type   <expressionValue>   indexedelem

%type   <intValue>          ifstmt_pre
%type   <intValue>          elsestmt_pre
%type   <intValue>          elsestmt
%type   <intValue>          funcdef_pre
%type   <intValue>          whilestmt_expr
%type   <intValue>          whilestmt_pre
%type   <intValue>          forstmt_arg1
%type   <intValue>          AND
%type   <intValue>          OR

%type   <forValue>          forstmt_arg2
%type   <forValue>          forstmt_arg3

%type   <callValue>         callsuffix
%type   <callValue>         methodcall
%type   <callValue>         normcall

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
                | //empty
                ;

stmt:           expr ';'
                {
                    reset_temps();
                }
                | ifstmt
                {
                    reset_temps();
                }
                | whilestmt
                {
                    reset_temps();
                }
                | forstmt
                {
                    reset_temps();
                }
                | returnstmt
                {
                    reset_temps();
                }
                | BREAK ';' 
                { 
                    if(loop_counter == 00) yyerror("cannot use BREAK outside of a loop");
                    else{
                        add_to_breakstack(quad_index);
                        emmit(jump, NULL, NULL, NULL, 00, yylineno);
                    }
                    reset_temps(); 
                }
                | CONTINUE ';' 
                { 
                    if(loop_counter == 00) yyerror("cannot use CONTINUE outside of a loop");
                    else{
                        add_to_continuestack(quad_index);
                        emmit(jump, NULL, NULL, NULL, 00, yylineno);
                    } 
                    reset_temps();
                }
                | block
                {
                    reset_temps();
                }
                | funcdef
                {
                    reset_temps();
                }
                | ';'
                | error ';' 
                {
                    yyerrok;
                    reset_temps();
                }
                ;

expr:           assignexpr
                {
                    $$ = $01;
                }
                | expr '+' expr
                {
                    if(check_expr(add, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = arithexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = arithexpr_e;
                        }else{
                            $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(add, $$, $01, $03, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'+\' operation");
                        $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr '-' expr
                {
                    if(check_expr(sub, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = arithexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = arithexpr_e;
                        }else{
                            $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(sub, $$, $01, $03, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'-\' operation");
                        $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr '*' expr
                {
                    if(check_expr(mul, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = arithexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = arithexpr_e;
                        }else{
                            $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(mul, $$, $01, $03, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'*\' operation");
                        $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr '/' expr
                {
                    if(check_expr(div_e, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = arithexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = arithexpr_e;
                        }else{
                            $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(div_e, $$, $01, $03, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'/\' operation");
                        $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr '%' expr
                {
                    if(check_expr(mod, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = arithexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = arithexpr_e;
                        }else{
                            $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(mod, $$, $01, $03, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'%\' operation");
                        $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr '>' expr
                {
                    if(check_expr(if_greater, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = boolexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = boolexpr_e;
                        }else{
                            $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(if_greater, NULL, $01, $03, quad_index + 02, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                        emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                        emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'>\' operation");
                        $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr BIGGER_EQUAL expr
                {
                    if(check_expr(if_greatereq, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = boolexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = boolexpr_e;
                        }else{
                            $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(if_greatereq, NULL, $01, $03, quad_index + 02, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                        emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                        emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'>=\' operation");
                        $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr '<' expr
                {
                    if(check_expr(if_less, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = boolexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = boolexpr_e;
                        }else{
                            $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(if_less, NULL, $01, $03, quad_index + 02, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                        emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                        emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'<\' operation");
                        $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr SMALLER_EQUAL expr
                {
                    if(check_expr(if_lesseq, $01, $03)){
                        if(is_temp_expr($01)){
                            $$ = $01;
                            $$->type = boolexpr_e;
                        }else if(is_temp_expr($03)){
                            $$ = $03;
                            $$->type = boolexpr_e;
                        }else{
                            $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(if_lesseq, NULL, $01, $03, quad_index + 02, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                        emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                        emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to \'<=\' operation");
                        $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | expr EQUAL expr
                {
                    $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    if(check_expr(if_eq, $01, $03)){
                        emmit(if_eq, NULL, $01, $03, quad_index + 02, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                        emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                        emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                    }else{
                        yyerror("Not same type expressions to \'==\' operation");
                    }
                }
                | expr NOT_EQUAL expr
                {
                    $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    if(check_expr(if_noteq, $01, $03)){
                        emmit(if_noteq, NULL, $01, $03, quad_index + 02, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                        emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                        emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                        emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                    }else{
                        yyerror("Not same type expressions to \'!=\' operation");
                    }
                }
                | expr AND
                {
                    $2 = quad_index;
                    emmit(if_eq, NULL, $01, false_temp_expr, 00, yylineno);
                }
                expr
                {   
                    $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    emmit(if_eq, NULL, $04, true_temp_expr, quad_index + 02, yylineno);
                    emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                    emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                    emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                    backpatch($02, quad_index);
                    emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                }
                | expr OR
                {
                    $2 = quad_index;
                    emmit(if_eq, NULL, $01, true_temp_expr, 00, yylineno);
                }
                expr
                {
                    $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    emmit(if_eq, NULL, $04, true_temp_expr, quad_index + 02, yylineno);
                    emmit(jump, NULL, NULL, NULL, quad_index + 03, yylineno);
                    backpatch($02, quad_index);
                    emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                    emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                    emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                }
                | term
                {
                    $$ = $01;
                }
                ;               

term:           '(' expr ')'
                {
                    $$ = $02;
                }
                | '-' expr %prec UMINUS
                {
                    if(check_expr(uminus, $02, NULL)){
                        if(is_temp_expr($02)){
                            $$ = $02;
                            $$->type = arithexpr_e;
                        }else{
                            $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                        }
                        emmit(uminus, $$, $02, NULL, 00, yylineno);
                    }else{
                        yyerror("Non arithmetic expression to uminus operation");
                        $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                }
                | NOT expr
                {
                    if(is_temp_expr($02)){
                        $$ = $02;
                        $$->type = boolexpr_e;
                    }else{
                        $$ = create_new_expression(boolexpr_e, new_temp(), NULL, 00, NULL, 02);
                    }
                    emmit(if_eq, NULL, $02, true_temp_expr, quad_index + 03, yylineno);
                    emmit(assign, $$, true_temp_expr, NULL, 00, yylineno);
                    emmit(jump, NULL, NULL, NULL, quad_index + 02, yylineno);
                    emmit(assign, $$, false_temp_expr, NULL, 00, yylineno);
                }
                | PLUS_PLUS lvalue 
                { 
                    if($02 != NULL){
                        if(($02->type == LIBFUNC) || ($02->type == USERFUNC)){
                            yyerror("use of function as variable (++)");
                        }else{
                            if($02->type == tableitem_e){
                                $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(tablegetelem, $$, $02, $02->index, 00, yylineno);
                                emmit(add, $$, $$, number_one_expr, 00, yylineno);
                                emmit(tablesetelem, $02, $02->index, $$, 00, yylineno);
                                temp_expr = NULL;
                            }else{
                                emmit(add, $02, $02, number_one_expr, 00, yylineno);
                                $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(assign, $$, $02, NULL, 00, yylineno);
                            }
                        }
                    }else{
                        $$ = NULL;
                    }
                }
                | lvalue PLUS_PLUS 
                { 
                    if($01 != NULL){
                        if(($01->type == LIBFUNC) || ($01->type == USERFUNC)){
                            yyerror("use of function as variable (++)");
                        }else{
                            if($01->type == tableitem_e){
                                $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                temp_expr = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(tablegetelem, temp_expr, $01, $01->index, 00, yylineno);
                                emmit(assign, $$, temp_expr, NULL, 00, yylineno);
                                emmit(add, temp_expr, temp_expr, number_one_expr, 00, yylineno);
                                emmit(tablesetelem, $01, $01->index, temp_expr, 00, yylineno);
                                temp_expr = NULL;
                            }else{
                                $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(assign, $$, $01, NULL, 00, yylineno);
                                emmit(add, $01, $01, number_one_expr, 00, yylineno); 
                            }
                        }
                    }else{
                        $$ = NULL;
                    }
                }
                | MINUS_MINUS lvalue 
                { 
                    if($02 != NULL){
                        if(($02->type == LIBFUNC) || ($02->type == USERFUNC)){
                            yyerror("use of function as variable (--)");
                        }else{
                            if($02->type == tableitem_e){
                                $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(tablegetelem, $$, $02, $02->index, 00, yylineno);
                                emmit(sub, $$, $$, number_one_expr, 00, yylineno);
                                emmit(tablesetelem, $02, $02->index, $$, 00, yylineno);
                                temp_expr = NULL;
                            }else{
                                emmit(sub, $02, $02, number_one_expr, 00, yylineno);
                                $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(assign, $$, $02, NULL, 00, yylineno);
                            }
                        }
                    }else{
                        $$ = NULL;
                    }
                }
                | lvalue MINUS_MINUS
                { 
                    if($01 != NULL){
                        if(($01->type == LIBFUNC) || ($01->type == USERFUNC)){
                            yyerror("use of function as variable (--)");
                        }else{
                            if($01->type == tableitem_e){
                                $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                temp_expr = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(tablegetelem, temp_expr, $01, $01->index, 00, yylineno);
                                emmit(assign, $$, $01, NULL, 00, yylineno);
                                emmit(sub, temp_expr, temp_expr, number_one_expr, 00, yylineno);
                                emmit(tablesetelem, $01, $01->index, temp_expr, 00, yylineno);
                                temp_expr = NULL;
                            }else{
                                $$ = create_new_expression(arithexpr_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(assign, $$, $01, NULL, 00, yylineno);
                                emmit(sub, $01, $01, number_one_expr, 00, yylineno); 
                            }
                        }
                    }else{
                        $$ = NULL;
                    }
                } 
                | primary
                {
                    $$ = $01;
                }
                ;

assignexpr:     lvalue '=' expr
                { 
                    if($01 != NULL){
                        if(($01->sym->type == LIBFUNC) || ($01->sym->type == USERFUNC)){
                            yyerror("assignment to function");
                        }else{
                            if($01->type == tableitem_e){
                                emmit(tablesetelem, $01, $01->index, $03, 00, yylineno);
                                $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(tablegetelem, $$, $01, $01->index, 00, yylineno);
                            }else{
                                emmit(assign, $01, $03, NULL, 00, yylineno);
                                $$ = create_new_expression(var_e, new_temp(), NULL, 00, NULL, 02);
                                emmit(assign, $$, $01, NULL, 00, yylineno);
                            }
                            
                        }
                    }else{
                        $$ = NULL;
                    }
                }
                ;

primary:        lvalue
                {   
                    if($01 != NULL){
                        if($01->type == tableitem_e){
                            $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                            emmit(tablegetelem, $$, $01, $01->index, 00, yylineno);
                        }else{
                            $$ = $01;
                        }
                    }else{
                        $$ = NULL;
                    }
                    
                }
                | call
                | objectdef
                | '(' funcdef ')'
                {
                    $$ = $2;
                }
                | constdef
                {
                    $$ = $01;
                }
                ;

constdef:       INT
                {
                    $$ = create_new_expression(constint_e, NULL, NULL, $01, NULL, 02);
                }
                | FLOAT
                {
                    $$ = create_new_expression(constfloat_e, NULL, NULL, $01, NULL, 02);
                }
                | STRING
                {
                    $$ = create_new_expression(conststring_e, NULL, NULL, 00, $01, 02);
                }
                | BOOL
                {
                    $$ = create_new_expression(constbool_e, NULL, NULL, 00, NULL, $01);
                }
                | NIL
                {
                    $$ = create_new_expression(nil_e, NULL, NULL, 00, NULL, 02);
                }
                ;

lvalue:         ID 
                {  
                    temp_symbol = lookup($01, "DESCENDING"); 
                    if(temp_symbol == NULL){ 
                        insert_to_symbol_table($01, LOCAL); 
                        temp_symbol = lookup($01, "CURRENT");
                         
                    }
            
                    if(temp_symbol->type == LIBFUNC){
                        $$ = create_new_expression(libraryfunc_e, temp_symbol, NULL, 00, NULL, 02);
                    }
                    else if(temp_symbol->type == USERFUNC){
                        $$ = create_new_expression(programfunc_e, temp_symbol, NULL, 00, NULL, 02);
                    }
                    else{ 
                        $$ = create_new_expression(var_e, temp_symbol, NULL, 00, NULL, 02);
                    }
                        
                    temp_symbol = NULL;
                    
                }
                | TOK_LOCAL ID 
                { 
                    temp_symbol = lookup($02, "CURRENT"); 
                    if(temp_symbol == NULL){ 
                        insert_to_symbol_table($02, LOCAL); 
                        temp_symbol = lookup($02, "CURRENT");  
                    }
                    if(temp_symbol->type == LIBFUNC)
                        $$ = create_new_expression(libraryfunc_e, temp_symbol, NULL, 00, NULL, 02);
                    else if(temp_symbol->type == USERFUNC)
                        $$ = create_new_expression(programfunc_e, temp_symbol, NULL, 00, NULL, 02);
                    else
                        $$ = create_new_expression(var_e, temp_symbol, NULL, 00, NULL, 02);
                    temp_symbol = NULL;
                }
                | DOUBLE_COLON ID 
                { 
                    temp_symbol = lookup($02, "GLOBAL"); 
                    if(temp_symbol == NULL){
                        yyerror(concat_ss("no global variable with name ", strup($02)));
                        $$ = NULL;
                    }else{
                        if(temp_symbol->type == LIBFUNC)
                            $$ = create_new_expression(libraryfunc_e, temp_symbol, NULL, 00, NULL, 02);
                        else if(temp_symbol->type == USERFUNC)
                            $$ = create_new_expression(programfunc_e, temp_symbol, NULL, 00, NULL, 02);
                        else
                            $$ = create_new_expression(var_e, temp_symbol, NULL, 00, NULL, 02);
                    }
                    temp_symbol = NULL;
                }
                | member
                {
                    $$ = $01;
                    
                }
                ;

member:         lvalue '[' expr ']'
                {
                    if($01 != NULL){
                        if($01->type == tableitem_e){
                            $$ = create_new_expression(tableitem_e, new_temp(), $03, 00, NULL, 02);
                            emmit(tablegetelem, $$, $01, $01->index, 00, yylineno);
                        }else{
                            $01->index = $03;
                            $01->type = tableitem_e;
                            $$ = $01;
                        }
                    }else{
                        $$ = NULL;
                    }
                }
                | lvalue '.' ID
                { 
                    temp_expr = create_new_expression(conststring_e, NULL, NULL, 00, $03, 02);
                    if($01 != NULL){
                        if($01->type == tableitem_e){
                            $$ = create_new_expression(tableitem_e, new_temp(), temp_expr, 00, NULL, 02);
                            emmit(tablegetelem, $$, $01, $01->index, 00, yylineno);
                        }else{
                            $01->index = temp_expr;
                            $01->type = tableitem_e;
                            $$ = $01;
                        }
                    }else{
                        $$ = NULL;
                    }
                    temp_expr = NULL;
                }
                | call '[' expr ']'
                {
                    if($01 != NULL){
                        $01->index = $03;
                        $01->type = tableitem_e;
                        $$ = $01;
                    }else{
                        $$ = NULL;
                    }
                }
                | call '.' ID
                { 
                    temp_expr = create_new_expression(conststring_e, NULL, NULL, 00, $03, 02);
                    if($01 != NULL){
                        $01->index = temp_expr;
                        $01->type = tableitem_e;
                    }
                    $$ = $01;
                    temp_expr = NULL;
                }
                ;

call:           call '(' elist ')'
                {
                    param_creator($03);
                    emmit(call, NULL, $01, NULL, 00, yylineno);
                    $$ = create_new_expression(var_e, new_temp(), NULL, 00, NULL, 02);
                    emmit(getretval, $$, NULL, NULL, 00, yylineno);
                }
                | lvalue callsuffix
                {
                    if($01 != NULL){

                        use_call_node($01, $02);
                        $$ = create_new_expression(var_e, new_temp(), NULL, 00, NULL, 02);
                        emmit(getretval, $$, NULL, NULL, 00, yylineno);
                    }else{
                        $$ = NULL;
                        yyerror("You are trying to call something that is not a value");
                    }
                }
                | '(' funcdef ')' '(' elist ')'
                {
                    param_creator($05);
                    emmit(call, NULL, $02, NULL, 00, yylineno);
                    $$ = create_new_expression(var_e, new_temp(), NULL, 00, NULL, 02);
                    emmit(getretval, $$, NULL, NULL, 00, yylineno);
                }
                ;

callsuffix:     normcall
                {
                    $$ = $1;
                }
                | methodcall
                {
                    $$ = $1;
                }
                ;

normcall:       '(' elist ')'
                {
                    $$ = create_call_node(01, NULL, $02);
                }
                ;

methodcall_pre: DOUBLE_DOT ID 
                { 
                    $$ = create_new_expression(conststring_e, NULL, NULL, 00, $02, 02);
                }
                ;

methodcall:     methodcall_pre '(' elist ')'
                {
                    $$ = create_call_node(00, $01, $03);
                }
                ;

elist:          expr elist_cont
                {
                    if($01 != NULL){
                        $01->next = $02;
                    }
                    $$ = $01;
                }
                | //empty
                {
                    $$ = NULL;
                }
                ;

elist_cont:     ',' expr elist_cont
                {
                    if($02 != NULL){
                        $02->next = $03;
                    }
                    $$ = $02;
                }
                | //empty
                {
                    $$ = NULL;
                }
                ;

objectdef:      '[' elist ']'
                {
                    $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                    emmit(tablecreate, NULL, $$, NULL, 00, yylineno);
                    create_elist_table_elements($$, $02);
                }
                | '[' indexed ']'
                {
                    $$ = create_new_expression(tableitem_e, new_temp(), NULL, 00, NULL, 02);
                    emmit(tablecreate, NULL, $$, NULL, 00, yylineno);
                    create_indexed_table_elements($$, $02);
                }
                ;

indexed:        indexedelem indexed_cont
                {
                    if($01 != NULL){
                        $01->next = $02;
                    }
                    $$ = $01;
                }
                ;

indexed_cont:   ',' indexedelem indexed_cont
                {
                    if($02 != NULL){
                        $02->next = $03;
                    }
                    $$ = $02;
                }
                | //empty
                {
                    $$ = NULL;
                }
                ;

indexedelem:    '{' expr ':' expr '}'
                {
                    $04->index = $02;
                    $$ = $04;
                }
                ;

block_pre:      '{'
                {
                    current_scope++;
                }
                ;

block:          block_pre block_inner '}' 
                {
                    hide(current_scope); 
                    current_scope--;
                }
                ;

block_inner:    stmt block_inner
                | //empty
                ;

funcdef_pre:    FUNCTION
                {
                    $$ = quad_index;
                    emmit(jump, NULL, NULL, NULL, 00, yylineno);
                }
                ;

func_name:      func_id   
                {
                    
                    function_scope = current_scope; 
                    push_function_scope_stack(function_scope);
                    current_scope++;

                    push_to_offset_stack(current_scopespace_offset());
                    scope_space_counter++;
                    set_current_scopespace_offset(1);

                    $$ = create_new_expression(programfunc_e, $01, NULL, 00, NULL, 02);
                    emmit(funcstart, NULL, $$, NULL, 00, yylineno);
                    
                }
                ;      

funcdef:        funcdef_pre func_name '(' idlist ')'
                {
                    scope_space_counter++;
                    set_current_scopespace_offset(1);
                } 
                '{' block_inner '}' 
                {
                    $$ = $02;
                    backpatch_return(quad_index);
                    add_function_info($02->sym, $01 + 01, current_scopespace_offset());
                    hide(current_scope);
                    current_scope--; 
                    function_scope = pop_function_scope_stack();
                    
                    scope_space_counter -= 02;
                    set_current_scopespace_offset(pop_from_offset_stack());

                    emmit(funcend, NULL, $02, NULL, 00, yylineno);
                    backpatch($01, quad_index);
                }
                ;

func_id:        ID 
                { 
                    if(lookup($01, "CURRENT") == NULL){
                        insert_to_symbol_table($01,USERFUNC);
                        $$ = lookup($01, "CURRENT");
                         
                    }else{
                        if(!check_in_libfunc($01)) yyerror(concat_ss("id ", concat_ss(strup($01), " already taken in current scope")));
                        $$ = NULL;
                    }
                    
                }
                | //empty
                { 
                    insert_to_symbol_table(concat_si("_func", unnamed_function),USERFUNC);
                    $$ = lookup(concat_si("_func", unnamed_function), "CURRENT"); 
                    unnamed_function++;
                }
                ;

idlist:         ID 
                { 
                    if(lookup($01, "CURRENT") == NULL){
                        insert_to_symbol_table($01,FORMAL);
                         
                    } 
                    
                } idlist_cont
                | //empty
                ;

idlist_cont:    ',' ID
                {
                    if(lookup($02, "CURRENT") == NULL){
                        insert_to_symbol_table($02,FORMAL);
                         
                    }else{
                        yyerror("formal redeclaration");
                    }
                } idlist_cont
                | //empty
                ;

ifstmt_pre:     IF '(' expr ')'
                {
                    emmit(if_eq, NULL, $03, true_temp_expr, quad_index + 02, yylineno);
                    $$ = quad_index;
                    emmit(jump, NULL, NULL, NULL, 00, yylineno);
                }
                ;

ifstmt:         ifstmt_pre stmt
                {  
                    backpatch($01, quad_index);
                }
                elsestmt
                {
                    if($04 != 0){
                        backpatch($04, quad_index);
                        quads[$01].label += 01;
                    }
                }
                ;

elsestmt_pre:   ELSE
                {
                    $$ = quad_index;
                    emmit(jump, NULL, NULL, NULL, 00, yylineno);
                }
                ;

elsestmt:       elsestmt_pre stmt
                {
                    $$ = $01;
                }
                | //empty
                {
                    $$ = 0;
                }
                ;

whilestmt_pre:  WHILE
                {
                    $$ = quad_index;
                }
                ;

whilestmt_expr: '(' expr ')'
                {
                    loop_counter++;

                    emmit(if_eq, NULL, $02, true_temp_expr, quad_index + 02, yylineno);
                    $$ = quad_index;
                    emmit(jump, NULL, NULL, NULL, 00, yylineno);
                }
                ;

whilestmt:      whilestmt_pre whilestmt_expr stmt 
                {
                    backpatch_break(quad_index + 01);
                    backpatch_continue($01);

                    loop_counter--;

                    emmit(jump, NULL, NULL, NULL, $01, yylineno);
                    backpatch($02, quad_index);
                }
                ;

forstmt_arg1:   '(' elist
                {
                    $$ = quad_index;
                }
                ;

forstmt_arg2:   ';' expr ';'
                {
                    $$ = (struct for_node *)malloc(sizeof(struct for_node));
                    $$->quad1 = quad_index;
                    emmit(if_eq, NULL, $02, true_temp_expr, 00, yylineno);
                    $$->quad2 = quad_index;
                    emmit(jump, NULL, NULL, NULL, 00, yylineno);
                    $$->firstQuad = quad_index;
                }
                ;

forstmt_arg3:   elist ')'
                {
                    loop_counter++;

                    $$ = (struct for_node *)malloc(sizeof(struct for_node));
                    $$->quad1 = quad_index;
                    emmit(jump, NULL, NULL, NULL, 00, yylineno);
                    $$->quad2 = 0;
                    $$->firstQuad = quad_index;
                }
                ;

forstmt:        FOR forstmt_arg1 forstmt_arg2 forstmt_arg3 stmt
                {
                    backpatch_break(quad_index + 01);
                    backpatch_continue($03->firstQuad);

                    loop_counter--;

                    emmit(jump, NULL, NULL, NULL, $03->firstQuad, yylineno);
                    backpatch($03->quad1, $04->firstQuad);
                    backpatch($03->quad2, quad_index);
                    backpatch($04->quad1, $02);
                    if($03) free($03);
                    if($04) free($04);
                }
                ;

returnstmt:     RETURN return_cont
                { 
                    if(function_stack_is_empty()) yyerror("cannot RETURN outside of a function"); 
                    else{
                        if($02){
                            emmit(ret, NULL, $02, NULL, 00, yylineno);
                            add_to_returnstack(quad_index);
                            emmit(jump, NULL, NULL, NULL, 00, yylineno);
                        }else{    
                            emmit(ret, NULL, NULL, NULL, 00, yylineno);
                            add_to_returnstack(quad_index);
                            emmit(jump, NULL, NULL, NULL, 00, yylineno);
                        }
                    }
                }
                ;

return_cont:    expr ';'
                {
                    $$ = $01;
                }
                | ';' 
                {
                    $$ = NULL;
                }
                ;

%%