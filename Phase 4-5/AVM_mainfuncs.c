// AVM MAINFUNCS
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "AVM.h"


int main(int argc, char** argv){
    FILE* fd;

    if(argc > 1){
        fd = fopen(argv[1], "rb");
        if(!fd){
            fprintf(stdout, "\nCannot read file: %s\n", argv[1]);
            return 1;
        }
    }else{
        fd = fopen("tcode.abc", "rb");
        if(!fd){
            fprintf(stderr, "\nYou haven\'t provide a file to execute!\n");
            exit(1);
        }
    }

    if(argc > 2){
        fprintf(stderr, "\nTOO MANY ARGUMENTS!\n");
        exit(1);
    }

    read_file_and_create_tables(fd);
    init_avm();
    avm_initstack();

    while(!executionFinished) execute_cycle();
    
     
}

void init_avm(){
    executionFinished = 0;
    pc = 0;
    currLine = 0;
    
    char* typeString[8] = {
        "number",
        "string",
        "bool",
        "table",
        "userfunc",
        "libfunc",
        "nil",
        "undef"
    };
}


void read_file_and_create_tables(FILE* fd){
    unsigned int tmp, i, string_size_helper;

    fread(&tmp, 1, sizeof(unsigned int), fd);
    if(tmp != 340062020) {
        fprintf(stderr, "Wrong magic number\n");
        exit(1);
    }

    fread(&total_global_vals, 1, sizeof(unsigned int), fd);

    fread(&const_string_array_size, 1, sizeof(unsigned int), fd);
    if(const_string_array_size > 0) const_string_array = (char**)malloc(const_string_array_size * sizeof(char*));
    else const_string_array = NULL;
    for(i = 0; i < const_string_array_size; i++){
        fread(&string_size_helper, 1, sizeof(unsigned int), fd);
        const_string_array[i] = (char *)malloc(string_size_helper * sizeof(char));
        fread(const_string_array[i], string_size_helper, sizeof(char) , fd);
    }

    fread(&const_number_array_size, 1, sizeof(unsigned int), fd);
    if(const_number_array_size > 0) const_number_array = (double*)malloc(const_number_array_size * sizeof(double));
    else const_number_array = NULL;
    for(i = 0; i < const_number_array_size; i++){
        fread(&(const_number_array[i]), 1, sizeof(double), fd);
    }

    fread(&userfunc_array_size, 1, sizeof(unsigned int), fd);
    if(userfunc_array_size > 0) userfunc_array = (struct userfunc_array*)malloc(userfunc_array_size * sizeof(struct userfunc_array));
    else userfunc_array = NULL;
    for(i = 0; i < userfunc_array_size; i++){
        fread(&string_size_helper, 1, sizeof(unsigned int), fd);
        userfunc_array[i].id = (char*)malloc(string_size_helper * sizeof(char));
        fread(userfunc_array[i].id, string_size_helper, sizeof(char) , fd);

        fread(&userfunc_array[i].address, 1, sizeof(unsigned int), fd);
        fread(&userfunc_array[i].localSize, 1, sizeof(unsigned int), fd);
    }

    fread(&libfunc_array_size, 1, sizeof(unsigned int), fd);
    if(libfunc_array_size > 0) libfunc_array = (char**)malloc(libfunc_array_size * sizeof(char*));
    else libfunc_array = NULL;
    for(i = 0; i < libfunc_array_size; i++){
        fread(&string_size_helper, 1, sizeof(unsigned int), fd);
        libfunc_array[i] = (char *)malloc(string_size_helper * sizeof(char));
        fread(libfunc_array[i], string_size_helper, sizeof(char) , fd);
    }

    fread(&codeSize, 1, sizeof(unsigned int), fd);
    if(!codeSize) {
        fprintf(stderr,"No code inside the file\n");
        exit(1);
    }
    code = (struct instruction*)malloc(codeSize * sizeof(struct instruction));
    for(i = 0; i < codeSize; i++){
        fread(&(code[i].opcode), 1, sizeof(enum vmopcode), fd);
        fread(&(code[i].srcLine), 1, sizeof(unsigned int), fd);
        
        switch(code[i].opcode){
            case add_v:
            case sub_v:
            case mul_v:
            case div_v:
            case mod_v:
            case tablegetelem_v:
            case tablesetelem_v:
            case jeq_v:
            case jne_v:
            case jgt_v:
            case jge_v:
            case jlt_v:
            case jle_v:
                fread(&(code[i].result.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].result.val), 1, sizeof(unsigned int), fd);
                fread(&(code[i].arg1.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].arg1.val), 1, sizeof(unsigned int), fd);
                fread(&(code[i].arg2.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].arg2.val), 1, sizeof(unsigned int), fd);
                break;
            case call_v:
            case funcenter_v:
            case funcexit_v:
            case pusharg_v:
            case newtable_v:
                fread(&(code[i].arg1.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].arg1.val), 1, sizeof(unsigned int), fd);
                break;
            case assign_v:
                fread(&(code[i].result.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].result.val), 1, sizeof(unsigned int), fd);
                fread(&(code[i].arg1.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].arg1.val), 1, sizeof(unsigned int), fd);
                break;
            case jump_v:
                fread(&(code[i].result.type), 1, sizeof(enum vmarg_t), fd);
                fread(&(code[i].result.val), 1, sizeof(unsigned int), fd);
                break;
            default: assert(0);
        }
    }

}

void execute_cycle(){

    struct instruction* instr;
    unsigned int oldPC;

    if(executionFinished) return;

    else if(pc == AVM_ENDING_PC){
        executionFinished = 1;
        return;
    }

    else{
        assert(pc < AVM_ENDING_PC);

        instr = code + pc;

        assert(
            instr->opcode >= 0 &&
            instr->opcode <= AVM_MAX_INSTRUCTIONS
        );

        if(instr->srcLine) currLine = instr->srcLine;
        oldPC = pc;
        execute_code(instr->opcode, instr);
        if(pc == oldPC) pc++;
    }

}

void execute_code(enum vmopcode opcode, struct instruction* instr){
    assert(instr);

    switch(opcode){
        case add_v:
        case sub_v:
        case mul_v:
        case div_v:
        case mod_v:
            execute_arithmetic(instr);
            break;
        case newtable_v:
            execute_newtable(instr);
            break;
        case tablegetelem_v:
            execute_tablegetelem(instr);
            break;
        case tablesetelem_v:
            execute_tablesetelem(instr);
            break;
        case assign_v:
            execute_assign(instr);
            break;
        case jump_v:
            execute_jump(instr);
            break;
        case jeq_v:
            execute_jeq(instr);
            break;
        case jne_v:
            execute_jne(instr);
            break;
        case jgt_v:
            execute_jgt(instr);
            break;
        case jge_v:
            execute_jge(instr);
            break;
        case jlt_v:
            execute_jlt(instr);
            break;
        case jle_v:
            execute_jle(instr);
            break;
        case pusharg_v:
            execute_pusharg(instr);
            break;
        case call_v:
            execute_call(instr);
            break;
        case funcenter_v:
            execute_funcenter(instr);
            break;
        case funcexit_v:
            execute_funcexit(instr);
            break;
    }
}