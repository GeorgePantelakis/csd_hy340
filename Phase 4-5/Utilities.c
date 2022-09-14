// UTILITIES FILE
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "Utilities.h"

//-------------MAIN FUNCTIONS----------------------

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

    init_quads();

    yyparse();
    
    show_lex_errors();

    if(succeeded){
        create_quad_file();

        init_instructions();
        generate_tcode();
        patch_incomplete_jumps();
        create_tcode_txt_file();
        create_tcode_abc_file();
    }else{
        fprintf(stderr, "\nIntermidiate code generation failed!\n");
    }

    return 0;
}

//-------------LEX FUNCTIONS-----------------------

void init_lex(){ 
    comment_stack = NULL;

    error_stack = NULL;

    comment_counter = 0;

    buffer = (char*)malloc(sizeof(char) * 1024);

    error_buffer = (char*)malloc(sizeof(char) * 256);
}

void show_lex_errors(){
    char *temp;
    while(error_stack){
        temp = Pop_from_error_stack();
        fprintf(stdout, "%s\n", temp);
        if(temp)free(temp);
    }
}

char* filter_string(char* input_string){
    int i, output_string_index = 0;
    char * output_string = (char *)malloc(sizeof(char) * strlen(input_string));

    for(i = 0; i < (strlen(input_string) - 1); i++){
        if(input_string[i] == '\\'){
            i++;
            if(input_string[i] == 'n'){
                output_string[output_string_index++] = '\n';
            }else if(input_string[i] == 't'){
                output_string[output_string_index++] = '\t';
            }else if(input_string[i] == '\\'){
                output_string[output_string_index++] = '\\';
            }else if(input_string[i] == '"'){
                output_string[output_string_index++] = '"';
            }else if(input_string[i] == ' '){
                output_string[output_string_index++] = ' ';
            }else{
                sprintf(error_buffer, "\033[1;33mWARNING\033[0m: unknown escape sequence \"\\%c\" at line %d!\n", input_string[i], yylineno); Push_to_error_stack(error_buffer);
            }
        }else{
            output_string[output_string_index++] = input_string[i];
        }
    }

    return output_string;
}

void Push_to_comment_stack(int line){
    struct comment_stack_node* tmp;

    tmp = (struct comment_stack_node*)malloc(sizeof(struct comment_stack_node));
    tmp->start_line = line;
    tmp->next = comment_stack;
    comment_stack = tmp;
}

int Pop_from_comment_stack(){
    int result;
    struct comment_stack_node* tmp;

    result = comment_stack->start_line;
    tmp = comment_stack;
    comment_stack = comment_stack->next;
    if(tmp) free(tmp);

    return result;
}

void Push_to_error_stack(char* text){
    struct error_stack_node* tmp;
    struct error_stack_node* last;

    last = error_stack;
    tmp = (struct error_stack_node*)malloc(sizeof(struct error_stack_node));
    tmp->text = (char *)malloc(sizeof(char) * strlen(text));
    strcpy(tmp->text, text);
    tmp->next = NULL;

    if(last == NULL){
        error_stack = tmp;
        return;
    }

    while(last->next) last = last->next;

    last->next = tmp;
    
}

char* Pop_from_error_stack(){
    char* text;
    struct error_stack_node* tmp;

    text = (char*)malloc(sizeof(error_stack->text));
    strcpy(text, error_stack->text);
    tmp = error_stack;
    error_stack = error_stack->next;
    if(tmp->next) free(tmp->text);
    if(tmp) free(tmp);

    return text;
}

void Add_to_buffer(){
    int i = strlen(buffer);
    int j;

    for(j = 0; j < strlen(yytext); j++){
        buffer[i + j] = yytext[j];
    }
}

void Clear_buffer(){
    int i;

    for(i = 0; i < 1024; i++) buffer[i] = '\0';
}

//-------------YACC FUNCTIONS----------------------

void init_bison(){

    int i;

    current_scope = 0;
    function_scope = 0;
    unnamed_function = 0;
    function_scope_stack = NULL;
    loop_counter = 0;

    for(i = 0; i < TABLE_SIZE; i++){
        SymbolTable[i] = NULL;
    }

    scope_list = (ScopeNode*)malloc(sizeof(ScopeNode));
    scope_list->scope = 0;
    scope_list->next = NULL;
    scope_list->value = NULL;

    insert_to_symbol_table("print", LIBFUNC);
    insert_to_symbol_table("input", LIBFUNC);
    insert_to_symbol_table("objectmemberkeys", LIBFUNC);
    insert_to_symbol_table("objecttotalmembers", LIBFUNC);
    insert_to_symbol_table("objectcopy", LIBFUNC);
    insert_to_symbol_table("totalarguments", LIBFUNC);
    insert_to_symbol_table("argument", LIBFUNC);
    insert_to_symbol_table("typeof", LIBFUNC);
    insert_to_symbol_table("strtonum", LIBFUNC);
    insert_to_symbol_table("sqrt", LIBFUNC);
    insert_to_symbol_table("cos", LIBFUNC);
    insert_to_symbol_table("sin", LIBFUNC);
}

bool check_in_libfunc(char* name){
    ScopeNodeValue* NodeValue = scope_list->value;

    while(NodeValue != NULL){
        if(NodeValue->value->type == LIBFUNC){
            if(!strcmp(NodeValue->value->value.funcVal->name, name)){
                return true;  
            }
        }
        NodeValue = NodeValue->next;
    }

    return false;
}

void insert_to_symbol_table(char* name, enum SymbolType type){

    uSymbolTableEntry *temp, *bucket;
    Variable* var = NULL;
    Function* func = NULL;
    int index;

    if(check_in_libfunc(name)){
        yyerror(concat_ss("collision with libfunc ", strup(name)));
        return;
    } 

    temp = (uSymbolTableEntry*)malloc(sizeof(uSymbolTableEntry));
    temp->isActive = true;

    if(current_scope == 0 && type == LOCAL) temp->type = GLOBAL;
    else temp->type = type;

    temp->next = NULL;
    temp->space = current_scopespace();
    if(type == LIBFUNC){
        temp->offset = 0;
    }else{
        temp->offset = current_scopespace_offset();
        inc_current_scopespace_offset();
    }
    
    

    if(type == GLOBAL || type == LOCAL || type == FORMAL){
        var = (Variable*)malloc(sizeof(Variable));
        var->name = strdup(name);
        var->scope = current_scope;
        var->line = ((name[0] == '_') ? 0 : yylineno);
        temp->value.varVal = var;
    }else{
        func = (Function*)malloc(sizeof(Function));
        func->name = strdup(name);
        func->scope = current_scope;
        func->line = ((type == LIBFUNC || name[0] == '_') ? 0 : yylineno);
        func->localVars = 0;
        temp->value.funcVal = func;
    }

    insert_to_scope_list(temp);

    index = hash_function(name);

    bucket = SymbolTable[index];

    if(bucket == NULL){
        SymbolTable[index] = temp;
        return;
    }

    while(bucket->next != NULL) bucket = bucket->next;

    bucket->next = temp;
}

void insert_to_scope_list(uSymbolTableEntry* entry){
    ScopeNode* temp = scope_list;
    ScopeNodeValue* value;
    ScopeNodeValue* latestNode;
    ScopeNode* newNode;

    latestNode = (ScopeNodeValue*)malloc(sizeof(ScopeNodeValue));
    latestNode->value = entry;
    latestNode->next = NULL;

    while(temp->scope != current_scope && temp->next != NULL)temp = temp->next;

    if(temp->scope == current_scope){
        value = temp->value;

        if(value == NULL){
            temp->value = latestNode;
            return;
        }

        while(value->next != NULL) value = value->next;

        value->next = latestNode;

    }else{
        newNode = (ScopeNode*)malloc(sizeof(ScopeNode));
        newNode->scope = current_scope;
        newNode->next = NULL;
        newNode->value = latestNode;
        temp->next = newNode;
    }
}


uSymbolTableEntry* lookup(char* name, char* lookup_mode){
    ScopeNode* temp = scope_list;
    ScopeNodeValue* NodeValue;
    
    uSymbolTableEntry* bucket;
    unsigned int index;
    int tmp_scope;
    int i;

    if(!strcmp(lookup_mode, "CURRENT")){
        
        index = hash_function(name);
        bucket = SymbolTable[index];

        if(bucket == NULL) return NULL;
        
        while(bucket != NULL){
            if(bucket->type == GLOBAL || bucket->type == LOCAL || bucket->type == FORMAL){
                
                if( (!strcmp(bucket->value.varVal->name, name)) &&  (bucket->value.varVal->scope == current_scope) && (bucket->isActive == true) ){ 
                    return bucket;
                }
                      
            }else if(bucket->type == LIBFUNC || bucket->type == USERFUNC){ //for functions
                
                if( (!strcmp(bucket->value.funcVal->name, name)) && (bucket->value.funcVal->scope == current_scope) && (bucket->isActive == true) && (!check_in_libfunc(name))){

                    return bucket;
                }
                
            }
            
            bucket = bucket->next;
            
        }
        
        return NULL;
      
    }else if(!strcmp(lookup_mode, "GLOBAL")){
        
        index = hash_function(name);
        bucket = SymbolTable[index];

        if(bucket == NULL) return NULL;

        while(bucket != NULL){
            if(bucket->type == GLOBAL || bucket->type == LOCAL || bucket->type == FORMAL){

                if( (!strcmp(bucket->value.varVal->name, name)) &&  (bucket->value.varVal->scope == 0) && (bucket->isActive == true) ){
                    return bucket;
                }
                    
            }else if(bucket->type == LIBFUNC || bucket->type == USERFUNC){

                if( (!strcmp(bucket->value.funcVal->name, name)) && (bucket->value.funcVal->scope == 0) && (bucket->isActive == true) ){
                    return bucket;
                }
                    
            }

            bucket = bucket->next;
        }
        
        return NULL;

        
    }else if(!strcmp(lookup_mode, "DESCENDING")){
        
        for(tmp_scope = current_scope; tmp_scope > (-1); tmp_scope--){
            
            temp = scope_list;
            
            while( (temp->scope != tmp_scope) && (temp->next != NULL) ) {
                temp = temp->next;
            }
            
            if(temp->scope != tmp_scope) continue;

            NodeValue = temp->value;

            while(NodeValue != NULL){
                if(NodeValue->value->type == GLOBAL || NodeValue->value->type == LOCAL || NodeValue->value->type == FORMAL){

                    if( (!strcmp(NodeValue->value->value.varVal->name, name)) && (NodeValue->value->isActive == true) ){
                        if( (NodeValue->value->value.varVal->scope > function_scope) || (NodeValue->value->value.varVal->scope == 0)){
                            return NodeValue->value;

                        }else{

                            fprintf(stdout, "\n\033[1;31mERROR\033[0m: cannot access %s in scope %d at line %d\n", strup(name), NodeValue->value->value.varVal->scope, yylineno);
                            succeeded = 0;
                            return NodeValue->value;
                        }  
                    }
                        
                }else if(NodeValue->value->type == LIBFUNC || NodeValue->value->type == USERFUNC){ //for function search

                    if( !strcmp(NodeValue->value->value.funcVal->name, name) && (NodeValue->value->isActive == true) ){
                        return NodeValue->value;
                    }
                        
                }

                NodeValue = NodeValue->next;
            }
        }

        return NULL;

    }else if(!strcmp(lookup_mode, "TEMP")){
        
        index = hash_function(name);
        bucket = SymbolTable[index];
        
        if(bucket == NULL) return NULL;
        
        while(bucket != NULL){
            if(bucket->type == GLOBAL || bucket->type == LOCAL || bucket->type == FORMAL){
                
                if( (!strcmp(bucket->value.varVal->name, name)) &&  (bucket->value.varVal->scope == current_scope) ){
                    return bucket;
                }
                      
            }

            bucket = bucket->next;
        }
        
        return NULL;

    }else{
        assert(0);
    }
}

void hide(int scope){
    ScopeNode* temp = scope_list;
    ScopeNodeValue* NodeValue;

    while(temp->scope != scope && temp->next != NULL)temp = temp->next;

    if(temp->scope == scope){
        NodeValue = temp->value;
        
        while(NodeValue != NULL){
            NodeValue->value->isActive = false;
            NodeValue = NodeValue->next;
        }
    }
}

unsigned int hash_function(char* string){
    unsigned int hash = 5381;
    int c;

    while (c = *string++)
        hash = ((hash << 5) + hash) + c;

    return (hash % TABLE_SIZE);
}

void print_scopelist(){
    ScopeNode* temp = scope_list;
    ScopeNodeValue* NodeValue;
    char* type_of_variable;
    Variable *var;
    Function *func;

    type_of_variable = (char*)malloc(20 * sizeof(char));

    while(temp != NULL){
        NodeValue = temp->value;
        fprintf(stdout, "\n\n\033[1m--------------------------- SCOPE #%d ---------------------------\033[0m\n\n", temp->scope);

        while(NodeValue != NULL){
          
            if(NodeValue->value->type == GLOBAL){ strcpy(type_of_variable, "Global variable"); var = NodeValue->value->value.varVal; }
            else if(NodeValue->value->type == LOCAL){ strcpy(type_of_variable, "Local variable"); var = NodeValue->value->value.varVal; }
            else if(NodeValue->value->type == FORMAL){ strcpy(type_of_variable, "Formal argument"); var = NodeValue->value->value.varVal; }
            else if(NodeValue->value->type == USERFUNC){ strcpy(type_of_variable, "User function");  func = NodeValue->value->value.funcVal; }
            else if(NodeValue->value->type == LIBFUNC){  strcpy(type_of_variable, "Library function"); func = NodeValue->value->value.funcVal; }  

            if(!strcmp(type_of_variable, "Global variable") || !strcmp(type_of_variable, "Local variable") || !strcmp(type_of_variable, "Formal argument"))
                fprintf(stdout, "\"%s\" [%s] (line %d) (scope %d) (offset %d)\n", var->name, type_of_variable, var->line, var->scope, NodeValue->value->offset);    
            else
                fprintf(stdout, "\"%s\" [%s] (line %d) (scope %d) (offset %d)\n", func->name, type_of_variable, func->line, func->scope, NodeValue->value->offset);  
            
            NodeValue = NodeValue->next;
        }

        temp = temp->next;
    }  

    fprintf(stdout, "\n\n");
}

void push_function_scope_stack(int scope){
    struct function_scope_stack* tmp;

    tmp = (struct function_scope_stack*)malloc(sizeof(struct function_scope_stack));
    tmp->scope = scope;
    tmp->next = function_scope_stack;
    function_scope_stack = tmp;

    if(tmp) free(tmp);
}

int pop_function_scope_stack(){
    int result;
    struct function_scope_stack* tmp;

    tmp = function_scope_stack;
    function_scope_stack = function_scope_stack->next;

    if(function_scope_stack == NULL)
        return 0;

    result = function_scope_stack->scope;

    return result;
}

int top_function_scope_stack(){
    int result;

    if(function_scope_stack->next == NULL)
        return 0;

    result = function_scope_stack->next->scope;

    return result;
}

bool function_stack_is_empty(){
    if(function_scope_stack == NULL) return true;
    return false;
}

int yyerror(char* Message){
    fprintf(stdout, "\n\033[1;31mERROR\033[0m: %s at line %d !\n", Message, yylineno);
    succeeded = 0;
    return 0;
}

char* concat_si(const char* s1, unsigned int s2){

    char *result = malloc(strlen(s1) + (2 * sizeof(char))); 
    char *tmp = (char *)malloc(5 * sizeof(char));
    strcpy(result, s1);
    sprintf(tmp, "%d", s2);
    strcat(result, tmp);
    return result;
    
}

char* concat_ss(const char* s1, const char* s2){

    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator

    if(!result) { fprintf(stderr, "Error with malloc!\n"); exit(1); }

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;

}

char* strup(char* s1){

    char *tmp = s1;
    while(*tmp){
        *tmp = toupper((unsigned char) *tmp);
        tmp++;
    }
    return s1;

}

//-------------QUAD FUNCTIONS----------------------

void init_quads(){
    max_quads = EXPAND_SIZE;
    quad_index = 0;
    quads = (struct quad *)malloc(CURR_SIZE);

    temp_counter = 0;
    scope_space_counter = 0;

    programm_var_offset = 0;
    function_local_offset = 0;
    formal_argument_offset = 0;

    temp_symbol = NULL;

    temp_expr = NULL;
    true_temp_expr = create_new_expression(constbool_e, NULL, NULL, 0, NULL, 1);
    false_temp_expr = create_new_expression(constbool_e, NULL, NULL, 0, NULL, 0);
    number_one_expr = create_new_expression(constint_e, NULL, NULL, 1, NULL, 2);
    number_minus_one_expr = create_new_expression(constint_e, NULL, NULL, -1, NULL, 2);

    offset_stack = NULL;

    break_stack = NULL;
    continue_stack = NULL;
    return_stack = NULL;

    succeeded = 1;
}

enum scopespace_t current_scopespace(){
    if(scope_space_counter == 0){
        return programvar;
    }else if(scope_space_counter % 2 == 1){
        return formalarg;
    }else{
        return functionlocal;
    }
}

void expand_table(){
    struct quad *temp_quads_pointer;
    int i;

    assert(quad_index >= max_quads);

    temp_quads_pointer = quads;
    quads = (struct quad *)malloc(NEW_SIZE);

    for(i = 0; i < max_quads; i++){
        quads[i] = temp_quads_pointer[i];
    }

    max_quads += EXPAND_SIZE;

    if(temp_quads_pointer) free(temp_quads_pointer);
}

void emmit(enum iopcode_t iopcode, struct expr *result, struct expr *arg1, struct expr *arg2, unsigned int label, unsigned int line){
    struct quad *current_quad;
    
    if(quad_index >= max_quads) expand_table();

    current_quad = &(quads[quad_index++]);

    current_quad->op = iopcode;
    current_quad->result = result;
    check_if_temp(current_quad->result, result);
    current_quad->arg1 = arg1;
    check_if_temp(current_quad->arg1, arg1);
    current_quad->arg2 = arg2;
    check_if_temp(current_quad->arg2, arg2);
    current_quad->label = label;
    current_quad->line = line;
}

struct expr *create_new_expression(enum expr_t type, uSymbolTableEntry *sym, struct expr *index, float numConst, char *strConst, unsigned int boolConst){
    struct expr *temp;

    temp = (struct expr *)malloc(sizeof(struct expr));

    temp->type = type;
    temp->sym = sym;
    temp->strConst = strConst;
    temp->numConst = numConst;
    temp->boolConst = boolConst;
    temp->index = index;
    temp->next = NULL;

    return temp;
}

int check_expr(enum expr_t type, struct expr *first, struct expr *second){
    enum expr_t first_type, second_type;
    
    if(first && second){
        first_type = first->type;
        second_type = second->type;

        if(type == add || type == sub || type == mul || type == div_e || type == mod ){
            if((first_type == arithexpr_e || first_type == constint_e || first_type == constfloat_e || first_type == var_e || first_type == tableitem_e)
                && (second_type == arithexpr_e || second_type == constint_e || second_type == constfloat_e || second_type == var_e || second_type == tableitem_e)){
                return 1;
            }else{
                return 0;
            }
        }else if(type == if_lesseq || type == if_greatereq || type == if_less || type == if_greater){
            if((first_type == arithexpr_e || first_type == constint_e || first_type == constfloat_e || first_type == var_e || first_type == tableitem_e)
                && (second_type == arithexpr_e || second_type == constint_e || second_type == constfloat_e || second_type == var_e || second_type == tableitem_e)){
                return 1;
            }else{
                return 0;
            }
        }else if(type == if_eq || type == if_noteq){
            if((first_type == arithexpr_e || first_type == constint_e || first_type == constfloat_e || first_type == var_e || first_type == tableitem_e)
                && (second_type == arithexpr_e || second_type == constint_e || second_type == constfloat_e || second_type == var_e || second_type == tableitem_e)){
                return 1;
            }else if((first_type == var_e || first_type == constbool_e || first_type == tableitem_e || first_type == boolexpr_e)
                    && (second_type == var_e || second_type == constbool_e || second_type == tableitem_e || second_type == boolexpr_e)){
                return 1;
            }else if((first_type == var_e || first_type == conststring_e || first_type == tableitem_e)
                    && (second_type == var_e || second_type == conststring_e || second_type == tableitem_e)){
                return 1;
            }else if((first_type == tableitem_e && second_type == nil_e)
                    ||(first_type == nil_e && second_type == tableitem_e)){
                return 1;
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }else if(first){
        first_type = first->type;

        if(type == uminus){
            if((first_type == arithexpr_e || first_type == constint_e || first_type == constfloat_e || first_type == var_e)){
                return 1;
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }else{
        return 0;
    }
}

char *new_temp_name(){
    char *buffer;
    
    buffer = (char *)malloc(6 * sizeof(char));
    sprintf(buffer, "_t%d", temp_counter);
    return buffer;
}

uSymbolTableEntry *new_temp(){
    uSymbolTableEntry *temp;
    char *name;

    name = new_temp_name();
    temp = lookup(name, "TEMP");

    if(temp == NULL){
        insert_to_symbol_table(name, LOCAL);
        temp = lookup(name, "TEMP");
    }else{
        if(!temp->isActive){
            temp->isActive = true;
            temp->offset = current_scopespace_offset();
            inc_current_scopespace_offset();
        }
    }
    
    temp_counter++;

    if(name) free(name);

    return temp;
}

void reset_temps(){
    temp_counter = 0;
}

unsigned int current_scopespace_offset(){
    switch (current_scopespace()){
        case programvar:
            return programm_var_offset;
        case functionlocal:
            return function_local_offset;
        case formalarg:
            return formal_argument_offset;
        default:
            assert(0);
    }
}

void inc_current_scopespace_offset(){
    switch (current_scopespace()){
        case programvar:
            programm_var_offset++;
            break;
        case functionlocal:
            function_local_offset++;
            break;
        case formalarg:
            formal_argument_offset++;
            break;
        default:
            assert(0);
    }
}

void set_current_scopespace_offset(int offset){
    switch (current_scopespace()){
        case programvar:
            programm_var_offset = offset;
            break;
        case functionlocal:
            function_local_offset = offset;
            break;
        case formalarg:
            formal_argument_offset = offset;
            break;
        default:
            assert(0);
    }
}

void push_to_offset_stack(int offset){
    struct offset_node *temp;

    temp = (struct offset_node *)malloc(sizeof(struct offset_node));
    temp->offset = offset;
    temp->next = offset_stack;
    offset_stack = temp;
}

int pop_from_offset_stack(){
    struct offset_node *temp;
    int offset;

    temp = offset_stack;
    offset_stack = offset_stack->next;
    offset = temp->offset;
    if(temp) free(temp);

    return offset;
}

void backpatch(int index, int label){
    assert(index >= 0 && index <= max_quads);
    quads[index].label = label;
}

void add_to_breakstack(unsigned int quad){
    struct breaknode *tmp;

    tmp = (struct breaknode*)malloc(sizeof(struct breaknode));
    
    tmp->quad = quad;
    tmp->loopNo = loop_counter;
    tmp->next = break_stack;
    break_stack = tmp;
}

void add_to_continuestack(unsigned int quad){
    struct continuenode *tmp;

    tmp = (struct continuenode*)malloc(sizeof(struct continuenode));
    
    tmp->quad = quad;
    tmp->loopNo = loop_counter;
    tmp->next = continue_stack;
    continue_stack = tmp;
}

void add_to_returnstack(unsigned int quad){
    struct returnnode *tmp;

    tmp = (struct returnnode*)malloc(sizeof(struct returnnode));
    
    tmp->quad = quad;
    tmp->scope = current_scope;
    tmp->next = return_stack;
    return_stack = tmp;
}

void backpatch_break(unsigned int label){
    struct breaknode *tmp;

    while(break_stack != NULL){
        tmp = break_stack;

        if(tmp->loopNo == loop_counter){
            quads[tmp->quad].label = label;
        }else{
            break;
        }

        break_stack = break_stack->next;
        if(tmp) free(tmp);
    }
}

void backpatch_continue(unsigned int label){
    struct continuenode *tmp;

    while(continue_stack != NULL){
        tmp = continue_stack;

        if(tmp->loopNo == loop_counter){
            quads[tmp->quad].label = label;
        }else{
            break;
        }

        continue_stack = continue_stack->next;
        if(tmp) free(tmp);
    }
}

void backpatch_return(unsigned int label){
    struct returnnode *tmp;

    while(return_stack != NULL){
        tmp = return_stack;

        if(tmp->scope >= top_function_scope_stack()){
            quads[tmp->quad].label = label;
        }else{
            break;
        }

        return_stack = return_stack->next;
        if(tmp) free(tmp);
    }
}

void add_function_info(uSymbolTableEntry *symbol, unsigned int StartingQuad, unsigned int totalLocalVars){
    if(symbol == NULL)return;
    if(symbol->type != USERFUNC)return;

    symbol->value.funcVal->startingQuad = StartingQuad;
    symbol->value.funcVal->localVars = totalLocalVars;
}

void param_creator(struct expr *elist){
    struct expr *temp;
    struct expr *last;

    temp = elist;
    last = NULL;

    while(temp != last){
        while(temp->next != last) temp = temp->next;
        emmit(param, NULL, temp, NULL, 00, yylineno);
        last = temp;
        temp = elist;
    }
}

struct call_node *create_call_node(int normalCall, struct expr *name, struct expr *elist){
    struct call_node *temp;

    temp = (struct call_node *)malloc(sizeof(struct call_node));

    temp->normalCall = normalCall;
    temp->name = name;
    temp->elist = elist;

    return temp;
}

void use_call_node(struct expr *lvalue, struct call_node *callNode){
    struct expr *temp;

    assert(callNode);
    if(lvalue == NULL)return;

    if(callNode->normalCall == 0){
        temp = create_new_expression(var_e, new_temp(), NULL, 00, NULL, 00);
        emmit(tablegetelem, temp, lvalue, callNode->name, 00, yylineno);
    }else{
        if(lvalue->type == tableitem_e){
            temp = create_new_expression(var_e, new_temp(), NULL, 00, NULL, 00);
            emmit(tablegetelem, temp, lvalue, lvalue->index, 00, yylineno);
        }else{
            temp = lvalue;
        }
    }

    param_creator(callNode->elist);

    if(callNode->normalCall == 0)emmit(param, NULL, lvalue, NULL, 0, yylineno);

    emmit(call, NULL, temp, NULL, 00, yylineno);
}

void create_elist_table_elements(struct expr *table, struct expr *elist){
    struct expr *temp;
    struct expr *int_expr;
    int index;

    temp = elist;

    for(index = 0; temp; index++){
        int_expr = create_new_expression(constint_e, NULL, NULL, index, NULL, 02);
        emmit(tablesetelem, table, int_expr, temp, 00, yylineno);
        temp = temp->next;
    }
}

void create_indexed_table_elements(struct expr *table, struct expr *elist){
    struct expr *temp;

    temp = elist;

    while(temp){
        emmit(tablesetelem, table, temp->index, temp, 00, yylineno);
        temp = temp->next;
    }
}

int is_temp_expr(struct expr *expr){
    if(expr == NULL)return 0;

    if(expr->sym == NULL)return 0;

    if(expr->sym->value.varVal->name[0] == '_')return 1;

    return 0;
}

void check_if_temp(struct expr *quad, struct expr *expr){
    if(!quad)return;

    if(!quad->sym)return;

    if(!quad->sym->value.varVal)return;

    if(!quad->sym->value.varVal->name)return;

    if(quad->sym->value.varVal->name[0] = '_'){
        quad->sym = (uSymbolTableEntry *)malloc(sizeof(uSymbolTableEntry));
        memcpy(quad->sym, expr->sym, sizeof(uSymbolTableEntry));
    }
}

//-------------END OF UTILITIES FILE---------------