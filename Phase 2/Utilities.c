// UTILITIES FILE
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "Utilities.h"

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
        if(!temp)free(temp);
    }
}

char* filter_string(char* input_string){
    int i, output_string_index = 0;
    char * output_string = (char *)malloc(sizeof(char) * strlen(input_string));

    for(i = 0; i < (strlen(input_string)); i++){
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
    if(!tmp) free(tmp);

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
    if(!tmp->next) free(tmp->text);
    if(!tmp) free(tmp);

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
    insert_to_symbol_table("arguments", LIBFUNC);
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

    uSymbolTableEntry* temp, *bucket;
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

    if(type == GLOBAL || type == LOCAL || type == FORMAL){
        var = (Variable*)malloc(sizeof(Variable));
        var->name = (char*)malloc(sizeof(name));
        strcpy(var->name, name);
        var->scope = current_scope;
        var->line = yylineno;
        temp->value.varVal = var;
    }else{
        func = (Function*)malloc(sizeof(Function));
        func->name = (char*)malloc(sizeof(name));
        strcpy(func->name, name);
        func->scope = current_scope;
        func->line = ((type == LIBFUNC) ? 0 : yylineno);
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

    
    if(!strcmp(lookup_mode, "CURRENT")){
        
        index = hash_function(name);
        bucket = SymbolTable[index];
        
        if(bucket == NULL) return NULL;
        
        while(bucket != NULL){
            if(bucket->type == GLOBAL || bucket->type == LOCAL || bucket->type == FORMAL){
                
                if( (!strcmp(bucket->value.varVal->name, name)) &&  (bucket->value.varVal->scope == current_scope) && (bucket->isActive == true) ){
                    return bucket;
                }
                      
            }else{ //for functions
                
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
                    
            }else{

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
                            return NodeValue->value;
                        }  
                    }
                        
                }else{ //for function search

                    if( !strcmp(NodeValue->value->value.funcVal->name, name) && (NodeValue->value->isActive == true) ){
                        return NodeValue->value;
                    }
                        
                }

                NodeValue = NodeValue->next;
            }
        }

        return NULL;
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

int hash_function(char* string){
    int hash = 5381;
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
                fprintf(stdout, "\"%s\" [%s] (line %d) (scope %d)\n", var->name, type_of_variable, var->line, var->scope);    
            else
                fprintf(stdout, "\"%s\" [%s] (line %d) (scope %d)\n", func->name, type_of_variable, func->line, func->scope);  
            
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

    if(!tmp) free(tmp);
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

bool function_stack_is_empty(){
    if(function_scope_stack == NULL) return true;
    return false;
}

int yyerror(char* Message){
    fprintf(stdout, "\n\033[1;31mERROR\033[0m: %s at line %d !\n", Message, yylineno);
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

//-------------END OF UTILITIES FILE------------