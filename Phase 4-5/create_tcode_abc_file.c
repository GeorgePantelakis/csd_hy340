// CREATE TCODE ABC FILE
// NIKOS NTORVAS - GEORGIOS STAVROS PANTELAKIS
// A.M.: 4013 - 4017
// all copyrights reserved

#include "Utilities.h"

void create_tcode_abc_file(){
    unsigned int i, tmp;
    struct const_string_list *tmp_string_list;
    struct const_number_list *tmp_number_list;
    struct userfunc_list *tmp_userfunc_list;
    struct libfunc_list *tmp_libfunc_list;
    struct instruction current_instruction;

    tmp_string_list = string_list;
    tmp_number_list = number_list;
    tmp_userfunc_list = userfunc_list;
    tmp_libfunc_list = libfunc_list;

    tcode_abc_file = fopen("tcode.abc", "wb");
    if(!tcode_abc_file){
        fprintf(stderr, "\nCannot create tcode file\n");
        exit(1);
    }

    fwrite(&magic_number, 1, sizeof(unsigned int), tcode_abc_file);
    programm_var_offset++;
    fwrite(&programm_var_offset, 1, sizeof(unsigned int), tcode_abc_file);

    fwrite(&string_array_size, 1, sizeof(unsigned int), tcode_abc_file);
    while(tmp_string_list != NULL){
        tmp = (strlen(tmp_string_list->value) + 1);
        fwrite(&tmp, 1, sizeof(unsigned int), tcode_abc_file);
        fwrite(tmp_string_list->value, tmp, sizeof(char), tcode_abc_file);
        tmp_string_list = tmp_string_list->next;
    }

    fwrite(&number_array_size, 1, sizeof(unsigned int), tcode_abc_file);
    while(tmp_number_list != NULL){
        fwrite(&(tmp_number_list->value), 1, sizeof(double), tcode_abc_file);
        tmp_number_list = tmp_number_list->next;
    }

    fwrite(&userfunc_array_size, 1, sizeof(unsigned int), tcode_abc_file);
    while(tmp_userfunc_list != NULL){
        tmp = (strlen(tmp_userfunc_list->value->value.funcVal->name) + 1);
        fwrite(&tmp, 1, sizeof(unsigned int), tcode_abc_file);
        fwrite(tmp_userfunc_list->value->value.funcVal->name, tmp, sizeof(char), tcode_abc_file);

        fwrite(&(tmp_userfunc_list->value->value.funcVal->taddress), 1, sizeof(unsigned int), tcode_abc_file);
        fwrite(&(tmp_userfunc_list->value->value.funcVal->localVars), 1, sizeof(unsigned int), tcode_abc_file);
        tmp_userfunc_list = tmp_userfunc_list->next;
    }

    fwrite(&libfunc_array_size, 1, sizeof(unsigned int), tcode_abc_file);
    while(tmp_libfunc_list != NULL){
        tmp = (strlen(tmp_libfunc_list->value) + 1);
        fwrite(&tmp, 1, sizeof(unsigned int), tcode_abc_file);
        fwrite(tmp_libfunc_list->value, tmp, sizeof(char), tcode_abc_file);
        tmp_libfunc_list = tmp_libfunc_list->next;
    }

    fwrite(&instruction_index, 1, sizeof(unsigned int), tcode_abc_file);
    for(i = 0; i < instruction_index; i++){
        fwrite(&(instructions[i].opcode), 1, sizeof(enum vmopcode), tcode_abc_file);
        fwrite(&(instructions[i].srcLine), 1, sizeof(unsigned int), tcode_abc_file);
        print_register_b(instructions[i].result);
        print_register_b(instructions[i].arg1);
        print_register_b(instructions[i].arg2);
    }

    fclose(tcode_abc_file);
}

void print_register_b(struct vmarg arg){
    switch(arg.type){
        case ignore_a:
            break;
        default:
            fwrite(&(arg.type), 1, sizeof(enum vmarg_t), tcode_abc_file);
            fwrite(&(arg.val), 1, sizeof(unsigned int), tcode_abc_file);
    }
}