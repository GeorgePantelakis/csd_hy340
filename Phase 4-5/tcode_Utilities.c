// TCODE UTILITIES FILE
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "Utilities.h"

//-------------FINAL CODE FUNCTIONS----------------

void init_instructions(){
    ij_head = NULL;
    ij_total = 0;

    magic_number = 340062020;

    string_list = NULL;
    number_list = NULL;
    userfunc_list = NULL;
    libfunc_list = NULL;
    string_array_size = 0;
    number_array_size = 0;
    userfunc_array_size = 0;
    libfunc_array_size = 0;

    t_funcstack = NULL;

    max_instructions = EXPAND_SIZE_IN;
    instruction_index = 0;
    instructions = (struct instruction*)malloc(CURR_SIZE_IN);
}

void generate_ASSIGN(struct quad *quad){ generate(assign_t, quad); }
void generate_ADD(struct quad *quad){ generate(add_t, quad); }
void generate_SUB(struct quad *quad){ generate(sub_t, quad); }
void generate_MUL(struct quad *quad){ generate(mul_t, quad); }
void generate_DIV(struct quad *quad){ generate(div_tc, quad); }
void generate_MOD(struct quad *quad){ generate(mod_t, quad); }
void generate_IF_EQ(struct quad *quad){ generate_relational(jeq, quad); }
void generate_IF_NOTEQ(struct quad *quad){ generate_relational(jne, quad);}
void generate_IF_LESSEQ(struct quad *quad){ generate_relational(jle, quad); }
void generate_IF_GREATEREQ(struct quad *quad){ generate_relational(jge, quad); }
void generate_IF_LESS(struct quad *quad){ generate_relational(jlt, quad); }
void generate_IF_GREATER(struct quad *quad){ generate_relational(jgt, quad); }
void generate_NEWTABLE(struct quad *quad){ generate(newtable, quad); }
void generate_TABLEGETELEM(struct quad *quad){ generate(tablegetelem_t, quad); }
void generate_TABLESETELEM(struct quad *quad){ generate(tablesetelem_t, quad); }
void generate_JUMP(struct quad *quad){ generate_relational(jump_t, quad); }
void generate_UMINUS(struct quad *quad){ generate(mul_t, quad); }

void generate_CALL(struct quad *quad){
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    t->opcode = callfunc;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}

void generate_PARAM(struct quad *quad){
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    t->opcode = pusharg;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}

void generate_RETURN(struct quad *quad){
    uSymbolTableEntry *f;
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    t->opcode = assign_t;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    make_retvaloperand(&(t->result));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
    
    append_tcode_returnlist(nextinstructionlabel());
}

void generate_GETRETVAL(struct quad *quad){
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    t->opcode = assign_t;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    make_retvaloperand(&(t->arg1));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}

void generate_FUNCSTART(struct quad *quad){
    uSymbolTableEntry *f;
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    f = quad->arg1->sym;
    f->value.funcVal->taddress = nextinstructionlabel();
    
    add_userfunc(f);
    push_tcode_funcstack();

    t->opcode = enterfunc;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}

void generate_FUNCEND(struct quad *quad){
    uSymbolTableEntry *f;
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));

    backpatch_tcode_returnlist();

    t->opcode = exitfunc;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}


void add_incomplete_jump(unsigned int instrNo, unsigned int iaddress){
    struct incomplete_jump *tmp;
    
    if(ij_head == NULL){
        ij_head = (struct incomplete_jump*)malloc(sizeof(struct incomplete_jump));
        ij_head->instrNo = instrNo;
        ij_head->iaddress = iaddress;
        ij_head->next = NULL;

    }else{
        tmp = ij_head;
        while(tmp->next) tmp = tmp->next;
        tmp->next = (struct incomplete_jump*)malloc(sizeof(struct incomplete_jump));
        tmp->next->instrNo = instrNo;
        tmp->next->iaddress = iaddress;
        tmp->next->next = NULL;
    }
    
    ij_total++;
}

void generate(enum vmopcode op, struct quad *quad){
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    t->opcode = op;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}

void generate_relational(enum vmopcode op, struct quad *quad){
    struct instruction *t = (struct instruction*)malloc(sizeof(struct instruction));
    t->opcode = op;
    make_operand(quad->arg1, &(t->arg1));
    make_operand(quad->arg2, &(t->arg2));
    make_operand(quad->result, &(t->result));
    t->result.type = label_a;

    if(quad->label < global_generate_index) t->result.val = quads[quad->label].taddress; 
    else add_incomplete_jump(nextinstructionlabel(), quad->label);
    
    quad->taddress = nextinstructionlabel();
    emmit_instruction(t);
}

void generate_tcode(void){ 
    for(global_generate_index = 0; global_generate_index < quad_index; global_generate_index++){
        switch(quads[global_generate_index].op){
            case assign: generate_ASSIGN(quads + global_generate_index); break;
            case add: generate_ADD(quads + global_generate_index); break;
            case sub: generate_SUB(quads + global_generate_index); break;
            case mul: generate_MUL(quads + global_generate_index); break;
            case div_e: generate_DIV(quads + global_generate_index); break;
            case mod: generate_MOD(quads + global_generate_index); break;
            case uminus: generate_UMINUS(quads + global_generate_index); break;
            case if_eq: generate_IF_EQ(quads + global_generate_index); break;
            case if_noteq: generate_IF_NOTEQ(quads + global_generate_index); break;
            case if_lesseq: generate_IF_LESSEQ(quads + global_generate_index); break;
            case if_greatereq: generate_IF_GREATEREQ(quads + global_generate_index); break;
            case if_less: generate_IF_LESS(quads + global_generate_index); break;
            case if_greater: generate_IF_GREATER(quads + global_generate_index); break;
            case call: generate_CALL(quads + global_generate_index); break;
            case param: generate_PARAM(quads + global_generate_index); break;
            case ret: generate_RETURN(quads + global_generate_index); break; 
            case getretval: generate_GETRETVAL(quads + global_generate_index); break;
            case funcstart: generate_FUNCSTART(quads + global_generate_index); break;
            case funcend: generate_FUNCEND(quads + global_generate_index); break;
            case tablecreate: generate_NEWTABLE(quads + global_generate_index); break;
            case tablegetelem: generate_TABLEGETELEM(quads + global_generate_index); break;
            case tablesetelem: generate_TABLESETELEM(quads + global_generate_index); break;
            case jump: generate_JUMP(quads + global_generate_index); break;
            default: assert(0);
        }
    }
}

unsigned int consts_newstring(char *s){
    struct const_string_list *tmp = string_list;
    assert(s);

    if(string_list == NULL){
        string_list = (struct const_string_list*)malloc(sizeof(struct const_string_list));
        string_list->index = 0;
        string_list->value = (char *)malloc((strlen(s) + 1) * sizeof(char));
        strcpy(string_list->value, s);
        string_list->next = NULL;

        string_array_size++;
        return 0;

    }else{

         while(tmp){
            if(strcmp(tmp->value, s) == 0)
                return tmp->index;
            tmp = tmp->next;
        }
        
        tmp = string_list;
        while(tmp->next) tmp = tmp->next;
        tmp->next = (struct const_string_list*)malloc(sizeof(struct const_string_list));
        tmp->next->index = (tmp->index + 1);
        tmp->next->value = (char *)malloc((strlen(s) + 1) * sizeof(char));
        strcpy(tmp->next->value, s);
        tmp->next->next = NULL;

        string_array_size++;
        return tmp->next->index;
    }
}

unsigned int consts_newnumber(double n){
    struct const_number_list *tmp = number_list;

    if(number_list == NULL){
        number_list = (struct const_number_list*)malloc(sizeof(struct const_number_list));
        number_list->index = 0;
        number_list->value = n;
        number_list->next = NULL;

        number_array_size++;
        return 0;

    }else{

        while(tmp){
            if(tmp->value == n)
                return tmp->index;
            tmp = tmp->next;
        }

        tmp = number_list;
        while(tmp->next) tmp = tmp->next;
        tmp->next = (struct const_number_list*)malloc(sizeof(struct const_number_list));
        tmp->next->index = (tmp->index + 1);
        tmp->next->value = n;
        tmp->next->next = NULL;

        number_array_size++;
        return tmp->next->index;
    }
}
unsigned int libfuncs_newused(char *s){
    struct libfunc_list *tmp = libfunc_list;
    assert(s);

    if(libfunc_list == NULL){
        libfunc_list = (struct libfunc_list*)malloc(sizeof(struct libfunc_list));
        libfunc_list->index = 0;
        libfunc_list->value = (char *)malloc((strlen(s) + 1) * sizeof(char));
        strcpy(libfunc_list->value, s);
        libfunc_list->next = NULL;

        libfunc_array_size++;
        return 0;

    }else{

        while(tmp){
            if(strcmp(tmp->value, s) == 0)
                return tmp->index;
            tmp = tmp->next;
        }
       
        tmp = libfunc_list;
        while(tmp->next) tmp = tmp->next;
        tmp->next = (struct libfunc_list*)malloc(sizeof(struct libfunc_list));
        tmp->next->index = (tmp->index + 1);
        tmp->next->value = (char *)malloc((strlen(s) + 1) * sizeof(char));
        strcpy(tmp->next->value, s);
        tmp->next->next = NULL;

        libfunc_array_size++;
        return tmp->next->index;
    }
}

void add_userfunc(uSymbolTableEntry *symbol){
    struct userfunc_list *tmp;

    if(userfunc_list == NULL){
        userfunc_list = (struct userfunc_list*)malloc(sizeof(struct userfunc_list));
        userfunc_list->index = 0;
        userfunc_list->value = symbol;
        userfunc_list->next = NULL;

    }else{
        tmp = userfunc_list;
        while(tmp->next) tmp = tmp->next;
        tmp->next = (struct userfunc_list*)malloc(sizeof(struct userfunc_list));
        tmp->next->index = (tmp->index + 1);
        tmp->next->value = symbol;
        tmp->next->next = NULL;
    }

    userfunc_array_size++;
}

void make_operand(struct expr *e, struct vmarg *arg){

    if(!e){
        arg->type = ignore_a;
        return;
    }
   
    switch(e->type){
        case var_e:
        case tableitem_e: 
        case arithexpr_e: 
        case boolexpr_e: 
        case newtable_e: {
            assert(e->sym);
            arg->val = e->sym->offset;
            
            switch(e->sym->space){
                case programvar: arg->type = global_a; break;
                case functionlocal: arg->type = local_a; break;
                case formalarg: arg->type = formal_a; break;
                default: assert(0);
            }

            break;
        }

        case constbool_e: {
            arg->val = e->boolConst;
            arg->type = bool_a;
            break;
        }

        case conststring_e: {
            arg->val = consts_newstring(e->strConst);
            arg->type = string_a;
            break;
        }

        case constint_e: {
            arg->val = consts_newnumber((double)e->numConst);
            arg->type = number_a;
            break;
        }

        case constfloat_e: {
            arg->val = consts_newnumber((double)e->numConst);
            arg->type = number_a;
            break;
        }

        case nil_e: {
            arg->type = nil_a;
            break;
        }

        case programfunc_e: {
            arg->type = userfunc_a;
            arg->val = e->sym->value.funcVal->taddress; 
            break;
        }

        case libraryfunc_e: {
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->value.funcVal->name);
            break;
        }

        default: assert(0);
    }
}

void make_retvaloperand(struct vmarg *arg){
    arg->type = retval_a;
}

void emmit_instruction(struct instruction *instruction){
    struct instruction* current_instruction;

    if(instruction_index >= max_instructions) expand_table_instructions();

    current_instruction =  &(instructions[instruction_index]);
    current_instruction->srcLine = quads[global_generate_index].line;
    current_instruction->opcode = instruction->opcode;
    current_instruction->arg1 = instruction->arg1;
    current_instruction->arg2 = instruction->arg2;
    current_instruction->result = instruction->result;
    
    instruction_index++; 

    if(instruction) free(instruction);  
}

void patch_incomplete_jumps(){  
    unsigned int i;
    struct incomplete_jump *tmp;

    while(ij_head){
        if(ij_head->iaddress == quad_index) instructions[ij_head->instrNo].result.val = quad_index;
        else instructions[ij_head->instrNo].result.val = quads[ij_head->iaddress].taddress;

        tmp = ij_head;
        ij_head = ij_head->next;
        if(tmp) free(tmp);
    }
    
}

void expand_table_instructions(){
    struct instruction *temp_instruction_pointer;
    int i;

    assert(instruction_index >= max_instructions);

    temp_instruction_pointer = instructions;
    instructions = (struct instruction*)malloc(NEW_SIZE_IN);

    for(i = 0; i < max_instructions; i++){
        instructions[i] = temp_instruction_pointer[i];
    }

    max_instructions += EXPAND_SIZE_IN;

    if(temp_instruction_pointer) free(temp_instruction_pointer);
}

unsigned int nextinstructionlabel(){
    return instruction_index;
}

void push_tcode_funcstack(){
    struct tcode_funcstack *tmp;

    if(t_funcstack == NULL){
        t_funcstack = (struct tcode_funcstack*)malloc(sizeof(struct tcode_funcstack));
        t_funcstack->returnlist = NULL;
        t_funcstack->next = NULL;

    }else{
        tmp = (struct tcode_funcstack*)malloc(sizeof(struct tcode_funcstack));
        tmp->returnlist = NULL;
        tmp->next = t_funcstack;
        t_funcstack = tmp;
    }
}

void append_tcode_returnlist(unsigned int index){
    struct tcode_returnlist *tmp;

    if(t_funcstack == NULL) fprintf(stdout, "Trying to append in empty funcstack\n");
    else{
        if(t_funcstack->returnlist == NULL){
            t_funcstack->returnlist = (struct tcode_returnlist*)malloc(sizeof(struct tcode_returnlist));
            t_funcstack->returnlist->index = index;
            t_funcstack->returnlist->next = NULL;
            
        }else{
            tmp = t_funcstack->returnlist;
            while(tmp) tmp = tmp->next;
            tmp = (struct tcode_returnlist*)malloc(sizeof(struct tcode_returnlist));
            tmp->index = index;
            tmp->next = NULL;
        }
    }
}

void backpatch_tcode_returnlist(){
    struct tcode_funcstack *tmp;

    if(t_funcstack == NULL) fprintf(stdout, "Trying to backpatch in empty funcstack\n");
    else{
        if(t_funcstack->returnlist != NULL){
            while(t_funcstack->returnlist){
                instructions[t_funcstack->returnlist->index].result.val = nextinstructionlabel();
                t_funcstack->returnlist = t_funcstack->returnlist->next;
            }
        }
        tmp = t_funcstack;
        t_funcstack = t_funcstack->next;
        if(tmp) free(tmp);
    }
}

//-------------END OF TCODE UTILITIES FILE---------------